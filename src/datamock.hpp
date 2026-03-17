#include "monitor_type.h"


namespace mock
{
	static QueryReturnDatas getSystemInfo()
	{
		QueryReturnDatas ret;
		return ret;
	}

	// 获取查询数据的接口
	QueryReturnDatas getQueryData(QueryParam &p) {
		QueryReturnDatas data;
		data.beginTime = p.beginTime;
		data.endTime = p.endTime;
		data.queryType = p.queryType;
		
		// 根据查询类型构造不同的数据
		switch (p.queryType) {
			case 0x201: // TOTAL_FLOW 总流量
				data.desc = "总流量统计";
				data.unit = "辆";
				{
					QueryReturnDatas::Table table;
					table.name = "总流量";
					for (uint32_t time = p.beginTime; time <= p.endTime; time += 300) { // 每5分钟一个数据点
						QueryReturnDatas::Data d;
						d.timestamp = time;
						d.value = rand() % 100 + 50; // 50-150之间的随机值
						d.flag = 0;
						table.datas.push_back(d);
					}
					data.tables.push_back(table);
				}
				break;
				
			case 0x202: // STRAIGHT_TOTAL_FLOW 直行总流量
				data.desc = "直行总流量统计";
				data.unit = "辆";
				{
					QueryReturnDatas::Table table;
					table.name = "直行总流量";
					for (uint32_t time = p.beginTime; time <= p.endTime; time += 300) {
						QueryReturnDatas::Data d;
						d.timestamp = time;
						d.value = rand() % 80 + 30; // 30-110之间的随机值
						d.flag = 0;
						table.datas.push_back(d);
					}
					data.tables.push_back(table);
				}
				break;
				
			case 0x203: // LEFT_TOTAL_FLOW 左转总流量
				data.desc = "左转总流量统计";
				data.unit = "辆";
				{
					QueryReturnDatas::Table table;
					table.name = "左转总流量";
					for (uint32_t time = p.beginTime; time <= p.endTime; time += 300) {
						QueryReturnDatas::Data d;
						d.timestamp = time;
						d.value = rand() % 60 + 20; // 20-80之间的随机值
						d.flag = 0;
						table.datas.push_back(d);
					}
					data.tables.push_back(table);
				}
				break;
				
			case 0x204: // RIGHT_TOTAL_FLOW 右转总流量
				data.desc = "右转总流量统计";
				data.unit = "辆";
				{
					QueryReturnDatas::Table table;
					table.name = "右转总流量";
					for (uint32_t time = p.beginTime; time <= p.endTime; time += 300) {
						QueryReturnDatas::Data d;
						d.timestamp = time;
						d.value = rand() % 50 + 15; // 15-65之间的随机值
						d.flag = 0;
						table.datas.push_back(d);
					}
					data.tables.push_back(table);
				}
				break;
				
			case 0x205: // MAX_QUEUE_LENGTH 最大排队长度
				data.desc = "最大排队长度统计";
				data.unit = "米";
				{
					QueryReturnDatas::Table table;
					table.name = "最大排队长度";
					for (uint32_t time = p.beginTime; time <= p.endTime; time += 300) {
						QueryReturnDatas::Data d;
						d.timestamp = time;
						d.value = rand() % 100 + 10; // 10-110之间的随机值
						d.flag = 0;
						table.datas.push_back(d);
					}
					data.tables.push_back(table);
				}
				break;
				
			default:
				data.desc = "未知查询类型";
				data.unit = "";
				break;
		}
		
		return data;
	}

	static QueryReturnDatas getFileSystemnfo(uint32_t begin,uint32_t end)
	{
		QueryReturnDatas ret;
		ret.beginTime = begin;
		ret.endTime		= end;
		ret.desc = "FileSystemInfo";
		ret.unit = "MB";

		std::vector<std::string > paths = {"/dav0","/syslog","/sdcard"};
		int idx = 0;
		for (const auto & p : paths) {
			idx++;
			QueryReturnDatas::Table table; table.name = p;
			uint32_t _step = begin;
			int cnt = 0;
			while (_step < end)  {
				cnt++;
				QueryReturnDatas::Data d;
				d.timestamp = _step;
				d.value = _step % 70 +  cnt  + idx*10;
				d.flag = 0;
				table.datas.push_back(d);

				_step += 15*60;
			}
			//printf("cnt %ld \n",cnt);
			ret.tables.push_back(std::move(table));
		}
		return ret;
	}


}
