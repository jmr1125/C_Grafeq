#include "value.hpp"
#include <cstdint>
#include <cstdlib>
#include <istream>
struct inst {
  enum opcode {
    add,
    sub,
    mul,
    div,
    pow,
    uon,
    sin,
    cos,
    tan,
    exp,
    log,
    ret // 1
  };
  opcode op;
  uint16_t d, s1, s2;
};
struct prog {
  vector<inst> prog;
  vector<varible> cons;
  int mem_size;
  varible eval(varible, varible) const;
  void load(std::istream &);
};
