#ifndef TRAFFIC_CONFIG_H
#define TRAFFIC_CONFIG_H

#include <stdint.h>
#include <string>
#include <vector>
#include <bitset>
#include <list>


/**
 * 车道方向枚举
 */
enum class LaneDirection {
    EAST = 1,            // 东
    SOUTH = 2,           // 南
    WEST = 3,            // 西
    NORTH = 4,           // 北
    OTHER = 5            // 其他
};

enum class EntryLamp
{
	LP_UNDEF	= 0,	//未定义
	LP_LEFT		= 1,	//左转箭头灯
	LP_STRAIGHT	= 2,	//直行箭头灯
	LP_RIGHT	= 3,	//右转箭头灯
	LP_MAIN		= 4		//圆盘主灯   
};

enum class MANUFACTURER
{
	TSC_HIKVISION		= 0,	//海康威视
	TSC_GB20999			= 1,	//国标20999
	TSC_SINGALCHECK		= 2,	//信号灯检测器
	TSC_OTHER			= 3,	//其他厂家
};

//信号机配置参数
struct TscConfig
{
	uint8_t		type = 0;				//信号机类型 枚举MANUFACTURER
	std::string	ip;						//设备通信IP
	uint16_t	port = 3000;			//设备通信端口
	bool		controlEnable = false;	//控制使能
};

//信号机的在线状态
struct TscOnlineStatus
{
	bool online = false;	//是否在线
	bool hasError = false;	//是否有故障
	std::string  version;	//版本信息
	std::string  desc;	//设备描述信息
};

//检测器配置参数
struct DetectorDevice
{
	uint8_t			id;
	std::string 	desc;
	uint8_t			dir;	//方向 取值 LaneDirection
	std::string 	ip;
};
struct DetectorDeviceStatus
{
	std::string	ip;
	bool		online = false;		//网络联通性
	bool		hasFault = false;	//是否有故障
	uint8_t		event = 0;		//设备事件，0正常，其他异常
};
typedef std::vector<DetectorDevice> DetectorTable;		//检测器配置表
typedef std::vector<DetectorDeviceStatus> DetectorStatusTable;	//检测器状态表

//通道配置参数
struct ChannelCtrl
{
	uint8_t id;			//通道编号
	uint8_t dir;		//方向 取值 LaneDirection
	uint8_t type;		//通道类型 0 = 未定义， 1 = 左转箭头灯，2 = 直行箭头灯， 3 = 右转箭头灯 ， 4 圆盘主灯
};
typedef std::vector<ChannelCtrl> ChannelCtrlTable;		//通道配置表



//通道实时状态参数
struct RtChannel
{
	uint8_t 	id;			//通道编号
	uint8_t 	cur;		//当前的灯色，只会有绿和INVALID 两种状态，以支持强电检测逻辑
	uint16_t	countdown;	//倒计时
	uint16_t	durnation;	// 持续时间 秒
	uint8_t		dir;		//方向 取值 LaneDirection
	uint8_t		type;		//通道类型 0 = 未定义， 1 = 左转箭头灯，2 = 直行箭头灯， 3 = 右转箭头灯 ， 4 圆盘主灯
	uint8_t		status;		//0 未知， 1 绿灯 ，2 红灯 ，3 黄灯

	int 		start_green = 0;	//绿灯的亮起时刻
	int 		end_green = 0;		//绿灯的熄灭时刻
	int 		round_durnation = 0;	//绿间隔

	bool 		ctrl_req;	//控制请求
	bool        ctrl_ret;   //控制结果
};
typedef std::vector<RtChannel> RtChannelTable;		//通道实时状态表

//车道实时状态参数
struct RtLaneInfo
{
	std::string ip;
	int 		laneNo;
	uint8_t 	dir;		//LaneDirection
	uint8_t		turn;		//TurnType 
	uint8_t		channel;	// 关联的通道
							//
	uint8_t		status;		// 关联通道的状态 0 未知， 1 绿灯 ，2 红灯 ，3 黄灯
	uint8_t		carNum;		// 当前车道的车辆数
	uint16_t	queueLength;	// 排队长度
};
typedef std::vector<RtLaneInfo> RtLaneInfoTable;	//车道信息表

enum PssRuleCtrlType
{
    PSS_LOCAL_CONTROL = 0,		//本地时段控制
    PSS_LOCAL_TRIGGER,			//本地触发控制
    PSS_CLIENT_CONTROL,			//客户端控制
    PSS_WEB_CONTROL,			//WEB客户端控制
    PSS_PLATFORM_CONTROL,		//平台控制
};

