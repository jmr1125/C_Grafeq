#include "eval.h"
#include <algorithm>
#include <assert.h>
#include <cmath>
#include <memory>
#include <ostream>
#include <sstream>
#include <stack>
#include <utility>
#include <vector>
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
    return value(0, r.v > 0 ? inf : ninf);
  if (r.type == inf && type == normal)
    return value(0, v > 0 ? inf : ninf);

  if (type == ninf && r.type == ninf)
    return value(0, inf);
  if (type == ninf && r.type == normal)
    return value(0, r.v > 0 ? ninf : inf);
  if (r.type == ninf && type == normal)
    return value(0, v > 0 ? ninf : inf);

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
      return true;
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
  expression ex;
  vector<shared_ptr<_expr>> &e = ex.exprs;
  stack<int> num;
  char c;
  for (int i = 0; ss >> c; ++i) {
    if (c == 'v') {
      double v;
      ss >> v;
      e.push_back(make_shared<CONSTANT>(v));
    } else if (c == 'a') {
      int l = num.top();
      num.pop();
      int r = num.top();
      num.pop();
      e.push_back(make_shared<ADD>(l, r));
    } else if (c == 's') {
      int l = num.top();
      num.pop();
      int r = num.top();
      num.pop();
      e.push_back(make_shared<SUB>(l, r));
    } else if (c == 'm') {
      int l = num.top();
      num.pop();
      int r = num.top();
      num.pop();
      e.push_back(make_shared<MUL>(l, r));
    } else if (c == 'd') {
      int l = num.top();
      num.pop();
      int r = num.top();
      num.pop();
      e.push_back(make_shared<DIV>(l, r));
    } else if (c == 'p') {
      int l = num.top();
      num.pop();
      int r = num.top();
      num.pop();
      e.push_back(make_shared<POW>(l, r));
    } else if (c == 'l') {
      int x = num.top();
      num.pop();
      e.push_back(make_shared<LOG>(x));
    } else if (c == 'S') {
      int x = num.top();
      num.pop();
      e.push_back(make_shared<SIN>(x));
    } else if (c == 'C') {
      int x = num.top();
      num.pop();
      e.push_back(make_shared<COS>(x));
    } else if (c == 'T') {
      int x = num.top();
      num.pop();
      e.push_back(make_shared<TAN>(x));
    } else if (c == 'x') {
      e.push_back(make_shared<VAR1>());
    } else if (c == 'y') {
      e.push_back(make_shared<VAR2>());
    }
    else {
      throw "";
    }
    num.push(i);
  }
  return ex;
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
  if (x.has_undefined)
    ost << "has undefined";
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
  ans.has_undefined = (a.has_undefined || b.has_undefined);
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
  ans.has_undefined = (a.has_undefined || b.has_undefined);
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
      ans.has_undefined = (ans.has_undefined || true);
    } else if (x.second < 0)
      ans.ranges.push_back(make_pair(x2, x1));
    else if (x.first > 0)
      ans.ranges.push_back(make_pair(x2, x1));
    else if (x.first == 0)
      ans.ranges.push_back(make_pair(x2, value(0, value::inf))),
          ans.has_undefined = (ans.has_undefined || true);
    else if (x.second == 0)
      ans.ranges.push_back(make_pair(value(0, value::ninf), x1)),
          ans.has_undefined = (ans.has_undefined || true);
  }
  ans.has_undefined = (ans.has_undefined || a.has_undefined);
  ans.sort();
  return ans;
}
varible div(varible a, varible b) { return mul(a, one_div(b)); }
varible tan(varible a) { return div(sin(a), cos(a)); }
varible pow(varible a, varible b) {
  varible ans;
  ans.ranges.reserve(a.ranges.size() * b.ranges.size());
  {
    varible a1;
    a1.has_undefined = a.has_undefined;
    for (auto &A : a.ranges) {
      if (A.first < 0 && A.second > 0) {
        a1.ranges.push_back({0, A.second});
        a1.ranges.push_back({A.first, 0});
      } else
        a1.ranges.push_back(A);
    }
    a = a1;
  }
  {
    varible b1;
    b1.has_undefined = a.has_undefined;
    for (auto &B : b.ranges) {
      if (B.first < 0 && B.second > 0) {
        b1.ranges.push_back({0, B.second});
        b1.ranges.push_back({B.first, 0});
      } else
        b1.ranges.push_back(B);
    }
    b = b1;
  }
  for (const auto A : a.ranges) {
    for (const auto B : b.ranges) {
      value low, high;
      if (A.first.type == value::normal && A.second.type == value::normal &&
          B.first.type == value::normal && B.second.type == value::normal) {
        auto p1 = std::pow(A.first.v, B.first.v);
        auto p2 = std::pow(A.first.v, B.second.v);
        auto p3 = std::pow(A.second.v, B.first.v);
        auto p4 = std::pow(A.second.v, B.second.v);
        low = value(min(min(p1, p2), min(p3, p4)));
        high = value(max(max(p1, p2), max(p3, p4)));
      }
      if (isnan(low.v) || isnan(high.v)) {
        ans.has_undefined = true;
      } else

        ans.ranges.push_back({low, high});
    }
  }
  ans.has_undefined = (a.has_undefined || b.has_undefined);
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

  ans.has_undefined = a.has_undefined;
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

  ans.has_undefined = a.has_undefined;
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
  ans.has_undefined = a.has_undefined;
  ans.sort();
  return ans;
}

