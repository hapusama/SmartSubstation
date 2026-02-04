#include <omnetpp.h>
#include <regex>
#include <cmath>
#include <unordered_map>
#include "inet/common/INETDefs.h"
#include "inet/common/packet/Packet.h"
#include "inet/common/Units.h"
#include "inet/common/packet/chunk/ByteCountChunk.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/common/InitStages.h"
#include "inet/transportlayer/contract/udp/UdpSocket.h"

using namespace omnetpp;
using namespace inet;

// DifferentialProtectionApp.cc
// 简易的差动保护应用：接收本地/远端 MU 的 SV 包并比较电流差，超过阈值时发送 GOOSE Trip 命令。

#include <omnetpp.h>
#include <regex>
#include <cmath>
#include "inet/common/INETDefs.h"
#include "inet/common/packet/Packet.h"
#include "inet/common/Units.h"
#include "inet/common/packet/chunk/ByteCountChunk.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/common/InitStages.h"
#include "inet/transportlayer/contract/udp/UdpSocket.h"

using namespace omnetpp;
using namespace inet;

//  - 通过两个 UDP 端口接收 SV（采样值）报文：一个端口接收本地 MU 的报文（socketLocal），
//    另一个端口接收远端 MU 的报文（socketRemote）。
//  - 从 Packet 的 name 中用正则提取电流值（例："SV:current=102.3"）。
//  - 当本地电流与远端电流的绝对差值超过参数 `threshold` 时，构造并发送 GOOSE（Trip）报文
//    到配置的 IT 终端地址（gooseDestLocal / gooseDestRemote）和端口（goosePort）。

class DifferentialProtectionApp : public cSimpleModule, public UdpSocket::ICallback
{
  private:
    // 接收本地 MU 的 UDP socket
    UdpSocket socketLocal;
    // 接收远端 MU 的 UDP socket
    UdpSocket socketRemote;
    // 用于发送 GOOSE 报文的 UDP socket
    UdpSocket socketGoose;

    // 从 NED/ini 读取的配置参数
    double threshold;          // 差动保护阈值（安培）
    int localPort = -1;        // 本地 MU 监听端口
    int remotePort = -1;       // 远端 MU 监听端口
    L3Address gooseLocalDest;  // GOOSE 发送目的地址（本地 IT）
    L3Address gooseRemoteDest; // GOOSE 发送目的地址（远端 IT）
    int goosePort;             // GOOSE 目的端口
    int gooseDscp = 48;        // GOOSE 报文的 DSCP 值（CS6，次高优先级）
    bool recordStats = true;   // 是否记录端到端时延/抖动

    // 端到端时延/抖动记录
    cOutVector delayLocalVec;
    cOutVector delayRemoteVec;
    cOutVector jitterLocalVec;
    cOutVector jitterRemoteVec;
    simtime_t lastDelayLocal;
    simtime_t lastDelayRemote;
    bool hasDelayLocal = false;
    bool hasDelayRemote = false;

    // 接收计数（用于运行结束时打印）
    long localRxCount = 0;
    long remoteRxCount = 0;

    // 差动比较计数与超阈值计数（按配对时隙/样本统计）
    long matchedSvCount = 0;
    long overThresholdCount = 0;

    // 存储最近一次接收到的電流值（可能为 NaN 表示尚未收到）
    double lastLocal = NAN;
    double lastRemote = NAN;

    // 是否严格按时隙标签匹配：true 时只在“同一时隙 slot”成对后才计算差值
    bool strictSlotMatch = true;
    int maxSlotLag = 10;

    // 按时隙缓存样本值（slot -> 电流值），用于配对
    std::unordered_map<long long, double> localSamples;
    std::unordered_map<long long, double> remoteSamples;

    void pruneOldSamples(std::unordered_map<long long, double> &samples, long long minSlot) {
        for (auto it = samples.begin(); it != samples.end(); ) {
            if (it->first < minSlot)
                it = samples.erase(it);
            else
                ++it;
        }
    }

