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
        rule.ctrlMode = PssCtrlMode::PSS_SCHEME_MODE;
        rule.ctrlId = 2;
        rule.boottime = time(NULL) - 100;
        rule.durnation = 100 + rts.utc % 100;
        rule.left = 999 - rts.utc % 999;

#if 1
        uint32_t _t = time(NULL);
    // 加载通道模拟数据（与车道数据对应）
    RtChannelTable &mockData = rts.channels;
    mockData.clear();

    // 东向：通道1-左转箭头，通道2-直行箭头
    {
        RtChannel ch;
        ch.id = 1;
        ch.dir = static_cast<uint8_t>(LaneDirection::EAST);
        ch.type = static_cast<uint8_t>(EntryLamp::LP_LEFT);
        ch.status = (_t/10 % 3) + 1;
        ch.durnation = _t%30;
        mockData.push_back(ch);
    }
    {
        RtChannel ch;
        ch.id = 2;
        ch.dir = static_cast<uint8_t>(LaneDirection::EAST);
        ch.type = static_cast<uint8_t>(EntryLamp::LP_STRAIGHT);
        ch.status = ((_t/10 + 1) % 3) + 1;
        ch.durnation = _t%25 + 5;
        mockData.push_back(ch);
    }

    // 西向：通道3-左转箭头，通道4-直行箭头
    {
        RtChannel ch;
        ch.id = 3;
        ch.dir = static_cast<uint8_t>(LaneDirection::WEST);
        ch.type = static_cast<uint8_t>(EntryLamp::LP_LEFT);
        ch.status = ((_t/10 + 2) % 3) + 1;
        ch.durnation = _t%35;
        mockData.push_back(ch);
    }
    {
        RtChannel ch;
        ch.id = 4;
        ch.dir = static_cast<uint8_t>(LaneDirection::WEST);
        ch.type = static_cast<uint8_t>(EntryLamp::LP_STRAIGHT);
        ch.status = ((_t/10 + 1) % 3) + 1;
        ch.durnation = _t%28 + 10;
        mockData.push_back(ch);
    }

    // 南向：通道5-左转箭头，通道6-直行箭头，通道7-右转箭头
    {
        RtChannel ch;
        ch.id = 5;
        ch.dir = static_cast<uint8_t>(LaneDirection::SOUTH);
        ch.type = static_cast<uint8_t>(EntryLamp::LP_LEFT);
        ch.status = ((_t/10 + 3) % 3) + 1;
        ch.durnation = _t%32;
        mockData.push_back(ch);
    }
    {
        RtChannel ch;
        ch.id = 6;
        ch.dir = static_cast<uint8_t>(LaneDirection::SOUTH);
        ch.type = static_cast<uint8_t>(EntryLamp::LP_STRAIGHT);
        ch.status = ((_t/10 + 1) % 3) + 1;
        ch.durnation = _t%26 + 8;
        mockData.push_back(ch);
    }
    {
        RtChannel ch;
        ch.id = 7;
        ch.dir = static_cast<uint8_t>(LaneDirection::SOUTH);
        ch.type = static_cast<uint8_t>(EntryLamp::LP_RIGHT);
        ch.status = ((_t/10 + 2) % 3) + 1;
        ch.durnation = _t%29 + 12;
        mockData.push_back(ch);
    }

    // 北向：通道8-圆盘主灯
    {
        RtChannel ch;
        ch.id = 8;
        ch.dir = static_cast<uint8_t>(LaneDirection::NORTH);
        ch.type = static_cast<uint8_t>(EntryLamp::LP_MAIN);
        ch.status = _t % 3 + 1;
        ch.durnation = _t%30 + 15;
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

#if 1
    // 加载阶段模拟数据
    SignalStageList &mockStages = rts.stages;
    mockStages.clear();

    // 阶段1：包含通道1,4,7，STAGE_FINISHED
    {
        SignalStage stage;
        stage.channels = {1, 4, 7};
        stage.stat = static_cast<uint8_t>(StageStatus::STAGE_FINISHED);
        stage.durnation = 30;
        stage.green = 30;
        stage.start = 0;
        stage.end_t = 30;
        mockStages.push_back(stage);
    }

    // 阶段2：包含通道1,2,6,7，STAGE_TRANSITION
    {
        SignalStage stage;
        stage.channels = {1, 2, 6, 7};
        stage.stat = static_cast<uint8_t>(StageStatus::STAGE_TRANSITION);
        stage.durnation = 25;
        stage.green = 20;
        stage.start = 30;
        stage.end_t = 55;
        mockStages.push_back(stage);
    }

    // 阶段3：包含通道5,6,7,8，STAGE_RUNNING
    {
        SignalStage stage;
        stage.channels = {5, 6, 7, 8};
        stage.stat = static_cast<uint8_t>(StageStatus::STAGE_RUNNING);
        stage.durnation = 18;
        stage.green = 8;
        stage.start = 55;
        stage.end_t = 73;
        mockStages.push_back(stage);
    }
#endif
    }
}
#endif // HOSTSDK_HPP
