#include <libstra/memory.hpp>
#include <cassert>
#include <iostream>

void test1() {
	static_assert(libstra::is_pointer<std::shared_ptr<int>>::value,
				  "static pointer test failed");
	static_assert(libstra::is_pointer<std::unique_ptr<int>>::value,
				  "static pointer test failed");
	static_assert(libstra::is_pointer<std::weak_ptr<int>>::value,
				  "static pointer test failed");
	static_assert(libstra::is_pointer<libstra::not_null_ptr<int *>>::value,
				  "static pointer test failed");

	static_assert(!libstra::is_pointer<libstra::not_null_ptr<int>>::value,
				  "static pointer test failed");

	int x = 0;
	libstra::not_null_ptr<int *> ptr{ &x };

	int y = *ptr;
	assert(x == y);
	ptr = &y;

	int *p2 = ptr;
	assert(p2 == ptr);
	assert((int *)0 != ptr);

	auto ptr2 = ptr;
	assert(ptr == ptr2);
}
struct A {
	int x = 0;
};
void test2() {
	libstra::not_null_ptr<std::unique_ptr<A>> p(std::make_unique<A>());
	assert(p->x == 0);
}

int main(int argc, char const *argv[]) {
	test1();
	test2();
}
