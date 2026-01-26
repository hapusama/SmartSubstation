package src.apps;

import inet.applications.contract.IApp;

simple DifferentialProtectionApp like IApp
{
    parameters:
        int localPort = default(2000);
        int remotePort = default(2001);
        double threshold @unit(A) = default(5A);
        string gooseDestLocal;
        string gooseDestRemote;
        int goosePort = default(3000);
        @display("i=block/app");
    gates:
        input socketIn;
        output socketOut;
}
