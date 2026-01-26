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

network SmartSubstationTopology
{
    parameters:
        @display("bgb=3485.8,2479.125");

    submodules:
        // 网络配置器
        configurator: Ipv4NetworkConfigurator {
            @display("p=928.54504,108.18");
        }

        // 甲站设备
        TSN_A: EthernetSwitch {
            @display("p=500,300;i=block/switch,blue");
        }
        // 甲站各类接入TSN（保护、母线SV分三条、视频语音/运维）
        TSN_A_ACC_PROT: EthernetSwitch {
            @display("p=350,210;i=block/switch,blue");
        }
        TSN_A_ACC_BUS1: EthernetSwitch {
            @display("p=280,340;i=block/switch,blue");
        }
        TSN_A_ACC_BUS2: EthernetSwitch {
            @display("p=340,380;i=block/switch,blue");
        }
        TSN_A_ACC_BUS3: EthernetSwitch {
            @display("p=400,340;i=block/switch,blue");
        }
        TSN_A_ACC_CAM1: EthernetSwitch {
            @display("p=320,520;i=block/switch,blue");
        }
        TSN_A_ACC_CAM2: EthernetSwitch {
            @display("p=320,700;i=block/switch,blue");
        }
        MU_A: StandardHost {
            @display("p=260,170;i=block/switch,green");
        }
        Protection_A: StandardHost {
            @display("p=330,150;i=block/computer,red");
        }
        IT_A: StandardHost {
            @display("p=430,200;i=block/plug,blue");
        }
        BusProtection_MU1_A: StandardHost {
            @display("p=220,330;i=block/switch,yellow");
        }
        BusProtection_MU2_A: StandardHost {
            @display("p=280,400;i=block/switch,yellow");
        }
        BusProtection_MU3_A: StandardHost {
            @display("p=440,330;i=block/switch,yellow");
        }
        BusProtection_A: StandardHost {
            @display("p=600,330;i=block/computer,orange");
        }
        Camera1_A: StandardHost {
            @display("p=200,500;i=device/camera,gray");
        }
        Camera2_A: StandardHost {
            @display("p=260,520;i=device/camera,gray");
        }
        Camera3_A: StandardHost {
            @display("p=320,500;i=device/camera,gray");
        }
        Camera4_A: StandardHost {
            @display("p=380,520;i=device/camera,gray");
        }
        Camera5_A: StandardHost {
            @display("p=440,500;i=device/camera,gray");
        }
        Camera6_A: StandardHost {
            @display("p=200,680;i=device/camera,gray");
        }
        Camera7_A: StandardHost {
            @display("p=260,720;i=device/camera,gray");
        }
        Camera8_A: StandardHost {
            @display("p=320,680;i=device/camera,gray");
        }
        Camera9_A: StandardHost {
            @display("p=380,720;i=device/camera,gray");
        }
        Camera10_A: StandardHost {
            @display("p=440,680;i=device/camera,gray");
        }
        VoIP1_A: StandardHost {
            @display("p=520,520;i=device/phone,gray");
        }
        VoIP2_A: StandardHost {
            @display("p=520,700;i=device/phone,gray");
        }
        MonitoringCenter_A: StandardHost {
            @display("p=640,500;i=block/server,blue");
        }

        // 乙站设备（整体右移以便区分）
        TSN_B: EthernetSwitch {
            @display("p=1800,300;i=block/switch,blue");
        }
        // 乙站各类接入TSN
        TSN_B_ACC_PROT: EthernetSwitch {
            @display("p=1650,210;i=block/switch,blue");
        }
        TSN_B_ACC_BUS1: EthernetSwitch {
            @display("p=1580,340;i=block/switch,blue");
        }
        TSN_B_ACC_BUS2: EthernetSwitch {
            @display("p=1640,380;i=block/switch,blue");
        }
        TSN_B_ACC_BUS3: EthernetSwitch {
            @display("p=1700,340;i=block/switch,blue");
        }
        TSN_B_ACC_CAM1: EthernetSwitch {
            @display("p=1620,520;i=block/switch,blue");
        }
        TSN_B_ACC_CAM2: EthernetSwitch {
            @display("p=1620,700;i=block/switch,blue");
        }
        MU_B: StandardHost {
            @display("p=1610,170;i=block/switch,green");
        }
        Protection_B: StandardHost {
            @display("p=1680,150;i=block/computer,red");
        }
        IT_B: StandardHost {
            @display("p=1780,200;i=block/plug,blue");
        }
        BusProtection_MU1_B: StandardHost {
            @display("p=1540,330;i=block/switch,yellow");
        }
        BusProtection_MU2_B: StandardHost {
            @display("p=1600,400;i=block/switch,yellow");
        }
        BusProtection_MU3_B: StandardHost {
            @display("p=1760,330;i=block/switch,yellow");
        }
        BusProtection_B: StandardHost {
            @display("p=1920,330;i=block/computer,orange");
        }
        Camera1_B: StandardHost {
            @display("p=1500,500;i=device/camera,gray");
        }
        Camera2_B: StandardHost {
            @display("p=1560,520;i=device/camera,gray");
        }
        Camera3_B: StandardHost {
            @display("p=1620,500;i=device/camera,gray");
        }
        Camera4_B: StandardHost {
            @display("p=1680,520;i=device/camera,gray");
        }
        Camera5_B: StandardHost {
            @display("p=1740,500;i=device/camera,gray");
        }
        Camera6_B: StandardHost {
            @display("p=1500,680;i=device/camera,gray");
        }
        Camera7_B: StandardHost {
            @display("p=1560,720;i=device/camera,gray");
        }
        Camera8_B: StandardHost {
            @display("p=1620,680;i=device/camera,gray");
        }
        Camera9_B: StandardHost {
            @display("p=1680,720;i=device/camera,gray");
        }
        Camera10_B: StandardHost {
            @display("p=1740,680;i=device/camera,gray");
        }
        VoIP1_B: StandardHost {
            @display("p=1820,520;i=device/phone,gray");
        }
        VoIP2_B: StandardHost {
            @display("p=3481.166,898.846;i=device/phone,gray");
        }
        MonitoringCenter_B: StandardHost {
            @display("p=2964.702,715.104;i=block/server,blue");
        }

        // 主站监控中心
        MonitoringCenter_Main: StandardHost {
            @display("p=1200,500;i=block/server,cyan");
        }

        // 跨域网络设备
        borderGateway1: Router {
            @display("p=1000,1100");
        }
        borderGateway2: Router {
            @display("p=1400,1100");
        }
        otnDevice: Router {
            @display("p=1200,900");
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

        // 跨域网络连接
        TSN_A.ethg++ <--> WanChannel <--> borderGateway1.ethg++;
        borderGateway1.pppg++ <--> WanChannel <--> otnDevice.pppg++;
        otnDevice.pppg++ <--> WanChannel <--> borderGateway2.pppg++;
        borderGateway2.ethg++ <--> WanChannel <--> TSN_B.ethg++;
}
