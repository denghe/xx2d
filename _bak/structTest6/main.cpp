#include "tiledmap.h"
#include "tiledmap_ajson.h"
#include <xx_string.h>

int main() {
	tiledmap_original::Map m;
	ajson::load_from_file(m, "tiledmap/a.json");

	// todo: 扫 m 序列化为 bin, 生成 m.cs 模板, 进而利用 m.cs 生成 bin 的 reader. 
	// 当前就是 简单的 struct 嵌套结构，但是部分 string 啥的转为 enum / color, 展开 properties 为 properties_xxx
	// 


	return 0;
}




// 部分文档在 tm.cs
//
// 思考:
// 参考网页, 结合工具具体导出的 json, 补齐一些默认值设定
// 
// 未来做成一个独立小工具, 输入 json, 输出 binary + Reader
// Reader 和原始 json 结构类似, 但是做出下列优化:
// 枚举性质的 string 直接以枚举格式存储, 数值性质的 string 直接存为相应数值
// 针对 Property, 直接在 Reader 结构中生成 类 成员, 避免查字典和类型转换
// 针对 颜色 啥的, 存储为 ARGB 专有格式 外部类型
// 
// 对于 Reader, 可给予更多定制空间? 类型映射/替换? 生成物可考虑从某些基类继承, include 抠洞啥的
// 如果 事先写好一批 外部类型, 做好上述优化( 除了 properties ), 所有 vector 就存储其 Shared 版本
// 这样 生成物可以 生成一批 继承自 外部类型 的派生类( 增加 properties 成员 )
// 但是在 Read 数据时, 允许指定 用户类型 替换 上述派生类( 用户类型继承自上述派生类 )
// 针对 id 引用的情况, 也就可以直接存储 Weak 指针了
// 可能需要纠结的问题: 由于连贯内存变指针, 故访问性能可能有所下降. 
// 但是能结合业务逻辑直接跑运行时逻辑, 或附加显示节点, 降低了地图数据和逻辑的双向链接成本
//
// 由于 Property 被 10 多种类型包裹, 理论上讲, 凡是包裹有它的类型, 都需要派生, 并且从 json 读出数据, 分类合并生成.
// 比如 tile1 包含 a = 123, tile2 包含 b = "asdf", 则 TileEx 将包含 int a, string b 两个扩展成员.
// 考虑到其 name 可能和 已有包裹类 重名, 可将其 生成为独立的 struct. 基类可模板化?
//
// 先尝试 ajson 是否能搞定 json 读取. 如果能, 就结合内存数据 做生成器. 生成物 还需要进一步思考, 可以抠出 xx::Data 精简一份, 尽量独立
// 智能指针 做成可映射. 默认映射到 std::shared_ptr, weak_ptr
