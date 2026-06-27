#include "prog.hpp"
#include "value.hpp"
#include <algorithm>
#include <cstdint>
#include <exception>
#include <iostream>
#include <memory>
#include <ostream>
#include <regex>
#include <stack>
#include <string>
#include <string_view>
#include <vector>
void hash_append(uint16_t &hash, uint16_t data) {
  hash *= 65599;
  hash += data;
}
void hash_append(uint16_t &) {}
template <typename Args>
void hash_append(uint16_t &hash, uint16_t data, Args A...) {
  hash *= 65599;
  hash += data;
  hash_append(hash, A);
}
struct expr_tree {
  ~expr_tree() { l.reset(), r.reset(); }
  inst::opcode op; // ret ==> num,x,y
  std::shared_ptr<expr_tree> l, r;
  uint16_t hash;
  bool ha = false;
  int get_hash(int c = 0) {
    if (ha)
      return hash;
    // std::cerr << c << ' ';
    bool can_swap = true;
    switch (op) {
    case inst::ret:
      return hash;
    case inst::sub:
    case inst::div:
    case inst::pow:
      can_swap = false;
    case inst::add:
    case inst::mul:
    case inst::uon: {
      uint16_t L = l->get_hash(c + 1) + 1, R = r->get_hash(c + 1) + 1;
      if (L > R && can_swap) {
        swap(l, r);
        return get_hash(c + 1);
      }
      hash_append(hash, L, R, op);
      ha = true;
      return hash;
    }
    case inst::neg:
    case inst::sin:
    case inst::cos:
    case inst::tan:
    case inst::exp:
    case inst::log:
    case inst::flr:
    case inst::cil:
    case inst::qrt: {
      uint16_t L = l->get_hash(c + 1);
      hash_append(hash, L, op);
      ha = true;
      return hash;
    }
    }
  }
  bool operator==(const expr_tree &rsh) const {
    if (op != rsh.op)
      return false;
    if (hash != rsh.hash)
      return false;
    switch (op) {
    case inst::ret:
      return hash == rsh.hash;
    case inst::sub:
    case inst::div:
    case inst::pow:
    case inst::add:
    case inst::mul:
    case inst::uon: {
      return (*l) == (*rsh.l) && (*r) == (*rsh.r);
    }
    case inst::neg:
    case inst::sin:
    case inst::cos:
    case inst::tan:
    case inst::exp:
    case inst::log:
    case inst::flr:
    case inst::cil:
    case inst::qrt: {
      return (*l) == (*rsh.l);
    }
    }
  }
  static std::string str[];
  void show(int c = 0) {
    std::cerr << this << "  " << std::string(c * 2, ' ') << str[op] << "   #"
              << std::hex << hash << std::dec << std::endl;
    switch (op) {
    case inst::ret:
      // std::cerr << std::string(c * 2, ' ') << hash << std::endl;
      break;
    case inst::sub:
    case inst::div:
    case inst::pow:
    case inst::add:
    case inst::mul:
    case inst::uon: {
      l->show(c + 1);
      r->show(c + 1);
      break;
    }
    case inst::neg:
    case inst::sin:
    case inst::cos:
    case inst::tan:
    case inst::exp:
    case inst::log:
    case inst::flr:
    case inst::cil:
    case inst::qrt: {
      l->show(c + 1);
      break;
    }
    }
  }
};
void get_all_node(shared_ptr<expr_tree> root,
                  std::vector<shared_ptr<expr_tree>> &res) {
  res.push_back(root);
  switch (root->op) {
  case inst::ret:
    break;
  case inst::sub:
  case inst::div:
  case inst::pow:
  case inst::add:
  case inst::mul:
  case inst::uon: {
    get_all_node(root->l, res);
    get_all_node(root->r, res);
    break;
  }
  case inst::neg:
  case inst::sin:
  case inst::cos:
  case inst::tan:
  case inst::exp:
  case inst::log:
  case inst::flr:
  case inst::cil:
  case inst::qrt: {
    get_all_node(root->l, res);
    break;
  }
  }
}
std::string expr_tree::str[] = {"",    "add", "sub", "neg", "mul",  "div",
                                "pow", "uon", "sin", "cos", "tan",  "exp",
                                "log", "flr", "cil", "qrt", "const"};
