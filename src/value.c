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

#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "define.h"
#include "value.h"

void value_set_int(value *val, long int ivalue) {
  number data;
  
  data.ivalue = ivalue;
  val->data = data;
  val->type = INT;
}

void value_set_float(value *val, double fvalue) {
  number data;

  data.fvalue = fvalue;
  val->data = data;
  val->type = FLOAT;
}

long int value_get_int(value *val) {
  switch (val->type) {
  case INT:
    return val->data.ivalue;
  default:
  case FLOAT:
    return (long int)val->data.fvalue;
  }
}

double value_get_float(value *val) {
  switch (val->type) {
  case INT:
    return val->data.ivalue;
  default:
  case FLOAT:
    return val->data.fvalue;
  }
}

int round_to_int(value *x) {
  if (x->type == INT)
    return SUCCESS;
  else {
    double fval = value_get_float(x);
    
    //see if value will fit in a long int
    double lower = nextafter(LONG_MIN + 0.5, 0);
    double upper = nextafter(LONG_MAX + 0.5, 0);
    if (fval >= lower && fval <= upper) {
      x->type = INT;
      x->data.ivalue = (fval >= 0)
                         ? (long)(fval + 0.5)
                         : (long)(fval - 0.5);
    } else {
      x->type = FLOAT;
      x->data.fvalue = round(fval);
    }

    return SUCCESS;
  }
}

int get_constant(char *identifier, value *val) {
  if (!strcmp(identifier, "PI")) {
    val->type = FLOAT;
    val->data.fvalue = M_PI;
  } else if (!strcmp(identifier, "E")) {
    val->type = FLOAT;
    val->data.fvalue = M_E;
  } else if (!strcmp(identifier, "PHI")) {
    val->type = FLOAT;
    val->data.fvalue = 1.6180339887498948482;
  } else if (!strcmp(identifier, "RAND_MAX")) {
    val->type = INT;
    val->data.ivalue = RAND_MAX;
  } else if (!strcmp(identifier, "CHAR_MAX")) {
    val->type = INT;
    val->data.ivalue = CHAR_MAX;
  } else if (!strcmp(identifier, "CHAR_MIN")) {
    val->type = INT;
    val->data.ivalue = CHAR_MIN;
  } else if (!strcmp(identifier, "DBL_DIG")) {
    val->type = INT;
    val->data.ivalue = DBL_DIG;
  } else if (!strcmp(identifier, "DBL_EPSILON")) {
    val->type = FLOAT;
    val->data.fvalue = DBL_EPSILON;
  } else if (!strcmp(identifier, "DBL_MANT_DIG")) {
    val->type = INT;
    val->data.ivalue = DBL_MANT_DIG;
  } else if (!strcmp(identifier, "DBL_MAX")) {
    val->type = FLOAT;
    val->data.fvalue = DBL_MAX;
  } else if (!strcmp(identifier, "DBL_MAX_EXP")) {
    val->type = INT;
    val->data.ivalue = DBL_MAX_EXP;
  } else if (!strcmp(identifier, "DBL_MIN")) {
    val->type = FLOAT;
    val->data.fvalue = DBL_MIN;
  } else if (!strcmp(identifier, "DBL_MIN_EXP")) {
    val->type = INT;
    val->data.ivalue = DBL_MIN_EXP;
  } else if (!strcmp(identifier, "FLT_DIG")) {
    val->type = INT;
    val->data.ivalue = FLT_DIG;
  } else if (!strcmp(identifier, "FLT_EPSILON")) {
    val->type = FLOAT;
    val->data.fvalue = FLT_EPSILON;
  } else if (!strcmp(identifier, "FLT_MANT_DIG")) {
    val->type = INT;
    val->data.ivalue = FLT_MANT_DIG;
  } else if (!strcmp(identifier, "FLT_MAX")) {
    val->type = FLOAT;
    val->data.fvalue = FLT_MAX;
  } else if (!strcmp(identifier, "FLT_MAX_EXP")) {
    val->type = INT;
    val->data.ivalue = FLT_MAX_EXP;
  } else if (!strcmp(identifier, "FLT_MIN")) {
    val->type = FLOAT;
    val->data.fvalue = FLT_MIN;
  } else if (!strcmp(identifier, "FLT_MIN_EXP")) {
    val->type = INT;
    val->data.ivalue = FLT_MIN_EXP;
  } else if (!strcmp(identifier, "FLT_RADIX")) {
    val->type = INT;
    val->data.ivalue = FLT_RADIX;
  } else if (!strcmp(identifier, "INT_MAX")) {
    val->type = INT;
    val->data.ivalue = INT_MAX;
  } else if (!strcmp(identifier, "INT_MIN")) {
    val->type = INT;
    val->data.ivalue = INT_MIN;
  } else if (!strcmp(identifier, "LONG_MAX")) {
    val->type = INT;
    val->data.ivalue = LONG_MAX;
  } else if (!strcmp(identifier, "LONG_MIN")) {
    val->type = INT;
    val->data.ivalue = LONG_MIN;
  } else if (!strcmp(identifier, "SCHAR_MAX")) {
    val->type = INT;
    val->data.ivalue = SCHAR_MAX;
  } else if (!strcmp(identifier, "SCHAR_MIN")) {
    val->type = INT;
    val->data.ivalue = SCHAR_MIN;
  } else if (!strcmp(identifier, "SHRT_MAX")) {
    val->type = INT;
    val->data.ivalue = SHRT_MAX;
  } else if (!strcmp(identifier, "SHRT_MIN")) {
    val->type = INT;
    val->data.ivalue = SHRT_MIN;
  } else if (!strcmp(identifier, "UCHAR_MAX")) {
    val->type = INT;
    val->data.ivalue = UCHAR_MAX;
  } else if (!strcmp(identifier, "USHRT_MAX")) {
    val->type = INT;
    val->data.ivalue = USHRT_MAX;
  } else {
    fprintf(stderr, "Error: unknown identifier '%s'\n", identifier);
    return ERROR_EXPR;
  }
  
  return SUCCESS;
}

