#include "eval.h"
#include <algorithm>
#include <assert.h>
#include <cmath>
#include <ostream>
#include <sstream>
#include <stack>
#include <utility>
using namespace std;

value value::operator+(value r) const {
  if (type == inf && r.type == inf)
    return value(0, inf);
  if (type == inf && r.type == normal)
    return value(0, inf);
  if (r.type == inf && type == normal)
    return value(0, inf);

  if (type == ninf && r.type == ninf)
    return value(0, ninf);
  if (type == ninf && r.type == normal)
    return value(0, ninf);
  if (r.type == ninf && type == normal)
    return value(0, ninf);

  if (type == inf && r.type == ninf) // ??
    return value(0, inf);
  if (type == ninf && r.type == inf)
    return value(0, ninf);

  assert(type == normal && r.type == normal);

  return value(v + r.v);
}
value value::operator-() const {
  if (type == inf)
    return value(0, ninf);
  if (type == ninf)
    return value(0, inf);
  return value(-v);
}
value value::operator-(value r) const { return -*this + r; }

value value::operator*(value r) const {
  if (type == inf && r.type == inf)
    return value(0, inf);
  if (type == inf && r.type == normal)
    return value(0, inf);
  if (r.type == inf && type == normal)
    return value(0, inf);

  if (type == ninf && r.type == ninf)
    return value(0, inf);
  if (type == ninf && r.type == normal)
    return value(0, ninf);
  if (r.type == ninf && type == normal)
    return value(0, ninf);

  if (type == inf && r.type == ninf)
    return value(0, ninf);
  if (type == ninf && r.type == inf)
    return value(0, ninf);

  assert(type == normal && r.type == normal);

  return value(v * r.v);
}
value value::operator/(value r) const {
  value one_div;
  if (r.type == inf || r.type == ninf)
    one_div = value(0);
  else
    one_div = value(1 / r.v);
  return *this * one_div;
}
bool value::operator<(value r) const {
  if (type == ninf) {
    if (r.type == ninf)
      return false;
    if (r.type == inf)
      return false;
    if (r.type == normal)
      return true;
  } else if (type == inf) {
    if (r.type == ninf)
      return false;
    if (r.type == inf)
      return false;
    if (r.type == normal)
      return false;
  }
  if (r.type == ninf)
    return false;
  if (r.type == inf)
    return true;
  assert(type == normal && r.type == normal);
  return v < r.v;
}
bool value::operator==(value r) const {
  if (!(type == normal && r.type == normal))
    return false;
  return v == r.v;
}
bool value::operator>(value r) const { return !(*this < r || *this == r); }
bool value::operator>=(value r) const { return !(*this < r); }
bool value::operator<=(value r) const { return !(*this > r); }
bool value::operator!=(value r) const { return !(*this == r); }

