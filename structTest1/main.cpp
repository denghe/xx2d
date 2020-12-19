#pragma once
#include "SceneTree.h"
#include "Viewport.h"
#include "Node.h"
#include "Signal.h"


/*
实现需求分析：
	发起方 有个接收者映射字典："信号", 接收者( Weak<Node>? )
	接收者 有处理函数映射字典："信号", 处理函数( 函数指针? )
	有字典相关 注册 & 反注册工具函数

	当 Button 发起方 产生 点击 事件时，会发出 "pressed" 信号。
	由于在 映射字典 中找到了相应条目，则常识联系接收方( .Lock() ), 并将信号塞给对方( 调用对方的接收入口函数 )

	如果接收者 已释放( Lock 后为空 ): 从字典移除该条目。 如果接收者 未释放 但未在 tree 中，则保留但不发出 ??
	接收者 收到 "pressed" 后，在 映射字典 中查找 并定位到相应的 处理函数，投递参数并调用。
	如果没有找到相应的处理函数，就算了。

	思考：有没有可能一对多发送? 顺序? 需要返回值来告知是否已 handled 以阻止继续向下传递?

	//std::unordered_map<std::string, xx::Weak<Node>> signalReceivers;
	//typedef void (Node::* SignalHandler)(Signal const& sig);
	//std::unordered_map<std::string, SignalHandler> signalHandlers;
	//void SignalHandle(Signal const& sig);

// godot 示例代码：

func _ready():
	get_node("Button").connect("pressed", self, "_on_Button_pressed")

func _on_Button_pressed():
	get_node("Label").text = "HELLO!"

*/


struct Canvas : Node {
	using Node::Node;
	void Ready() override;
	void Receive(Signal const& sig) override;	// 收到 Button 的 pressed 去改 Label.text
};

struct Button : Node {
	using Node::Node;
	void Process(float delta) override;	// 模拟产生 pressed

	static constexpr const std::string_view SIGNAL_PRESSED = "pressed";
	xx::Weak<Node> pressedReceiver;
	template<typename ...Args>
	void pressedSignalEmit(Args&&... args);

	void Connect(std::string_view const& signalName, xx::Shared<Node> const& receiver);
};

struct Label : Node {
	using Node::Node;
	std::string text;
	void Process(float delta) override;	// 模拟打印 text
};

int main() {
	SceneTree tree;
	{
		auto canvas = tree.CreateNode<Canvas>();
		canvas->name = "Canvas";

		auto button = tree.CreateNode<Button>();
		button->name = "Button";
		canvas->AddChild(button);

		auto label = tree.CreateNode<Label>();
		label->name = "Label";
		canvas->AddChild(label);

		tree.root->AddChild(canvas);
		canvas->PrintTreePretty();
	}
	return tree.MainLoop();
}

void Canvas::Ready() {
	// get_node("Button").connect("pressed", self, "_on_Button_pressed")
	GetNode("Button").As<Button>()->Connect(Button::SIGNAL_PRESSED, this);
}

void Canvas::Receive(Signal const& sig) {
	if (sig.name == Button::SIGNAL_PRESSED) {
		// get_node("Label").text = "HELLO!"
		GetNode("Label").As<Label>()->text = "HELLO!";
	}
}

void Button::Connect(std::string_view const& signalName, xx::Shared<Node> const& receiver) {
	if (signalName == SIGNAL_PRESSED) {
		pressedReceiver = receiver;
	}
}

template<typename ...Args>
void Button::pressedSignalEmit(Args&&... args) {
	if (auto r = pressedReceiver.Lock()) {
		r->Receive({SIGNAL_PRESSED});
	}
}

void Button::Process(float delta) {
	// 模拟点击 产生 pressed 信号
	pressedSignalEmit();
}

void Label::Process(float delta) {
	if (!text.empty()) {
		std::cout << text << std::endl;
		GetNode("../")->Remove();
	}
}