int call_function(char *identifier, value *result, int argc, value argv[],
		  bool degrees) {
  bool bad_args = false;
  double arg = 0.0;

  if (!strcmp(identifier, "pow")) {
    if (argc == 2) {
      return power(&argv[0], &argv[1], result);
    } else bad_args = true;
  } else if (!strcmp(identifier, "sqrt")) {
    if (argc == 1) {
      result->type = FLOAT;
      result->data.fvalue = sqrt(value_get_float(&argv[0]));
    } else bad_args = true;
  } else if (!strcmp(identifier, "cbrt")) {
    if (argc == 1) {
      result->type = FLOAT;
      result->data.fvalue = cbrt(value_get_float(&argv[0]));
    } else bad_args = true;
  } else if (!strcmp(identifier, "abs")) {
    if (argc == 1) {
      if (argv[0].type == INT) {
	result->type = INT;
	result->data.ivalue = abs(value_get_int(&argv[0]));
      } else if (argv[0].type == FLOAT) {
	result->type = FLOAT;
	result->data.fvalue = fabs(value_get_float(&argv[0]));
      }
    } else bad_args = true;
  } else if (!strcmp(identifier, "floor")) {
    if (argc == 1) {
      if (argv[0].type == INT) {
	*result = argv[0];
      } else if (argv[0].type == FLOAT) {
	result->type = FLOAT;
	result->data.fvalue = floor(value_get_float(&argv[0]));

	//convert to int if possible
	return round_to_int(result);
      }
    } else bad_args = true;
  } else if (!strcmp(identifier, "ceil")) {
    if (argc == 1) {
      if (argv[0].type == INT) {
	*result = argv[0];
      } else if (argv[0].type == FLOAT) {
	result->type = FLOAT;
	result->data.fvalue = ceil(value_get_float(&argv[0]));

	//convert to int if possible
	return round_to_int(result);
      }
    } else bad_args = true;
  } else if (!strcmp(identifier, "trunc")) {
    if (argc == 1) {
      if (argv[0].type == INT) {
	*result = argv[0];
      } else if (argv[0].type == FLOAT) {
	result->type = FLOAT;
	result->data.fvalue = trunc(value_get_float(&argv[0]));

	//convert to int if possible
	return round_to_int(result);
      }
    } else bad_args = true;
  } else if (!strcmp(identifier, "round")) {
    if (argc == 1) {
      *result = argv[0];

      return round_to_int(result);
    } else bad_args = true;
  } else if (!strcmp(identifier, "sin")) {
    if (argc == 1) {
      arg = value_get_float(&argv[0]);
      if (degrees) {
	arg *= M_PI / 180;
      }
      
      result->type = FLOAT;
      result->data.fvalue = sin(arg);
    } else bad_args = true;
  } else if (!strcmp(identifier, "cos")) {
    if (argc == 1) {
      arg = value_get_float(&argv[0]);
      if (degrees) {
	arg *= M_PI / 180;
      }
      result->type = FLOAT;
      result->data.fvalue = cos(arg);
    } else bad_args = true;
  } else if (!strcmp(identifier, "tan")) {
    if (argc == 1) {
      arg = value_get_float(&argv[0]);
      if (degrees) {
	arg *= M_PI / 180;
      }
      result->type = FLOAT;
      result->data.fvalue = tan(arg);
    } else bad_args = true;
  } else if (!strcmp(identifier, "asin")) {
    if (argc == 1) {
      result->type = FLOAT;
      result->data.fvalue = asin(value_get_float(&argv[0]));

      if (degrees)
	result->data.fvalue *= 180 / M_PI;
    } else bad_args = true;
  } else if (!strcmp(identifier, "acos")) {
    if (argc == 1) {
      result->type = FLOAT;
      result->data.fvalue = acos(value_get_float(&argv[0]));

      if (degrees)
	result->data.fvalue *= 180 / M_PI;
    } else bad_args = true;
  } else if (!strcmp(identifier, "atan")) {
    if (argc == 1) {
      result->type = FLOAT;
      result->data.fvalue = atan(value_get_float(&argv[0]));

      if (degrees)
	result->data.fvalue *= 180 / M_PI;
    } else bad_args = true;
  } else if (!strcmp(identifier, "atan2")) {
    if (argc == 2) {
      result->type = FLOAT;
      result->data.fvalue = atan2(value_get_float(&argv[0]),
				  value_get_float(&argv[1]));

      if (degrees)
	result->data.fvalue *= 180 / M_PI;
    } else bad_args = true;
  } else if (!strcmp(identifier, "exp")) {
    if (argc == 1) {
      result->type = FLOAT;
      result->data.fvalue = exp(value_get_float(&argv[0]));
    } else bad_args = true;
  } else if (!strcmp(identifier, "exp2")) {
    if (argc == 1) {
      value two;
      two.type = INT;
      two.data.ivalue = 2;

      return power(&two, &argv[0], result);
    } else bad_args = true;
  } else if (!strcmp(identifier, "log")) {
    if (argc == 1) {
      result->type = FLOAT;
      result->data.fvalue = log(value_get_float(&argv[0]));
    } else bad_args = true;
  } else if (!strcmp(identifier, "log10")) {
    if (argc == 1) {
      result->type = FLOAT;
      result->data.fvalue = log10(value_get_float(&argv[0]));
    } else bad_args = true;
  } else if (!strcmp(identifier, "log2")) {
    if (argc == 1) {
      result->type = FLOAT;
      result->data.fvalue = log2(value_get_float(&argv[0]));
    } else bad_args = true;
  } else if (!strcmp(identifier, "rand")) {
    if (argc == 0) {
      result->type = INT;
      result->data.ivalue = rand();
    } else bad_args = true;
  } else if (!strcmp(identifier, "hypot")) {
    if (argc == 2) {
      result->type = FLOAT;
      result->data.fvalue = hypot(value_get_float(&argv[0]),
				  value_get_float(&argv[1]));
    } else bad_args = true;
  } else if (!strcmp(identifier, "expm1")) {
    if (argc == 1) {
      result->type = FLOAT;
      result->data.fvalue = expm1(value_get_float(&argv[0]));
    } else bad_args = true;
  } else if (!strcmp(identifier, "log1p")) {
    if (argc == 1) {
      result->type = FLOAT;
      result->data.fvalue = log1p(value_get_float(&argv[0]));
    } else bad_args = true;
  } else if (!strcmp(identifier, "sinh")) {
    if (argc == 1) {
      result->type = FLOAT;
      result->data.fvalue = sinh(value_get_float(&argv[0]));
    } else bad_args = true;
  } else if (!strcmp(identifier, "cosh")) {
    if (argc == 1) {
      result->type = FLOAT;
      result->data.fvalue = cosh(value_get_float(&argv[0]));
    } else bad_args = true;
  } else if (!strcmp(identifier, "tanh")) {
    if (argc == 1) {
      result->type = FLOAT;
      result->data.fvalue = tanh(value_get_float(&argv[0]));
    } else bad_args = true;
  } else if (!strcmp(identifier, "asinh")) {
    if (argc == 1) {
      result->type = FLOAT;
      result->data.fvalue = asinh(value_get_float(&argv[0]));
    } else bad_args = true;
  } else if (!strcmp(identifier, "acosh")) {
    if (argc == 1) {
      result->type = FLOAT;
      result->data.fvalue = acosh(value_get_float(&argv[0]));
    } else bad_args = true;
  } else if (!strcmp(identifier, "atanh")) {
    if (argc == 1) {
      result->type = FLOAT;
      result->data.fvalue = atanh(value_get_float(&argv[0]));
    } else bad_args = true;
  } else if (!strcmp(identifier, "erf")) {
    if (argc == 1) {
      result->type = FLOAT;
      result->data.fvalue = erf(value_get_float(&argv[0]));
    } else bad_args = true;
  } else if (!strcmp(identifier, "erfc")) {
    if (argc == 1) {
      result->type = FLOAT;
      result->data.fvalue = erfc(value_get_float(&argv[0]));
    } else bad_args = true;
  } else if (!strcmp(identifier, "lgamma")) {
    if (argc == 1) {
      result->type = FLOAT;
      result->data.fvalue = lgamma(value_get_float(&argv[0]));
    } else bad_args = true;
  } else if (!strcmp(identifier, "tgamma")) {
    if (argc == 1) {
      result->type = FLOAT;
      result->data.fvalue = tgamma(value_get_float(&argv[0]));
    } else bad_args = true;
  } else if (!strcmp(identifier, "fmod")) {
    if (argc == 2) {
      if (argv[0].type == INT && argv[1].type == INT) {
	return modulo(&argv[0], &argv[1], result);
      } else {
	result->type = FLOAT;
	result->data.fvalue = fmod(value_get_float(&argv[0]),
				   value_get_float(&argv[1]));
      }
    } else bad_args = true;
  } else if (!strcmp(identifier, "remainder")) {
    if (argc == 2) {
      result->type = FLOAT;
      result->data.fvalue = remainder(value_get_float(&argv[0]),
				      value_get_float(&argv[1]));

      //make an integer if possible
      if (argv[0].type == INT && argv[1].type == INT)
	return round_to_int(result);
    } else bad_args = true;
  } else if (!strcmp(identifier, "nextafter")) {
    if (argc == 2) {
      result->type = FLOAT;
      result->data.fvalue = nextafter(value_get_float(&argv[0]),
				      value_get_float(&argv[1]));
    } else bad_args = true;
  } else if (!strcmp(identifier, "max")) {
    if (argc == 2) {
      if (argv[0].type == INT && argv[1].type == INT) {
	int x = value_get_int(&argv[0]);
	int y = value_get_int(&argv[1]);
	
	result->type = INT;
	result->data.ivalue = (x > y) ? x : y;
      } else {
	result->type = FLOAT;
	result->data.fvalue = fmax(value_get_float(&argv[0]),
				   value_get_float(&argv[1]));
      }
    } else bad_args = true;
  } else if (!strcmp(identifier, "min")) {
    if (argc == 2) {
      if (argv[0].type == INT && argv[1].type == INT) {
	int x = value_get_int(&argv[0]);
	int y = value_get_int(&argv[1]);
	
	result->type = INT;
	result->data.ivalue = (x < y) ? x : y;
      } else {
	result->type = FLOAT;
	result->data.fvalue = fmin(value_get_float(&argv[0]),
				   value_get_float(&argv[1]));
      }
    } else bad_args = true;
  } else {
    fprintf(stderr, "Error: unknown function identifier '%s'\n", identifier);
    return ERROR_EXPR;
  }

  if (bad_args) {
    if (argc == 1)
      fprintf(stderr, "Error: function '%s' requires more than 1 argument\n",
	      identifier);
    else
      fprintf(stderr, "Error: function '%s' does not take %d arguments\n",
	      identifier, argc);
    
    return ERROR_EXPR;
  } else if (result->type == FLOAT && !isfinite(value_get_float(result))) {
    fprintf(stderr, "Error: domain error in function '%s'\n", identifier);
    return ERROR_EXPR;
  }
  
  return SUCCESS;
}

