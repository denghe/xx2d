#pragma once

inline void Timer::RemoveFromWheel() {
	if (indexAtWheel != -1) {
		if (timerNext != nullptr) {
			timerNext->timerPrev = timerPrev;
		}
		if (timerPrev != nullptr) {
			timerPrev->timerNext = timerNext;
		}
		else {
			tree->wheel[indexAtWheel] = timerNext;
		}
	}
}

inline void Timer::SetTimeout(float const& seconds) {
	// 如果有传入超时时长 则将当前对象挂到时间轮相应节点
	if (seconds != 0) {
		// 试着从 wheel 链表中移除
		RemoveFromWheel();

		// 计算帧间隔( 下标偏移 )
		auto interval = seconds <= tree->secondsPerFrame ? 1 : (size_t)(seconds * tree->frameRate);

		// 帧间隔长度安全检查. 时间轮长度设置过小可能导致无法支持指定时长的超时行为
		if (interval > tree->wheel.size()) {
			throw std::logic_error("Timer SetTimeout out of wheel.size() range");
		}

		// 环形定位到 wheel 元素目标链表下标
		indexAtWheel = (int)((interval + tree->cursor) & (tree->wheel.size() - 1));

		// 放入相应的链表, 成为链表头
		timerPrev = nullptr;
		timerNext = tree->wheel[indexAtWheel];
		tree->wheel[indexAtWheel] = this;

		// 和之前的链表头连起来( 如果有的话 )
		if (timerNext) {
			timerNext->timerPrev = this;
		}
	}
	else {
		ClearTimeout();
	}
}

inline void Timer::ClearTimeout() {
	if (indexAtWheel != -1) {
		RemoveFromWheel();
		indexAtWheel = -1;
		timerPrev = nullptr;
		timerNext = nullptr;
	}
}

inline Timer::~Timer() {
	RemoveFromWheel();
}

void Timer::Timeout() {
	EmitSignal("timeout");
}