  protected:
    // 需要多个初始化阶段以确保网络接口表等可解析地址的组件已就绪
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }

    virtual void initialize(int stage) override {
        cSimpleModule::initialize(stage);
        if (stage == INITSTAGE_LOCAL) {
            // 在本地阶段读取简单数值参数，不做地址解析
            localPort = par("localPort");
            remotePort = par("remotePort");
            threshold = par("threshold");
            goosePort = par("goosePort");
            gooseDscp = par("gooseDscp");
            recordStats = par("recordStats");
            strictSlotMatch = par("strictSlotMatch");
            maxSlotLag = par("maxSlotLag");

            delayLocalVec.setName("svDelayLocal");
            delayRemoteVec.setName("svDelayRemote");
            jitterLocalVec.setName("svJitterLocal");
            jitterRemoteVec.setName("svJitterRemote");
        }
        else if (stage == INITSTAGE_APPLICATION_LAYER) {
            // 在应用层阶段解析地址（依赖于接口表），并初始化/绑定 UDP sockets
            gooseLocalDest = L3AddressResolver().resolve(par("gooseDestLocal"));
            gooseRemoteDest = L3AddressResolver().resolve(par("gooseDestRemote"));

            // 配置本地接收 socket：将输出门设置为模块的 socketOut，用回调处理收到的数据
            socketLocal.setOutputGate(gate("socketOut"));
            socketLocal.setCallback(this);
            socketLocal.bind(localPort);

            // 配置远端接收 socket
            socketRemote.setOutputGate(gate("socketOut"));
            socketRemote.setCallback(this);
            socketRemote.bind(remotePort);

            // 配置用于发送 GOOSE 的 socket（未绑定具体端口，使用 ephemeral）
            socketGoose.setOutputGate(gate("socketOut"));
            socketGoose.bind(-1);
            // 通过 IPv4 TOS 设置 DSCP（DSCP 位于 TOS 的高 6 位）：将 gooseDscp 左移 2 位
            // 例如 CS6=48 -> TOS=192；此设置将应用于该 socket 发送的所有 GOOSE 报文
            socketGoose.setTos(gooseDscp << 2);
        }
    }

    // 将收到的消息分派给对应的 UdpSocket 进行处理（由 UdpSocket 回调触发 socketDataArrived）
    virtual void handleMessage(cMessage *msg) override {
        if (socketLocal.belongsToSocket(msg)) {
            socketLocal.processMessage(msg);
        }
        else if (socketRemote.belongsToSocket(msg)) {
            socketRemote.processMessage(msg);
        }
        else if (socketGoose.belongsToSocket(msg)) {
            socketGoose.processMessage(msg);
        }
        else {
            // 防御性处理：非 socket 消息或意外自定时消息
            delete msg;
        }
    }

    // UdpSocket 的回调：当数据到达时调用
    virtual void socketDataArrived(UdpSocket *socket, Packet *packet) override {
        // 本实现假设 SV 报文的 Packet::getName() 中包含类似 "current=123.45" 的文本
        // 因此直接在名字中用正则提取电流值（简化处理，未解析二进制报文体）
        std::string name = packet->getName();
        std::smatch m;
        double value = NAN;
        if (std::regex_search(name, m, std::regex("current=([+-]?[0-9]*\\.?[0-9]+)"))) {
            value = std::stod(m[1].str());
        }
        // 从报文名中解析时隙标签 slot（由 SV 发送端写入）
        long long slot = -1;
        bool hasSlot = false;
        if (std::regex_search(name, m, std::regex("slot=([0-9]+)"))) {
            slot = std::stoll(m[1].str());
            hasSlot = true;
        }

        // 计算端到端时延和抖动（基于 Packet 创建时间）
        simtime_t sent = packet->getTimestamp();
        if (sent == SIMTIME_ZERO)
            sent = packet->getCreationTime();
        simtime_t delay = simTime() - sent;

        // 根据数据来自哪个 socket 更新相应的寄存值
        if (socket == &socketLocal) {
            localRxCount++;
            if (recordStats) {
                delayLocalVec.record(delay);
                if (hasDelayLocal) {
                    jitterLocalVec.record(delay - lastDelayLocal);
                }
                lastDelayLocal = delay;
                hasDelayLocal = true;
            }
            lastLocal = value;
        } else {
            remoteRxCount++;
            if (recordStats) {
                delayRemoteVec.record(delay);
                if (hasDelayRemote) {
                    jitterRemoteVec.record(delay - lastDelayRemote);
                }
                lastDelayRemote = delay;
                hasDelayRemote = true;
            }
            lastRemote = value;
        }

        // 释放 packet（我们没有进一步解析其 payload）
        delete packet;

        if (std::isnan(value))
            return;

        // 严格按时隙标签匹配：同一 slot 成对后才计算差值
        if (strictSlotMatch) {
            // 没有 slot 字段则不参与配对
            if (!hasSlot) {
                EV_WARN << "SV packet missing slot tag; ignored in strict mode" << endl;
                return;
            }
            // 将本地/远端样本按 slot 缓存
            if (socket == &socketLocal)
                localSamples[slot] = value;
            else
                remoteSamples[slot] = value;

            // 清理过旧的样本，避免缓存无限增长
            long long minSlot = slot - maxSlotLag;
            if (maxSlotLag > 0) {
                pruneOldSamples(localSamples, minSlot);
                pruneOldSamples(remoteSamples, minSlot);
            }

            // 若本地与远端同一 slot 都已到达，则执行差动计算
            auto itLocal = localSamples.find(slot);
            auto itRemote = remoteSamples.find(slot);
            if (itLocal != localSamples.end() && itRemote != remoteSamples.end()) {
                double diff = fabs(itLocal->second - itRemote->second);
                matchedSvCount++;
                EV_INFO << "Differential(slot=" << slot << ") |I_local - I_remote| = " << diff
                        << " A, threshold=" << threshold << endl;
                if (diff > threshold) {
                    overThresholdCount++;
                    // 超阈值则发送 GOOSE Trip 报文
                    auto goosePkt = new Packet("GOOSE:TripCommand");
                    auto chunk = makeShared<ByteCountChunk>(B(64));
                    goosePkt->insertAtBack(chunk);
                    socketGoose.sendTo(goosePkt->dup(), gooseLocalDest, goosePort);
                    socketGoose.sendTo(goosePkt, gooseRemoteDest, goosePort);
                }
                // 该 slot 已处理，移除缓存
                localSamples.erase(slot);
                remoteSamples.erase(slot);
            }
            return;
        }

        // 非严格模式：当两侧都有有效电流值时计算差动并判决
        if (!std::isnan(lastLocal) && !std::isnan(lastRemote)) {
            double diff = fabs(lastLocal - lastRemote);
            matchedSvCount++;
            EV_INFO << "Differential |I_local - I_remote| = " << diff << " A, threshold=" << threshold << endl;
            if (diff > threshold) {
                overThresholdCount++;
                // 构造一个简单的 GOOSE 包并发送到两个 IT 目的地
                // 注意：这里用 ByteCountChunk(B(64)) 代表报文体占位，没有实现 GOOSE 格式细节
                auto goosePkt = new Packet("GOOSE:TripCommand");
                // DSCP 已通过 socket 的 IPv4 TOS 设置（在 initialize 阶段）；此处构造报文体占位。
                auto chunk = makeShared<ByteCountChunk>(B(64));
                goosePkt->insertAtBack(chunk);

                // 发送两份副本：到本地 IT 和远端 IT
                socketGoose.sendTo(goosePkt->dup(), gooseLocalDest, goosePort);
                socketGoose.sendTo(goosePkt, gooseRemoteDest, goosePort);
            }
        }
    }

    // 处理 socket 错误回调（此处仅释放 indication）
    virtual void socketErrorArrived(UdpSocket *socket, Indication *indication) override {
        delete indication;
    }

    // socket 被关闭的回调（本示例不需要特殊处理）
    virtual void socketClosed(UdpSocket *socket) override {}

    virtual void finish() override {
        EV_INFO << getFullPath() << ": received local=" << localRxCount
                << ", remote=" << remoteRxCount << " packets" << endl;
        EV_INFO << getFullPath() << ": received total=" << (localRxCount + remoteRxCount)
            << " packets" << endl;
        EV_INFO << getFullPath() << ": matchedSv=" << matchedSvCount
            << ", overThreshold=" << overThresholdCount << endl;
        recordScalar("localRxCount", localRxCount);
        recordScalar("remoteRxCount", remoteRxCount);
        recordScalar("totalRxCount", localRxCount + remoteRxCount);
        recordScalar("matchedSvCount", matchedSvCount);
        recordScalar("overThresholdCount", overThresholdCount);
    }
};

// 将模块注册到 OMNeT++ 模块工厂
Define_Module(DifferentialProtectionApp);
