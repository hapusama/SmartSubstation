#include <cmath>
#include <map>
#include <set>
#include <string>
#include <vector>
#include <omnetpp.h>
#include "inet/common/INETDefs.h"
#include "inet/common/packet/Packet.h"

using namespace omnetpp;
using namespace inet;

class TrafficKpiReporter : public cSimpleModule, public cListener
{
  private:
    struct FlowStats {
        long sent = 0;
        long received = 0;
        long delaySamples = 0;
        double delaySum = 0;
        long jitterSamples = 0;
        double jitterSum = 0;
        double maxJitter = 0;
        simtime_t lastDelay = SIMTIME_ZERO;
        bool hasLastDelay = false;
    };

    simsignal_t packetSentSignal = cComponent::registerSignal("packetSent");
    simsignal_t packetReceivedSignal = cComponent::registerSignal("packetReceived");
    simsignal_t gateStateChangedSignal = cComponent::registerSignal("gateStateChanged");
    std::map<std::string, FlowStats> stats;

    struct GateStats {
        bool initialized = false;
        bool currentOpen = false;
        simtime_t lastChangeTime = SIMTIME_ZERO;
        simtime_t openTime = SIMTIME_ZERO;
        long openEvents = 0;
        long closeEvents = 0;
    };
    std::map<std::string, GateStats> gateStats;
    std::set<std::string> discoveredTsnSwitches;
    std::set<std::string> discoveredGateModules;

    int tsnSwitchCount = 0;
    int tsnShapingEnabledCount = 0;
    int transmissionGateModuleCount = 0;

    void discoverAndSubscribe(cModule *module) {
        if (module == nullptr)
            return;

        std::string fullPath = module->getFullPath();
        std::string fullName = module->getFullName();
        std::string nedType = module->getNedTypeName();

        if (fullName.rfind("TSN_", 0) == 0 && module->hasPar("hasEgressTrafficShaping")) {
            bool isNewSwitch = discoveredTsnSwitches.insert(fullPath).second;
            if (isNewSwitch) {
                tsnSwitchCount++;
                bool enabled = module->par("hasEgressTrafficShaping").boolValue();
                if (enabled)
                    tsnShapingEnabledCount++;
                EV_INFO << "TSNConfig: switch=" << fullPath
                        << ", hasEgressTrafficShaping=" << (enabled ? "true" : "false") << endl;
            }
        }

        bool isTransmissionGate = (module->hasPar("initiallyOpen") && module->hasPar("durations"))
                      || nedType.find("PeriodicGate") != std::string::npos
                      || fullName.find("transmissionGate") != std::string::npos;
        if (isTransmissionGate) {
            bool isNewGate = discoveredGateModules.insert(fullPath).second;
            if (isNewGate) {
                transmissionGateModuleCount++;
                module->subscribe(gateStateChangedSignal, this);
                bool initiallyOpen = module->hasPar("initiallyOpen") ? module->par("initiallyOpen").boolValue() : false;
                std::string durations = module->hasPar("durations") ? module->par("durations").str() : "[]";
                EV_INFO << "TSNGateConfig: gate=" << fullPath
                        << ", type=" << nedType
                        << ", initiallyOpen=" << (initiallyOpen ? "true" : "false")
                        << ", durations=" << durations
                        << endl;
            }
        }

        for (cModule::SubmoduleIterator it(module); !it.end(); ++it)
            discoverAndSubscribe(*it);
    }

  protected:
    virtual int numInitStages() const override { return INITSTAGE_LAST + 1; }

    virtual void initialize(int stage) override {
        auto systemModule = getSimulation()->getSystemModule();
        if (stage == INITSTAGE_LOCAL) {
            systemModule->subscribe(packetSentSignal, this);
            systemModule->subscribe(packetReceivedSignal, this);
            systemModule->subscribe(gateStateChangedSignal, this);
            discoverAndSubscribe(systemModule);
        }
        else if (stage == INITSTAGE_LAST) {
            discoverAndSubscribe(systemModule);
            EV_INFO << "TSNConfigSummary: tsn_switches=" << tsnSwitchCount
                    << ", shaping_enabled=" << tsnShapingEnabledCount
                    << ", transmissionGate_modules=" << transmissionGateModuleCount << endl;
        }
    }

