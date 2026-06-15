#include <flint/arb.h>
#include <flint/flint.h>
#include <memory>
#include <vector>
using std::shared_ptr;
using std::vector;
struct fval {
  fval();
  fval(double);
  fval(const arf_t &);
  ~fval();
  void set_pinf();
  void set_ninf();
  void set_nan();
  void pinf() const;
  void ninf() const;
  void nan() const;
  arf_t val;
};
struct range {
  enum type_t { clse, pinf, ninf, inf };
  virtual type_t t() = 0;
};
struct fball : public range {
  fball();
  fball(double, double);
  fball(const arf_t &);
  virtual ~fball();
  virtual type_t t() override;
  arb_t val;
};
struct hprange : public range {
  hprange();
  hprange(const fval &);
  virtual type_t t() override;
  fval v;
};
struct hnrange : public range {
  hnrange();
  hnrange(const fval &);
  virtual type_t t() override;
  fval v;
};
struct infrange : public range {
  infrange();
  virtual type_t t() override;
};
struct varible {
  std::vector<std::shared_ptr<range>> r;
};
bool isintersect(const range &, const range &);
shared_ptr<range> add(shared_ptr<range>, shared_ptr<range>);
shared_ptr<range> opposite(shared_ptr<range>);
shared_ptr<range> mul(shared_ptr<range>, shared_ptr<range>);
varible reciprocal(shared_ptr<range>);

shared_ptr<range> sin(shared_ptr<range>);
shared_ptr<range> cos(shared_ptr<range>);
varible tan(shared_ptr<range>);
varible log(shared_ptr<range>);
shared_ptr<range> exp(shared_ptr<range>);
varible pow(shared_ptr<range>, shared_ptr<range>);
