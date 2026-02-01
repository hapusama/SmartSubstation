package src.apps;

import inet.applications.contract.IApp;

simple DifferentialProtectionApp like IApp
{
    parameters:
        // localPort: 本地接收 MU/SV 数据的 UDP 端口
        int localPort = default(2000);
        // remotePort: 用于接收对端 MU/SV 的 UDP 端口
        int remotePort = default(2001);
        // threshold: 差动触发阈值（单位 A），当 |I_local - I_remote| > threshold 时触发
        double threshold @unit(A) = default(5A);
        // gooseDestLocal / gooseDestRemote: GOOSE 三态命令的目的地址（通常为 IT 设备名）
        string gooseDestLocal;
        string gooseDestRemote;
        // goosePort: 发送 GOOSE 的 UDP 端口
        int goosePort = default(3000);
        // gooseDscp: GOOSE 报文的 DSCP 值（默认 48=CS6，次高优先级）
        int gooseDscp = default(48);
        // recordStats: 是否记录端到端时延/抖动指标
        bool recordStats = default(true);
        // strictSlotMatch: 是否严格按时隙标签匹配本地/远端 SV
        bool strictSlotMatch = default(true);
        // maxSlotLag: 允许缓存的最大时隙滞后，用于清理过旧样本
        int maxSlotLag = default(1000);
        @display("i=block/app");
    gates:
        input socketIn;
        output socketOut;
}
