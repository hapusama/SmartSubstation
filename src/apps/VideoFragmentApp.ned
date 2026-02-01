package src.apps;

import inet.applications.contract.IApp;

simple VideoFragmentApp like IApp
{
    parameters:
        // destAddresses: 目标接收端模块名或地址（只取第一个地址）
        string destAddresses;
        // destPort: 目标 UDP 端口
        int destPort;
        // localPort: 本地 UDP 端口（-1 表示使用临时端口）
        int localPort = default(-1);
        // frameLength: 单帧大小（字节），例如 10MB
        int frameLength @unit(B) = default(10MB);
        // fragmentLength: 单个分片大小（字节），建议小于 MTU
        int fragmentLength @unit(B) = default(1400B);
        // sendInterval: 帧发送间隔
        double sendInterval @unit(s) = default(0.03333s);
        // startTime/stopTime: 发送起止时间
        double startTime @unit(s) = default(0s);
        double stopTime @unit(s) = default(-1s);
        // packetName: 分片报文名前缀
        string packetName = default("Video_Frame");
        // dscp: DSCP 值（0-63）
        int dscp = default(0);
        // verbose: 是否打印简要日志
        bool verbose = default(false);
        @display("i=block/app");
    gates:
        input socketIn;
        output socketOut;
}
