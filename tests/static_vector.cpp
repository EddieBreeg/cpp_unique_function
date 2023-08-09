#include <libstra/static_vector.hpp>
#include <cassert>
#include <iterator>

struct Foo {
	static int copies, instances, moves;
	Foo() { ++instances; };
	Foo(const Foo &) {
		++copies;
		++instances;
	}
	Foo(Foo &&) { ++moves; }
	Foo &operator=(Foo &&) { return *this; }
	Foo &operator=(const Foo &) {
		++copies;
		return *this;
	}
	~Foo() { --instances; }
};
int Foo::copies = 0, Foo::moves = 0, Foo::instances = 0;

int main(int argc, char const *argv[]) {
	{
		libstra::static_vector<Foo, 10> v;
		assert(!Foo::instances);
		assert(v.is_empty());
	}
	{
		libstra::static_vector<Foo, 10> v(3);
		assert(Foo::instances == 3);
	}
#ifndef NDEBUG
	try {
		libstra::static_vector<Foo, 2> v(3);
		abort();
	} catch (const std::out_of_range &) {
	}
#endif
	{
		const libstra::static_vector<int, 2> v(2, 'a');
		for (int x : v) {
			assert(x == (int)'a');
		}
	}
	{
		libstra::static_vector<int, 10> v(3, 42);
		assert(v.size() == 3);
		auto it = v.begin();
		assert(it++ == v.begin());
		assert(--it == v.begin());
		assert(++it == (v.begin() + 1));
		assert(it-- != (v.begin()));
		assert(std::distance(v.begin(), v.end()) == v.size());

		auto v2 = v;
		for (size_t i = 0; i < v.size(); i++) {
			assert(v[i] == v2[i]);
		}
	}
	{
		libstra::static_vector<Foo, 10> v(5);
		auto v2 = v;
		assert(Foo::copies == v.size());
		auto v3 = std::move(v);
		assert(v.is_empty() && Foo::moves == v3.size());
	}
	assert(Foo::instances == 0);
	{
		Foo::moves = Foo::copies = 0;
		libstra::static_vector<Foo, 10> v1(5);
		decltype(v1) v2(2);
		v2 = std::move(v1);
		assert(v1.size() == 2 && v2.size() == 5);
	}
	{
		libstra::static_vector<int, 10> v1(5, 42);
		decltype(v1) v2(2);
		v2 = v1;
		assert(v1.size() == v2.size());
		for (int x : v2)
			assert(x == 42);
	}
	{
		constexpr int X[] = { 0, 11, 666 };
		libstra::static_vector<int, 10> v(std::begin(X), std::end(X));
		constexpr auto N = libstra::extent_v<decltype(X)>;
		assert(v.size() == N);
		for (size_t i = 0; i < N; ++i)
			assert(v[i] == X[i]);
	}
	{
		libstra::static_vector<int, 10> v = { 0, 1, 2 };
		assert(v.size() == 3);
		for (size_t i = 0; i < 3; i++)
			assert(v[i] == i);
	}
	{
		libstra::static_vector<int, 2> v;
		auto it = v.push_back(1ULL);
		assert(v.size() == 1 && *it == 1);
	}
	{
		struct A {
			int val = 0;
			A() = default;
			A(int x) : val(x) {}
			A(A &) = delete;
			A(A &&other) : val(other.val) {}
		};
		libstra::static_vector<A, 5> v(1);
		assert(v.front().val == 0);
		v.emplace_back(42);
		assert(v.back().val == 42);
	}
	{
		Foo::instances = 0;
		libstra::static_vector<Foo, 1> v(1);
		v.pop_back();
		assert(!Foo::instances);
	}
	{
		libstra::static_vector<int, 3> v(3, 42);
		assert(*v.data() == 42);
	}
}
