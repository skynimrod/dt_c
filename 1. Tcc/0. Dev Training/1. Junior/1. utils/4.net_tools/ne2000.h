/*==============================================*/
//	RTL8019AS基本定义
//	FOR AVR DEV 1.0
//	版本:		1.0
//	作者:		HAN		3366524@QQ.COM
//	日期:		2005年1月3日
/*==============================================*/

/*==============================================*/
//	CP220X net packet define
//	FOR AVR Mega16
//	for little ram, max packet size = all - 1200
//	版本:		1.0
//	作者:		HAN		3366524@QQ.COM
//	日期:		2006年11月10日
/*==============================================*/

#ifndef	_ne2000_h_
#define	_ne2000_h_

typedef unsigned char  INT8U;
typedef unsigned int	INT16U;
typedef unsigned long   INT32U;

struct net_node						//	本机节点信息
{
	INT32U	ip;						//	IP地址
	INT32U	mask;					//	子网掩码
	INT32U	gate;					//	网关
	INT8U	port[2];					//	端口号
	INT8U	mac[6];   				//	以太网地址
};

struct net_nodebytes
{
	INT8U	ipbytes[4];
	INT8U	maskbytes[4];
	INT8U	gatebytes[4];
	INT8U	portbytes[2];
	INT8U	macwords[6];
};

union NetNode{
	struct net_node node;
	struct net_nodebytes nodebytes;
};
	

#define IP4_ADDR(a,b,c,d) ((INT32U)(a & 0xff) << 24) | ((INT32U)(b & 0xff) << 16) | \
                          ((INT32U)(c & 0xff) << 8) | (INT32U)(d & 0xff)

union ethernet_address_type{
	INT8U	words[6];
	INT8U	bytes[6];
};

union ip_address_type{
	INT32U	dwords;
	INT8U	words[4];
	INT8U	bytes[4];
};

union arp_table_type{
	INT8U	bytes[12];
	INT8U	words[12];
	struct{
		INT8U	status;
		INT8U	ttl;
		union ip_address_type ip_address;
		union ethernet_address_type ethernet_address;
	}arp;							//状态,生存时间,ip地址,以太网地址
};

//以太网帧                  
struct ethernet{
	INT8U	status;				//接收状态
	INT8U	nextpage;			//下一个页
	INT8U	length[2];				//以太网长度，以字节为单位
	INT8U	destnodeid[6];		//目的网卡地址
	INT8U	sourcenodeid[6];		//源网卡地址
	INT8U	protocal[2];			//下一层协议
	INT8U	packet[300];		//包的内容
};

//IP包   仅当IHL=5时用 当不=5时作转换
struct ip{
	INT8U	head[18];			//以太网包头
	INT8U	verandihl;			//版本与头长度
	INT8U	typeofserver;		//服务类型
	INT8U	totallength[2];			//总长度
	INT8U	frameindex[2];		//IP帧序号
	INT8U	segment[2];			//分段标志
	INT8U	ttl;					//生存时间
	INT8U	protocal;			//下一层协议
	INT8U	crc[2];				//校验和
	INT8U	sourceip[4];			//源IP
	INT8U	destip[4];			//目的IP
	INT8U	packet[280];		//IP包的内容
};

struct ippacket{
	INT8U	head[18];			//以太网包头
	INT8U	packet[300];		//IP包的内容
};
                
struct arp{
	INT8U	head[18];  			//以太网头
//arp报文的内容总长28字节
	INT16U	harewaretype;		//以太网为0x0001
	INT16U	protocaltype;		//ip 为0X0800
	INT8U	halength;			//=0X06
	INT8U	palength;			//=0X04
	INT16U	operation;			//操作  0X0001为请求 0X0002为应答 0X0003为反向地址请求 0X0004为反向地址应答
	INT8U	sourcenodeid[6];		//源网卡地址
	INT8U	sourceip[4];			//源IP地址
	INT8U	destnodeid[6];		//目的网卡地址
	INT8U	destip[4];			//目的IP地址
}; 

struct icmp{						//包含在IP包中，是IP的上层为0X01的应用
	INT16U	head[9];			//以太网头
	INT16U	iphead[10];			//IP头
	INT8U	type;				//0X08 PING请求 0X00 PING应答
	INT8U	option;				//0X00 PING
	INT16U	crc;      
	INT16U	id;
	INT16U	seq;
	INT8U	icmpdata[272];
};

