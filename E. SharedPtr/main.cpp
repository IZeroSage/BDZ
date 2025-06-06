#include "shared_ptr.hpp"
#include <iostream>

int main() {
    SharedPtr<int> b = SharedPtr<int>(new int(322));
    std::cout << "Count b1: " << b.UseCount() << "\n";
    SharedPtr<int> c = b;
    std::cout << "Count b2: " << b.UseCount() << "\n";
    std::cout << "Count c1: " << b.UseCount() << "\n";
}