using namespace std;
int main() {
  string expr;
  getline(cin, expr);
  const regex number(R"((-?[0-9]+(?:\.[0-9]+)?)(?:e(-?[0-9]+))?.*)");
  const regex token(
      R"(([xy+*/(),-]|(?:pow|union|sin|cos|tan|exp|log|floor|ceil|sqrt)).*$)");
  vector<pair<bool, string>> tokens;
  vector<string> constants;
  int i = 0;
  while (i < expr.size()) {
    match_results<string::const_iterator> res;
    if (regex_match(expr.cbegin() + i, expr.cend(), res, token)) {
      // cout << "tok: " << res[1].str() << endl;
      if (res[1].str() == "x" || res[1].str() == "y") {
        tokens.push_back({true, res[1].str()});
      } else {
        tokens.push_back({false, res[1].str()});
      }
      i += res[1].str().size();
    } else if (regex_match(expr.cbegin() + i, expr.cend(), res, number)) {
      // cout << "num: " << res[1].str() << endl;
      tokens.push_back({true, res[1].str()});
      try {
        constants.push_back(res[1].str());
      } catch (const exception &e) {
        cout << e.what() << endl;
        return 1;
      }
      i += res[1].str().size();
    } else {
      cerr << expr << endl;
      cerr << string(i, ' ') << "^" << endl;
      cerr << "syntax error at: " << i << endl;
      return 1;
    }
  }
  sort(constants.begin(), constants.end());
  constants.erase(unique(constants.begin(), constants.end()), constants.end());
  const int prio[256] = {
      [inst::add] = 100, [inst::sub] = 100, [inst::mul] = 200,
      [inst::div] = 200, [inst::neg] = 300, [inst::sin] = 300,
      [inst::cos] = 300, [inst::tan] = 300, [inst::exp] = 300,
      [inst::log] = 300, [inst::flr] = 300, [inst::cil] = 300,
      [inst::qrt] = 300};
  // cout << "consts: " << endl;
  std::stack<shared_ptr<expr_tree>> num;
  std::stack<inst::opcode> op; // ret ==> (
  bool begin = true;
  auto exit_if_empty = [](const stack<shared_ptr<expr_tree>> &num) {
    if (num.empty()) {
      cerr << "operand not match" << endl;
      throw "";
    }
  };
  auto pop_one = [&exit_if_empty](stack<inst::opcode> &op,
                                  stack<shared_ptr<expr_tree>> &num) {
    exit_if_empty(num);
    const auto oper = op.top();
    op.pop();
    switch (oper) {
    case inst::add:
    case inst::sub:
    case inst::mul:
    case inst::div:
    case inst::pow:
    case inst::uon: {
      shared_ptr<expr_tree> r(new expr_tree);
      r->op = oper;
      exit_if_empty(num);
      r->r = num.top();
      num.pop();
      exit_if_empty(num);
      r->l = num.top();
      num.pop();
      num.push(r);
      break;
    }
    case inst::neg:
    case inst::sin:
    case inst::cos:
    case inst::tan:
    case inst::exp:
    case inst::log:
    case inst::flr:
    case inst::cil:
    case inst::qrt: {
      shared_ptr<expr_tree> r(new expr_tree);
      r->op = oper;
      exit_if_empty(num);
      r->l = num.top();
      num.pop();
      num.push(r);
      break;
    }
    case inst::ret:
      throw "???";
      break;
    }
  };
  tokens.insert(tokens.begin(), {false, "("});
  tokens.insert(tokens.end(), {false, ")"});
  int ind = -1;
  try {
    for (int i = 0; i < tokens.size(); ind += tokens[i].second.size(), ++i) {
      const auto [isnum, t] = tokens[i];
      if (isnum) {
        begin = false;
        shared_ptr<expr_tree> e(new expr_tree);
        e->op = inst::ret;
        if (t == "x")
          e->hash = 0;
        else if (t == "y")
          e->hash = 1;
        else
          e->hash = lower_bound(constants.begin(), constants.end(), t) -
                    constants.begin() + 2;
        num.push(e);
      } else {
        if (t == "(") {
          begin = true;
          op.push(inst::ret);
        } else if (t == ")") {
          if (op.empty()) {
            cerr << "not match (" << endl;
            return 1;
          }
          while (op.top() != inst::ret) {
            pop_one(op, num);
          }
          op.pop();
        } else if (t == ",") {
          begin = true;
          continue;
        } else {
          string ss;
          ss = t;
          if (t == "+")
            ss = "add";
          else if (t == "-" && begin)
            ss = "neg";
          else if (t == "-" && !begin)
            ss = "sub";
          else if (t == "*")
            ss = "mul";
          else if (t == "/")
            ss = "div";
          else if (t == "floor")
            ss = "flr";
          else if (t == "ceil")
            ss = "cil";
          else if (t == "sqrt")
            ss = "qrt";
          while (prio[op.top()] >= prio[str2op.at(ss)]) {
            pop_one(op, num);
          }
          op.push(str2op.at(ss));
          begin = true;
        }
      }
    }
  } catch (...) {
    return 1;
  }
  if (num.size() == 0) {
    cerr << "empty!" << endl;
    return 1;
  }
  if (num.size() != 1) {
    cerr << "excessive value!" << endl;
    return 1;
  }
  num.top()->get_hash();
  // num.top()->show();
  stack<shared_ptr<expr_tree>> stk;
  vector<pair<shared_ptr<expr_tree>, pair<bool, bool>>> all_node;
  vector<shared_ptr<expr_tree>> topo;
  {
    vector<shared_ptr<expr_tree>> allnode;
    get_all_node(num.top(), allnode);
    for (const auto &t : allnode) {
      all_node.push_back({t, {false, false}});
    }
  }
  while (!all_node.empty()) {
    // cerr << all_node.size() << ' ' << flush;
    int i = 0;
    for (; i < all_node.size(); ++i) {
      bool f = false;
      switch (all_node[i].first->op) {
      case inst::add:
      case inst::sub:
      case inst::mul:
      case inst::div:
      case inst::pow:
      case inst::uon: {
        if (all_node[i].second == pair{true, true}) {
          f = true;
          topo.push_back(all_node[i].first);
        }
        break;
      }
      case inst::neg:
      case inst::sin:
      case inst::cos:
      case inst::tan:
      case inst::exp:
      case inst::log:
      case inst::flr:
      case inst::cil:
      case inst::qrt: {
        if (all_node[i].second.first) {
          f = true;
          topo.push_back(all_node[i].first);
        }
        break;
      }
      case inst::ret:
        f = true;
        topo.push_back(all_node[i].first);
        break;
      }
      if (f) {
        for (int j = 0; j < all_node.size(); ++j) {
          if (all_node[j].first->l == all_node[i].first) {
            all_node[j].second.first = true;
          }
          if (all_node[j].first->r == all_node[i].first) {
            all_node[j].second.second = true;
          }
        }
        all_node.erase(all_node.begin() + i);
        i--;
      }
    }
  }
  int max_i = 2 + constants.size();
  vector<vector<pair<shared_ptr<expr_tree>, int>>> map(65536);
  auto find = [&map](const shared_ptr<expr_tree> &x) -> int {
    if (x->op == inst::ret)
      return x->hash;
    if (map.at(x->hash).size() == 0)
      return -1;
    for (const auto &t : map.at(x->hash)) {
      if ((*t.first) == (*x)) {
        return t.second;
      }
    }
    return -1;
  };
  auto insert = [&map](const shared_ptr<expr_tree> &x, int i) {
    map[x->hash].push_back({x, i});
  };
  topo.erase(remove_if(topo.begin(), topo.end(),
                       [](const auto &x) { return x->op == inst::ret; }),
             topo.end());
  cout << constants.size() << endl;
  for (const auto &x : constants)
    cout << x << ' ';
  cout << endl;
  for (const auto &t : topo) {
    // cerr << t << endl;
    if (t->op == inst::ret) {
      insert(t, t->hash);
      continue;
    }
    const auto i = find(t);
    if (i == -1) {
      // cerr << t << ' ' << expr_tree::str[t->op] << ' ' << t->hash << ' ' <<
      // t->l
      //      << ' ' << t->r << endl;
      cout << expr_tree::str[t->op] << ' ' << max_i << ' ';
      insert(t, max_i);
      switch (t->op) {
      case inst::add:
      case inst::sub:
      case inst::mul:
      case inst::div:
      case inst::pow:
      case inst::uon: {
        cout << find(t->l) << ' ' << find(t->r) << endl;
        break;
      }
      case inst::neg:
      case inst::sin:
      case inst::cos:
      case inst::tan:
      case inst::exp:
      case inst::log:
      case inst::flr:
      case inst::cil:
      case inst::qrt: {
        cout << find(t->l) << ' ' << 0 << endl;
        break;
      }
      default:
        throw "???";
      }
      ++max_i;
    }
  }
  cout << "ret 0 " << max_i - 1 << " 0" << endl;
}