int add(value *left, value *right, value *result) {
  if (left->type == FLOAT || right->type == FLOAT) {
    result->type = FLOAT;
    result->data.fvalue = value_get_float(left) + value_get_float(right);
  } else {
    result->type = INT;
    result->data.ivalue = value_get_int(left) + value_get_int(right);
  }

  return SUCCESS;
}

int subtract(value *left, value *right, value *result) {
  if (left->type == FLOAT || right->type == FLOAT) {
    result->type = FLOAT;
    result->data.fvalue = value_get_float(left) - value_get_float(right);
  } else {
    result->type = INT;
    result->data.ivalue = value_get_int(left) - value_get_int(right);
  }

  return SUCCESS;
}

int multiply(value *left, value *right, value *result) {
  if (left->type == FLOAT || right->type == FLOAT) {
    result->type = FLOAT;
    result->data.fvalue = value_get_float(left) * value_get_float(right);
  } else {
    result->type = INT;
    result->data.ivalue = value_get_int(left) * value_get_int(right);
  }

  return SUCCESS;
}

int divide(value *left, value *right, value *result) {
  //check for division by zero
  if ((right->type == INT && right->data.ivalue == 0)
      || (right->type == FLOAT && right->data.fvalue == 0)) {
    fprintf(stderr, "Error: division by zero\n");
    return ERROR_EXPR;
  }
  
  //use int division only if numerator is perfect multiple of denominator
  if (left->type == INT && right->type == INT
      && value_get_int(left) % value_get_int(right) == 0) {
    result->type = INT;
    result->data.ivalue = value_get_int(left) / value_get_int(right);
  } else {
    result->type = FLOAT;
    result->data.fvalue = value_get_float(left) / value_get_float(right);
  }

  return SUCCESS;
}

