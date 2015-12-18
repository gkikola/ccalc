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

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <sys/time.h>
#include <unistd.h>

#include "error.h"
#include "evaluate.h"
#include "options.h"
#include "value.h"

int print_int(long value, int base, bool uppercase);
void print_version();

int main(int argc, char *argv[]) {
  //parse program arguments
  options opts;
  int expr_index;
  int result;

  result = read_options(argc, argv, &expr_index, &opts);

  if (result != SUCCESS)
    return result;

  if (opts.show_version) { //show version information
    print_version();
    return 0;
  }

  if (opts.precision < 0) {
    fprintf(stderr, "Error: precision cannot be less than 0\n");
    return ERROR_EXPR;
  }
  
  char *expression = NULL;
  int expr_length = 0;

  //figure out how long the expression string is
  for (int i = expr_index; i < argc; i++) {
    expr_length += strlen(argv[i]);
  }

  if (expr_length > 0) { //copy expression string
    expression = malloc(expr_length + 1);
    expression[0] = '\0';

    if (!expression) {
      fprintf(stderr, "Error: memory allocation error\n");
      return ERROR_SYS;
    }

    for (int i = expr_index; i < argc; i++) {
      strcat(expression, argv[i]);
    }
  } else { //no expression given, read from standard input
    size_t n = 0;
    expr_length = getline(&expression, &n, stdin);

    if (expr_length == 1) //just the newline
      expr_length = 0;
  }

  if (expr_length <= 0) {
    fprintf(stderr, "Error: no expression given\n");
    fprintf(stderr, "Try 'ccalc --help' for more information.\n");
    return ERROR_EXPR;
  }

  //seed RNG
  srand(time(NULL));

  //evaluate the expression, and time it if the user wants
  value result_value;
  struct timeval tv_start, tv_end;
  value_set_int(&result_value, 0);

  if (opts.show_time)
    gettimeofday(&tv_start, NULL);
  
  result = evaluate(expression, &result_value, &opts);

  if (opts.show_time)
    gettimeofday(&tv_end, NULL);

  //print the calculated value
  if (result == SUCCESS) {
    bool bresult = false;

    switch (result_value.type) {
    case INT:
      if (opts.boolean)
	bresult = value_get_int(&result_value) ? true : false;
      else
	if (opts.radix == 10) {
	  printf("%ld", value_get_int(&result_value));
	} else {
	  result = print_int(value_get_int(&result_value),
			     opts.radix, opts.uppercase);
	  if (result != SUCCESS)
	    return result;
        }
      break;
    case FLOAT:
      if (opts.boolean)
	bresult = value_get_float(&result_value) != 0.0 ? true : false;
      else
	printf("%.*f", value_get_float(&result_value), opts.precision);
      break;
    default:
      fprintf(stderr, "Error: unknown result type\n");
      return ERROR_EXPR;
    }

    if (opts.boolean) {
      if (opts.uppercase) {
	if (bresult)
	  printf("TRUE");
	else
	  printf("FALSE");
      } else {
	if (bresult)
	  printf("true");
	else
	  printf("false");
      }
    }

    //show the elapsed time if requested
    if (opts.show_time) {
      int sec = 0, usec = 0;

      sec = tv_end.tv_sec - tv_start.tv_sec;
      if (tv_end.tv_usec < tv_start.tv_usec) {
	--sec;
	usec = 1000000 - (tv_start.tv_usec - tv_end.tv_usec);
      } else {
	usec = tv_end.tv_usec - tv_start.tv_usec;
      }

      printf(" (time: %d.%06d seconds)", sec, usec);
    }

    printf("\n");
  }
  
  if (expression) {
    free(expression);
  }

  return result;
}

int print_int(long value, int base, bool uppercase) {
  if (base <= 1) {
    fprintf(stderr, "Error: radix cannot be less than 2\n");
    return ERROR_EXPR;
  }

  if (value == 0) {
    printf("0");
    return SUCCESS;
  }
  if (value < 0) {
    printf("-");
    value = -value;
  }

  if (base == 2)
    printf("0b");
  else if (base == 8)
    printf("0");
  else if (base == 16)
    printf("0x");

  //figure out how much space we need
  int digits = floor(log(value) / log(base)) + 1;
  int *digit_str = malloc(digits * sizeof(int));
  int place = digits - 1;

  if (!digit_str) {
    fprintf(stderr, "Error: memory allocation failed\n");
    return ERROR_SYS;
  }

  while (value > 0 && place >= 0) {
    digit_str[place--] = value % base;
    value /= base;
  }

  for (int i = 0; i < digits; i++) {
    if (base > 16) {
      if (i > 0)
	printf(":");

      printf("%d", digit_str[i]);
    } else {
      int cur_digit = digit_str[i];

      if (cur_digit < 10) {
	printf("%d", cur_digit);
      } else {
	if (uppercase)
	  printf("%c", 'A' + (cur_digit - 10));
	else
	  printf("%c", 'a' + (cur_digit - 10));
      }
    }
  }
  
  free(digit_str);
  
  return SUCCESS;
}

void print_version() {
  printf("\
ccalc 0.1\n\
Copyright (C) 2015 Gregory Kikola\n\
License GPLv3+: GNU GPL version 3 or later\n\
<http://www.gnu.org/licenses/gpl.html>.\n\
This is free software: you are free to change and redistribute it.\n\
There is NO WARRANTY, to the extent permitted by law.\n\
\n\
Written by Gregory Kikola <gkikola@gmail.com>.\n");
}
