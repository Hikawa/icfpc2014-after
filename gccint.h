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
#include <boost/smart_ptr/intrusive_ptr.hpp>
#include <boost/smart_ptr/intrusive_ref_counter.hpp>
#include <utility>
#include <string>
#include <vector>
#include <stack>
#include <iostream>

#define TRACE_GC

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

class GCBaseContainer;

class GC {
public:
  GC() { first = NULL; objectCount = 0; objectThreshold = MIN_OBJECT_THRESHOLD; }
  void collect();
  
  GCBaseContainer* first;
  int objectCount;
  int objectThreshold;
  static const int MIN_OBJECT_THRESHOLD = 1000;
};

class GCBaseContainer {
public:
  GCBaseContainer(GC& gc): gc(gc) {
#ifdef TRACE_GC
    gc.collect();
#else
    if (++gc.objectCount > gc.objectThreshold) {
      gc.collect(); // O_o
      if (gc.objectCount > (2*gc.objectThreshold)/3)
        gc.objectThreshold = (4*gc.objectCount)/3;
      else if (gc.objectCount < 2*MIN_OBJECT_THRESHOLD/3)
        gc.objectThreshold = MIN_OBJECT_THRESHOLD;
    }
#endif

    if (gc.first)
      gc.first->prev = this;
    next = gc.first;
    prev = NULL;
    gc.first = this;
  }
  
  virtual ~GCBaseContainer() {
    --gc.objectCount;
    if (prev) {
      prev->next = next;
    }
    else {
      gc.first = next;
    }
    if (next)
      next->prev = prev;
  }
  
  void moveTo(GCBaseContainer* &oldFirst, GCBaseContainer* &newFirst);
  
  virtual unsigned int useCount() const = 0;
  virtual void decChildrenRefs() = 0;
  virtual void moveChildrenTo(GCBaseContainer* &oldFirst, GCBaseContainer* &newFirst) = 0;
  virtual void deref() = 0;
  
  GCBaseContainer* next;
  GCBaseContainer* prev;
  int gcRefs;
  bool isMoved;
  GC& gc;
};

template <class DerivedT>
class GCContainer: public boost::intrusive_ref_counter<DerivedT>, public GCBaseContainer {
public:
  GCContainer(GC& gc): GCBaseContainer(gc) {}
  virtual unsigned int useCount() const { return boost::intrusive_ref_counter<DerivedT>::use_count(); }
};

struct Cons;

struct Closure;

typedef boost::variant<int,
                       boost::intrusive_ptr<Cons>,
                       boost::intrusive_ptr<Closure> > GccValue;

struct Cons: GCContainer<Cons> {
  Cons(GC& gc, const GccValue& car, const GccValue& cdr):  GCContainer(gc), car(car), cdr(cdr) {
#ifdef TRACE_GC
    std::cout << "new Cons() " << this << std::endl;
#endif
  }
  
#ifdef TRACE_GC
  ~Cons() {
    std::cout << "delete Cons " << this << std::endl;
  }
#endif
  
  virtual void decChildrenRefs();
  virtual void moveChildrenTo(GCBaseContainer* &oldFirst, GCBaseContainer* &newFirst);
  virtual void deref();
  
  GccValue car;
  GccValue cdr;
};

struct EnvFrame: GCContainer<EnvFrame> {
  EnvFrame(GC& gc,
           int n,
           const boost::intrusive_ptr<EnvFrame>& parent,
           bool isDummy = false)
  : GCContainer(gc), values(n), parent(parent), isDummy(isDummy) {
#ifdef TRACE_GC
    std::cout << "new EnvFrame() " << this << std::endl;
#endif
  }

#ifdef TRACE_GC
  ~EnvFrame() {
    std::cout << "delete EnvFrame " << this << std::endl;
  }
#endif

  virtual void decChildrenRefs();
  virtual void moveChildrenTo(GCBaseContainer* &oldFirst, GCBaseContainer* &newFirst);
  virtual void deref();
  
  bool isDummy;
  std::vector<GccValue> values;
  boost::intrusive_ptr<EnvFrame> parent;
};

struct Closure: GCContainer<Closure> {
  Closure(GC& gc, int addr, const boost::intrusive_ptr<EnvFrame>& env)
  : GCContainer(gc), addr(addr), env(env) {
#ifdef TRACE_GC
    std::cout << "new Closure() " << this << std::endl;
#endif
  }
  
#ifdef TRACE_GC
  ~Closure() {
    std::cout << "delete Closure " << this << std::endl;
  }
#endif
  
  virtual void decChildrenRefs();
  virtual void moveChildrenTo(GCBaseContainer* &oldFirst, GCBaseContainer* &newFirst);
  virtual void deref();
  
  int addr;
  boost::intrusive_ptr<EnvFrame> env;
};

struct ControlFrame {
  enum Tag {
    STOP,
    RET,
    JOIN
  } tag;
  int addr;
  boost::intrusive_ptr<EnvFrame> env;
  
  ControlFrame(Tag tag, int addr, const boost::intrusive_ptr<EnvFrame>& env)
  : tag(tag), addr(addr), env(env) {}
};

struct GccInterpreter {
public:
  GccInterpreter(const std::vector<Instruction>& programm);
  void operator()();
  GccValue runMain();
  
  int c;
  std::stack<GccValue> dataStack;
  std::stack<ControlFrame> controlStack;
  boost::intrusive_ptr<EnvFrame> env;
  std::vector<Instruction> programm;
  GC gc;
};

#endif
