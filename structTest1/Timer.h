#pragma once

#include "Node.h"

struct Timer : Node {
protected:
	friend SceneTree;
	// 位于时间轮的下标
	int indexAtWheel = -1;
	// 双链表
	Timer* timerPrev = nullptr;
	Timer* timerNext = nullptr;
	// 试着从时间轮链表移除
	void RemoveFromWheel();
public:
	using Node::Node;
	~Timer() override;
	// 设置超时( 单位：秒 ). 如果时长小于 1 帧的间隔时长，则至少为 1 帧的间隔时长
	void SetTimeout(float const& seconds);
	void ClearTimeout();
	virtual void Timeout();
};