expression tokenize(const string &expr) {
  stringstream ss(expr);
  expression e;
  char c;
  while (ss >> c) {
    if (c == 'v') {
      double v;
      ss >> v;
      e.push_back({CONSTANT, v});
    } else if (c == 'a') {
      e.push_back({ADD});
    } else if (c == 's') {
      e.push_back({SUB});
    } else if (c == 'm') {
      e.push_back({MUL});
    } else if (c == 'd') {
      e.push_back({DIV});
    } else if (c == 'p') {
      e.push_back({POW});
    } else if (c == 'l') {
      e.push_back({LOG});
    } else if (c == 'S') {
      e.push_back({SIN});
    } else if (c == 'C') {
      e.push_back({COS});
    } else if (c == 'T') {
      e.push_back({TAN});
    } else if (c == 'x') {
      e.push_back({VAR1});
    } else if (c == 'y') {
      e.push_back({VAR2});
    }
  }
  return e;
}
ostream &operator<<(ostream &ost, value x) {
  if (x.type == value::ninf)
    ost << "-inf";
  else if (x.type == value::inf)
    ost << "inf";
  else
    ost << x.v;
  return ost;
}
ostream &operator<<(ostream &ost, varible x) {
  for (const auto &x : x.ranges) {
    ost << "[" << x.first << "," << x.second << "] ";
  }
  return ost;
}
void varible::sort() {
  ::sort(ranges.begin(), ranges.end());
  if (ranges.size() <= 1)
    return;
  auto it = ranges.begin() + 1;
  auto cur = *ranges.begin();
  decltype(ranges) tmp;
  for (; it != ranges.end(); ++it) {
    if (cur.second >= it->first) {
      cur.second = max(cur.second, it->second);
    } else {
      tmp.push_back(cur);
      cur = *it;
    }
  }
  tmp.push_back(cur);
  ranges = tmp;
}
varible add(varible a, varible b) {
  varible ans;
  ans.ranges.reserve(a.ranges.size() * b.ranges.size());
  for (const auto A : a.ranges) {
    for (const auto B : b.ranges) {
      ans.ranges.push_back({A.first + B.first, A.second + B.second});
    }
  }
  ans.sort();
  return ans;
}
varible neg(varible a) {
  for (auto &x : a.ranges) {
    swap(x.first, x.second);
    x.first = -x.first;
    x.second = -x.second;
  }
  a.sort();
  return a;
}
varible mul(varible a, varible b) {
  varible ans;
  ans.ranges.reserve(a.ranges.size() * b.ranges.size());
  for (const auto A : a.ranges) {
    for (const auto B : b.ranges) {
      auto a = A.first * B.first;
      auto b = A.first * B.second;
      auto c = A.second * B.first;
      auto d = A.second * B.second;
      ans.ranges.push_back(
          {min(min(a, b), min(c, d)), max(max(a, b), max(c, d))});
    }
  }
  ans.sort();
  return ans;
}
varible one_div(varible a) {
  varible ans;
  ans.ranges.reserve(a.ranges.size());
  for (const auto x : a.ranges) {
    value x1 = value(1.0) / x.first;
    value x2 = value(1.0) / x.second;
    if (x.first < 0 && x.second > 0) {
      ans.ranges.push_back(
          make_pair(value(0, value::ninf), value(value(1) / x.first)));
      ans.ranges.push_back(
          make_pair(value(value(1) / x.second), value(0, value::inf)));
    } else if (x.second < 0)
      ans.ranges.push_back(make_pair(x2, x1));
    else if (x.first > 0)
      ans.ranges.push_back(make_pair(x2, x1));
    else if (x.first == 0)
      ans.ranges.push_back(make_pair(x2, value(0, value::inf)));
    else if (x.second == 0)
      ans.ranges.push_back(make_pair(value(0, value::ninf), x1));
  }
  ans.sort();
  return ans;
}
varible div(varible a, varible b) { return mul(a, one_div(b)); }
varible pow(varible a, varible b) {
  varible ans;
  ans.ranges.reserve(a.ranges.size() * b.ranges.size());
  for (const auto A : a.ranges) {
    for (const auto B : b.ranges) {
      value low, high;

      if (A.first.type == value::inf || A.second.type == value::inf) {
        if (B.first.type == value::ninf || B.second.type == value::ninf) {
          low = value(0);
          high = value(0);
        } else if (B.first.type == value::inf || B.second.type == value::inf) {
          low = value(0);
          high = value::inf;
        } else {
          low = value(0);
          high = value::inf;
        }
      } else if (A.first.type == value::ninf || A.second.type == value::ninf) {
        if (B.first.type == value::ninf || B.second.type == value::ninf) {
          low = value(0);
          high = value(0);
        } else if (B.first.type == value::inf || B.second.type == value::inf) {
          low = value(0);
          high = value::inf;
        } else {
          low = value::ninf;
          high = value(0);
        }
      } else {
        auto p1 = std::pow(A.first.v, B.first.v);
        auto p2 = std::pow(A.first.v, B.second.v);
        auto p3 = std::pow(A.second.v, B.first.v);
        auto p4 = std::pow(A.second.v, B.second.v);
        low = value(min(min(p1, p2), min(p3, p4)));
        high = value(max(max(p1, p2), max(p3, p4)));
      }

      ans.ranges.push_back({low, high});
    }
  }
  ans.sort();
  return ans;
}

