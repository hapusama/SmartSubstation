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
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }

    virtual void initialize(int stage) override {
        cSimpleModule::initialize(stage);
        if (stage == INITSTAGE_LOCAL) {
            localPort = par("localDestPort");
            remotePort = par("remoteDestPort");
            base = par("currentBase");
            noise = par("noiseStd");
            msgLenBytes = par("messageLength");
            interval = par("sendInterval");
            timer = new cMessage("sendTimer");
        }
        else if (stage == INITSTAGE_APPLICATION_LAYER) {
            localDest = L3AddressResolver().resolve(par("localDestAddress"));
            remoteDest = L3AddressResolver().resolve(par("remoteDestAddress"));

            socket.setOutputGate(gate("socketOut"));
            socket.bind(-1);

            scheduleAt(simTime() + interval, timer);
        }
    }

    virtual void handleMessage(cMessage *msg) override {
        if (msg == timer) {
            double value = normal(base, noise);
            char name[128];
            sprintf(name, "SV:current=%.6f", value);
            auto packet = new Packet(name);
            const auto chunk = makeShared<ByteCountChunk>(B(msgLenBytes));
            packet->insertAtBack(chunk);
            socket.sendTo(packet->dup(), localDest, localPort);
            socket.sendTo(packet, remoteDest, remotePort);
            scheduleAt(simTime() + interval, timer);
        }
        else {
            delete msg;
        }
    }

    virtual void finish() override {
        cancelAndDelete(timer);
    }
};

Define_Module(SvGeneratorApp);
