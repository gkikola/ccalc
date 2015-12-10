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

#include <argp.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "define.h"
#include "options.h"

#define OPTION_BOOL 1
#define OPTION_VERSION 2

error_t parse_opt(int key, char *arg, struct argp_state *state);

int read_options(int argc, char *argv[], int *expr_index, options *opts) {
  //set default options
  opts->radix = 10;
  opts->precision = 6;
  opts->boolean = false;
  opts->caret_exp = false;
  opts->degrees = false;
  opts->uppercase = false;
  opts->show_time = false;
  opts->show_version = false;
  
  //option descriptions for argp  
  struct argp_option opt_desc[] = {
    { "binary", 'b', 0, 0, "Print integer results in binary (base 2)" },
    { "bool", OPTION_BOOL, 0, 0, "Interpret the result as a boolean value and "
      "print true or false" },
    { "degrees", 'd', 0, 0, "Use degrees instead of radians for trigonometric "
      "functions" },
    { 0, 'e', 0, 0, "Use caret ^ for exponentiation rather than for bitwise "
      "XOR" },
    { "octal", 'o', 0, 0, "Print integer results in octal (base 8)" },
    { "precision", 'p', "DIGITS", 0, "Print floating-point results with "
      "DIGITS digits after the decimal point (default 6)" },
    { "radix", 'r', "RADIX", 0, "Print integer results in base RADIX" },
    { "time", 't', 0, 0, "Show how much time the computation took" },
    { "uppercase", 'u', 0, 0, "Use uppercase rather than lowercase letters for "
      "digits in bases greater than 10" },
    { "hexadecimal", 'x', 0, 0, "Print integer results in hexadecimal (base "
      "16)" },
    { "version", OPTION_VERSION, 0, 0, "Display version information and "
      "exit" },
    { 0 }
  };

  //more argp setup
  struct argp args;
  args.options = opt_desc;
  args.parser = parse_opt;
  args.args_doc = "EXPRESSION";
  args.doc = "\
Evaluate the C-style EXPRESSION and display the result. If no expression is\n\
given, read from standard input.\
\v\
The EXPRESSION is interpreted as a C expression and evaluated. Integer\n\
operations are used when possible. If a floating-point value is given\n\
explicitly, or if an operation results in a non-integer value, then\n\
floating-point arithmetic is used for the remainder of the calculation.\n\
\n\
WARNING: The caret ^ indicates bitwise XOR, like in C, and not\n\
exponentiation. This behavior can be changed with the '-e' option.\n\
Exponentiation can also be performed using the pow function or (unlike in C)\n\
the ** operator.\n\
\n\
Many mathematical functions from the C standard library are available. Common\n\
mathematical constants like PI and E are also defined. Consult the program\n\
documentation for a complete list.\n\
\n\
Exit status:\n\
0  if the calculation completed successfully,\n\
1  if an invalid expression was given,\n\
2  if a system error occurred.";
  args.children = 0;
  args.help_filter = 0;
  args.argp_domain = 0;

  //parse the command-line options
  error_t result = argp_parse(&args, argc, argv, ARGP_NO_ARGS,
                              expr_index, opts);

  //do error checking
  if (result) {
    switch (result) {
    case ENOMEM:
      fprintf(stderr, "Error: memory allocation error\n");
      return ERROR_SYS;
    case EINVAL:
      fprintf(stderr, "Error: invalid option\n");
      return ERROR_SYS;
    default:
      fprintf(stderr, "Error reading program arguments\n");
      return ERROR_SYS;
    }
  }

  return SUCCESS;
}

error_t parse_opt(int key, char *arg, struct argp_state *state) {
  options *input = (options *)state->input;
  
  switch (key) {
  case 'b':
    input->radix = 2;
    break;
  case OPTION_BOOL:
    input->boolean = true;
    break;
  case 'd':
    input->degrees = true;
    break;
  case 'e':
    input->caret_exp = true;
    break;
  case 'o':
    input->radix = 8;
    break;
  case 'p':
    input->precision = atoi(arg);
    break;
  case 'r':
    input->radix = atoi(arg);
    break;
  case 't':
    input->show_time = true;
    break;
  case 'u':
    input->uppercase = true;
    break;
  case 'x':
    input->radix = 16;
    break;
  case OPTION_VERSION:
    input->show_version = true;
    break;
  case ARGP_KEY_ARG:
    break;
  default:
    return ARGP_ERR_UNKNOWN;
  }
  return 0;
}
