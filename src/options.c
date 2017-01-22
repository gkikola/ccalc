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

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "options.h"

void print_usage();
void print_help();

void read_options(int argc, char *argv[], int *expr_index, options *opts) {
  //set default options
  opts->radix = 10;
  opts->precision = 6;
  opts->grouping = -1;
  opts->boolean = false;
  opts->caret_exp = false;
  opts->degrees = false;
  opts->sci_notation = false;
  opts->show_time = false;
  opts->show_help = false;
  opts->show_version = false;
  opts->uppercase = false;

  int i;
  bool read_argument = false;
  int *arg = &opts->radix;
  for (i = 1; i < argc; ++i) {
    if (read_argument) {
      *arg = atoi(argv[i]);
      read_argument = false;
      continue;
    }
    
    if (argv[i][0] != '-')
      break;

    char c = argv[i][1];
    if (c == '-') { //'--': long option
      //extract option name
      int arg_length = strlen(argv[i]);
      char* option = malloc(arg_length + 1);
      if (!option)
        raise_error(ERROR_SYS, "memory allocation error");
      strcpy(option, argv[i]);
      
      char* eq = strchr(option, '=');      
      if (eq)
        *eq = '\0';
      int opt_length = strlen(option);
      
      if (!strcmp(argv[i], "--")) { //end of options
        ++i;
        break;
      } else if (!strcmp(option, "--binary")) {
        opts->radix = 2;
      } else if (!strcmp(option, "--bool")) {
        opts->boolean = true;
      } else if (!strcmp(option, "--caret-exp")) {
        opts->caret_exp = true;
      } else if (!strcmp(option, "--degrees")) {
        opts->degrees = true;
      } else if (!strcmp(option, "--grouping")) {
        read_argument = true;
        arg = &opts->grouping;
      } else if (!strcmp(option, "--octal")) {
        opts->radix = 8;
      } else if (!strcmp(option, "--precision")) {
        read_argument = true;
        arg = &opts->precision;
      } else if (!strcmp(option, "--radix")) {
        read_argument = true;
        arg = &opts->radix;
      } else if (!strcmp(option, "--scientific-notation")) {
        opts->sci_notation = true;
      } else if (!strcmp(option, "--time")) {
        opts->show_time = true;
      } else if (!strcmp(option, "--uppercase")) {
        opts->uppercase = true;
      } else if (!strcmp(option, "--hexadecimal")) {
        opts->radix = 16;
      } else if (!strcmp(option, "--help")) {
        print_help();
        opts->show_help = true;
        return;
      } else if (!strcmp(option, "--usage")) {
        print_usage();
        opts->show_help = true;
        return;
      } else if (!strcmp(option, "--version")) {
        opts->show_version = true;
        return;
      } else {
        raise_error(ERROR_SYS, "invalid option '%s'", argv[i] + 2);
      }

      if (opt_length != arg_length) {
        if (!read_argument)
          raise_error(ERROR_SYS, "unexpected argument for option '%s'",
                      option);
        else {
          char* val = argv[i] + opt_length + 1;
          if (*val == '\0')
            raise_error(ERROR_SYS, "expected argument for option '%s'",
                        option);
          
          *arg = atoi(val);
          read_argument = false;
        }
      }

      free(option);
    } else if (!isalpha(c) && c != '?' && c != '=') { //not an option
      break;
    } else { //short options
      int length = strlen(argv[i]);
      for (int pos = 1; pos < length; ++pos) {
        switch (argv[i][pos]) {
        case 'b':
          opts->radix = 2;
          break;
        case 'c':
          opts->caret_exp = true;
          break;
        case 'd':
          opts->degrees = true;
          break;
        case 'g':
          read_argument = true;
          arg = &opts->grouping;
          break;
        case 'o':
          opts->radix = 8;
          break;
        case 'p':
          read_argument = true;
          arg = &opts->precision;
          break;
        case 'r':
          read_argument = true;
          arg = &opts->radix;
          break;
        case 's':
          opts->sci_notation = true;
          break;
        case 't':
          opts->show_time = true;
          break;
        case 'u':
          opts->uppercase = true;
          break;
        case 'x':
          opts->radix = 16;
          break;
        case '?':
          print_help();
          opts->show_help = true;
          return;
        case '=':
          if (!read_argument) {
            if (pos > 0)
              raise_error(ERROR_SYS, "unexpected argument for option '%c'",
                          argv[i][pos - 1]);
            else
              raise_error(ERROR_SYS, "invalid option '='");
          }

          char* val = argv[i] + pos + 1;
          *arg = atoi(val);
          pos += strlen(val);
          read_argument = false;
          break;
        default:
          raise_error(ERROR_SYS, "invalid option '%c'", argv[i][pos]);
        } //end switch

        if (read_argument && pos < length - 1 && argv[i][pos + 1] != '=')
          raise_error(ERROR_SYS, "expected argument for option '%c'",
                      argv[i][pos]);
      } //end for
    } //end else
  }

  *expr_index = i;

  //set default grouping if user didn't specify otherwise
  if (opts->grouping < 0) {
    switch (opts->radix) {
    default:
      opts->grouping = 0;
      break;
    case 2: //binary
      opts->grouping = 8;
      break;
    case 8: //octal
    case 16: //hexadecimal
      opts->grouping = 4;
      break;
    }
  }
}

