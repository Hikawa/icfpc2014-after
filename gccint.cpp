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

#include "gccint.h"

#include <boost/variant/get.hpp>

typedef bool (*OperatorHandler)(GccInterpreter& gcc, int* params); // false if machinestop

bool doLDC(GccInterpreter& gcc, int* params) {
  gcc.dataStack.push(params[0]);
  ++gcc.c;
  return true;
}

bool doLD(GccInterpreter& gcc, int* params) {
  EnvFrame* fp = gcc.env;
  int n = params[0];
  while (n > 0) {
    fp = fp->parent;
    --n;
  }
  // TODO check
  gcc.dataStack.push(fp->values[params[1]]);
  ++gcc.c;
  return true;
}

bool doADD(GccInterpreter& gcc, int* params) {
  GccValue y = gcc.dataStack.top();
  gcc.dataStack.pop();
  GccValue x = gcc.dataStack.top();
  gcc.dataStack.pop();
  GccValue z = boost::get<int, int, Cons*, Closure*>(x) +
               boost::get<int, int, Cons*, Closure*>(y);
  gcc.dataStack.push(z);
  ++gcc.c;
  return true;
}

bool doSUB(GccInterpreter& gcc, int* params) {
  GccValue y = gcc.dataStack.top();
  gcc.dataStack.pop();
  GccValue x = gcc.dataStack.top();
  gcc.dataStack.pop();
  GccValue z = boost::get<int, int, Cons*, Closure*>(x) -
               boost::get<int, int, Cons*, Closure*>(y);
  gcc.dataStack.push(z);
  ++gcc.c;
  return true;
}

bool doMUL(GccInterpreter& gcc, int* params) {
  GccValue y = gcc.dataStack.top();
  gcc.dataStack.pop();
  GccValue x = gcc.dataStack.top();
  gcc.dataStack.pop();
  GccValue z = boost::get<int, int, Cons*, Closure*>(x) *
               boost::get<int, int, Cons*, Closure*>(y);
  gcc.dataStack.push(z);
  ++gcc.c;
  return true;
}

bool doDIV(GccInterpreter& gcc, int* params) {
  GccValue y = gcc.dataStack.top();
  gcc.dataStack.pop();
  GccValue x = gcc.dataStack.top();
  gcc.dataStack.pop();
  GccValue z = boost::get<int, int, Cons*, Closure*>(x) /
               boost::get<int, int, Cons*, Closure*>(y);
  gcc.dataStack.push(z);
  ++gcc.c;
  return true;
}

bool doCEQ(GccInterpreter& gcc, int* params) {
  GccValue y = gcc.dataStack.top();
  gcc.dataStack.pop();
  GccValue x = gcc.dataStack.top();
  gcc.dataStack.pop();
  GccValue z = boost::get<int, int, Cons*, Closure*>(x) ==
               boost::get<int, int, Cons*, Closure*>(y)? 1: 0;
  gcc.dataStack.push(z);
  ++gcc.c;
  return true;
}

bool doCGT(GccInterpreter& gcc, int* params) {
  GccValue y = gcc.dataStack.top();
  gcc.dataStack.pop();
  GccValue x = gcc.dataStack.top();
  gcc.dataStack.pop();
  GccValue z = boost::get<int, int, Cons*, Closure*>(x) >
               boost::get<int, int, Cons*, Closure*>(y)? 1: 0;
  gcc.dataStack.push(z);
  ++gcc.c;
  return true;
}

bool doCGTE(GccInterpreter& gcc, int* params) {
  GccValue y = gcc.dataStack.top();
  gcc.dataStack.pop();
  GccValue x = gcc.dataStack.top();
  gcc.dataStack.pop();
  GccValue z = boost::get<int, int, Cons*, Closure*>(x) >=
               boost::get<int, int, Cons*, Closure*>(y)? 1: 0;
  gcc.dataStack.push(z);
  ++gcc.c;
  return true;
}

bool doATOM(GccInterpreter& gcc, int* params) {
  class Visitor: public boost::static_visitor<int> {
  public:
    int operator()(int) const {
        return 1;
    }
    
    int operator()(Cons*) const {
        return 0;
    }
    
    int operator()(Closure*) const {
        return 0;
    }
  };
  
  GccValue y = boost::apply_visitor(Visitor(), gcc.dataStack.top());
  gcc.dataStack.pop();
  gcc.dataStack.push(y);
  ++gcc.c;
  return true;
}

