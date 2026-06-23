#include "value.hpp"
#include <cstdint>
#include <cstdlib>
#include <istream>
#include <map>
struct inst {
  enum opcode {
    add = 1,
    sub = 2,
    neg = 3,
    mul = 4,
    div = 5,
    pow = 6,
    uon = 7, // 2
    sin = 8,
    cos = 9,
    tan = 10,
    exp = 11,
    log = 12,
    flr = 13,
    cil = 14,
    qrt = 15,
    ret = 16 // 1
  };
  opcode op;
  uint16_t d, s1, s2;
};
struct prog {
  vector<inst> prog;
  vector<varible> cons;
  uint16_t mem_size;
  varible eval(varible, varible) const;
  void load(std::istream &);
};
extern const std::map<std::string, inst::opcode> str2op;
