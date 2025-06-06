#include <algorithm>
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
      ++words[text];
    } else {
      words[text] = 1;
    }
  }

  std::vector<std::pair<int, std::string>> pairs;
  for (const auto& [bro1, bro2] : words) {
    pairs.emplace_back(bro2, bro1);
  }

  std::sort(pairs.begin(), pairs.end(), [](const auto& a, const auto& b) {
    if (a.first != b.first) {
      return a.first > b.first;
    }
    return a.second < b.second;
  });

  for (auto i : pairs) {
    std::cout << i.second << "\n";
  }

  return 0;
}