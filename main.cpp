#include <zoo/swar/SWAR.h>

using u16 = uint16_t;
static_assert(zoo::swar::SWAR<4, u16>{0b0000}.value() == 0);

int main() {
    constexpr auto one = zoo::swar::SWAR<4, u16>{0b0001};
    constexpr auto two = zoo::swar::SWAR<4, u16>{0b0010};
    constexpr auto three = zoo::swar::SWAR<4, u16>{0b0011};

    constexpr auto six = one + two + three;
    static_assert(six.value() == 6);

    return 0;
}