int int_divide(value *left, value *right, value *result) {
  //make sure we have int operands
  if (left->type == FLOAT || right->type == FLOAT) {
    fprintf(stderr, "Error: integer division operator '//' requires integer "
	    "operands\n");
    return ERROR_EXPR;
  }

  //check for div by zero
  if (right->data.ivalue == 0) {
    fprintf(stderr, "Error: division by zero\n");
    return ERROR_EXPR;
  }

  result->type = INT;
  result->data.ivalue = value_get_int(left) / value_get_int(right);
  return SUCCESS;
}

int power(value *left, value *right, value *result) {
  if ((left->type == FLOAT || right->type == FLOAT)
      || right->data.ivalue < 0) {
    result->type = FLOAT;
    result->data.fvalue = pow(value_get_float(left), value_get_float(right));
  } else {
    int ivalue = 1;
    int base = value_get_int(left);
    int exp = value_get_int(right);
    while (exp) {
      if (exp & 1)
	ivalue *= base;
      exp >>= 1;
      base *= base;
    }

    result->type = INT;
    result->data.ivalue = ivalue;
  }
  return SUCCESS;
}

int modulo(value *left, value *right, value *result) {
  //make sure arguments are ints and right value is nonzero
  if (left->type == FLOAT || right->type == FLOAT) {
    fprintf(stderr, "Error: modulo operator '%' requires integer operands\n");
    return ERROR_EXPR;
  } else if (right->data.ivalue == 0) {
    fprintf(stderr, "Error: mod by zero\n");
    return ERROR_EXPR;
  } else {
    result->type = INT;
    result->data.ivalue = value_get_int(left) % value_get_int(right);
    return SUCCESS;
  }
}