bool doCONS(GccInterpreter& gcc, int* params) {
  GccValue y = gcc.dataStack.top();
  gcc.dataStack.pop();
  GccValue x = gcc.dataStack.top();
  gcc.dataStack.pop();
  GccValue z = new Cons(x, y);
  gcc.dataStack.push(z);
  ++gcc.c;
  return true;
}

bool doCAR(GccInterpreter& gcc, int* params) {
  GccValue x = gcc.dataStack.top();
  gcc.dataStack.pop();
  // TODO check
  gcc.dataStack.push(boost::get<Cons*, int, Cons*, Closure*>(x)->car);
  ++gcc.c;
  return true;
}

bool doCDR(GccInterpreter& gcc, int* params) {
  GccValue x = gcc.dataStack.top();
  gcc.dataStack.pop();
  // TODO check
  gcc.dataStack.push(boost::get<Cons*, int, Cons*, Closure*>(x)->cdr);
  ++gcc.c;
  return true;
}

bool doSEL(GccInterpreter& gcc, int* params) {
  GccValue x = gcc.dataStack.top();
  gcc.dataStack.pop();
  // TODO check
  gcc.controlStack.push(ControlFrame(ControlFrame::JOIN, gcc.c+1, gcc.env));
  if (boost::get<int, int, Cons*, Closure*>(x) == 0)
    gcc.c = params[1];
  else
    gcc.c = params[0];
  return true;
}

bool doJOIN(GccInterpreter& gcc, int* params) {
  ControlFrame cf = gcc.controlStack.top();
  gcc.controlStack.pop();
  // TODO check
  gcc.c = cf.addr;
  return true;
}

bool doLDF(GccInterpreter& gcc, int* params) {
  GccValue x = new Closure(params[0], gcc.env);
  gcc.dataStack.push(x);
  ++gcc.c;
  return true;
}

bool doAP(GccInterpreter& gcc, int* params) {
  GccValue x = gcc.dataStack.top();
  gcc.dataStack.pop();
  // TODO check
  Closure* cl = boost::get<Closure*, int, Cons*, Closure*>(x);
  const int n = params[0];
  EnvFrame* fp = new EnvFrame(n, cl->env);
  for (int i = n-1; i > -1; --i) {
    fp->values[i] = gcc.dataStack.top();
    gcc.dataStack.pop();
  }
  gcc.controlStack.push(ControlFrame(ControlFrame::RET, gcc.c+1, gcc.env));
  gcc.env = fp;
  gcc.c = cl->addr;
  return true;
}

bool doRTN(GccInterpreter& gcc, int* params) {
  ControlFrame cf = gcc.controlStack.top();
  gcc.controlStack.pop();
  switch (cf.tag) {
  case ControlFrame::STOP:
    return false;
  case ControlFrame::RET:
    gcc.env = cf.env;
    gcc.c = cf.addr;
    return true;
  case ControlFrame::JOIN:
    // todo
    return true;
  }
}
  /*
  doDUM,
  doRAP,
  doSTOP,
  doTSEL,
  doTAP,
  doTRAP,
  doST,
  doDBUG,
  doBRK,
*/
static const OperatorHandler operators[OPERATOR_COUNT] = {
  doLDC,
  doLD,
  doADD,
  doSUB,
  doMUL,
  doDIV,
  doCEQ,
  doCGT,
  doCGTE,
  doATOM,
  doCONS,
  doCAR,
  doCDR,
  doSEL,
  doJOIN,
  doLDF,
  doAP,
  doRTN,
 /* doDUM,
  doRAP,
  doSTOP,
  doTSEL,
  doTAP,
  doTRAP,
  doST,
  doDBUG,
  doBRK,*/
};

GccInterpreter::GccInterpreter(const std::vector<Instruction>& programm): programm(programm) {
  c = 0;
  env = NULL;
}

void GccInterpreter::operator()() {
  while ((*operators[programm[c].op])(*this, programm[c].params));
}

GccValue GccInterpreter::runMain() {
  controlStack.push(ControlFrame(ControlFrame::STOP, 0, NULL));
  c = 0;
  operator()();
  return dataStack.top();
}
