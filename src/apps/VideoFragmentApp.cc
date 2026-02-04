#include <algorithm>
#include <string>
#include <omnetpp.h>
#include "inet/common/INETDefs.h"
#include "inet/common/InitStages.h"
#include "inet/common/packet/Packet.h"
#include "inet/common/packet/chunk/ByteCountChunk.h"
#include "inet/common/Units.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/transportlayer/contract/udp/UdpSocket.h"

using namespace omnetpp;
using namespace inet;

class VideoFragmentApp : public cSimpleModule
{
  private:
    // UDP套接字，用于发送视频分片
    UdpSocket socket;
    // 触发周期发送的自消息定时器
    cMessage *timer = nullptr;
    // 从参数解析得到的目的地址
    L3Address dest;
    // UDP目的端口
    int destPort = -1;
    // UDP本地端口（可选）
    int localPort = -1;
    // 单个视频帧大小（字节）
    int frameLengthBytes = 0;
    // 分片大小（字节）
    int fragmentLengthBytes = 0;
    // 帧发送间隔
    simtime_t sendInterval;
    // 发送开始/结束时间
    simtime_t startTime;
    simtime_t stopTime;
    // 分片包基础名称
    std::string packetName;
    // QoS的DSCP值
    int dscp = 0;
    // 是否输出详细日志
    bool verbose = false;
    // 递增的帧序号
    long frameSeq = 0;

  protected:
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }

    virtual void initialize(int stage) override {
        cSimpleModule::initialize(stage);
        if (stage == INITSTAGE_LOCAL) {
            // 读取模块参数
            destPort = par("destPort");
            localPort = par("localPort");
            frameLengthBytes = par("frameLength");
            fragmentLengthBytes = par("fragmentLength");
            sendInterval = par("sendInterval");
            startTime = par("startTime");
            stopTime = par("stopTime");
            packetName = par("packetName").stdstringValue();
            dscp = par("dscp");
            verbose = par("verbose");
            // 创建自消息，用于周期触发发送
            timer = new cMessage("videoSendTimer");
        }
        else if (stage == INITSTAGE_APPLICATION_LAYER) {
            // 解析目的地址（只使用第一个地址）
            const char *destStr = par("destAddresses");
            cStringTokenizer tokenizer(destStr);
            const char *first = tokenizer.nextToken();
            if (!first || !*first)
                throw cRuntimeError("destAddresses is empty");
            dest = L3AddressResolver().resolve(first);

            // 初始化套接字并绑定本地端口（若提供）
            socket.setOutputGate(gate("socketOut"));
            socket.bind(localPort >= 0 ? localPort : -1);
            // 将DSCP映射到IPv4的TOS/Traffic Class
            socket.setTos(dscp << 2);

            // 确保开始时间非负
            if (startTime < SIMTIME_ZERO)
                startTime = SIMTIME_ZERO;
            // 安排首次发送
            scheduleAt(startTime, timer);
        }
    }

    void sendFrame() {
        // 校验参数
        if (frameLengthBytes <= 0 || fragmentLengthBytes <= 0)
            throw cRuntimeError("frameLength and fragmentLength must be > 0");

        // remaining: 当前帧剩余未发送的字节数
        int remaining = frameLengthBytes;
        // numFrags: 根据帧大小和分片大小计算总分片数（向上取整）
        int numFrags = (frameLengthBytes + fragmentLengthBytes - 1) / fragmentLengthBytes;
        // fragIndex: 当前分片索引（从0开始）
        int fragIndex = 0;

        // 分片发送：每次循环发送一个分片，直到本帧的所有字节发送完毕
        while (remaining > 0) {
            // 计算本次分片长度（最后一个分片可能不足fragmentLengthBytes）
            int chunkLen = std::min(fragmentLengthBytes, remaining);
            char nameBuf[160];
            // 分片命名：包含帧序号与分片序号，便于日志/调试
            sprintf(nameBuf, "%s frame=%ld frag=%d/%d", packetName.c_str(), frameSeq, fragIndex + 1, numFrags);
            // 创建分片包并添加payload
            auto packet = new Packet(nameBuf);
            packet->setTimestamp(simTime());
            const auto chunk = makeShared<ByteCountChunk>(B(chunkLen));
            packet->insertAtBack(chunk);
            // 立即发送分片（分片之间无时间间隔）
            socket.sendTo(packet, dest, destPort);

            // 更新剩余字节数与分片索引
            remaining -= chunkLen;
            fragIndex++;
        }

        // 本帧全部分片发送完成后再输出日志并递增帧序号
        if (verbose) {
            EV_INFO << "Sent frame=" << frameSeq << " size=" << frameLengthBytes
                    << "B in " << numFrags << " fragments\n";
        }
        frameSeq++;
    }

    virtual void handleMessage(cMessage *msg) override {
        if (msg == timer) {
            // 到达stopTime后停止发送（若配置）
            if (stopTime >= SIMTIME_ZERO && simTime() >= stopTime) {
                return;
            }
            // 发送整帧（分片连续发送），并安排下一次
            sendFrame();
            scheduleAt(simTime() + sendInterval, timer);
        }
        else {
            // 丢弃非预期消息
            delete msg;
        }
    }

    virtual void finish() override {
        // 清理自消息
        cancelAndDelete(timer);
    }
};

Define_Module(VideoFragmentApp);
