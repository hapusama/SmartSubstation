import inet.node.inet.StandardHost;
import inet.node.tsn.TsnSwitch;
import inet.node.inet.Router;
import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;

channel EthChannel extends ned.DatarateChannel
{
    delay = 0.1ms;
    datarate = 1Gbps;
}

channel HighPriorityChannel extends ned.DatarateChannel
{
    delay = 0.05ms;
    datarate = 1Gbps;
    @display("ls=green,2");
}

channel WanChannel extends ned.DatarateChannel
{
    delay = 2ms;
    datarate = 100Mbps;
}

// OTN I/II 专用通道：低时延、高确定性，用于承载 SV/GOOSE 业务
channel OTN_I_II_Channel extends ned.DatarateChannel
{
    delay = 0.5ms;  // 低时延
    datarate = 50Mbps;  // I/II 业务带宽预留
    @display("ls=green,3");  // 绿色显示专用通道
}

// OTN III/IV 通用通道：用于视频、运维、VoIP 等业务
channel OTN_III_IV_Channel extends ned.DatarateChannel
{
    delay = 1.5ms;  // 中等时延
    datarate = 50Mbps;  // III/IV 业务带宽
    @display("ls=blue,2");  // 蓝色显示通用通道
}

network SmartSubstationTopology
{
    parameters:
        @display("bgb=30000,18000");

    submodules:
        // 网络配置器
        configurator: Ipv4NetworkConfigurator {
            @display("p=6992.128,1415.3099");
        }

        // A 站设备
        TSN_A: TsnSwitch {
            @display("p=9132.681,9454.612;i=block/switch,blue");
        }
        // A 站接入层 TSN：3 个母线 MU + 1 个综合接入交换机
        TSN_A_ACC_BUS1: TsnSwitch {
            @display("p=5760.875,8133;i=block/switch,blue");
        }
        TSN_A_ACC_BUS2: TsnSwitch {
            @display("p=9454.612,6099.75;i=block/switch,blue");
        }
        TSN_A_ACC_BUS3: TsnSwitch {
            @display("p=7506.081,6760.556;i=block/switch,blue");
        }
        TSN_A_ACC_COMMON: TsnSwitch {
            @display("p=8471.875,14131.087;i=block/switch,orange");
        }
        MU_A: StandardHost {
            @display("p=12013.118,16011.844;i=block/switch,green");
        }
        Protection_A: StandardHost {
            @display("p=10725.394,16672.65;i=block/computer,red");
        }
        IT_A: StandardHost {
            @display("p=12809.475,14910.5;i=block/plug,blue");
        }
        BusProtection_MU1_A: StandardHost {
            @display("p=3134.5938,4845.9126;i=block/switch,yellow");
        }
        BusProtection_MU2_A: StandardHost {
            @display("p=9234.344,3744.5686;i=block/switch,yellow");
        }
        BusProtection_MU3_A: StandardHost {
            @display("p=6065.8623,3964.8374;i=block/switch,yellow");
        }
        BusProtection_A: StandardHost {
            @display("p=12250.331,6946.9375;i=block/computer,orange");
        }
        Camera1_A: StandardHost {
            @display("p=1033.5687,15842.406;i=device/camera,gray");
        }
        Camera2_A: StandardHost {
            @display("p=796.35626,14690.231;i=device/camera,gray");
        }
        Camera3_A: StandardHost {
            @display("p=898.01874,12436.712;i=device/camera,gray");
        }
        Camera4_A: StandardHost {
            @display("p=1033.5687,11267.594;i=device/camera,gray");
        }
        Camera5_A: StandardHost {
            @display("p=2168.8,16672.65;i=device/camera,gray");
        }
        Camera6_A: StandardHost {
            @display("p=1457.1625,10149.306;i=device/camera,gray");
        }
        Camera7_A: StandardHost {
            @display("p=3998.7249,17062.355;i=device/camera,gray");
        }
        Camera8_A: StandardHost {
            @display("p=2829.6062,9556.274;i=device/camera,gray");
        }
        Camera9_A: StandardHost {
            @display("p=7929.675,17333.455;i=device/camera,gray");
        }
        Camera10_A: StandardHost {
            @display("p=9454.612,17553.725;i=device/camera,gray");
        }
        VoIP1_A: StandardHost {
            @display("p=1219.95,13622.774;i=device/phone,gray");
        }
        VoIP2_A: StandardHost {
            @display("p=6218.356,17553.725;i=device/phone,gray");
        }
        MonitoringCenter_A: StandardHost {
            @display("p=11758.962,11572.581;i=block/server,blue");
        }

        // B 站设备（与 A 站对称）
        TSN_B: TsnSwitch {
            @display("p=20095.287,9302.118;i=block/switch,blue");
        }
        // B 站接入层 TSN：3 个母线 MU + 1 个综合接入交换机
        TSN_B_ACC_BUS1: TsnSwitch {
            @display("p=24619.268,10911.774;i=block/switch,blue");
        }
        TSN_B_ACC_BUS2: TsnSwitch {
            @display("p=22348.807,11470.919;i=block/switch,blue");
        }
        TSN_B_ACC_BUS3: TsnSwitch {
            @display("p=19925.85,11470.919;i=block/switch,blue");
        }
        TSN_B_ACC_COMMON: TsnSwitch {
            @display("p=20586.656,6099.75;i=block/switch,orange");
        }
        MU_B: StandardHost {
            @display("p=24144.844,4845.9126;i=block/switch,green");
        }
        Protection_B: StandardHost {
            @display("p=24382.057,3456.525;i=block/computer,red");
        }
        IT_B: StandardHost {
            @display("p=23856.799,6286.1313;i=block/plug,blue");
        }
        BusProtection_MU1_B: StandardHost {
            @display("p=25669.781,12538.375;i=block/switch,yellow");
        }
        BusProtection_MU2_B: StandardHost {
            @display("p=24144.844,13622.774;i=block/switch,yellow");
        }
        BusProtection_MU3_B: StandardHost {
            @display("p=21027.193,13351.675;i=block/switch,yellow");
        }
        BusProtection_B: StandardHost {
            @display("p=25873.105,7014.7124;i=block/computer,orange");
        }
        Camera1_B: StandardHost {
            @display("p=18011.205,7675.5186;i=device/camera,gray");
        }
        Camera2_B: StandardHost {
            @display("p=15842.406,7167.206;i=device/camera,gray");
        }
        Camera3_B: StandardHost {
            @display("p=14910.5,6099.75;i=device/camera,gray");
        }
        Camera4_B: StandardHost {
            @display("p=14910.5,4845.9126;i=device/camera,gray");
        }
        Camera5_B: StandardHost {
            @display("p=14910.5,3609.0188;i=device/camera,gray");
        }
        Camera6_B: StandardHost {
            @display("p=14910.5,2270.4624;i=device/camera,gray");
        }
        Camera7_B: StandardHost {
            @display("p=16503.213,1626.6;i=device/camera,gray");
        }
        Camera8_B: StandardHost {
            @display("p=18367.025,1406.3312;i=device/camera,gray");
        }
        Camera9_B: StandardHost {
            @display("p=19925.85,1863.8125;i=device/camera,gray");
        }
        Camera10_B: StandardHost {
            @display("p=21688,1626.6;i=device/camera,gray");
        }
        VoIP1_B: StandardHost {
            @display("p=24144.844,2236.575;i=device/phone,gray");
        }
        VoIP2_B: StandardHost {
            @display("p=23195.994,1626.6;i=device/phone,gray");
        }
        MonitoringCenter_B: StandardHost {
            @display("p=25873.105,9132.681;i=block/server,blue");
        }

        // 主监控中心
        MonitoringCenter_Main: StandardHost {
            @display("p=14910.5,8793.806;i=block/server,cyan");
        }

        // 广域互联设备
        // 站点边界网关1，连接 I/II 与 III/IV 两类业务的 OTN 通道
        borderGateway1: Router {
            @display("p=13622.774,10149.306");
        }
        // 站点边界网关2
        borderGateway2: Router {
            @display("p=16503.213,10318.743");
        }
        // OTN 核心设备（中间节点），汇聚 I/II 专用与 III/IV 通用通道
        otnDevice: Router {
            @display("p=15418.8125,12758.644");
        }

    connections:
        // A 站综合接入交换机连接 MU、保护、IT、摄像头与 VoIP
        MU_A.ethg++ <--> HighPriorityChannel <--> TSN_A_ACC_COMMON.ethg++;
        Protection_A.ethg++ <--> HighPriorityChannel <--> TSN_A_ACC_COMMON.ethg++;
        IT_A.ethg++ <--> EthChannel <--> TSN_A_ACC_COMMON.ethg++;
        Camera1_A.ethg++ <--> EthChannel <--> TSN_A_ACC_COMMON.ethg++;
        Camera2_A.ethg++ <--> EthChannel <--> TSN_A_ACC_COMMON.ethg++;
        Camera3_A.ethg++ <--> EthChannel <--> TSN_A_ACC_COMMON.ethg++;
        Camera4_A.ethg++ <--> EthChannel <--> TSN_A_ACC_COMMON.ethg++;
        Camera5_A.ethg++ <--> EthChannel <--> TSN_A_ACC_COMMON.ethg++;
        Camera6_A.ethg++ <--> EthChannel <--> TSN_A_ACC_COMMON.ethg++;
        Camera7_A.ethg++ <--> EthChannel <--> TSN_A_ACC_COMMON.ethg++;
        Camera8_A.ethg++ <--> EthChannel <--> TSN_A_ACC_COMMON.ethg++;
        Camera9_A.ethg++ <--> EthChannel <--> TSN_A_ACC_COMMON.ethg++;
        Camera10_A.ethg++ <--> EthChannel <--> TSN_A_ACC_COMMON.ethg++;
        VoIP1_A.ethg++ <--> EthChannel <--> TSN_A_ACC_COMMON.ethg++;
        VoIP2_A.ethg++ <--> EthChannel <--> TSN_A_ACC_COMMON.ethg++;
        TSN_A_ACC_COMMON.ethg++ <--> EthChannel <--> TSN_A.ethg++; // common eth[15] 关键链路承载业务，需配置 GCL

        // A 站母线保护：3 个 MU 分别接入各母线 TSN
        BusProtection_MU1_A.ethg++ <--> EthChannel <--> TSN_A_ACC_BUS1.ethg++;
        BusProtection_MU2_A.ethg++ <--> EthChannel <--> TSN_A_ACC_BUS2.ethg++;
        BusProtection_MU3_A.ethg++ <--> EthChannel <--> TSN_A_ACC_BUS3.ethg++;
        TSN_A_ACC_BUS1.ethg++ <--> EthChannel <--> TSN_A.ethg++;
        TSN_A_ACC_BUS2.ethg++ <--> EthChannel <--> TSN_A.ethg++;
        TSN_A_ACC_BUS3.ethg++ <--> EthChannel <--> TSN_A.ethg++;

        // A 站母线保护与监控中心直接接入 TSN
        BusProtection_A.ethg++ <--> EthChannel <--> TSN_A.ethg++;
        MonitoringCenter_A.ethg++ <--> EthChannel <--> TSN_A.ethg++;// tsn_a eth[5] 关键链路承载业务，需配置 GCL

        // B 站综合接入交换机连接 MU、保护、IT、摄像头与 VoIP
        MU_B.ethg++ <--> HighPriorityChannel <--> TSN_B_ACC_COMMON.ethg++;
        Protection_B.ethg++ <--> HighPriorityChannel <--> TSN_B_ACC_COMMON.ethg++;
        IT_B.ethg++ <--> EthChannel <--> TSN_B_ACC_COMMON.ethg++;
        Camera1_B.ethg++ <--> EthChannel <--> TSN_B_ACC_COMMON.ethg++;
        Camera2_B.ethg++ <--> EthChannel <--> TSN_B_ACC_COMMON.ethg++;
        Camera3_B.ethg++ <--> EthChannel <--> TSN_B_ACC_COMMON.ethg++;
        Camera4_B.ethg++ <--> EthChannel <--> TSN_B_ACC_COMMON.ethg++;
        Camera5_B.ethg++ <--> EthChannel <--> TSN_B_ACC_COMMON.ethg++;
        Camera6_B.ethg++ <--> EthChannel <--> TSN_B_ACC_COMMON.ethg++;
        Camera7_B.ethg++ <--> EthChannel <--> TSN_B_ACC_COMMON.ethg++;
        Camera8_B.ethg++ <--> EthChannel <--> TSN_B_ACC_COMMON.ethg++;
        Camera9_B.ethg++ <--> EthChannel <--> TSN_B_ACC_COMMON.ethg++;
        Camera10_B.ethg++ <--> EthChannel <--> TSN_B_ACC_COMMON.ethg++;
        VoIP1_B.ethg++ <--> EthChannel <--> TSN_B_ACC_COMMON.ethg++;
        VoIP2_B.ethg++ <--> EthChannel <--> TSN_B_ACC_COMMON.ethg++;
        TSN_B_ACC_COMMON.ethg++ <--> EthChannel <--> TSN_B.ethg++;

        // B 站母线保护：3 个 MU 分别接入各母线 TSN
        BusProtection_MU1_B.ethg++ <--> EthChannel <--> TSN_B_ACC_BUS1.ethg++;
        BusProtection_MU2_B.ethg++ <--> EthChannel <--> TSN_B_ACC_BUS2.ethg++;
        BusProtection_MU3_B.ethg++ <--> EthChannel <--> TSN_B_ACC_BUS3.ethg++;
        TSN_B_ACC_BUS1.ethg++ <--> EthChannel <--> TSN_B.ethg++;
        TSN_B_ACC_BUS2.ethg++ <--> EthChannel <--> TSN_B.ethg++;
        TSN_B_ACC_BUS3.ethg++ <--> EthChannel <--> TSN_B.ethg++;

        // B 站母线保护与监控中心直接接入 TSN
        BusProtection_B.ethg++ <--> EthChannel <--> TSN_B.ethg++;
        MonitoringCenter_B.ethg++ <--> EthChannel <--> TSN_B.ethg++;

        // 主监控中心分别连接两站核心 TSN
        TSN_A.ethg++ <--> EthChannel <--> MonitoringCenter_Main.ethg++; // tsn_a eth[6] 关键链路也需启用 TAS
        TSN_B.ethg++ <--> EthChannel <--> MonitoringCenter_Main.ethg++;

        // 跨站广域连接：OTN 架构
        // TSN_A 通过 WAN 连接到边界网关1，进入跨站传输
        TSN_A.ethg++ <--> WanChannel <--> borderGateway1.ethg++;

        // 边界网关1与 OTN 之间分为 I/II 专用通道和 III/IV 通用通道
        // I/II 类（SV/GOOSE）走低时延专用通道
        borderGateway1.pppg++ <--> OTN_I_II_Channel <--> otnDevice.pppg++;
        // III/IV 类（视频/运维/VoIP）走普通通道
        borderGateway1.pppg++ <--> OTN_III_IV_Channel <--> otnDevice.pppg++;

        // OTN 到边界网关2同样区分 I/II 与 III/IV 通道
        // I/II 类专用通道
        otnDevice.pppg++ <--> OTN_I_II_Channel <--> borderGateway2.pppg++;
        // III/IV 类通用通道
        otnDevice.pppg++ <--> OTN_III_IV_Channel <--> borderGateway2.pppg++;

        // TSN_B 通过 WAN 连接到边界网关2
        borderGateway2.ethg++ <--> WanChannel <--> TSN_B.ethg++;
}
