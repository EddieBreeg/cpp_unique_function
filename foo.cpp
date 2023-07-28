#include <type_traits>

int main(int argc, char const *argv[]) {
	using T = int (*)();
	using U = std::remove_pointer_t<T>;
	return 0;
}
