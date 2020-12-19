#pragma once
#include "Ref.h"
#include <variant>

struct Signal/* : Ref<Signal>*/ {
	std::string name;
	// bool 不能弄进去。否则 "asdf" 这种 literal string 会被认为是 bool 类型
	std::vector<std::variant<int32_t, int64_t, float, double, std::string>> args;
	/* ... todo more type */

	Signal() = default;
	template<typename Name, typename ...Args>
	Signal(Name&& name, Args&&... args) {
		this->name = std::forward<Name>(name);
		((this->args.emplace_back(std::forward<Args>(args)), 0), ...);
	}
};