varible expression::eval(const varible &x, const varible &y) {
  return exprs.back()->eval(x, y, exprs);
}
varible ADD::eval(const varible &x, const varible &y,
                  const vector<shared_ptr<_expr>> &exprs) const {
  return add(exprs[l]->eval(x, y, exprs), exprs[r]->eval(x, y, exprs));
}
varible SUB::eval(const varible &x, const varible &y,
                  const vector<shared_ptr<_expr>> &exprs) const {
  return add(exprs[l]->eval(x, y, exprs), neg(exprs[r]->eval(x, y, exprs)));
}
varible MUL::eval(const varible &x, const varible &y,
                  const vector<shared_ptr<_expr>> &exprs) const {
  return mul(exprs[l]->eval(x, y, exprs), exprs[r]->eval(x, y, exprs));
}
varible DIV::eval(const varible &x, const varible &y,
                  const vector<shared_ptr<_expr>> &exprs) const {
  return mul(exprs[l]->eval(x, y, exprs), one_div(exprs[r]->eval(x, y, exprs)));
}

varible POW::eval(const varible &x, const varible &y,
                  const vector<shared_ptr<_expr>> &exprs) const {
  return pow(exprs[l]->eval(x, y, exprs), exprs[r]->eval(x, y, exprs));
}
varible LOG::eval(const varible &x, const varible &y,
                  const vector<shared_ptr<_expr>> &exprs) const {
  return log(exprs[l]->eval(x, y, exprs));
}
varible SIN::eval(const varible &x, const varible &y,
                  const vector<shared_ptr<_expr>> &exprs) const {
  return sin(exprs[l]->eval(x, y, exprs));
}
varible COS::eval(const varible &x, const varible &y,
                  const vector<shared_ptr<_expr>> &exprs) const {
  return cos(exprs[l]->eval(x, y, exprs));
}
varible TAN::eval(const varible &x, const varible &y,
                  const vector<shared_ptr<_expr>> &exprs) const {
  return tan(exprs[l]->eval(x, y, exprs));
}
varible VAR1::eval(const varible &x, const varible &,
                   const vector<shared_ptr<_expr>> &) const {
  return x;
}
varible VAR2::eval(const varible &, const varible &y,
                   const vector<shared_ptr<_expr>> &) const {
  return y;
}
varible CONSTANT::eval(const varible &x, const varible &y,
                       const vector<shared_ptr<_expr>> &) const {
  return value(l);
};
