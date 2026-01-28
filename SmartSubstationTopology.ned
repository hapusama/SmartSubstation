import inet.node.inet.StandardHost;
import inet.node.ethernet.EthernetSwitch;
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

// OTN I/II区专用通道：低延迟、确定性、为SV/GOOSE保留
channel OTN_I_II_Channel extends ned.DatarateChannel
{
    delay = 0.5ms;  // 低延迟
    datarate = 50Mbps;  // I/II区流量占用
    @display("ls=green,3");  // 绿色表示优先通道
}

// OTN III/IV区混跑通道：容纳视频、运维、VoIP等背景流
channel OTN_III_IV_Channel extends ned.DatarateChannel
{
    delay = 1.5ms;  // 中等延迟
    datarate = 50Mbps;  // III/IV区流量共享
    @display("ls=blue,2");  // 蓝色表示混跑通道
}

network SmartSubstationTopology
{
    parameters:
        @display("bgb=13551.221,8415.881");

    submodules:
        // 网络配置器
        configurator: Ipv4NetworkConfigurator {
            @display("p=6992.128,1415.3099");
        }

        // 甲站设备
        TSN_A: EthernetSwitch {
            @display("p=4340.2837,4156.542;i=block/switch,blue");
        }
        // 甲站各类接入TSN（保护、母线SV分三条、视频语音/运维）
        TSN_A_ACC_PROT: EthernetSwitch {
            @display("p=2045.992,2944.838;i=block/switch,blue");
        }
        TSN_A_ACC_BUS1: EthernetSwitch {
            @display("p=1042.86,3828.786;i=block/switch,blue");
        }
        TSN_A_ACC_BUS2: EthernetSwitch {
            @display("p=4042.324,2180.074;i=block/switch,blue");
        }
        TSN_A_ACC_BUS3: EthernetSwitch {
            @display("p=3237.832,2244.6318;i=block/switch,blue");
        }
        TSN_A_ACC_CAM1: EthernetSwitch {
            @display("p=1897.012,5214.3;i=block/switch,blue");
        }
        TSN_A_ACC_CAM2: EthernetSwitch {
            @display("p=5815.186,5795.322;i=block/switch,blue");
        }
        MU_A: StandardHost {
            @display("p=1042.86,2274.428;i=block/switch,green");
        }
        Protection_A: StandardHost {
            @display("p=2165.176,1946.672;i=block/computer,red");
        }
        IT_A: StandardHost {
            @display("p=1464.97,1867.216;i=block/plug,blue");
        }
        BusProtection_MU1_A: StandardHost {
            @display("p=461.83798,3098.784;i=block/switch,yellow");
        }
        BusProtection_MU2_A: StandardHost {
            @display("p=4201.236,1176.942;i=block/switch,yellow");
        }
        BusProtection_MU3_A: StandardHost {
            @display("p=2790.8918,1261.364;i=block/switch,yellow");
        }
        BusProtection_A: StandardHost {
            @display("p=6098.248,2334.02;i=block/computer,orange");
        }
        Camera1_A: StandardHost {
            @display("p=253.26599,5517.226;i=device/camera,gray");
        }
        Camera2_A: StandardHost {
            @display("p=2329.054,6743.8276;i=device/camera,gray");
        }
        Camera3_A: StandardHost {
            @display("p=585.988,4777.292;i=device/camera,gray");
        }
        Camera4_A: StandardHost {
            @display("p=3113.682,5288.79;i=device/camera,gray");
        }
        Camera5_A: StandardHost {
            @display("p=1018.02997,6301.854;i=device/camera,gray");
        }
        Camera6_A: StandardHost {
            @display("p=4767.36,6038.656;i=device/camera,gray");
        }
        Camera7_A: StandardHost {
            @display("p=6694.168,5844.982;i=device/camera,gray");
        }
        Camera8_A: StandardHost {
            @display("p=4881.578,6699.134;i=device/camera,gray");
        }
        Camera9_A: StandardHost {
            @display("p=5522.192,7026.8896;i=device/camera,gray");
        }
        Camera10_A: StandardHost {
            @display("p=6460.766,7170.904;i=device/camera,gray");
        }
        VoIP1_A: StandardHost {
            @display("p=3361.982,6108.1797;i=device/phone,gray");
        }
        VoIP2_A: StandardHost {
            @display("p=6639.542,6356.48;i=device/phone,gray");
        }
        MonitoringCenter_A: StandardHost {
            @display("p=5462.6,1628.848;i=block/server,blue");
        }

        // 乙站设备（整体右移以便区分）
        TSN_B: EthernetSwitch {
            @display("p=11039.418,4350.216;i=block/switch,blue");
        }
        // 乙站各类接入TSN
        TSN_B_ACC_PROT: EthernetSwitch {
            @display("p=10711.662,2060.89;i=block/switch,blue");
        }
        TSN_B_ACC_BUS1: EthernetSwitch {
            @display("p=12102.142,5760.56;i=block/switch,blue");
        }
        TSN_B_ACC_BUS2: EthernetSwitch {
            @display("p=10815.948,5844.982;i=block/switch,blue");
        }
        TSN_B_ACC_BUS3: EthernetSwitch {
            @display("p=9763.156,5795.322;i=block/switch,blue");
        }
        TSN_B_ACC_CAM1: EthernetSwitch {
            @display("p=8760.023,1961.57;i=block/switch,blue");
        }
        TSN_B_ACC_CAM2: EthernetSwitch {
            @display("p=12186.563,2468.102;i=block/switch,blue");
        }
        MU_B: StandardHost {
            @display("p=10642.138,1206.7379;i=block/switch,green");
        }
        Protection_B: StandardHost {
            @display("p=11272.819,1296.126;i=block/computer,red");
        }
        IT_B: StandardHost {
            @display("p=10170.368,1355.718;i=block/plug,blue");
        }
        BusProtection_MU1_B: StandardHost {
            @display("p=12295.815,6977.23;i=block/switch,yellow");
        }
        BusProtection_MU2_B: StandardHost {
            @display("p=10711.662,6942.468;i=block/switch,yellow");
        }
        BusProtection_MU3_B: StandardHost {
            @display("p=9614.176,6892.8076;i=block/switch,yellow");
        }
        BusProtection_B: StandardHost {
            @display("p=12881.804,4201.236;i=block/computer,orange");
        }
        Camera1_B: StandardHost {
            @display("p=7637.708,1584.1539;i=device/camera,gray");
        }
        Camera2_B: StandardHost {
            @display("p=8278.322,1167.01;i=device/camera,gray");
        }
        Camera3_B: StandardHost {
            @display("p=7667.504,2244.6318;i=device/camera,gray");
        }
        Camera4_B: StandardHost {
            @display("p=8665.67,2706.47;i=device/camera,gray");
        }
        Camera5_B: StandardHost {
            @display("p=9529.754,1752.9979;i=device/camera,gray");
        }
        Camera6_B: StandardHost {
            @display("p=11655.202,1877.148;i=device/camera,gray");
        }
        Camera7_B: StandardHost {
            @display("p=13115.206,2845.5178;i=device/camera,gray");
        }
        Camera8_B: StandardHost {
            @display("p=11312.548,2353.884;i=device/camera,gray");
        }
        Camera9_B: StandardHost {
            @display("p=12380.237,3287.492;i=device/camera,gray");
        }
        Camera10_B: StandardHost {
            @display("p=13075.478,2160.21;i=device/camera,gray");
        }
        VoIP1_B: StandardHost {
            @display("p=9008.324,1167.01;i=device/phone,gray");
        }
        VoIP2_B: StandardHost {
            @display("p=12568.945,1738.1;i=device/phone,gray");
        }
        MonitoringCenter_B: StandardHost {
            @display("p=12881.804,4970.966;i=block/server,blue");
        }

        // 主站监控中心
        MonitoringCenter_Main: StandardHost {
            @display("p=8139.274,3630.146;i=block/server,cyan");
        }

        // 跨域网络设备
        // 甲站边界网关：分离I/II与III/IV流量到两条OTN通道
        borderGateway1: Router {
            @display("p=7185.802,4603.482");
        }
        // 乙站边界网关
        borderGateway2: Router {
            @display("p=8804.718,4668.04");
        }
        // OTN设备（中心节点）：管理I/II专用与III/IV混跑通道
        otnDevice: Router {
            @display("p=8084.648,5343.416");
        }

    connections:
        // 甲站关键保护：接入保护接入TSN，再上联核心
        MU_A.ethg++ <--> HighPriorityChannel <--> TSN_A_ACC_PROT.ethg++;
        Protection_A.ethg++ <--> HighPriorityChannel <--> TSN_A_ACC_PROT.ethg++;
        IT_A.ethg++ <--> HighPriorityChannel <--> TSN_A_ACC_PROT.ethg++;
        TSN_A_ACC_PROT.ethg++ <--> EthChannel <--> TSN_A.ethg++;

        // 甲站母线保护：间隔MU接入母线接入TSN，上联核心，再到母线保护装置
        BusProtection_MU1_A.ethg++ <--> EthChannel <--> TSN_A_ACC_BUS1.ethg++;
        BusProtection_MU2_A.ethg++ <--> EthChannel <--> TSN_A_ACC_BUS2.ethg++;
        BusProtection_MU3_A.ethg++ <--> EthChannel <--> TSN_A_ACC_BUS3.ethg++;
        TSN_A_ACC_BUS1.ethg++ <--> EthChannel <--> TSN_A.ethg++;
        TSN_A_ACC_BUS2.ethg++ <--> EthChannel <--> TSN_A.ethg++;
        TSN_A_ACC_BUS3.ethg++ <--> EthChannel <--> TSN_A.ethg++;
        BusProtection_A.ethg++ <--> EthChannel <--> TSN_A.ethg++;

        // 甲站视频/语音/运维：分两个接入TSN汇聚，再上联核心
        Camera1_A.ethg++ <--> EthChannel <--> TSN_A_ACC_CAM1.ethg++;
        Camera2_A.ethg++ <--> EthChannel <--> TSN_A_ACC_CAM1.ethg++;
        Camera3_A.ethg++ <--> EthChannel <--> TSN_A_ACC_CAM1.ethg++;
        Camera4_A.ethg++ <--> EthChannel <--> TSN_A_ACC_CAM1.ethg++;
        Camera5_A.ethg++ <--> EthChannel <--> TSN_A_ACC_CAM1.ethg++;
        VoIP1_A.ethg++ <--> EthChannel <--> TSN_A_ACC_CAM1.ethg++;

        Camera6_A.ethg++ <--> EthChannel <--> TSN_A_ACC_CAM2.ethg++;
        Camera7_A.ethg++ <--> EthChannel <--> TSN_A_ACC_CAM2.ethg++;
        Camera8_A.ethg++ <--> EthChannel <--> TSN_A_ACC_CAM2.ethg++;
        Camera9_A.ethg++ <--> EthChannel <--> TSN_A_ACC_CAM2.ethg++;
        Camera10_A.ethg++ <--> EthChannel <--> TSN_A_ACC_CAM2.ethg++;
        VoIP2_A.ethg++ <--> EthChannel <--> TSN_A_ACC_CAM2.ethg++;
        MonitoringCenter_A.ethg++ <--> EthChannel <--> TSN_A.ethg++;

        TSN_A_ACC_CAM1.ethg++ <--> EthChannel <--> TSN_A.ethg++;
        TSN_A_ACC_CAM2.ethg++ <--> EthChannel <--> TSN_A.ethg++;

        // 乙站关键保护：接入保护接入TSN，再上联核心
        MU_B.ethg++ <--> HighPriorityChannel <--> TSN_B_ACC_PROT.ethg++;
        Protection_B.ethg++ <--> HighPriorityChannel <--> TSN_B_ACC_PROT.ethg++;
        IT_B.ethg++ <--> HighPriorityChannel <--> TSN_B_ACC_PROT.ethg++;
        TSN_B_ACC_PROT.ethg++ <--> EthChannel <--> TSN_B.ethg++;

        // 乙站母线保护：间隔MU接入母线接入TSN，上联核心，再到母线保护装置
        BusProtection_MU1_B.ethg++ <--> EthChannel <--> TSN_B_ACC_BUS1.ethg++;
        BusProtection_MU2_B.ethg++ <--> EthChannel <--> TSN_B_ACC_BUS2.ethg++;
        BusProtection_MU3_B.ethg++ <--> EthChannel <--> TSN_B_ACC_BUS3.ethg++;
        TSN_B_ACC_BUS1.ethg++ <--> EthChannel <--> TSN_B.ethg++;
        TSN_B_ACC_BUS2.ethg++ <--> EthChannel <--> TSN_B.ethg++;
        TSN_B_ACC_BUS3.ethg++ <--> EthChannel <--> TSN_B.ethg++;
        BusProtection_B.ethg++ <--> EthChannel <--> TSN_B.ethg++;

        // 乙站视频/语音/运维：分两个接入TSN汇聚，再上联核心
        Camera1_B.ethg++ <--> EthChannel <--> TSN_B_ACC_CAM1.ethg++;
        Camera2_B.ethg++ <--> EthChannel <--> TSN_B_ACC_CAM1.ethg++;
        Camera3_B.ethg++ <--> EthChannel <--> TSN_B_ACC_CAM1.ethg++;
        Camera4_B.ethg++ <--> EthChannel <--> TSN_B_ACC_CAM1.ethg++;
        Camera5_B.ethg++ <--> EthChannel <--> TSN_B_ACC_CAM1.ethg++;
        VoIP1_B.ethg++ <--> EthChannel <--> TSN_B_ACC_CAM1.ethg++;

        Camera6_B.ethg++ <--> EthChannel <--> TSN_B_ACC_CAM2.ethg++;
        Camera7_B.ethg++ <--> EthChannel <--> TSN_B_ACC_CAM2.ethg++;
        Camera8_B.ethg++ <--> EthChannel <--> TSN_B_ACC_CAM2.ethg++;
        Camera9_B.ethg++ <--> EthChannel <--> TSN_B_ACC_CAM2.ethg++;
        Camera10_B.ethg++ <--> EthChannel <--> TSN_B_ACC_CAM2.ethg++;
        VoIP2_B.ethg++ <--> EthChannel <--> TSN_B_ACC_CAM2.ethg++;
        MonitoringCenter_B.ethg++ <--> EthChannel <--> TSN_B.ethg++;

        // 主站监控中心直连两站核心TSN
        TSN_A.ethg++ <--> EthChannel <--> MonitoringCenter_Main.ethg++;
        TSN_B.ethg++ <--> EthChannel <--> MonitoringCenter_Main.ethg++;

        TSN_B_ACC_CAM1.ethg++ <--> EthChannel <--> TSN_B.ethg++;
        TSN_B_ACC_CAM2.ethg++ <--> EthChannel <--> TSN_B.ethg++;

        // 跨域网络连接（OTN架构）
        // TSN_A主干连接到边界网关1（承载所有流量）
        TSN_A.ethg++ <--> WanChannel <--> borderGateway1.ethg++;
        
        // 边界网关1到OTN：分为I/II专用通道和III/IV混跑通道
        // I/II区（SV/GOOSE）：走专用低延迟通道
        borderGateway1.pppg++ <--> OTN_I_II_Channel <--> otnDevice.pppg++;
        // III/IV区（视频/运维/VoIP）：走混跑通道
        borderGateway1.pppg++ <--> OTN_III_IV_Channel <--> otnDevice.pppg++;
        
        // OTN到边界网关2：同样分为I/II与III/IV两条通道
        // I/II区专用通道
        otnDevice.pppg++ <--> OTN_I_II_Channel <--> borderGateway2.pppg++;
        // III/IV区混跑通道
        otnDevice.pppg++ <--> OTN_III_IV_Channel <--> borderGateway2.pppg++;
        
        // TSN_B主干连接到边界网关2
        borderGateway2.ethg++ <--> WanChannel <--> TSN_B.ethg++;
}
