#include <iostream>
#include <fstream>
#include <vector>
#include <map>

int main() {
  std::map<std::string, int> words;
  std::ifstream file("input.txt", std::ios::in);
  std::string text;
  while (file >> text) {
    if (words.find(text) != words.end()) {
      std::cout << words[text] << " ";
      ++words[text];
    } else {
      std::cout << 0 << " ";
      words[text] = 1;
    }
  }
  return 0;
}