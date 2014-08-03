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
  EnvFrame* fp = gcc.env.get();
  int n = params[0];
  while (n > 0) {
    fp = fp->parent.get();
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
  GccValue z = boost::get<int, int, boost::intrusive_ptr<Cons>, boost::intrusive_ptr<Closure> >(x) +
               boost::get<int, int, boost::intrusive_ptr<Cons>, boost::intrusive_ptr<Closure> >(y);
  gcc.dataStack.push(z);
  ++gcc.c;
  return true;
}

bool doSUB(GccInterpreter& gcc, int* params) {
  GccValue y = gcc.dataStack.top();
  gcc.dataStack.pop();
  GccValue x = gcc.dataStack.top();
  gcc.dataStack.pop();
  GccValue z = boost::get<int, int, boost::intrusive_ptr<Cons>, boost::intrusive_ptr<Closure> >(x) -
               boost::get<int, int, boost::intrusive_ptr<Cons>, boost::intrusive_ptr<Closure> >(y);
  gcc.dataStack.push(z);
  ++gcc.c;
  return true;
}

bool doMUL(GccInterpreter& gcc, int* params) {
  GccValue y = gcc.dataStack.top();
  gcc.dataStack.pop();
  GccValue x = gcc.dataStack.top();
  gcc.dataStack.pop();
  GccValue z = boost::get<int, int, boost::intrusive_ptr<Cons>, boost::intrusive_ptr<Closure> >(x) *
               boost::get<int, int, boost::intrusive_ptr<Cons>, boost::intrusive_ptr<Closure> >(y);
  gcc.dataStack.push(z);
  ++gcc.c;
  return true;
}

bool doDIV(GccInterpreter& gcc, int* params) {
  GccValue y = gcc.dataStack.top();
  gcc.dataStack.pop();
  GccValue x = gcc.dataStack.top();
  gcc.dataStack.pop();
  GccValue z = boost::get<int, int, boost::intrusive_ptr<Cons>, boost::intrusive_ptr<Closure> >(x) /
               boost::get<int, int, boost::intrusive_ptr<Cons>, boost::intrusive_ptr<Closure> >(y);
  gcc.dataStack.push(z);
  ++gcc.c;
  return true;
}

bool doCEQ(GccInterpreter& gcc, int* params) {
  GccValue y = gcc.dataStack.top();
  gcc.dataStack.pop();
  GccValue x = gcc.dataStack.top();
  gcc.dataStack.pop();
  GccValue z = boost::get<int, int, boost::intrusive_ptr<Cons>, boost::intrusive_ptr<Closure> >(x) ==
               boost::get<int, int, boost::intrusive_ptr<Cons>, boost::intrusive_ptr<Closure> >(y)? 1: 0;
  gcc.dataStack.push(z);
  ++gcc.c;
  return true;
}

bool doCGT(GccInterpreter& gcc, int* params) {
  GccValue y = gcc.dataStack.top();
  gcc.dataStack.pop();
  GccValue x = gcc.dataStack.top();
  gcc.dataStack.pop();
  GccValue z = boost::get<int, int, boost::intrusive_ptr<Cons>, boost::intrusive_ptr<Closure> >(x) >
               boost::get<int, int, boost::intrusive_ptr<Cons>, boost::intrusive_ptr<Closure> >(y)? 1: 0;
  gcc.dataStack.push(z);
  ++gcc.c;
  return true;
}

