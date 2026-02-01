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
    UdpSocket socket;
    cMessage *timer = nullptr;
    L3Address dest;
    int destPort = -1;
    int localPort = -1;
    int frameLengthBytes = 0;
    int fragmentLengthBytes = 0;
    simtime_t sendInterval;
    simtime_t startTime;
    simtime_t stopTime;
    std::string packetName;
    int dscp = 0;
    bool verbose = false;
    long frameSeq = 0;

  protected:
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }

    virtual void initialize(int stage) override {
        cSimpleModule::initialize(stage);
        if (stage == INITSTAGE_LOCAL) {
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
            timer = new cMessage("videoSendTimer");
        }
        else if (stage == INITSTAGE_APPLICATION_LAYER) {
            const char *destStr = par("destAddresses");
            cStringTokenizer tokenizer(destStr);
            const char *first = tokenizer.nextToken();
            if (!first || !*first)
                throw cRuntimeError("destAddresses is empty");
            dest = L3AddressResolver().resolve(first);

            socket.setOutputGate(gate("socketOut"));
            socket.bind(localPort >= 0 ? localPort : -1);
            socket.setTos(dscp << 2);

            if (startTime < SIMTIME_ZERO)
                startTime = SIMTIME_ZERO;
            scheduleAt(startTime, timer);
        }
    }

    void sendFrame() {
        if (frameLengthBytes <= 0 || fragmentLengthBytes <= 0)
            throw cRuntimeError("frameLength and fragmentLength must be > 0");

        int remaining = frameLengthBytes;
        int numFrags = (frameLengthBytes + fragmentLengthBytes - 1) / fragmentLengthBytes;
        int fragIndex = 0;

        while (remaining > 0) {
            int chunkLen = std::min(fragmentLengthBytes, remaining);
            char nameBuf[160];
            sprintf(nameBuf, "%s frame=%ld frag=%d/%d", packetName.c_str(), frameSeq, fragIndex + 1, numFrags);
            auto packet = new Packet(nameBuf);
            packet->setTimestamp(simTime());
            const auto chunk = makeShared<ByteCountChunk>(B(chunkLen));
            packet->insertAtBack(chunk);
            socket.sendTo(packet, dest, destPort);
            remaining -= chunkLen;
            fragIndex++;
        }

        if (verbose) {
            EV_INFO << "Sent frame=" << frameSeq << " size=" << frameLengthBytes
                    << "B in " << numFrags << " fragments\n";
        }
        frameSeq++;
    }

    virtual void handleMessage(cMessage *msg) override {
        if (msg == timer) {
            if (stopTime >= SIMTIME_ZERO && simTime() >= stopTime) {
                return;
            }
            sendFrame();
            scheduleAt(simTime() + sendInterval, timer);
        }
        else {
            delete msg;
        }
    }

    virtual void finish() override {
        cancelAndDelete(timer);
    }
};

Define_Module(VideoFragmentApp);
