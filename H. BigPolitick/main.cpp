#include <iostream>
#include <vector>
#include <algorithm>

int main() {
  int n = 0;
  int answer = 0;
  std::cin >> n;
  std::vector<int> kings(n);

  for (int i = 0; i < n; ++i) {
    std::cin >> kings[i];
  }

  while (kings.size() > 1) {
    std::vector<int> kings_temp;
    std::sort(kings.begin(), kings.end());
    int temp = kings[0] + kings[1];
    answer += temp;
    kings.push_back(temp);

    for (size_t i = 2; i < kings.size(); ++i) {
      kings_temp.push_back(kings[i]);
    }
    kings = kings_temp;
  }

  std::cout << answer;
  return 0;
}

/*
1 2 3 4 5 6 7 8 9 10

11 11 11 11 11

(11) 22 22

22 33

55



1 2 3 4 5 6 7 8 9 10

3   | 3 3 4 5 6 7 8 9 10
9   | 4 5 6 6 7 8 9 10
18  | 6 6 7 8 9 9 10
30  | 7 8 9 9 10 12
45  | 9 9 10 12 15
63  | 10 12 15 18
85  | 15 18 22
118 | 22 33
173 | 55
*/