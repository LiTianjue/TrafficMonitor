#ifndef MOCK_HPP
#define MOCK_HPP

#include <string>
#include "ylt/struct_json/json_writer.h"
#include "TrafficConfig.h"

namespace mock {

template<typename T>
std::string load() {
    // 默认实现返回空的json对象
    return "{}";
}

// TscOnlineStatus 的特化实现
template<>
std::string load<TscOnlineStatus>() {
    TscOnlineStatus status;
    status.online = true;
    status.hasError = false;
    status.version = "V0.0.1";
    status.desc = "欢迎使用";
    std::string result;
    struct_json::to_json(status, result);
    return result;
}

// DetectorStatusTable 的特化实现
template<>
std::string load<std::vector<DetectorDeviceStatus>>() {
    std::vector<DetectorDeviceStatus> table;
    
    // 创建4组数据
    for (int i = 0; i < 4; ++i) {
        DetectorDeviceStatus status;
        status.ip = "10.11.96." + std::to_string(157 + i);
        status.online = true;
        status.hasFault = false;
        status.event = 0;
        
        table.push_back(status);
    }
    
    std::string result;
    struct_json::to_json(table, result);
    return result;
}

// EventLog 向量的特化实现
template<>
std::string load<std::vector<EventLog>>() {
    std::vector<EventLog> logs;
    
    // 创建几个示例事件日志
    for (int i = 0; i < 5; ++i) {
        EventLog log;
        log.time = 1700000000 + i * 3600; // 每小时一个事件
        log.maiEvent = 1;
        log.subEvent = i + 1;
        
        switch (i) {
            case 0:
                log.event = "系统启动";
                log.detail = "交通监控系统成功启动";
                break;
            case 1:
                log.event = "设备连接";
                log.detail = "检测器设备 10.11.96.157 连接成功";
                break;
            case 2:
                log.event = "数据采集";
                log.detail = "开始采集车道流量数据";
                break;
            case 3:
                log.event = "信号灯状态";
                log.detail = "信号灯切换为绿灯";
                break;
            case 4:
                log.event = "报警事件";
                log.detail = "检测到排队长度超过阈值";
                break;
        }
        
        logs.push_back(log);
    }
    
    std::string result;
    struct_json::to_json(logs, result);
    return result;
}

} // namespace mock

#endif // MOCK_HPP