    virtual void finish() override {
        EV_INFO << "\n========== Traffic KPI Summary ==========" << endl;

        std::vector<std::string> order = {"SV", "GOOSE", "VoIP", "Video", "OM_Data", "Other", "ALL"};
        for (const auto& flow : order) {
            auto it = stats.find(flow);
            if (it == stats.end())
                continue;
            const auto& s = it->second;
            if (s.sent == 0 && s.received == 0)
                continue;

            double pdr = s.sent > 0 ? (100.0 * s.received / s.sent) : 0.0;
            double loss = s.sent > 0 ? (100.0 - pdr) : 0.0;
            double avgDelayMs = s.delaySamples > 0 ? (1000.0 * s.delaySum / s.delaySamples) : 0.0;
            double avgJitterMs = s.jitterSamples > 0 ? (1000.0 * s.jitterSum / s.jitterSamples) : 0.0;
            double maxJitterMs = 1000.0 * s.maxJitter;

                EV_INFO << "TrafficKPI: "
                    << "flow=" << flow
                    << ", sent=" << s.sent
                    << ", recv=" << s.received
                    << ", pdr_pct=" << pdr
                    << ", loss_pct=" << loss
                    << ", avg_delay_ms=" << avgDelayMs
                    << ", avg_jitter_ms=" << avgJitterMs
                    << ", max_jitter_ms=" << maxJitterMs
                    << endl;
        }
        EV_INFO << "=========================================\n" << endl;

        EV_INFO << "\n========== TSN Gate Runtime Summary ==========" << endl;
        EV_INFO << "TSNConfigSummary: tsn_switches=" << tsnSwitchCount
            << ", shaping_enabled=" << tsnShapingEnabledCount
            << ", transmissionGate_modules=" << transmissionGateModuleCount << endl;
        if (gateStats.empty()) {
            EV_INFO << "(no transmissionGate state events observed)" << endl;
            EV_INFO << "Possible reasons: running NoTSN config, hasEgressTrafficShaping=false, or transmissionGate path filter mismatch." << endl;
        }
        for (const auto& item : gateStats) {
            const auto& gatePath = item.first;
            auto gate = item.second;

            if (gate.initialized) {
                simtime_t endTime = simTime();
                if (gate.currentOpen && endTime > gate.lastChangeTime)
                    gate.openTime += endTime - gate.lastChangeTime;
            }

            double openRatio = simTime() > SIMTIME_ZERO ? (100.0 * gate.openTime.dbl() / simTime().dbl()) : 0.0;
        EV_INFO << "GateKPI: "
            << "gate=" << gatePath
            << ", open_events=" << gate.openEvents
            << ", close_events=" << gate.closeEvents
            << ", open_ratio_pct=" << openRatio
            << endl;
        }
        EV_INFO << "==============================================\n" << endl;
    }

    virtual void receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj, cObject *details) override {
        auto packet = dynamic_cast<Packet *>(obj);
        if (packet == nullptr)
            return;

        std::string sourcePath = source->getFullPath();
        if (sourcePath.find(".app[") == std::string::npos)
            return;

        std::string flow = classifyFlow(packet->getName());

        if (signalID == packetSentSignal) {
            stats[flow].sent++;
            stats["ALL"].sent++;
        }
        else if (signalID == packetReceivedSignal) {
            updateReceiveStats(stats[flow], packet);
            updateReceiveStats(stats["ALL"], packet);
        }
    }

    virtual void receiveSignal(cComponent *source, simsignal_t signalID, bool value, cObject *details) override {
        if (signalID != gateStateChangedSignal)
            return;

        std::string sourcePath = source->getFullPath();
        std::string nedType = source->getNedTypeName();
        if (nedType.find("PeriodicGate") == std::string::npos)
            return;

        auto& gate = gateStats[sourcePath];
        simtime_t now = simTime();
        if (!gate.initialized) {
            gate.initialized = true;
            gate.currentOpen = value;
            gate.lastChangeTime = now;
            return;
        }

        if (gate.currentOpen && now > gate.lastChangeTime)
            gate.openTime += now - gate.lastChangeTime;

        if (value)
            gate.openEvents++;
        else
            gate.closeEvents++;

        gate.currentOpen = value;
        gate.lastChangeTime = now;
    }

    std::string classifyFlow(const char *packetName) const {
        std::string name = packetName;
        if (name.find("GOOSE") != std::string::npos)
            return "GOOSE";
        if (name.find("SV") != std::string::npos)
            return "SV";
        if (name.find("VoIP") != std::string::npos)
            return "VoIP";
        if (name.find("Video") != std::string::npos)
            return "Video";
        if (name.find("OM_Data") != std::string::npos)
            return "OM_Data";
        return "Other";
    }

    void updateReceiveStats(FlowStats& flowStats, Packet *packet) {
        flowStats.received++;

        simtime_t sentTime = packet->getTimestamp();
        if (sentTime == SIMTIME_ZERO)
            sentTime = packet->getCreationTime();
        simtime_t delay = simTime() - sentTime;

        flowStats.delaySamples++;
        flowStats.delaySum += delay.dbl();

        if (flowStats.hasLastDelay) {
            double jitter = fabs((delay - flowStats.lastDelay).dbl());
            flowStats.jitterSamples++;
            flowStats.jitterSum += jitter;
            if (jitter > flowStats.maxJitter)
                flowStats.maxJitter = jitter;
        }
        flowStats.lastDelay = delay;
        flowStats.hasLastDelay = true;
    }
};

Define_Module(TrafficKpiReporter);
