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
struct fval {
  enum type_t { normal, inf, ninf };
  fval() : v(0), type(normal){};
  fval(double x) : v(x), type(normal){};
  fval(double x, type_t t) : v(x), type(t){};
  double v;
  type_t type;
  fval operator+(fval) const;
  fval operator-(fval) const;
  fval operator-() const;
  fval operator*(fval) const;
  fval operator/(fval) const;
  bool operator>(fval) const;
  bool operator==(fval) const;
  bool operator<(fval) const;
  bool operator>=(fval) const;
  bool operator<=(fval) const;
  bool operator!=(fval) const;
};
struct varible {
  varible() = default;
  varible(pair<fval, fval> r) : ranges(r), has_undefined(false){};
  varible(fval x) : ranges({std::make_pair(x, x)}), has_undefined(false){};
  varible(fval l, fval r)
      : ranges({std::make_pair(l, r)}), has_undefined(false){};
  void sort();
  pair<fval, fval> ranges;
  bool has_undefined;
};
std::ostream &operator<<(std::ostream &, varible);
std::ostream &operator<<(std::ostream &, fval);
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