struct tcp{
	INT16U	head[9];
	INT16U	iphead[10];
	INT16U	sourceport;			//源端口
	INT16U	destport;			//目的端口
	INT16U	seqnumberH;		//顺序号
	INT16U	seqnumberL;
	INT16U	acknumberH;
	INT16U	acknumberL;		//确认号
	INT8U	offset;				//数据偏移量
	INT8U	control;				//连接控制
	INT16U	window;			//流控
	INT16U	crc;				//校验和 ，包括伪头部，TCP头部，数据
	INT16U	urg;				//紧急指针
	INT8U	tcpdata[260];		//TCP数据
};

struct udp{
	INT16U	head[9];
	INT16U	iphead[10];
	INT16U	sourceport;		//源端口
	INT16U	destport;			//目的端口
	INT16U	length;    
	INT16U	crc;				//校验和 ，包括伪头部，udp头部，数据
	INT8U	udpdata[272];	//udp数据
};

struct dhcp {
	INT16U  head[23];
	INT8U   op;				// 若是client 送给server的封包， 设为1，反向为2
	INT8U   htype;			// 硬体类别， Ethernet 为1
	INT8U   hlen;			// 硬体位址长度， Ethernet 为6
	INT8U	hops;			// 若封包需经过router  传送， 每站加1，若在同一网内， 为0
	INT32U	transactionID;	// DHCP request 时产生的数值， 以作DHCP reply 时的依据
	INT16U	seconds;		// client 端启动时间(秒)
	INT16U	flags;			// 从0到15共16 bits， 最左一bit  为1 时， 表示server 将以广播方式传送封包给client， 其余尚未使用
	INT32U	ciaddr;			// 要是client 端想继续使用之前取得的IP地址， 则列于这里
	INT32U	yiaddr;			// 从server 送回client的 DHCP offer 与 DHCP ack 封包中， 此栏填写分配给client 的IP地址
	INT32U	siaddr;			// 若client 需要透过网路开机， 从server 送出的DHCP offer、DHCP ack、DHCP nack 封包中， 此栏填写开机程式码所在server 的位置
	INT32U	giaddr;			// 若需跨网域进行DHCP 发放, 此栏为replayagent 的位置, 否则为0
	INT16U	chaddr;			// Client 的 硬体位址
	INT8U	sname[8];		// Server 的名称字串， 以0x00 结尾
	INT8U	file[16];		// 若 client 需要透过网络开机， 此栏将指出开机程序名称, 稍后以TFTP传送.
	INT8U	options[39];	// 允许厂商定义选项(Vendor-SpecificArea), 以提供更多的设定资讯(如: Netmask、Gateway、DNS、等等)。 其长度可变，同时可携带多个选项, 每一选项的第一个byte 为资讯代码, 其后一个byte 为该项资料长度，最后为项目内容.  完全兼容BOOTP
};

struct tftprw{
	INT16U	head[9];
	INT16U	iphead[10];
	INT16U	udphead[4];
	INT16U	opencode;		//	读写代码标志
	INT8U	fileandmode[270];	//	文件名和操作类型
};

struct tftpd{
	INT16U	head[9];
	INT16U	iphead[10];
	INT16U	udphead[4];
	INT16U	opencode;		//	读写代码标志
	INT16U	number;			//	块编号或者差错码
	INT8U	tftpdata[268];	//	数据或者差错信息
};

struct bytes{
	INT8U	bytebuf[336];
};
             
//所有协议的共用体
union netcard{
	struct bytes bytedata;
	struct ethernet etherframe;	
	struct arp arpframe;
	struct icmp icmpframe;
	struct tcp tcpframe;  
	struct ip ipframe;
	struct udp udpframe;
	struct ippacket ippacket;
	struct tftprw tftprwframe;
	struct tftpd tftpdataframe;
	struct dhcppacket dhcpframe;
};

typedef struct
{
	union ip_address_type	ipaddr;
	union ip_address_type	his_sequence;
	union ip_address_type	my_sequence;
	union ip_address_type	old_sequence;
	union ip_address_type	his_ack;
	INT16U	port;
	INT8U	timer;
	INT8U	inactivity;	 
	INT8U	state;
	INT8U	query[23];
}tcpconnection;

#define IP_PACKET           0x0008
#define ARP_PACKET          0x0608
#define RARP_PACKET			0x3580

#define	MaxLenARPtable		0x02

#endif