int negate(value *right, value *result) {
  if (right->type == FLOAT) {
    result->type = FLOAT;
    result->data.fvalue = -value_get_float(right);
  } else {
    result->type = INT;
    result->data.ivalue = -value_get_int(right);
  }

  return SUCCESS;
}

int equal(value *left, value *right, value *result) {
  result->type = INT;
  if (left->type == FLOAT || right->type == FLOAT) {
    result->data.ivalue = value_get_float(left) == value_get_float(right);
  } else {
    result->data.ivalue = value_get_int(left) == value_get_int(right);
  }

  return SUCCESS;
}

int not_equal(value *left, value *right, value *result) {
  result->type = INT;
  if (left->type == FLOAT || right->type == FLOAT) {
    result->data.ivalue = value_get_float(left) != value_get_float(right);
  } else {
    result->data.ivalue = value_get_int(left) != value_get_int(right);
  }

  return SUCCESS;
}

int less_than(value *left, value *right, value *result) {
  result->type = INT;
  if (left->type == FLOAT || right->type == FLOAT) {
    result->data.ivalue = value_get_float(left) < value_get_float(right);
  } else {
    result->data.ivalue = value_get_int(left) < value_get_int(right);
  }

  return SUCCESS;
}

int less_than_eq(value *left, value *right, value *result) {
  result->type = INT;
  if (left->type == FLOAT || right->type == FLOAT) {
    result->data.ivalue = value_get_float(left) <= value_get_float(right);
  } else {
    result->data.ivalue = value_get_int(left) <= value_get_int(right);
  }

  return SUCCESS;
}

