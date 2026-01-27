#include <omnetpp.h>
#include "inet/common/INETDefs.h"
#include "inet/common/packet/Packet.h"
#include "inet/common/Units.h"
#include "inet/common/packet/chunk/ByteCountChunk.h"
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
            timer = new cMessage("sendTimer");
        }
        else if (stage == INITSTAGE_APPLICATION_LAYER) {
            // 此时接口表已建立，可以解析名字到地址
            localDest = L3AddressResolver().resolve(par("localDestAddress"));
            remoteDest = L3AddressResolver().resolve(par("remoteDestAddress"));

            // 配置 UDP socket 并绑定输出 gate
            socket.setOutputGate(gate("socketOut"));
            socket.bind(-1); // 发送方不需要固定端口，使用 ephemeral port

            // 启动发送定时器
            scheduleAt(simTime() + interval, timer);
        }
    }

    virtual void handleMessage(cMessage *msg) override {
        // 定时器触发时发送一帧 SV
        if (msg == timer) {
            // 生成带噪声的采样值
            double value = normal(base, noise);
            // 将电流值编码到 packet 名称中（示例性做法，实际可用 payload）
            char name[128];
            sprintf(name, "SV:current=%.6f", value);
            auto packet = new Packet(name);
            // 使用 ByteCountChunk 指定包长（仅用于占位）
            const auto chunk = makeShared<ByteCountChunk>(B(msgLenBytes));
            packet->insertAtBack(chunk);
            // 发送到本地保护（本地复制）和远端保护
            socket.sendTo(packet->dup(), localDest, localPort);
            socket.sendTo(packet, remoteDest, remotePort);
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
    }
};

Define_Module(SvGeneratorApp);