enum PssRuleCtrlMode
{
    PSS_SYSTEM_MODE = 0,		//系统控制模式
    PSS_SCHEME_MODE,			//方案模式
    PSS_IDLE_MODE,				//空闲模式
};

#define MAX_CHANNEL_SIZE 96
enum StageStatus
{
	STAGE_FINISHED 		= 0,	//已结束
	STAGE_RUNNING		= 1,	//运行中
	STAGE_TRANSITION	= 2		//过渡中
};

struct SignalStage
{
	//std::bitset<MAX_CHANNEL_SIZE> channel = 0;
	//std::string channel;		//关联的通道
	std::vector<uint8_t>		channels;
	int							start;
	int							end_t;

	uint8_t 	stat = 0;		//阶段状态，取值枚举 StageStatus
	uint16_t 	durnation = 0;	//持续的时长
	uint16_t    green;			//绿灯的时长
};
typedef std::list<SignalStage> SignalStageList;

struct PssRule
{
    uint8_t ctrlType = 0;	//控制类型 取值 PssRuleCtrlType
    uint8_t ctrlMode = 0;	//控制模式 取值 PssRuleCtrlMode
    uint8_t	ctrlId = 0;		//控制编号
    uint8_t	triggerId = 0;	//触发编号

    std::string desc;		//规则描述信息
    uint32_t boottime;		//启动时刻点
    uint32_t updatetime;	//更新时刻点
    uint16_t durnation;		//持续时间
    uint16_t left;			//控制剩余时间
};
//汇总的实时状态信息
struct PssRealTimeStatus
{
    uint32_t 		utc;
    PssRule			rule;
    RtLaneInfoTable lanes;
    RtChannelTable  channels;
    SignalStageList stages;
};

//检测器车道和通道的关联信息
struct LaneRelateInfo
{
	std::string ip;			//检测器配置表中的IP,配置时，下拉可选检测器配置表中的检测器选项展示为(desc(ip)) 
	int laneNo;				//车道号，取值范围1-8
	uint8_t	turn;			//车道转向类型，取值 TurnType
	uint8_t	channel;		//可选通道配置表中的通道编号
};
typedef std::vector<LaneRelateInfo> LaneRelateInfoTable;

enum TurnType
{
	TURN_UNKNOW				= 0x00,	//未知  (0000)
	TURN_LEFT				= 0x01,	//左转  (0001)
	TURN_STRAIGHT			= 0x02,	//直行 (0010)
	TURN_RIGHT				= 0x04,	//右转  (0100)
	TURN_LEFT_STRAIGHT		= 0x03,	//左直 (0011)
	TURN_STRAIGHT_RIGHT		= 0x06,	//右直 (0110)
	TURN_LEFT_STRAIGHT_RIGHT = 0x07	//左直右 (0111)
};

/**
 * 数据属性字段枚举
 */
enum class DataAttribute {
    QUEUE_LENGTH = 0,           // 排队长度
    QUEUE_VEHICLE_COUNT,        // 排队车辆数
    TOTAL_FLOW,                 // 车道总流量
    LEFT_TURN_FLOW,             // 左转流量
    STRAIGHT_FLOW,              // 直行流量
    RIGHT_TURN_FLOW,            // 右转流量
    LANE_VEHICLE_COUNT,         // 车道车辆数
    HEAVY_VEHICLE_COUNT,        // 大车数量
    PEDESTRIAN_COUNT,           // 行人数量
    NON_MOTOR_VEHICLE_COUNT,    // 非机动车数量
    FIRST_VEHICLE_DISTANCE,     // 首车距离停止线距离
    MAX_VEHICLE_DISTANCE,       // 最大车间距离
    VEHICLE_COUNT_30M,          // 停止线30米范围内车辆数
    VEHICLE_COUNT_50M,          // 停止线50米范围内车辆数
    VEHICLE_COUNT_80M,          // 停止线80米范围内车辆数
    RED_LIGHT_DURATION,         // 信号灯红灯时长
    GREEN_LIGHT_DURATION,       // 信号灯绿灯时长
    COUNTDOWN                   // 信号灯倒计时
};

/**
 * 返回类型枚举
 */
enum class ReturnType {
    MAX_VALUE,      // 最大值
    MIN_VALUE,      // 最小值
    SUM_VALUE,      // 求和
    AVG_VALUE,      // 平均值
    COUNT_VALUE     // 数据条目数
};

//交通元素信息
struct TrafficAttribute
{
	uint8_t id;
	std::string desc;
	std::string tag;
	
