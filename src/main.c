/* ccalc -- evaluate C-style expressions from the command line.
   Copyright (C) 2015-2017 Gregory Kikola.

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

#define BUFFER_SIZE 128

void process_expression(char *expression, options *opts);

void print_value(value *val, options *opts);
void print_int(long value, int base, bool uppercase);
void print_version();

ssize_t readline(char **lineptr, size_t *n, FILE *stream);

int main(int argc, char *argv[]) {
  //parse program arguments
  options opts;
  int expr_index;

  read_options(argc, argv, &expr_index, &opts);

  if (opts.show_help) { //help text was shown, now exit
    return 0;
  }
  
  if (opts.show_version) { //show version information
    print_version();
    return 0;
  }

  if (opts.precision < 0)
    raise_error(ERROR_EXPR, "precision cannot be less than 0");

  //seed RNG
  srand(time(NULL));
  
  char *expression = NULL;
  int expr_length = 0;

  //figure out how long the expression string is
  for (int i = expr_index; i < argc; i++) {
    expr_length += strlen(argv[i]);
  }

  if (expr_length > 0) { //copy expression string
    expression = malloc(expr_length + 1);

    if (!expression)
      raise_error(ERROR_EXPR, "memory allocation error");

    expression[0] = '\0';

    for (int i = expr_index; i < argc; i++) {
      strcat(expression, argv[i]);
    }

    process_expression(expression, &opts);

    if (expression)
      free(expression);
  } else { //no expression given, read from standard input
    bool done = false;
    size_t n;

    while (!done) {
      expression = NULL;
      n = 0;
      expr_length = readline(&expression, &n, stdin);

      if (expr_length > 0) //we have something to evaluate
	process_expression(expression, &opts);
      else //reached eof
	done = true;

      if (expression)
	free(expression);
    }
  }

  return 0;
}

void process_expression(char *expression, options *opts) {
  value result_value;
  struct timeval tv_start, tv_end;
  value_set_int(&result_value, 0);

  if (opts->show_time)
    gettimeofday(&tv_start, NULL);
  
  evaluate(expression, &result_value, opts);

  if (opts->show_time)
    gettimeofday(&tv_end, NULL);

  //print the calculated value
  print_value(&result_value, opts);
  
  //show the elapsed time if requested
  if (opts->show_time) {
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

void print_value(value *val, options *opts) {
  if (opts->boolean) { // -- boolean value --
    bool result;

    if (val->type == INT)
      result = value_get_int(val);
    else
      result = value_get_float(val);

    if (opts->uppercase)
      printf("%s", result ? "TRUE" : "FALSE");
    else
      printf("%s", result ? "true" : "false");
  } else if (val->type == INT) { // -- integer value --
    long svalue = value_get_int(val);
    unsigned long uvalue;

    if (opts->radix <= 1)
      raise_error(ERROR_EXPR, "radix cannot be less than 2");

    if (svalue < 0) { //negative number, need to print minus sign
      printf("-");
      uvalue = -svalue;
    } else {
      uvalue = svalue;
    }

    if (uvalue == 0) {
      printf("0");
    } else {
      int num_digits = floor(log(uvalue) / log(opts->radix)) + 2;
      int *digit_str = malloc(num_digits * sizeof(int));
      int place = num_digits - 1;

      if (!digit_str)
	raise_error(ERROR_SYS, "memory allocation failed");

      //compute digits
      while (place >= 0) {
	digit_str[place--] = uvalue % opts->radix;
	uvalue /= opts->radix;
      }

      //find first nonzero digit
      int start = 0;
      while (start < num_digits && digit_str[start] == 0)
	start++;
      
      //print each digit
      for (int i = start; i < num_digits; i++) {
	//print spacer
	if (i > start) {
	  if (opts->grouping > 0 && (num_digits - i) % opts->grouping == 0) {
	    printf(" ");

	    if (opts->radix > 16)
	      printf(": ");
	  } else if (opts->radix > 16)
	    printf(":");
	}
      
	int cur_digit = digit_str[i];

	if (opts->radix <= 16 && cur_digit >= 10) {
	  if (opts->uppercase)
	    printf("%c", 'A' + (cur_digit - 10));
	  else
	    printf("%c", 'a' + (cur_digit - 10));
	} else {
	  printf("%d", cur_digit);
	}
      }

      free(digit_str);
    }
  } else { // -- floating-point value --
    double value = value_get_float(val);
    bool sci_not = false;

    if (value < 0) { //print minus sign for negative number
      printf("-");
      value = -value;
    }

    if (opts->sci_notation || (value > 0 && value < 1.e-6) || value >= 1.e9)
      sci_not = true;

    int max_len = abs(floor(log10(value + 1))) + opts->precision + 12;
    char flt_str[max_len];

    if (sci_not)
      snprintf(flt_str, max_len, "%.*e", value, opts->precision);
    else
      snprintf(flt_str, max_len, "%.*f", value, opts->precision);

    int str_len = strlen(flt_str);
    
    //determine location of decimal point
    int dec_pos = 0;
    while (dec_pos < str_len && flt_str[dec_pos] != '.')
      dec_pos++;

    //print the string
    bool in_exponent = false;
    for (int i = 0; i < str_len; i++) {
      if (flt_str[i] == 'E' || flt_str[i] == 'e')
	in_exponent = true;
      
      //print spacers
      if (!in_exponent && opts->grouping > 0) {
	if (i > 0 && i < dec_pos && (dec_pos - i) % opts->grouping == 0)
	  printf(" ");
	else if (i > dec_pos + 1 && (i - dec_pos - 1) % opts->grouping == 0)
	  printf(" ");
      }

      //print digit from number string
      printf("%c", flt_str[i]);
    }
  }
}

void print_version() {
  printf("\
ccalc 1.01\n\
Copyright (C) 2015-2017 Gregory Kikola\n\
License GPLv3+: GNU GPL version 3 or later\n\
<http://www.gnu.org/licenses/gpl.html>.\n\
This is free software: you are free to change and redistribute it.\n\
There is NO WARRANTY, to the extent permitted by law.\n\
\n\
Written by Gregory Kikola <gkikola@gmail.com>.\n");
}

//readline function based on GNU getline
//we're avoiding getline in order to maintain portability
ssize_t readline(char **lineptr, size_t *n, FILE *stream) {
  if (lineptr == NULL || n == NULL) {
    raise_error(ERROR_SYS, "unexpected NULL pointer");
    return -1;
  }

  //allocate initial memory if necessary
  if (*lineptr == NULL || *n == 0) {
    *n = BUFFER_SIZE;
    *lineptr = (char *)malloc(*n);
    if (*lineptr == NULL) {
      raise_error(ERROR_SYS, "memory allocation failure");
      return -1;
    }
  }

  if (feof(stream) || ferror(stream))
    return -1;

  char buffer[BUFFER_SIZE];
  ssize_t read = 0;
  for (;;) {
    if (!fgets(buffer, BUFFER_SIZE, stream))
      return read;

    int length = strlen(buffer);
    
    if (length <= 0)
      return read;
    else if (buffer[length - 1] == '\n') {
      buffer[length - 1] == '\0';
      --length;
    }

    strcpy(*lineptr + read, buffer);
    read += length;

    if (length + 1 < BUFFER_SIZE) {
      return read;
    }

    *lineptr = (char *)realloc(*lineptr, read + BUFFER_SIZE);
  }

  return read;
}
