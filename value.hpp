#include <flint/arb.h>
#include <flint/flint.h>
#include <memory>
#include <ostream>
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
  bool pinf() const;
  bool ninf() const;
  bool nan() const;
  arf_t val;
  bool operator<(const fval &) const;
  bool operator==(const fval &) const;
  bool operator>(const fval &) const;
};
struct range {
  fval hi, lo;
};
struct varible {
  std::vector<range> r;
};
bool isintersect(const range &, const range &);
range add(const range &, const range &);
range neg(const range &);
range mul(const range &, const range &);
varible reciprocal(const range &);

range sin(const range &);
range cos(const range &);
varible tan(const range &);
varible log(const range &);
range exp(const range &);
varible pow(const range &, const range &);

varible Union(const varible &, const varible &);
varible add(varible, varible);
varible neg(varible);
varible mul(varible, varible);
varible reciprocal(varible);
varible sin(varible);
varible cos(varible);
varible tan(varible);
varible log(varible);
varible exp(varible);
varible pow(varible, varible);

std::ostream &operator<<(std::ostream &, const range &);
std::ostream &operator<<(std::ostream &, const varible &);
