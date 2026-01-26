package src.apps;

import inet.applications.contract.IApp;

simple SvGeneratorApp like IApp
{
    parameters:
        string localDestAddress;
        int localDestPort;
        string remoteDestAddress;
        int remoteDestPort;
        double currentBase @unit(A) = default(100A);
        double noiseStd @unit(A) = default(1A);
        double sendInterval @unit(s) = default(0.00025s);
        int messageLength @unit(B) = default(140B);
        @display("i=block/app");
    gates:
        input socketIn;
        output socketOut;
}
