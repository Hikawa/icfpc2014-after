# icfpc2014-after: icfpc 2014 related code made after the competition was finished
# Copyright (C) 2014  aankor (The sound of lambda team)
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

testgcc: main.o gccint.o
	clang++ -g $^ -o $@

main.o: main.cpp gccint.h
	clang++ -g -c $< -o $@

gccint.o: gccint.cpp gccint.h
	clang++ -g -c $< -o $@