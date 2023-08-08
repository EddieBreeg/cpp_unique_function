#include <libstra/static_vector.hpp>
#include <cassert>
#include <iterator>

struct Foo {
	static int copies, instances, moves;
	Foo() { ++instances; };
	~Foo() { --instances; }
};
int Foo::copies = 0, Foo::moves = 0, Foo::instances = 0;

int main(int argc, char const *argv[]) {
	{
		libstra::static_vector<Foo, 10> v;
		assert(!Foo::instances);
	}
	{
		libstra::static_vector<Foo, 10> v(3);
		assert(Foo::instances == 3);
	}
	try {
		libstra::static_vector<Foo, 2> v(3);
		abort();
	} catch (const std::out_of_range &) {
	}
	{
		const libstra::static_vector<int, 2> v(2, 'a');
		for (int x : v) {
			assert(x == (int)'a');
		}
	}
	{
		libstra::static_vector<int, 3> v;
		auto it = v.begin();
		assert(it++ == v.begin());
		assert(--it == v.begin());
		assert(++it == (v.begin() + 1));
		assert(it-- != (v.begin()));
		assert(std::distance(v.begin(), v.end()) ==
			   libstra::extent_v<decltype(v)>);
	}

	assert(Foo::instances == 0);
}
