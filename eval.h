#include <ostream>
#include <utility>
#include <vector>
using std::pair;
using std::vector;
enum OP {
  ADD,     // a
  SUB,     // s
  MUL,     // m
  DIV,     // d
  POW,     // p
  LOG,     // l
  SIN,     // S
  COS,     // C
  TAN,     // T
  VAR1,    // x
  VAR2,    // y
  CONSTANT // v
};
struct term {
  OP op;
  double constant;
};
struct value {
  enum type_t { normal, inf, ninf };
  value() : v(0), type(normal){};
  value(double x) : v(x), type(normal){};
  value(double x, type_t t) : v(x), type(t){};
  double v;
  type_t type;
  value operator+(value) const;
  value operator-(value) const;
  value operator-() const;
  value operator*(value) const;
  value operator/(value) const;
  bool operator>(value) const;
  bool operator==(value) const;
  bool operator<(value) const;
  bool operator>=(value) const;
  bool operator<=(value) const;
  bool operator!=(value) const;
};
struct varible {
  varible() = default;
  varible(vector<pair<value, value>> r) : ranges(r), has_undefined(false){};
  varible(value x) : ranges({std::make_pair(x, x)}), has_undefined(false){};
  varible(value l, value r)
      : ranges({std::make_pair(l, r)}), has_undefined(false){};
  void sort();
  vector<pair<value, value>> ranges;
  bool has_undefined;
};
std::ostream &operator<<(std::ostream &, varible);
std::ostream &operator<<(std::ostream &, value);
using expression = vector<term>;
expression tokenize(const std::string &expr);
varible eval(const expression &expr, varible var1, varible var2);

varible pow(varible, varible);
varible log(varible);
varible sin(varible);
varible cos(varible);
varible add(varible, varible);
varible neg(varible);
varible mul(varible, varible);
varible one_div(varible);
varible div(varible, varible);
