#ifndef REALTIMEMOCK_HPP
#define REALTIMEMOCK_HPP
#include "include/TrafficConfig.h"
namespace mock {
	void getRealtime(PssRealTimeStatus &rts)
    {
        rts.utc = time(NULL);

        auto &rule = rts.rule;
        rule.desc = "测试控制模式";
        rule.ctrlType = PSS_LOCAL_CONTROL;
        rule.ctrlMode = PSS_SCHEME_MODE;
        rule.ctrlId = 2;
        rule.boottime = time(NULL) - 100;
        rule.durnation = 100 + rts.utc % 100;
        rule.left = 999 - rts.utc % 999;

#if 1
        uint32_t _t = time(NULL);
    // 加载通道模拟数据（与车道数据对应）
    RtChannelTable &mockData = rts.channels;
    mockData.clear();
    // 东向：3个通道
    for (int i = 1; i <= 3; ++i) {
        RtChannel ch;
        ch.id = i;
        ch.dir = static_cast<uint8_t>(LaneDirection::EAST);
        ch.type = (i == 1) ? 1 : ((i == 2) ? 2 : 3); // 左转/直行/右转箭头灯
        ch.status = (_t/10 % 3) + 1;
        ch.durnation = _t%30;
        mockData.push_back(ch);
    }
    // 南向：4个通道
    for (int i = 1; i <= 4; ++i) {
        RtChannel ch;
        ch.id = i + 3;
        ch.dir = static_cast<uint8_t>(LaneDirection::SOUTH);
        ch.type = (i == 1) ? 1 : ((i == 4) ? 3 : 2);
        ch.status = ((i + 1) % 3) + 1;
        ch.durnation = _t%25 + i * 5;
        mockData.push_back(ch);
    }
    // 西向：3个通道
    for (int i = 1; i <= 3; ++i) {
        RtChannel ch;
        ch.id = i + 7;
        ch.dir = static_cast<uint8_t>(LaneDirection::WEST);
        ch.type = (i == 1) ? 1 : ((i == 3) ? 3 : 2);
        ch.status = ((i + 2) % 3) + 1;
        ch.durnation = 35 + i * 5;
        mockData.push_back(ch);
    }
    // 北向：4个通道
    for (int i = 1; i <= 4; ++i) {
        RtChannel ch;
        ch.id = i + 10;
        ch.dir = static_cast<uint8_t>(LaneDirection::NORTH);
        ch.type = (i == 1) ? 1 : ((i == 4) ? 3 : 2);
        ch.status = i % 3 + 1;
        ch.durnation = 40 + i * 5;
        mockData.push_back(ch);
    }
#endif

#if 1
    // 加载车道模拟数据
    RtLaneInfoTable &mockLaneData = rts.lanes;
    mockLaneData.clear();
    // 东向：3条车道
    for (int i = 1; i <= 3; ++i) {
        RtLaneInfo lane;
        lane.ip = "192.168.1.101";
        lane.laneNo = i;
        lane.dir = static_cast<uint8_t>(LaneDirection::EAST);
        lane.turn = (i == 1) ? TURN_LEFT : ((i == 2) ? TURN_STRAIGHT : TURN_RIGHT);
        lane.channel = i;
        lane.status = ((_t/10) % 3) + 1;
        lane.carNum = ((_t/10) % 3) + 1;
        mockLaneData.push_back(lane);
    }
    // 南向：4条车道
    for (int i = 1; i <= 4; ++i) {
        RtLaneInfo lane;
        lane.ip = "192.168.1.102";
        lane.laneNo = i;
        lane.dir = static_cast<uint8_t>(LaneDirection::SOUTH);
        lane.turn = (i == 1) ? TURN_LEFT : ((i == 4) ? TURN_RIGHT : TURN_STRAIGHT);
        lane.channel = i + 3;
        lane.status = ((i + 1) % 3) + 1;
        lane.carNum = ((_t/10) % 2) + i;
        mockLaneData.push_back(lane);
    }
    // 西向：3条车道
    for (int i = 1; i <= 3; ++i) {
        RtLaneInfo lane;
        lane.ip = "192.168.1.103";
        lane.laneNo = i;
        lane.dir = static_cast<uint8_t>(LaneDirection::WEST);
        lane.turn = (i == 1) ? TURN_LEFT : ((i == 3) ? TURN_RIGHT : TURN_STRAIGHT);
        lane.channel = i + 7;
        lane.status = ((i + 2) % 3) + 1;
        lane.carNum = i;
        mockLaneData.push_back(lane);
    }
    // 北向：4条车道
    for (int i = 1; i <= 4; ++i) {
        RtLaneInfo lane;
        lane.ip = "192.168.1.104";
        lane.laneNo = i;
        lane.dir = static_cast<uint8_t>(LaneDirection::NORTH);
        lane.turn = (i == 1) ? TURN_LEFT : ((i == 4) ? TURN_RIGHT : TURN_STRAIGHT);
        lane.channel = i + 10;
        lane.status = i % 3 + 1;
        lane.carNum = i + 3;
        mockLaneData.push_back(lane);
    }
#endif
    }
}
#endif // HOSTSDK_HPP
