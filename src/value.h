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

#ifndef CCALC_VALUE_H
#define CCALC_VALUE_H

typedef union {
  long int ivalue;
  double fvalue;
} number;

typedef struct {
  number data;
  enum { INT, FLOAT } type;
} value;

void value_set_int(value *val, long int ivalue);
void value_set_float(value *val, double fvalue);
long int value_get_int(value *val);
double value_get_float(value *val);

void round_to_int(value *x);

void get_constant(char *identifier, value *val);
void call_function(char *identifier, value *result, int argc, value argv[],
		   bool degrees);

void add(value *left, value *right, value *result);
void subtract(value *left, value *right, value *result);
void multiply(value *left, value *right, value *result);
void int_divide(value *left, value *right, value *result);
void divide(value *left, value *right, value *result);
void power(value *left, value *right, value *result);
void modulo(value *left, value *right, value *result);
void negate(value *right, value *result);
void equal(value *left, value *right, value *result);
void not_equal(value *left, value *right, value *result);
void less_than(value *left, value *right, value *result);
void less_than_eq(value *left, value *right, value *result);
void greater_than(value *left, value *right, value *result);
void greater_than_eq(value *left, value *right, value *result);
void and(value *left, value *right, value *result);
void or(value *left, value *right, value *result);
void not(value *right, value *result);
void bit_and(value *left, value *right, value *result);
void bit_or(value *left, value *right, value *result);
void bit_xor(value *left, value *right, value *result);
void bit_not(value *right, value *result);
void bit_shift_left(value *left, value *right, value *result);
void bit_shift_right(value *left, value *right, value *result);
void conditional(value *condition, value *on_true, value *on_false,
		 value *result);

#endif
