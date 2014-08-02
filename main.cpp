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

#include <iostream>
#include "gccint.h"

int main() {
  std::vector<Instruction> prog;
  prog.push_back(Instruction(LDC, 21));
  prog.push_back(Instruction(LDF, 4));
  prog.push_back(Instruction(AP, 1));
  prog.push_back(Instruction(RTN));
  prog.push_back(Instruction(LD, 0, 0));
  prog.push_back(Instruction(LD, 0, 0));
  prog.push_back(Instruction(ADD));
  prog.push_back(Instruction(RTN));
  GccInterpreter gcc(prog);
  std::cout << gcc.runMain() << std::endl;
}
