#include <map>
#include "prog.hpp"
const std::map<std::string, inst::opcode> str2op{
    {"add", inst::add}, {"sub", inst::sub}, {"neg", inst::neg},
    {"mul", inst::mul}, {"div", inst::div}, {"pow", inst::pow},
    {"uon", inst::uon}, {"sin", inst::sin}, {"cos", inst::cos},
    {"tan", inst::tan}, {"exp", inst::exp}, {"log", inst::log},
    {"flr", inst::flr}, {"cil", inst::cil}, {"qrt", inst::qrt},
    {"ret", inst::ret}};
