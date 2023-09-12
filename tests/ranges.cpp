#include <libstra/ranges.hpp>

int main(int argc, char const *argv[]) {
	{
		struct A {
			constexpr auto begin() const { return nullptr; }
			constexpr auto end() const { return nullptr; }
		};
		constexpr A x{};
		constexpr auto b = libstra::ranges::begin(x),
					   e = libstra::ranges::end(x);
		static_assert(b == nullptr && e == nullptr, "begin/end test failed");
		static_assert(libstra::ranges::is_range_v<A>, "is_range_v test failed");
	}
	return 0;
}
