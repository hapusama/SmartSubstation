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

class DifferentialProtectionApp : public cSimpleModule, public UdpSocket::ICallback
{
  private:
    UdpSocket socketLocal;
    UdpSocket socketRemote;
    UdpSocket socketGoose;
    double threshold;
    int localPort = -1;
    int remotePort = -1;
    L3Address gooseLocalDest;
    L3Address gooseRemoteDest;
    int goosePort;
    double lastLocal = NAN;
    double lastRemote = NAN;

  protected:
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }

    virtual void initialize(int stage) override {
        cSimpleModule::initialize(stage);
        if (stage == INITSTAGE_LOCAL) {
            localPort = par("localPort");
            remotePort = par("remotePort");
            threshold = par("threshold");
            goosePort = par("goosePort");
        }
        else if (stage == INITSTAGE_APPLICATION_LAYER) {
            gooseLocalDest = L3AddressResolver().resolve(par("gooseDestLocal"));
            gooseRemoteDest = L3AddressResolver().resolve(par("gooseDestRemote"));

            socketLocal.setOutputGate(gate("socketOut"));
            socketLocal.setCallback(this);
            socketLocal.bind(localPort);

            socketRemote.setOutputGate(gate("socketOut"));
            socketRemote.setCallback(this);
            socketRemote.bind(remotePort);

            socketGoose.setOutputGate(gate("socketOut"));
            socketGoose.bind(-1);
        }
    }

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
            delete msg; // defensive: unexpected self-msg or stray packet
        }
    }

    virtual void socketDataArrived(UdpSocket *socket, Packet *packet) override {
        // Parse current from packet name: "SV:current=..."
        std::string name = packet->getName();
        std::smatch m;
        double value = NAN;
        if (std::regex_search(name, m, std::regex("current=([+-]?[0-9]*\\.?[0-9]+)"))) {
            value = std::stod(m[1].str());
        }
        if (socket == &socketLocal) {
            lastLocal = value;
        } else {
            lastRemote = value;
        }
        delete packet;

        if (!std::isnan(lastLocal) && !std::isnan(lastRemote)) {
            double diff = fabs(lastLocal - lastRemote);
            EV_INFO << "Differential |I_local - I_remote| = " << diff << " A, threshold=" << threshold << endl;
            if (diff > threshold) {
                auto goosePkt = new Packet("GOOSE:TripCommand");
                auto chunk = makeShared<ByteCountChunk>(B(64));
                goosePkt->insertAtBack(chunk);
                socketGoose.sendTo(goosePkt->dup(), gooseLocalDest, goosePort);
                socketGoose.sendTo(goosePkt, gooseRemoteDest, goosePort);
            }
        }
    }

    virtual void socketErrorArrived(UdpSocket *socket, Indication *indication) override {
        delete indication;
    }

    virtual void socketClosed(UdpSocket *socket) override {}
};

Define_Module(DifferentialProtectionApp);
