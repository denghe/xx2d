/*******************************************************************************************************************************/
// type mappings & tmps
/*******************************************************************************************************************************/

namespace xx::Lua {
	std::string tmpStr;

	template<typename T>
	struct PushToFuncs<T, std::enable_if_t<std::is_same_v<xx::XY, std::decay_t<T>>>> {
		static constexpr int checkStackSize = 2;
		static int Push_(lua_State* const& L, T&& in) {
			return Push(L, in.x, in.y);
		}
		static void To_(lua_State* const& L, int const& idx, T& out) {
			To(L, idx, out.x, out.y);
		}
	};

	template<typename T>
	struct PushToFuncs<T, std::enable_if_t<std::is_same_v<xx::RGBA8, std::decay_t<T>>>> {
		static constexpr int checkStackSize = 4;
		static int Push_(lua_State* const& L, T&& in) {
			return Push(L, in.r, in.g, in.b, in.a);
		}
		static void To_(lua_State* const& L, int const& idx, T& out) {
			To(L, idx, out.r, out.g, out.b, out.a);
		}
	};

}
