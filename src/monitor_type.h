#include <stdint.h>
#include <vector>
#include <string>

#include "ylt/struct_json/json_reader.h"
#include "ylt/struct_json/json_writer.h"


enum  StatisticsAttribute 
{
	TOTAL_FLOW = 0x201,				//总流量
	STRAIGHT_TOTAL_FLOW = 0x202,	//直行总流量
	LEFT_TOTAL_FLOW = 0x203,		//左转总流量
	RIGHT_TOTAL_FLOW = 0x204,		//右转总流量
	MAX_QUEUE_LENGTH = 0x205,		//最大排队长度
	MAX_QUEUE_VEHICLE_COUNT = 0x206,//最大排队车辆数
	LANE_FREE_RATIO = 0x207,		//车道空闲率
	LANE_SATURATION_RATE = 0x208,	//车道饱和率
	MAX_RED_START_VEHICLE_COUNT = 0x209,//红初最大车辆数
	GREEN_END_LOSS_RATIO = 0x20A,	//绿灯损失
	GREEN_SIGNAL_RATIO = 0x20B,		//绿信比
};


struct QueryParam
{
    uint32_t beginTime;
    uint32_t endTime;
    int 	queryType;
};
YLT_REFL(QueryParam,beginTime,endTime,queryType);

struct QueryReturnDatas
{
	uint32_t beginTime;	//数据开始时间
	uint32_t endTime;	//数据结束时间
    int		queryType;	//查询返回的数据类型
    std::string desc;	//查询返回的数据描述

	struct Data
	{
		uint32_t timestamp;	//时间戳utc
#if 0
        double value;
#else
		uint64_t value;		//数据值			
#endif
		uint8_t flag = 0;	//数据标记，0表示正常	
	};
	struct Table
	{
		std::string name;		//数据项名称
		std::vector<Data> datas;//数据	
	};

	std::string unit;
	std::vector<Table> tables;
};

struct EventLog
{
	uint32_t time;
	uint8_t  maiEvent;
	uint8_t	 subEvent;

	std::string event;
	std::string detail;
};
YLT_REFL(EventLog,time,maiEvent,subEvent,event,detail);

YLT_REFL(QueryReturnDatas,beginTime,endTime,desc,unit,tables);
YLT_REFL(QueryReturnDatas::Data,timestamp,value,flag);
YLT_REFL(QueryReturnDatas::Table,name,datas);