void print_usage() {
  printf("\
Usage: ccalc [-bcdostux?] [-g DIGITS] [-p DIGITS] [-r RADIX] [--binary]\n\
            [--bool] [--caret-exp] [--degrees] [--grouping=DIGITS] [--octal]\n\
            [--precision=DIGITS] [--radix=RADIX] [--scientific-notation]\n\
            [--time] [--uppercase] [--hexadecimal] [--help] [--usage]\n\
            [--version] EXPRESSION\n");
}

void print_help() {
  printf("\
Usage: ccalc [OPTION...] EXPRESSION\n\
Evaluate the C-style EXPRESSION and display the result. If no expression is\n\
given, read from standard input.\n\
\n\
  -b, --binary               Print integer results in binary (base 2)\n\
      --bool                 Interpret the result as a boolean value and\n\
                             print true or false\n\
  -c, --caret-exp            Use caret ^ for exponentiation rather than for\n\
                             bitwise XOR\n\
  -d, --degrees              Use degrees instead of radians for trigonometric\n\
                             functions\n\
  -g, --grouping=DIGITS      Group each set of DIGITS digits and separate\n\
                             each group with spaces (use 0 for no grouping)\n\
  -o, --octal                Print integer results in octal (base 8)\n\
  -p, --precision=DIGITS     Print floating-point results with DIGITS digits\n\
                             after the decimal point (default 6)\n\
  -r, --radix=RADIX          Print integer results in base RADIX\n\
  -s, --scientific-notation  Always print floating-point results in\n\
                             scientific notation, [-]d.ddde±dd\n\
  -t, --time                 Show how much time the computation took\n\
  -u, --uppercase            Use uppercase rather than lowercase letters for\n\
                             digits in bases greater than 10\n\
  -x, --hexadecimal          Print integer results in hexadecimal (base 16)\n\
  -?, --help                 Give this help list\n\
      --usage                Give a short usage message\n\
      --version              Display version information and exit\n\
\n\
Mandatory or optional arguments to long options are also mandatory or\n\
optional for any corresponding short options.\n\
\n\
The EXPRESSION is interpreted as a C expression and evaluated. Integer\n\
operations are used when possible. If a floating-point value is given\n\
explicitly, or if an operation results in a non-integer value, then\n\
floating-point arithmetic is used for the remainder of the calculation.\n\
\n\
WARNING: The caret ^ indicates bitwise XOR, like in C, and not\n\
exponentiation. This behavior can be changed with the '-c' option.\n\
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
2  if a system error occurred.\n");
}
