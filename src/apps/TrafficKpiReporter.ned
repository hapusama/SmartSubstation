package src.apps;

//
// TrafficKpiReporter
//
// 作用：
//  - 在仿真结束时，将关键业务 KPI 直接打印到仿真控制台，避免依赖 ANF/SCA 后处理。
//
// 工作机制（自动，无需连接 gate）：
//  - 订阅全网应用层的 packetSent / packetReceived 信号；
//  - 按报文名聚合业务类型（SV、GOOSE、VoIP、Video、OM_Data、Other、ALL）；
//  - 统计每类流量的发送数、接收数、得包率(PDR)、丢包率、平均时延、平均抖动、最大抖动；
//  - 订阅 TSN transmissionGate 的 gateStateChanged，输出门控开关次数与开门时间占比(OpenRatio)。
//
// 输出位置：
//  - 直接输出到 OMNeT++/Qtenv 或 Cmdenv 运行日志（仿真结束时的 summary 段）。
//
// 使用方式：
//  - 在网络拓扑中实例化一个该模块（例如 SmartSubstationTopology 中的 kpiReporter）。
//  - 无需在 ini 中额外连线或配置参数，默认即生效。
//
simple TrafficKpiReporter
{
    parameters:
        @display("i=block/table");
}