bool doCGTE(GccInterpreter& gcc, int* params) {
  GccValue y = gcc.dataStack.top();
  gcc.dataStack.pop();
  GccValue x = gcc.dataStack.top();
  gcc.dataStack.pop();
  GccValue z = boost::get<int, int, boost::intrusive_ptr<Cons>, boost::intrusive_ptr<Closure> >(x) >=
               boost::get<int, int, boost::intrusive_ptr<Cons>, boost::intrusive_ptr<Closure> >(y)? 1: 0;
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
    
    int operator()(const boost::intrusive_ptr<Cons>&) const {
        return 0;
    }
    
    int operator()(const boost::intrusive_ptr<Closure>&) const {
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
  gcc.gc.tryCollect();
  GccValue y = gcc.dataStack.top();
  gcc.dataStack.pop();
  GccValue x = gcc.dataStack.top();
  gcc.dataStack.pop();
  GccValue z = boost::intrusive_ptr<Cons>(new Cons(gcc.gc, x, y));
  gcc.dataStack.push(z);
  ++gcc.c;
  return true;
}

bool doCAR(GccInterpreter& gcc, int* params) {
  GccValue x = gcc.dataStack.top();
  gcc.dataStack.pop();
  // TODO check
  gcc.dataStack.push(boost::get<boost::intrusive_ptr<Cons>, int, boost::intrusive_ptr<Cons>, boost::intrusive_ptr<Closure> >(x)->car);
  ++gcc.c;
  return true;
}

bool doCDR(GccInterpreter& gcc, int* params) {
  GccValue x = gcc.dataStack.top();
  gcc.dataStack.pop();
  // TODO check
  gcc.dataStack.push(boost::get<boost::intrusive_ptr<Cons>, int, boost::intrusive_ptr<Cons>, boost::intrusive_ptr<Closure> >(x)->cdr);
  ++gcc.c;
  return true;
}

bool doSEL(GccInterpreter& gcc, int* params) {
  GccValue x = gcc.dataStack.top();
  gcc.dataStack.pop();
  // TODO check
  gcc.controlStack.push(ControlFrame(ControlFrame::JOIN, gcc.c+1, gcc.env));
  if (boost::get<int, int, boost::intrusive_ptr<Cons>, boost::intrusive_ptr<Closure> >(x) == 0)
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
  gcc.gc.tryCollect();
  GccValue x = boost::intrusive_ptr<Closure>(new Closure(gcc.gc, params[0], gcc.env));
  gcc.dataStack.push(x);
  ++gcc.c;
  return true;
}

bool doAP(GccInterpreter& gcc, int* params) {
  gcc.gc.tryCollect();
  GccValue x = gcc.dataStack.top();
  gcc.dataStack.pop();
  // TODO check
  boost::intrusive_ptr<Closure> cl = boost::get<boost::intrusive_ptr<Closure>, int, boost::intrusive_ptr<Cons>, boost::intrusive_ptr<Closure> >(x);
  const int n = params[0];
  boost::intrusive_ptr<EnvFrame> fp = boost::intrusive_ptr<EnvFrame>(new EnvFrame(gcc.gc, n, cl->env));
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

bool doDUM(GccInterpreter& gcc, int* params) {
  gcc.gc.tryCollect();
  const int n = params[0];
  boost::intrusive_ptr<EnvFrame> fp = boost::intrusive_ptr<EnvFrame>(new EnvFrame(gcc.gc, n, gcc.env, true));
  gcc.env = fp;
  ++gcc.c;
  return true;
}

bool doRAP(GccInterpreter& gcc, int* params) {
  GccValue x = gcc.dataStack.top();
  gcc.dataStack.pop();
  // TODO check
  boost::intrusive_ptr<Closure> cl = boost::get<boost::intrusive_ptr<Closure>, int, boost::intrusive_ptr<Cons>, boost::intrusive_ptr<Closure> >(x);
  const int n = params[0];
  // TODO check
  for (int i = n-1; i > -1; --i) {
    gcc.env->values[i] = gcc.dataStack.top();
    gcc.dataStack.pop();
  }
  gcc.controlStack.push(ControlFrame(ControlFrame::RET, gcc.c+1, gcc.env->parent));
  gcc.env->isDummy = false;
  gcc.c = cl->addr;
  return true;
}
/*  doSTOP,
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
  doDUM,
  doRAP,
/*  doSTOP,
  doTSEL,
  doTAP,
  doTRAP,
  doST,
  doDBUG,
  doBRK,*/
};

void GCBaseContainer::moveTo(GCBaseContainer* &oldFirst, GCBaseContainer* &newFirst) {
  if (isMoved)
    return;
#ifdef TRACE_GC_MOVE
  std::cout << "Move: " << this << std::endl;
#endif
  if (prev)
    prev->next = next;
  else
    oldFirst = next;
  if (next)
    next->prev = prev;
      
  if (newFirst)
    newFirst->prev = this;
  next = newFirst;
  prev = NULL;
  newFirst = this;
  
  isMoved = true;
  
  moveChildrenTo(oldFirst, newFirst);
}

void GC::collect() {
#ifdef TRACE_GC
  std::cout << "GC::collect started: init" << std::endl;
#endif
  for (GCBaseContainer* p = first; p; p = p->next) {
    p->gcRefs = p->useCount();
    p->isMoved = false;
  }
#ifdef TRACE_GC_STAGES
  std::cout << "GC::collect: dec refs" << std::endl;
#endif
  for (GCBaseContainer* p = first; p; p = p->next)
    p->decChildrenRefs();
#ifdef TRACE_GC_REFS
  for (GCBaseContainer* p = first; p; p = p->next)
    std::cout << "gcRefs(" << p << ") = " << p->gcRefs << std::endl;
#endif
#ifdef TRACE_GC_STAGES
  std::cout << "GC::collect: move" << std::endl;
#endif
  GCBaseContainer* newFirst = NULL;
  GCBaseContainer** pp = &first;
  while (*pp) {
    if ((*pp)->gcRefs > 0)
      (*pp)->moveTo(first, newFirst); // updates pointer to next element at *pp
    else
      pp = &(*pp)->next;
  }
#ifdef TRACE_GC_STAGES
  std::cout << "GC::collect: delete" << std::endl;
#endif
  while (first)
    first->deref();
  first = newFirst;
#ifdef TRACE_GC
  std::cout << "GC::collect finished" << std::endl;
#endif
}

void GC::tryCollect() {
#ifdef TRACE_GC
    collect();
#else
    if (++objectCount > objectThreshold) {
      collect();
      if (objectCount > (2*objectThreshold)/3)
        objectThreshold = (4*objectCount)/3;
      else if (objectCount < 2*MIN_OBJECT_THRESHOLD/3)
        objectThreshold = MIN_OBJECT_THRESHOLD;
    }
#endif
}

class DecChildrenRefsVisitor: public boost::static_visitor<void> {
public:
  void operator()(int) const {
  }
    
  void operator()(const boost::intrusive_ptr<Cons>& x) const {
    if (x) {
      --x->gcRefs;
    }
  }
  
  void operator()(const boost::intrusive_ptr<Closure>& x) const {
    if (x) {
      --x->gcRefs;
    }
  }
};

class MoveChildrenToVisitor: public boost::static_visitor<void> {
public:
  MoveChildrenToVisitor(GCBaseContainer* &oldFirst, GCBaseContainer* &newFirst): oldFirst(oldFirst), newFirst(newFirst) {}
  void operator()(int) const {
  }
    
  void operator()(const boost::intrusive_ptr<Cons>& x) const {
    if (x)
      x->moveTo(oldFirst, newFirst);
  }
  
  void operator()(const boost::intrusive_ptr<Closure>& x) const {
    if (x)
      x->moveTo(oldFirst, newFirst);
  }
  
  GCBaseContainer* &oldFirst;
  GCBaseContainer* &newFirst;
};

class DerefVisitor: public boost::static_visitor<void> {
public:
  void operator()(int) const {
  }
    
  void operator()(boost::intrusive_ptr<Cons>& x) const {
    x.reset();
  }
  
  void operator()(boost::intrusive_ptr<Closure>& x) const {
    x.reset();
  }
};

void Cons::decChildrenRefs() {
  boost::apply_visitor(DecChildrenRefsVisitor(), car);
  boost::apply_visitor(DecChildrenRefsVisitor(), cdr);
}

void Cons::moveChildrenTo(GCBaseContainer* &oldFirst, GCBaseContainer* &newFirst) {
  boost::apply_visitor(MoveChildrenToVisitor(oldFirst, newFirst), car);
  boost::apply_visitor(MoveChildrenToVisitor(oldFirst, newFirst), cdr);
}

void Cons::deref() {
#ifdef TRACE_GC_DEREF
  std::cout << "Cons::deref " << this << ": " << use_count() << std::endl;
#endif
  {
    boost::intrusive_ptr<Cons> self = this; // prevent circular deletion
    boost::apply_visitor(DerefVisitor(), car);
    boost::apply_visitor(DerefVisitor(), cdr);
  }
#ifdef TRACE_GC_DEREF
  std::cout << "Cons::after deref " << this << ": " << use_count() << std::endl;
#endif
}

void EnvFrame::decChildrenRefs() {
  for(int i=0; i<values.size(); ++i)
    boost::apply_visitor(DecChildrenRefsVisitor(), values[i]);
  if (parent)
    --parent->gcRefs;
}

void EnvFrame::moveChildrenTo(GCBaseContainer* &oldFirst, GCBaseContainer* &newFirst) {
  for(int i=0; i<values.size(); ++i)
    boost::apply_visitor(MoveChildrenToVisitor(oldFirst, newFirst), values[i]);
  if (parent)
    parent->moveTo(oldFirst, newFirst);
}

void EnvFrame::deref() {
#ifdef TRACE_GC_DEREF
  std::cout << "EnvFrame::deref " << this << ": " << use_count() << std::endl;
#endif
  {
    boost::intrusive_ptr<EnvFrame> self = this; // prevent circular deletion
    for(int i=0; i<values.size(); ++i)
      boost::apply_visitor(DerefVisitor(), values[i]);
    parent.reset();
  }
#ifdef TRACE_GC_DEREF
  std::cout << "EnvFrame::after deref " << this << ": " << use_count() << std::endl;
#endif
}

void Closure::decChildrenRefs() {
  if (env)
    --env->gcRefs;
}

void Closure::moveChildrenTo(GCBaseContainer* &oldFirst, GCBaseContainer* &newFirst) {
  if (env)
    env->moveTo(oldFirst, newFirst);
}

void Closure::deref() {
#ifdef TRACE_GC_DEREF
  std::cout << "Closure::deref " << this << ": " << use_count() << std::endl;
#endif
  {
    boost::intrusive_ptr<Closure> self = this; // prevent circular deletion
    env.reset();
  }
#ifdef TRACE_GC_DEREF
  std::cout << "Closure::after deref " << this << ": " << use_count() << std::endl;
#endif
}

GccInterpreter::GccInterpreter(const std::vector<Instruction>& programm): programm(programm) {
  c = 0;
}

bool GccInterpreter::step() {
  return (*operators[programm[c].op])(*this, programm[c].params);
}

void GccInterpreter::operator()() {
  while (step());
}
/*
std::ostream& operator << (std::ostream&, const GccInterpreter& gcc) {
  std::cout << c << ": " << programm[c] << std::endl;
  std::cout 
}*/

GccValue GccInterpreter::runMain() {
  controlStack.push(ControlFrame(ControlFrame::STOP, 0, NULL));
  c = 0;
  operator()();
  return dataStack.top();
}