	//数据特征，可以配置为无效值
	std::string dataSourceIP;	//数据源IP，检测配置表中的IP， 空字符串表示为无效值
	int			signalLightId;	//取值通道配置表中的通道编号 有效取值 1-96， 无效时取值 -1 
	int			laneId;		//车道号，取值范围1-8, 无效是取值 -1 
	int			regionId;	//区域编号，取值范围1-8, 无效时取值 -1
	int         laneType;	// 取值范围  1 = 上行，2 = 下行   无效时取值 -1
	uint8_t     laneTurn;	// 转向  按位定义 ， TurnType,    无效时取值 0
	
	//元素数据
	int			attribute = 0;			//DataAttribute
    int 		timeWindowSeconds = 2;	//时间窗口 ，单位秒
    int			returnType;				//返回类型 ， ReturnType
};
typedef std::vector<TrafficAttribute> TrafficAttributeTable;		//交通元素配置表



enum PssAdditionType
{
	AdditionTypeNoUSE = 0,		//不使用 NOT
	AdditionTypeRequired = 1,	//必选  AND
	AdditionTypeOptional = 1,	//可选  OR
	AdditionTypeReplace = 1,	//替换 Replace
	
};

//方案信息
//定义运行模式
enum PssCtrlMode
{
	TscSelfCtrlMode = 0,		//信号机自控
	MotorInductionMode = 1,		//智能感应
	MotorHalfInductionMode = 2,	//智能半感应
	ExpertMode = 3				//专家模式 
};
struct PssScheme
{
	uint8_t id;
	std::string desc;
	uint8_t 	mode;	//控制模式
	
	struct ChannelParam
	{
		uint8_t 		channel = 0;		//通道号
		uint8_t			addition_type = 0;	//取值PssAdditionType
		std::string 	addition_expr ;
		uint8_t 		request_type = 0;	//取值PssAdditionType
		std::string 	request_expr ;
	};
	std::vector<ChannelParam> rules;	//可以配置多个通道的参数
};
typedef std::vector<PssScheme> PssSchemeTable;		//方案配置表



//触发规则信息
struct PssTriger
{
	uint8_t id;
	std::string desc;
	uint8_t prio;	//优先级  0-255
	uint8_t	schemeId;	//0-255 
	std::string expr;	//触发表达式·
};
typedef std::vector<PssTriger> PssTrigerTable;		//触发规则表




#include "include/namedkey.h"
NAMEDKEY(TscConfig);
NAMEDKEY(TscOnlineStatus);
NAMEDKEY(DetectorTable);
NAMEDKEY(DetectorStatusTable);
NAMEDKEY(ChannelCtrlTable);
NAMEDKEY(LaneRelateInfoTable);
NAMEDKEY(TrafficAttributeTable);
NAMEDKEY(PssSchemeTable);
NAMEDKEY(PssTrigerTable);

NAMEDKEY(RtChannelTable);
NAMEDKEY(RtLaneInfoTable);
NAMEDKEY(PssRealTimeStatus);
NAMEDKEY(SignalStageList);

#include "ylt/struct_json/json_reader.h"
#include "ylt/struct_json/json_writer.h"

YLT_REFL(TscConfig,type,ip,port,controlEnable);
YLT_REFL(TscOnlineStatus,online,hasError,version,desc);
YLT_REFL(DetectorDevice,id,desc,dir,ip);
YLT_REFL(DetectorDeviceStatus,ip,online,hasFault,event);
YLT_REFL(ChannelCtrl,id,dir,type);
YLT_REFL(LaneRelateInfo,ip,laneNo,turn,channel);
YLT_REFL(TrafficAttribute,id,desc,tag,dataSourceIP,signalLightId,laneId,regionId,laneType,laneTurn,attribute,timeWindowSeconds,returnType);
YLT_REFL(PssScheme,id,desc,mode,rules);
YLT_REFL(PssScheme::ChannelParam,channel,addition_type,addition_expr,request_type,request_expr);
YLT_REFL(PssTriger,id,desc,prio,schemeId,expr);


YLT_REFL(PssRealTimeStatus,utc,rule,lanes,channels,stages);
YLT_REFL(PssRule,ctrlType,ctrlMode,ctrlId,triggerId,desc,boottime,updatetime,durnation,left);
YLT_REFL(RtLaneInfo,ip,laneNo,dir,turn,channel,status,carNum,queueLength);
YLT_REFL(RtChannel,id,cur,countdown,durnation,dir,type,status,start_green,end_green,round_durnation,ctrl_req,ctrl_ret);
YLT_REFL(SignalStage,channels,start,end_t,stat,durnation,green);







#endif
