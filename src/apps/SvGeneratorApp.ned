package src.apps;

import inet.applications.contract.IApp;

simple SvGeneratorApp like IApp
{
    parameters:
        // localDestAddress: 本地保护设备的模块名或地址（例如 "Protection_A"）
        string localDestAddress;
        // localDestPort: 发送到本地保护的 UDP 端口
        int localDestPort;
        // remoteDestAddress: 对端保护设备的模块名或地址（例如 "Protection_B"）
        string remoteDestAddress;
        // remoteDestPort: 发送到对端保护的 UDP 端口
        int remoteDestPort;
        // currentBase: SV 帧中模拟的基准电流值（单位 A），默认为 100A
        double currentBase @unit(A) = default(100A);
        // noiseStd: 采样噪声的标准差（单位 A），用于 normal() 生成噪声
        double noiseStd @unit(A) = default(1A);
        // sendInterval: 发送间隔（秒），默认 250us（即 4000 帧/秒）
        double sendInterval @unit(s) = default(0.00025s);
        // messageLength: 每帧字节数（字节），用于构造 Packet 的 ByteCountChunk
        int messageLength @unit(B) = default(140B);
        // dscp: 为 SV 报文设置的 DSCP（例如 56=CS7，最高优先级）
        int dscp = default(56);
        // fault injection: 是否在指定时间窗口叠加故障电流（用于触发差动动作）
        bool faultEnabled = default(false);
        double faultStart @unit(s) = default(2s);
        double faultDuration @unit(s) = default(0.02s);
        double faultDelta @unit(A) = default(200A);
        @display("i=block/app");
    gates:
        input socketIn;
        output socketOut;
}
