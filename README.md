# Cpp unique_function

## What?

`unique_function` is a generic type that allows you to represent a callable object with a given signature.
Thanks to the magic of type erasure, the underlying callable target can be anything, which is very useful in a lot of cases, like when working with concurrency.

This library also provides the `function_ref` class, a lighter type which only 
references the target callable object, and thus doesn't manage its lifetime at all.

## Why?

I can already hear you argue that the C++ standard already has a similar class: `std::function`.
But `std::function` is sometimes enough because it is required to be copyable. What do you 
do when the callable type you pass in is not? (And yes, it does happen perhaps more often than 
you'd think). For example, the following code is invalid:

```cpp
#include <iostream>
#include <functional>

// A non-copyable class
struct A {
	int val = 0;
	A() = default;
	A(int val) : val(val) {}
	A(const A &) = delete;
	A(A &&o) : val(o.val) {}
};

int main(int argc, char const *argv[]) {
	// this is fine
	auto l = [x = A{}]() mutable { return std::move(x); };

	std::function<A()> f =
		std::move(l); // error: std::function will try to copy l anyway

	std::cout << f().val << '\n';
}
```

On the other hand, this works:
```cpp
#include <iostream>
#include "unique_function.hpp"

// A non-copyable class
struct A {
	int val = 0;
	A() = default;
	A(int val) : val(val) {}
	A(const A &) = delete;
	A(A &&o) : val(o.val) {}
};

int main(int argc, char const *argv[]) {
	// this is fine
	auto l = [x = A{ 1 }]() mutable { return std::move(x); };

	unique_function<A()> f = std::move(l); 

	std::cout << f().val << '\n';
}
```

## How?

I encourage you to simply look at the code, it is not that complicated. If you're not familiar 
with the concepts of type erasure, I strongly recommend that you watch [this talk](https://youtu.be/tbUCHifyT24) because it is what taught me what I needed to know, and also what motivated me
to write this implementation. In fact I suggest you watch the whole back to basics series it is
super informative.