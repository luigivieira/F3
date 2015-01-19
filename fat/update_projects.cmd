:;# 
:;# Copyright (C) 2015 Luiz Carlos Vieira (http://www.luiz.vieira.nom.br)
:;#
:;# This file is part of Fun from Faces (f3).
:;#
:;# f3 is free software: you can redistribute it and/or modify
:;# it under the terms of the GNU General Public License as published by
:;# the Free Software Foundation, either version 3 of the License, or
:;# (at your option) any later version.
:;#
:;# f3 is distributed in the hope that it will be useful,
:;# but WITHOUT ANY WARRANTY; without even the implied warranty of
:;# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
:;# GNU General Public License for more details.
:;#
:;# You should have received a copy of the GNU General Public License
:;# along with this program. If not, see <http://www.gnu.org/licenses/>.
:;#

:;#
:;# This is a combined Batch (Windows) + Bash (Linux) command file that:
:;# - (re)creates the Visual Studio project files (if ran on Windows)
:;# - (re)creates the project Makefile (if ran on Linux)
:;#
:;# The syntax used to create the combined Batch-Bash commands comes from this
:;# answer in StackOverflow: http://stackoverflow.com/a/17623721/2896619
:;#
:;# The .cmd extension was used just to make it less weird when used on Linux! :)
:;#
:;# Author: Luiz C. Vieira
:;# Version: 1.0
:;#

:; qmake -o Makefile fat.pro; exit
@echo off
set QMAKESPEC=win32-msvc2012
qmake fat.pro