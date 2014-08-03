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

/*
0:  DUM 1
1:  LDF factorial=5
2:  LDF main=20
3:  RAP 1
4:  RTN
factorial=5: ; (x)
5:  LD 0 0 ; push x
6:  LDC 1
7:  CEQ
8:  SEL one=10 notOne=12
9:  RTN
one=10:
10:  LDC 1 ; return 1
11: JOIN
notOne=12:
12: LD 0 0 ; push x
13: LD 0 0
14: LDC 1
15: SUB ; push x-1
16: LD 1 0 ; push factorial
17: AP 1 ; push factorial(x-1)
18: MUL ; push x*factorial(x-1)
19: JOIN
main=20:
20: LDC 7
21: LD 0 0 ; push factorial
22: AP 1
23: RTN
*/
int main() {
  std::vector<Instruction> prog;
  prog.push_back(Instruction(DUM, 1));
  prog.push_back(Instruction(LDF, 5));
  prog.push_back(Instruction(LDF, 20));
  prog.push_back(Instruction(RAP, 1));
  prog.push_back(Instruction(RTN));
// factorial=5: ; (x)
  prog.push_back(Instruction(LD, 0, 0));
  prog.push_back(Instruction(LDC, 1));
  prog.push_back(Instruction(CEQ));
  prog.push_back(Instruction(SEL, 10, 12));
  prog.push_back(Instruction(RTN));
// one=10:
  prog.push_back(Instruction(LDC, 1));
  prog.push_back(Instruction(JOIN));
// notOne=12:
  prog.push_back(Instruction(LD, 0, 0));
  prog.push_back(Instruction(LD, 0, 0));
  prog.push_back(Instruction(LDC, 1));
  prog.push_back(Instruction(SUB));
  prog.push_back(Instruction(LD, 1, 0));
  prog.push_back(Instruction(AP, 1));
  prog.push_back(Instruction(MUL));
  prog.push_back(Instruction(JOIN));
// main=20:
  prog.push_back(Instruction(LDC, 7));
  prog.push_back(Instruction(LD, 0, 0));
  prog.push_back(Instruction(AP, 1));
  prog.push_back(Instruction(RTN));
  GccInterpreter gcc(prog);
  std::cout << gcc.runMain() << std::endl;
  gcc.gc.collect();
}
