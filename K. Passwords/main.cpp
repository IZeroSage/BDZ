#include <iostream>
#include <vector>
#include <string>

int main() {
  int n = 0;
  std::cin >> n;
  std::vector<std::string> passwords(n);
  for (int i = 0; i < n; ++i) {
    std::cin >> passwords[i];
  }

  int count = 0;
  for (int i = 0; i < n; ++i) {
    for (int k = 0; k < n; ++k) {
      if ((k != i) && (passwords[k].find(passwords[i]) != std::string::npos)) {
        ++count;
      }
    }
  }
  std::cout << count;

  return 0;
}