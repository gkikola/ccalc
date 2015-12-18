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

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "error.h"

void raise_error(int exit_code, char *fmt_str, ...) {
  va_list vargs;

  fprintf(stderr, "Error: ");
  
  va_start(vargs, fmt_str);
  vfprintf(stderr, fmt_str, vargs);
  va_end(vargs);

  fprintf(stderr, "\n");

  exit(exit_code);
}
