#include <memory>
#include <ostream>
#include <utility>
#include <vector>
using std::pair;
using std::shared_ptr;
using std::vector;
// enum OP {
//   ADD,     // a
//   SUB,     // s
//   MUL,     // m
//   DIV,     // d
//   POW,     // p
//   LOG,     // l
//   SIN,     // S
//   COS,     // C
//   TAN,     // T
//   VAR1,    // x
//   VAR2,    // y
//   CONSTANT // v
// };
// struct term {
//   OP op;
//   double constant;
// };
struct varible;
struct _expr {
  virtual ~_expr() = default;
  virtual varible eval(const varible &x, const varible &y,
                       const vector<shared_ptr<_expr>> &) const = 0;
};
struct expression {
  varible eval(const varible &x, const varible &y);
  vector<shared_ptr<_expr>> exprs;
};
struct ADD : public _expr {
  ADD(int l, int r) : l(l), r(r) {}
  virtual varible eval(const varible &x, const varible &y,
                       const vector<shared_ptr<_expr>> &exprs) const;
  int l, r;
};
struct SUB : public _expr {
  SUB(int l, int r) : l(l), r(r) {}
  virtual varible eval(const varible &x, const varible &y,
                       const vector<shared_ptr<_expr>> &exprs) const;
  int l, r;
};
struct MUL : public _expr {
  MUL(int l, int r) : l(l), r(r) {}
  virtual varible eval(const varible &x, const varible &y,
                       const vector<shared_ptr<_expr>> &exprs) const;
  int l, r;
};
struct DIV : public _expr {
  DIV(int l, int r) : l(l), r(r) {}
  virtual varible eval(const varible &x, const varible &y,
                       const vector<shared_ptr<_expr>> &exprs) const;
  int l, r;
};
struct POW : public _expr {
  POW(int l, int r) : l(l), r(r) {}
  virtual varible eval(const varible &x, const varible &y,
                       const vector<shared_ptr<_expr>> &exprs) const;
  int l, r;
};
struct LOG : public _expr {
  LOG(int l) : l(l) {}
  virtual varible eval(const varible &x, const varible &y,
                       const vector<shared_ptr<_expr>> &exprs) const;
  int l;
};
struct SIN : public _expr {
  SIN(int l) : l(l) {}
  virtual varible eval(const varible &x, const varible &y,
                       const vector<shared_ptr<_expr>> &exprs) const;
  int l;
};
struct COS : public _expr {
  COS(int l) : l(l) {}
  virtual varible eval(const varible &x, const varible &y,
                       const vector<shared_ptr<_expr>> &exprs) const;
  int l;
};
struct TAN : public _expr {
  TAN(int l) : l(l) {}
  virtual varible eval(const varible &x, const varible &y,
                       const vector<shared_ptr<_expr>> &exprs) const;
  int l;
};
struct VAR1 : public _expr {
  virtual varible eval(const varible &x, const varible &y,
                       const vector<shared_ptr<_expr>> &exprs) const;
};
struct VAR2 : public _expr {
  virtual varible eval(const varible &x, const varible &y,
                       const vector<shared_ptr<_expr>> &exprs) const;
};
struct CONSTANT : public _expr {
  CONSTANT(double l) : l(l) {}
  virtual varible eval(const varible &x, const varible &y,
                       const vector<shared_ptr<_expr>> &) const override;
  double l;
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
  varible(pair<value, value> r) : ranges(r), has_undefined(false){};
  varible(value x) : ranges({std::make_pair(x, x)}), has_undefined(false){};
  varible(value l, value r)
      : ranges({std::make_pair(l, r)}), has_undefined(false){};
  void sort();
  pair<value, value> ranges;
  bool has_undefined;
};
std::ostream &operator<<(std::ostream &, varible);
std::ostream &operator<<(std::ostream &, value);
// using _expr = vector<term>;
expression tokenize(const std::string &expr);

varible pow(varible, varible);
varible log(varible);
varible sin(varible);
varible cos(varible);
varible tan(varible);
varible add(varible, varible);
varible neg(varible);
varible mul(varible, varible);
varible one_div(varible);
varible div(varible, varible);
