#include <algorithm>
#include <iostream>
#include <utility>
#include <vector>
using namespace std;
struct rect {
  ~rect() {}
  pair<int, int> ll, hh;
};
int main() {
  vector<rect> r;
  for (int i = 0; i < 16; ++i)
    for (int j = 0; j < 16; ++j) {
      r.push_back(rect());
      r.back().ll = r.back().hh = {i, j};
    }
  sort(r.begin(), r.end(), [](const rect &a, const rect &b) {
    int base = (1 << 16);
    while (base) {
      // cout << base << endl;
      if ((a.ll.first & base) < (b.ll.first & base))
        return true;
      if ((a.ll.first & base) > (b.ll.first & base))
        return false;
      if ((a.ll.second & base) < (b.ll.second & base))
        return true;
      if ((a.ll.second & base) > (b.ll.second & base))
        return false;
      base >>= 1;
    }
    // return a.ll < b.ll;
    return false;
  });
  for (const auto &u : r) {
    cout << u.ll.first << ' ' << u.ll.second << endl;
  }
}
