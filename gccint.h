/* icfpc2014-after: icfpc 2014 related code made after the competition was finished
 * Copyright (C) 2014  aankor (The sound of lambda team)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GCC_INT_H_INCLUDED
#define GCC_INT_H_INCLUDED

#include <boost/variant.hpp>
#include <utility>
#include <string>
#include <vector>
#include <stack>

enum Operator {
  LDC,
  LD,
  ADD,
  SUB,
  MUL,
  DIV,
  CEQ,
  CGT,
  CGTE,
  ATOM,
  CONS,
  CAR,
  CDR,
  SEL,
  JOIN,
  LDF,
  AP,
  RTN,
  DUM,
  RAP,
  STOP,
  TSEL,
  TAP,
  TRAP,
  ST,
  DBUG,
  BRK,
  OPERATOR_COUNT
};

struct Instruction {
  Instruction(Operator op): op(op) {}
  Instruction(Operator op, int x): op(op) {
    params[0] = x;
  }
  Instruction(Operator op, int x, int y): op(op) {
    params[0] = x;
    params[1] = y;
  }
  Operator op;
  int params[2];
  std::string comment; 
};

struct Cons;

struct Closure;

typedef boost::variant<int,
                       Cons*,
                       Closure*> GccValue;

struct Cons {
  Cons(const GccValue& car, const GccValue& cdr): car(car), cdr(cdr) {}
  GccValue car;
  GccValue cdr;
};

struct EnvFrame {
  EnvFrame(int n, EnvFrame* parent, bool isDummy = false): values(n), parent(parent), isDummy(isDummy) {}
  bool isDummy;
  std::vector<GccValue> values;
  EnvFrame* parent;
};

struct Closure {
  Closure(int addr, EnvFrame* env): addr(addr), env(env) {}
  int addr;
  EnvFrame* env;
};

struct ControlFrame {
  enum Tag {
    STOP,
    RET,
    JOIN
  } tag;
  int addr;
  EnvFrame* env;
  
  ControlFrame(Tag tag, int addr, EnvFrame* env): tag(tag), addr(addr), env(env) {}
};

struct GccInterpreter {
public:
  GccInterpreter(const std::vector<Instruction>& programm);
  void operator()();
  GccValue runMain();
  
  int c;
  std::stack<GccValue> dataStack;
  std::stack<ControlFrame> controlStack;
  EnvFrame* env;
  std::vector<Instruction> programm;
};

#endif
