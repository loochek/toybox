#include <utility>
#include <iostream>
#include <vector>

class lol
{
public:
    lol(int, double) {}
    int lollol;

private:
    lol() {}
};

template<typename T, typename... Args>
struct is_constructible
{
    template<typename T_, typename... Args_>
    static decltype(new T_(std::declval<Args_>()...)) kek(const T_ &type_deductor, const Args_&... args_type_deductor); 

    static int kek(...);

    static constexpr bool value_ = std::is_same<int, decltype(kek(std::declval<T>(), std::declval<Args>()...))>::value;
    static constexpr bool value = !value_;
};

int main()
{
    std::cout << is_constructible<int, int>::value << std::endl; // true
    std::cout << is_constructible<lol>::value << std::endl; // false
    std::cout << is_constructible<lol, int, double>::value << std::endl; // true
    std::cout << is_constructible<lol, int, std::vector<int>>::value << std::endl; // false
}