int greater_than(value *left, value *right, value *result) {
  result->type = INT;
  if (left->type == FLOAT || right->type == FLOAT) {
    result->data.ivalue = value_get_float(left) > value_get_float(right);
  } else {
    result->data.ivalue = value_get_int(left) > value_get_int(right);
  }

  return SUCCESS;
}

int greater_than_eq(value *left, value *right, value *result) {
  result->type = INT;
  if (left->type == FLOAT || right->type == FLOAT) {
    result->data.ivalue = value_get_float(left) >= value_get_float(right);
  } else {
    result->data.ivalue = value_get_int(left) >= value_get_int(right);
  }

  return SUCCESS;
}

int and(value *left, value *right, value *result) {
  result->type = INT;
  if (left->type == FLOAT || right->type == FLOAT) {
    result->data.ivalue = value_get_float(left) && value_get_float(right);
  } else {
    result->data.ivalue = value_get_int(left) && value_get_int(right);
  }

  return SUCCESS;
}

int or(value *left, value *right, value *result) {
  result->type = INT;
  if (left->type == FLOAT || right->type == FLOAT) {
    result->data.ivalue = value_get_float(left) || value_get_float(right);
  } else {
    result->data.ivalue = value_get_int(left) || value_get_int(right);
  }

  return SUCCESS;
}

int not(value *right, value *result) {
  result->type = INT;
  if (right->type == FLOAT) {
    result->data.ivalue = !value_get_float(right);
  } else {
    result->data.ivalue = !value_get_int(right);
  }

  return SUCCESS;
}

int bit_and(value *left, value *right, value *result) {
  if (left->type == INT && right->type == INT) {
    result->type = INT;
    result->data.ivalue = value_get_int(left) & value_get_int(right);
  } else {
    fprintf(stderr, "Error: bitwise AND operator '&' requires integer operands\n");
    return ERROR_EXPR;
  }

  return SUCCESS;
}

int bit_or(value *left, value *right, value *result) {
  if (left->type == INT && right->type == INT) {
    result->type = INT;
    result->data.ivalue = value_get_int(left) | value_get_int(right);
  } else {
    fprintf(stderr, "Error: bitwise OR operator '|' requires integer operands\n");
    return ERROR_EXPR;
  }

  return SUCCESS;
}

int bit_xor(value *left, value *right, value *result) {
  if (left->type == INT && right->type == INT) {
    result->type = INT;
    result->data.ivalue = value_get_int(left) ^ value_get_int(right);
  } else {
    fprintf(stderr, "Error: bitwise XOR operator '^' requires integer operands\n");
    return ERROR_EXPR;
  }

  return SUCCESS;
}

int bit_not(value *right, value *result) {
  if (right->type == INT) {
    result->type = INT;
    result->data.ivalue = ~value_get_int(right);
  } else {
    fprintf(stderr, "Error: bitwise NOT operator '~' requires an integer operand\n");
    return ERROR_EXPR;
  }

  return SUCCESS;
}

int bit_shift_left(value *left, value *right, value *result) {
  if (left->type == INT && right->type == INT) {
    result->type = INT;
    result->data.ivalue = value_get_int(left) << value_get_int(right);
  } else {
    fprintf(stderr, "Error: bit shift operator '<<' requires integer operands\n");
    return ERROR_EXPR;
  }

  return SUCCESS;
}

int bit_shift_right(value *left, value *right, value *result) {
  if (left->type == INT && right->type == INT) {
    result->type = INT;
    result->data.ivalue = value_get_int(left) >> value_get_int(right);
  } else {
    fprintf(stderr, "Error: bit shift operator '>>' requires integer operands\n");
    return ERROR_EXPR;
  }

  return SUCCESS;
}

int conditional(value *condition, value *on_true, value *on_false,  value *result) {
  if ((condition->type == INT && condition->data.ivalue)
      || (condition->type == FLOAT && condition->data.fvalue != 0.0))
    *result = *on_true;
  else
    *result = *on_false;

  return SUCCESS;
}
