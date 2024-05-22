#include <iostream>
#include <zoo/swar/SWAR.h>

using u16 = uint16_t;
static_assert(zoo::swar::SWAR<4, u16>{0b0000}.value() == 0);

int main() {
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
