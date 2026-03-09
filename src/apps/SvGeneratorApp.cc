#include <omnetpp.h>
#include <cmath>
#include <string>
#include <vector>
#include "inet/common/INETDefs.h"
#include "inet/common/packet/Packet.h"
#include "inet/common/Units.h"
#include "inet/common/packet/chunk/ByteCountChunk.h"
#include "inet/common/packet/chunk/BytesChunk.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/common/InitStages.h"
#include "inet/transportlayer/contract/udp/UdpSocket.h"

using namespace omnetpp;
using namespace inet;

// SvGeneratorApp：
//  - 该模块模拟 MU（测量单元）周期性发送采样值（SV）帧。
//  - 使用 UdpSocket 发送 UDP 包到本地保护与远端保护设备。
//  - 为了避免初始化时序问题，读取参数与解析地址分两个初始化阶段完成（见 initialize(int stage)）。
class SvGeneratorApp : public cSimpleModule
{
  private:
    UdpSocket socket;
    cMessage *timer = nullptr;
    L3Address localDest;
    int localPort;
    L3Address remoteDest;
    int remotePort;
    double base;
    double noise;
    int msgLenBytes;
    simtime_t interval;
    bool faultEnabled = false;
    simtime_t faultStart;
    simtime_t faultDuration;
    double faultDelta = 0;
    int dscp = 56;
    long long seq = 0;
    long txCount = 0;

  protected:
    // 指定需要的初始化阶段数（常量由 INET/OMNeT 提供）
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }

    // 分阶段初始化：
    //  - INITSTAGE_LOCAL: 读取参数并创建定时器（不做地址解析和 socket 绑定）
    //  - INITSTAGE_APPLICATION_LAYER: 当网络接口和地址表就绪后，再解析地址并绑定 socket，启动定时器
    virtual void initialize(int stage) override {
        cSimpleModule::initialize(stage);
        if (stage == INITSTAGE_LOCAL) {
            // 只读取参数（安全，不依赖于网络接口）
            localPort = par("localDestPort");
            remotePort = par("remoteDestPort");
            base = par("currentBase");
            noise = par("noiseStd");
            msgLenBytes = par("messageLength");
            interval = par("sendInterval");
            faultEnabled = par("faultEnabled");
            faultStart = par("faultStart");
            faultDuration = par("faultDuration");
            faultDelta = par("faultDelta");
            dscp = par("dscp");
            timer = new cMessage("sendTimer");
        }
        else if (stage == INITSTAGE_APPLICATION_LAYER) {
            // 此时接口表已建立，可以解析名字到地址
            localDest = L3AddressResolver().resolve(par("localDestAddress"));
            remoteDest = L3AddressResolver().resolve(par("remoteDestAddress"));

            // 配置 UDP socket 并绑定输出 gate
            socket.setOutputGate(gate("socketOut"));
            socket.bind(-1); // 发送方不需要固定端口，使用 ephemeral port
            // 通过 IPv4 TOS 设置 DSCP（DSCP 位于 TOS 的高 6 位）：将 dscp 左移 2 位
            // SV 默认使用 CS7=56 -> TOS=224，优先级高于 GOOSE 的 CS6=48
            socket.setTos(dscp << 2);

            // 启动发送定时器，每隔 interval将timer传给handleMessage处理，触发发送SV报文
            scheduleAt(simTime() + interval, timer);
        }
    }

    virtual void handleMessage(cMessage *msg) override {
        // 定时器触发时发送一帧 SV
        if (msg == timer) {
            // 生成带故障扰动的采样值，但是事实上infault就算是false，计算过程中也会出现超过阈值的情况，因为噪声的存在可能导致采样值偏离基值超过阈值。
            bool inFault = faultEnabled && simTime() >= faultStart && simTime() < (faultStart + faultDuration);
            double mean = base + (inFault ? faultDelta : 0.0); // base是来自于currentBase
            double value = normal(mean, noise);
            // slot 计算：基于发送时间与发送间隔计算时隙编号，interval是demo传进来的报文发送间隔
            // floor的作用在于向下取整，确保时隙编号是整数
            long long slot = (long long)floor(simTime().dbl() / interval.dbl());

            // 将业务字段写入报文负载，而不是写到 packet name 里。
            std::string payload = "slot=" + std::to_string(slot)
                                + ";seq=" + std::to_string(seq)
                                + ";current=" + std::to_string(value);
            std::vector<uint8_t> payloadBytes(payload.begin(), payload.end());

            auto packet = new Packet("SV");
            packet->setTimestamp(simTime());
            const auto svChunk = makeShared<BytesChunk>(payloadBytes);
            packet->insertAtBack(svChunk);

            // 若配置的报文长度更大，则补齐占位字节，保持链路负载规模不变。
            int paddingBytes = msgLenBytes - static_cast<int>(payloadBytes.size());
            if (paddingBytes > 0) {
                const auto padding = makeShared<ByteCountChunk>(B(paddingBytes));
                packet->insertAtBack(padding);
            }
            // 发送到本地保护（本地复制）和远端保护
            socket.sendTo(packet->dup(), localDest, localPort);
            socket.sendTo(packet, remoteDest, remotePort);
            txCount += 2;
            seq++;
            // 安排下一次发送
            scheduleAt(simTime() + interval, timer);
        }
        else {
            // 不是本模块的自定消息，安全删除
            delete msg;
        }
    }

    virtual void finish() override {
        cancelAndDelete(timer);
        EV_INFO << getFullPath() << ": sent SV packets=" << txCount << endl;
        recordScalar("svTxCount", txCount);
    }
};

Define_Module(SvGeneratorApp);
