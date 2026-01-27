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

    // 存储最近一次接收到的電流值（可能为 NaN 表示尚未收到）
    double lastLocal = NAN;
    double lastRemote = NAN;

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

        // 根据数据来自哪个 socket 更新相应的寄存值
        if (socket == &socketLocal) {
            lastLocal = value;
        } else {
            lastRemote = value;
        }

        // 释放 packet（我们没有进一步解析其 payload）
        delete packet;

        // 当两侧都有有效电流值时计算差动并判决
        if (!std::isnan(lastLocal) && !std::isnan(lastRemote)) {
            double diff = fabs(lastLocal - lastRemote);
            EV_INFO << "Differential |I_local - I_remote| = " << diff << " A, threshold=" << threshold << endl;
            if (diff > threshold) {
                // 构造一个简单的 GOOSE 包并发送到两个 IT 目的地
                // 注意：这里用 ByteCountChunk(B(64)) 代表报文体占位，没有实现 GOOSE 格式细节
                auto goosePkt = new Packet("GOOSE:TripCommand");
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
};

// 将模块注册到 OMNeT++ 模块工厂
Define_Module(DifferentialProtectionApp);
