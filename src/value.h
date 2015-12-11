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

int round_to_int(value *x);

int get_constant(char *identifier, value *val);
int call_function(char *identifier, value *result, int argc, value argv[],
		  bool degrees);

int add(value *left, value *right, value *result);
int subtract(value *left, value *right, value *result);
int multiply(value *left, value *right, value *result);
int divide(value *left, value *right, value *result);
int power(value *left, value *right, value *result);
int modulo(value *left, value *right, value *result);
int negate(value *right, value *result);
int equal(value *left, value *right, value *result);
int not_equal(value *left, value *right, value *result);
int less_than(value *left, value *right, value *result);
int less_than_eq(value *left, value *right, value *result);
int greater_than(value *left, value *right, value *result);
int greater_than_eq(value *left, value *right, value *result);
int and(value *left, value *right, value *result);
int or(value *left, value *right, value *result);
int not(value *right, value *result);
int bit_and(value *left, value *right, value *result);
int bit_or(value *left, value *right, value *result);
int bit_xor(value *left, value *right, value *result);
int bit_not(value *right, value *result);
int bit_shift_left(value *left, value *right, value *result);
int bit_shift_right(value *left, value *right, value *result);
int conditional(value *condition, value *on_true, value *on_false,
		 value *result);

#endif
