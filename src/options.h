/* ccalc -- evaluate C-style expressions from the command line.
   Copyright (C) 2015 Gregory Kikola.

   This file is part of ccalc.

   ccalc is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   ccalc is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with ccalc.  If not, see <http://www.gnu.org/licenses/>.
*/

/* Written by Gregory Kikola <gkikola@gmail.com>. */

#ifndef CCALC_OPTIONS_H
#define CCALC_OPTIONS_H

#include <stdbool.h>

typedef struct {
  int radix;
  int precision;
  bool boolean;
  bool caret_exp;
  bool degrees;
  bool uppercase;
  bool show_time;
  bool show_version;
} options;

int read_options(int argc, char *argv[], int *expr_index, options *opts);

#endif
