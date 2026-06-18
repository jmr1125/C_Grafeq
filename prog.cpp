#include "prog.hpp"
#include "value.hpp"
#include <cstdint>
#include <functional>
#include <iostream>
#include <istream>
#include <map>
#include <stdexcept>
varible prog::eval(varible x, varible y) const {
  vector<varible> mem(mem_size);
  auto ld = [&](uint16_t i) {
    if (i == 0)
      return std::cref(x);
    if (i == 1)
      return std::cref(y);
    if (i < cons.size() + 2)
      return std::cref(cons[i - 2]);
    return std::cref(mem[i - 2 - cons.size()]);
  };
  // auto print = [&]() {
  //   for (int i = 0; i < mem.size() + 2 + cons.size(); ++i) {
  //     std::cout << i << " : " << ld(i) << std::endl;
  //   }
  //   std::cout << std::endl;
  // };
  for (const auto &inst : prog) {
    switch (inst.op) {
    case inst::add:
      mem[inst.d - cons.size() - 2] = add(ld(inst.s1), ld(inst.s2));
      // print();
      break;
    case inst::sub:
      mem[inst.d - cons.size() - 2] = add(ld(inst.s1), neg(ld(inst.s2)));
      // print();
      break;
    case inst::mul:
      mem[inst.d - cons.size() - 2] = mul(ld(inst.s1), ld(inst.s2));
      // print();
      break;
    case inst::div:
      mem[inst.d - cons.size() - 2] = mul(ld(inst.s1), reciprocal(ld(inst.s2)));
      // print();
      break;
    case inst::pow:
      mem[inst.d - cons.size() - 2] = pow(ld(inst.s1), ld(inst.s2));
      // print();
      break;
    case inst::uon:
      mem[inst.d - cons.size() - 2] = Union(ld(inst.s1), ld(inst.s2));
      break;
    case inst::sin:
      mem[inst.d - cons.size() - 2] = sin(ld(inst.s1));
      // print();
      break;
    case inst::cos:
      mem[inst.d - cons.size() - 2] = cos(ld(inst.s1));
      // print();
      break;
    case inst::tan:
      mem[inst.d - cons.size() - 2] =
          mul(sin(ld(inst.s1)), reciprocal(cos(ld(inst.s2))));
      // print();
      break;
    case inst::exp:
      mem[inst.d - cons.size() - 2] = exp(ld(inst.s1));
      // print();
      break;
    case inst::log:
      mem[inst.d - cons.size() - 2] = log(ld(inst.s1));
      // print();
      break;
    case inst::ret:
      // std::cout << "return: " << ld(inst.s1) << std::endl;
      return ld(inst.s1);
    }
  }
  throw std::runtime_error("no ret");
}
const std::map<std::string, inst::opcode> mp{
    {"add", inst::add}, {"sub", inst::sub}, {"mul", inst::mul},
    {"div", inst::div}, {"pow", inst::pow}, {"uon", inst::uon},
    {"sin", inst::sin}, {"cos", inst::cos}, {"tan", inst::tan},
    {"exp", inst::exp}, {"log", inst::log}, {"ret", inst::ret}};
void prog::load(std::istream &ist) {
  int mem_size, const_size, prog_size;
  ist >> mem_size >> const_size >> prog_size;
  this->mem_size = mem_size;
  for (int i = 0; i < const_size; ++i) {
    double c;
    ist >> c;
    cons.push_back(varible());
    cons.back().cont = {true, true};
    cons.back().r.push_back(range());
    arf_set_d(cons.back().r[0].lo.val, c);
    arf_set_d(cons.back().r[0].hi.val, c);
  }
  for (int i = 0; i < prog_size; ++i) {
    std::string op;
    uint16_t d, s1, s2;
    ist >> op >> d >> s1 >> s2;
    prog.push_back({.op = mp.at(op), .d = d, .s1 = s1, .s2 = s2});
  }
}
//            cons                mem
//[ x ][ y ][              ][            ]
//             const_size       mem_size
// 0 ...    c_s-1  c_s  c_s+1 c_s+2
//
// mem_size  const_size  prog_size
// const_v
// . . . .
// prog
// .
// .