varible sin(varible a) {
  varible ans;
  ans.ranges.reserve(a.ranges.size());
  for (const auto x : a.ranges) {
    if (x.first.type == value::inf || x.second.type == value::inf ||
        x.first.type == value::ninf || x.second.type == value::ninf) {
      ans.ranges.push_back({value(-1), value(1)});
      continue;
    }
    value low(sin(x.first.v)), high(sin(x.second.v));

    if (low > high)
      swap(low, high);
    {
      // min (-0.5 1.5 3.5 5.5)
      // max (-1.5 0.5 2.5 4.5)
      double l;
      double r;
      l = x.first.v / M_PI;
      r = x.second.v / M_PI;
      l += 0.5;
      r += 0.5;
      l /= 2;
      r /= 2;
      if (r - l >= 1 || ceil(l) == floor(r))
        low = min(low, value(-1));
      l = x.first.v / M_PI;
      r = x.second.v / M_PI;
      l -= 0.5;
      r -= 0.5;
      l /= 2;
      r /= 2;
      if (r - l >= 1 || ceil(l) == floor(r))
        high = max(high, value(1));
    }

    ans.ranges.push_back({low, high});
  }
  ans.sort();
  return ans;
}
varible cos(varible a) {
  varible ans;
  ans.ranges.reserve(a.ranges.size());
  for (const auto x : a.ranges) {
    if (x.first.type == value::inf || x.second.type == value::inf ||
        x.first.type == value::ninf || x.second.type == value::ninf) {
      ans.ranges.push_back({value(-1), value(1)});
      continue;
    }
    value low(cos(x.first.v)), high(cos(x.second.v));
    if (low > high)
      std::swap(low, high);
    {
      // min (-1 1 3 5)
      // max (-2 0 2 4)
      double l, r;
      l = x.first.v / M_PI;
      r = x.second.v / M_PI;
      l += 1, r += 1;
      l /= 2, r /= 2;
      if (r - l >= 1 || ceil(l) == floor(r))
        low = min(low, value(-1));
      l = x.first.v / M_PI;
      r = x.second.v / M_PI;
      l /= 2, r /= 2;
      if (r - l >= 1 || ceil(l) == floor(r))
        high = max(high, value(1));
    }

    ans.ranges.push_back({low, high});
  }
  ans.sort();
  return ans;
}
varible log(varible a) {
  varible ans;
  ans.ranges.reserve(a.ranges.size());
  for (const auto &x : a.ranges) {
    if (x.first.type == value::ninf && x.second.type == value::inf) {
      // log([-inf, inf]) = [-inf, inf]
      ans.ranges.push_back({value(0, value::ninf), value(0, value::inf)});
    } else if (x.first.v <= 0 && x.second.type == value::inf) {
      // log([x, inf]) = [log(x), inf] and x.first <= 0
      ans.ranges.push_back({value(0, value::ninf), value(0, value::inf)});
    } else if (x.first.v <= 0 && x.second.v > 0) {
      // log([a, x]) where a <= 0 = [-inf, log(x)]
      ans.ranges.push_back({value(0, value::ninf), value(log(x.second.v))});
    } else if (x.first.v > 0) {
      // log([a, b]) where a > 0 = [log(a), log(b)]
      if (x.second.type == value::inf)
        ans.ranges.push_back({value(log(x.first.v)), value(0, value::inf)});
      else
        ans.ranges.push_back({value(log(x.first.v)), value(log(x.second.v))});
    }
  }
  ans.sort();
  return ans;
}

varible eval(const expression &e, varible x, varible y) {
  stack<varible> num;
  varible a, b;
  for (auto &i : e) {
    switch (i.op) {
    case ADD:
      b = num.top();
      num.pop();
      a = num.top();
      num.pop();
      num.push(add(a, b));
      break;
    case SUB:
      b = num.top();
      num.pop();
      a = num.top();
      num.pop();
      num.push(add(a, neg(b)));
      break;
    case MUL:
      b = num.top();
      num.pop();
      a = num.top();
      num.pop();
      num.push(mul(a, b));
      break;
    case DIV:
      b = num.top();
      num.pop();
      a = num.top();
      num.pop();
      num.push(mul(a, one_div(b)));
      break;
    case POW:
      b = num.top();
      num.pop();
      a = num.top();
      num.pop();
      num.push(pow(a, b));
      break;
    case LOG:
      b = num.top();
      num.pop();
      a = num.top();
      num.pop();
      num.push(mul(log(a), one_div(log(b))));
      break;
    case SIN:
      a = num.top();
      num.pop();
      num.push(sin(a));
      break;
    case COS:
      a = num.top();
      num.pop();
      num.push(cos(a));
      break;
    case TAN:
      a = num.top();
      num.pop();
      num.push(mul(sin(a), one_div(cos(a))));
      break;
    case VAR1:
      num.push(x);
      break;
    case VAR2:
      num.push(y);
      break;
    case CONSTANT:
      num.push(varible({make_pair(value(i.constant), value(i.constant))}));
      break;
    }
  }
  return num.top();
}
