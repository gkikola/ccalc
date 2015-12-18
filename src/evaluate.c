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
#include <string.h>

#include "error.h"
#include "evaluate.h"

#define MAX_IDENTIFIER_LENGTH 32
#define MAX_ARGUMENTS 8

typedef enum {
  TOKEN_UNKNOWN,
  TOKEN_END,
  TOKEN_IDENTIFIER,
  TOKEN_LITERAL,
  TOKEN_LEFT_PAREN,
  TOKEN_RIGHT_PAREN,
  TOKEN_QMARK,
  TOKEN_COLON,
  TOKEN_OP_COMMA,
  TOKEN_OP_PLUS,
  TOKEN_OP_MINUS,
  TOKEN_OP_TIMES,
  TOKEN_OP_DIVIDE,
  TOKEN_OP_IDIVIDE,
  TOKEN_OP_MOD,
  TOKEN_OP_POW,
  TOKEN_OP_EQUAL,
  TOKEN_OP_NOT_EQUAL,
  TOKEN_OP_LESS_THAN,
  TOKEN_OP_LESS_THAN_EQ,
  TOKEN_OP_GREATER_THAN,
  TOKEN_OP_GREATER_THAN_EQ,
  TOKEN_OP_AND,
  TOKEN_OP_OR,
  TOKEN_OP_NOT,
  TOKEN_OP_BIT_AND,
  TOKEN_OP_BIT_OR,
  TOKEN_OP_BIT_XOR,
  TOKEN_OP_BIT_NOT,
  TOKEN_OP_BIT_SHIFT_LEFT,
  TOKEN_OP_BIT_SHIFT_RIGHT,
} token_type;

typedef struct Parser {
  char *expr;
  int pos;
  options *program_opts;

  token_type cur_token;
  char str_value[MAX_IDENTIFIER_LENGTH + 1];
  value numeric_value;
} parser;

bool is_whitespace(char c);

void get_token(parser *parse);
token_type peek_token(parser *parse);
void get_identifier(parser *parse);
void get_literal(parser *parse);

void parse_expression(parser *parse, value *val);
void parse_conditional_expression(parser *parse, value *val);
void parse_logical_or_expression(parser *parse, value *val);
void parse_logical_and_expression(parser *parse, value *val);
void parse_inclusive_or_expression(parser *parse, value *val);
void parse_exclusive_or_expression(parser *parse, value *val);
void parse_and_expression(parser *parse, value *val);
void parse_equality_expression(parser *parse, value *val);
void parse_relational_expression(parser *parse, value *val);
void parse_shift_expression(parser *parse, value *val);
void parse_additive_expression(parser *parse, value *val);
void parse_multiplicative_expression(parser *parse, value *val);
void parse_unary_expression(parser *parse, value *val);
void parse_exponential_expression(parser *parse, value *val);
void parse_primary(parser *parse, value *val);

void evaluate(char *expr, value *result, options *opts) {
  parser parse;
  parse.expr = expr;
  parse.pos = 0;
  parse.program_opts = opts;

  parse_expression(&parse, result);

  //next token should be TOKEN_END
  get_token(&parse);
  
  switch (parse.cur_token) {
  case TOKEN_END:
    break;
  case TOKEN_RIGHT_PAREN:
    raise_error(ERROR_EXPR, "unmatched parenthesis ')'");
  default:
    raise_error(ERROR_EXPR, "unexpected token '%s'", parse.str_value);
  }
}

bool is_whitespace(char c) {
  return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

void get_token(parser *parse) {
  parse->numeric_value.type = INT;
  parse->numeric_value.data.ivalue = 0;
  parse->str_value[0] = 0;

  //skip whitespace
  while (is_whitespace(parse->expr[parse->pos]))
    parse->pos++;

  parse->cur_token = peek_token(parse);

  switch (parse->cur_token) {
  case TOKEN_END:
    break;

  case TOKEN_IDENTIFIER:
    get_identifier(parse);
    break;

  case TOKEN_LITERAL:
    get_literal(parse);
    break;

  case TOKEN_LEFT_PAREN:
  case TOKEN_RIGHT_PAREN:
  case TOKEN_QMARK:
  case TOKEN_COLON:
  case TOKEN_OP_COMMA:
  case TOKEN_OP_PLUS:
  case TOKEN_OP_MINUS:
  case TOKEN_OP_TIMES:
  case TOKEN_OP_DIVIDE:
  case TOKEN_OP_MOD:
  case TOKEN_OP_LESS_THAN:
  case TOKEN_OP_GREATER_THAN:
  case TOKEN_OP_NOT:
  case TOKEN_OP_BIT_AND:
  case TOKEN_OP_BIT_OR:
  case TOKEN_OP_BIT_XOR:
  case TOKEN_OP_BIT_NOT:
    parse->str_value[0] = parse->expr[parse->pos];
    parse->str_value[1] = '\0';
    
    parse->pos++;
    break;

  case TOKEN_OP_IDIVIDE:
  case TOKEN_OP_EQUAL:
  case TOKEN_OP_NOT_EQUAL:
  case TOKEN_OP_LESS_THAN_EQ:
  case TOKEN_OP_GREATER_THAN_EQ:
  case TOKEN_OP_AND:
  case TOKEN_OP_OR:
  case TOKEN_OP_BIT_SHIFT_LEFT:
  case TOKEN_OP_BIT_SHIFT_RIGHT:
    parse->str_value[0] = parse->expr[parse->pos];
    parse->str_value[1] = parse->expr[parse->pos + 1];
    parse->str_value[2] = '\0';

    parse->pos += 2;
    break;

  case TOKEN_OP_POW:
    if (!parse->program_opts->caret_exp || parse->expr[parse->pos] == '*') {
      strncpy(parse->str_value, "**", 3);
      parse->pos += 2;
    } else if (parse->expr[parse->pos] == '^') {
      strncpy(parse->str_value, "^", 3);
      parse->pos++;
    }
  }
  
  //skip whitespace
  while (is_whitespace(parse->expr[parse->pos]))
    parse->pos++;
}

token_type peek_token(parser *parse) {
  //skip whitespace
  while (is_whitespace(parse->expr[parse->pos]))
    parse->pos++;
  
  char next_ch = parse->expr[parse->pos];

  if ((next_ch >= 'A' && next_ch <= 'Z')
      || (next_ch >= 'a' && next_ch <= 'z')
      || next_ch == '_') {
    return TOKEN_IDENTIFIER;
  } else if ((next_ch >= '0' && next_ch <= '9') || next_ch == '.') {
    return TOKEN_LITERAL;
  } else {    
    switch (next_ch) {
    case 0:
      return TOKEN_END;
    case '(':
      return TOKEN_LEFT_PAREN;
    case ')':
      return TOKEN_RIGHT_PAREN;
    case '?':
      return TOKEN_QMARK;
    case ':':
      return TOKEN_COLON;
    case ',':
      return TOKEN_OP_COMMA;
    case '+':
      return TOKEN_OP_PLUS;
    case '-':
      return TOKEN_OP_MINUS;

    case '*':
      //could be * or **
      if (parse->expr[parse->pos + 1] == '*')
	return TOKEN_OP_POW;
      else
	return TOKEN_OP_TIMES;

    case '/':
      //could be / or //
      if (parse->expr[parse->pos + 1] == '/')
	return TOKEN_OP_IDIVIDE;
      else
	return TOKEN_OP_DIVIDE;

    case '%':
      return TOKEN_OP_MOD;

    case '=':
      //must be ==, assignment not supported
      if (parse->expr[parse->pos + 1] == '=') {
	return TOKEN_OP_EQUAL;
      }
      else {
        raise_error(ERROR_EXPR, "assignment operator '=' is not supported");
	return TOKEN_UNKNOWN;
      }

    case '!':
      //could be ! or !=
      if (parse->expr[parse->pos + 1] == '=')
	return TOKEN_OP_NOT_EQUAL;
      else
	return TOKEN_OP_NOT;

    case '<':
      //could be <, <=, or <<
      if (parse->expr[parse->pos + 1] == '=')
	return TOKEN_OP_LESS_THAN_EQ;
      else if (parse->expr[parse->pos + 1] == '<')
	return TOKEN_OP_BIT_SHIFT_LEFT;
      else
	return TOKEN_OP_LESS_THAN;

    case '>':
      //could be >, >=, or >>
      if (parse->expr[parse->pos + 1] == '=')
	return TOKEN_OP_GREATER_THAN_EQ;
      else if (parse->expr[parse->pos + 1] == '>')
	return TOKEN_OP_BIT_SHIFT_RIGHT;
      else
	return TOKEN_OP_GREATER_THAN;

    case '&':
      //could be & or &&
      if (parse->expr[parse->pos + 1] == '&')
	return TOKEN_OP_AND;
      else
	return TOKEN_OP_BIT_AND;

    case '|':
      //could be | or ||
      if (parse->expr[parse->pos + 1] == '|')
	return TOKEN_OP_OR;
      else
	return TOKEN_OP_BIT_OR;

    case '^':
      if (parse->program_opts->caret_exp)
	return TOKEN_OP_POW;
      else
	return TOKEN_OP_BIT_XOR;

    case '~':
      return TOKEN_OP_BIT_NOT;

    default:
      raise_error(ERROR_EXPR, "unexpected character '%c'", next_ch);
      return TOKEN_UNKNOWN;
    }
  }
}

void get_literal(parser *parse) {
  int base = 10;
  bool is_float = false;
  int start_pos = parse->pos;
  
  //check for a decimal point
  int pos = parse->pos;
  while (true) {
    char cur_ch = parse->expr[pos];

    if ((cur_ch < '0' || cur_ch > '9') && (cur_ch < 'A' || cur_ch > 'F')
	&& (cur_ch < 'a' || cur_ch > 'f') && cur_ch != '.')
      break;
    else if (cur_ch == '.')
      is_float = true;
    
    pos++;
  }

  //look for binary, hex, or octal prefix
  if (parse->expr[parse->pos] == '0') {
    switch (parse->expr[parse->pos + 1]) {
    case 'x':
    case 'X':
      base = 16;
      parse->pos += 2;
      break;
    case 'b':
    case 'B':
      base = 2;
      parse->pos += 2;
      break;
    default:
      if (!is_float) {
	base = 8;
	parse->pos += 1;
      }
      break;
    }
  }

  value result;
  result.type = INT;
  result.data.ivalue = 0;

  int float_exponent = 0;
  double float_frac_factor = 0.1;
  bool reading_exponent = false;
  bool negative_exponent = false;

  bool done = false;
  while(!done) {
    char cur_ch = parse->expr[parse->pos];

    if (result.type == INT) {
      switch(cur_ch) {
      default:
	done = true;
	break;
      case '0': case '1': case '2': case '3': case '4':
      case '5': case '6': case '7': case '8': case '9':
	if (base == 2 && cur_ch > '1')
          raise_error(ERROR_EXPR, "unexpected digit '%c' in binary constant",
		      cur_ch);
        else if (base == 8 && cur_ch > '7')
          raise_error(ERROR_EXPR, "unexpected digit '%c' in octal constant",
		      cur_ch);

	result.data.ivalue = base * result.data.ivalue + (cur_ch - '0');
	break;
      case 'A':
      case 'B':
      case 'C':
      case 'D':
      case 'E':
      case 'F':
	if (base == 10)
	  raise_error(ERROR_EXPR, "unexpected digit '%c' in constant", cur_ch);
        else if (base == 2)
	  raise_error(ERROR_EXPR, "unexpected digit '%c' in binary constant",
		      cur_ch);
        else if (base == 8)
          raise_error(ERROR_EXPR, "unexpected digit '%c' in octal constant",
		      cur_ch);

	result.data.ivalue = base * result.data.ivalue + (10 + cur_ch - 'A');
	break;
      case 'a':
      case 'b':
      case 'c':
      case 'd':
      case 'e':
      case 'f':
	if (base == 10)
	  raise_error(ERROR_EXPR, "unexpected digit '%c' in constant", cur_ch);
        else if (base == 2)
	  raise_error(ERROR_EXPR, "unexpected digit '%c' in binary constant",
		      cur_ch);
        else if (base == 8)
	  raise_error(ERROR_EXPR, "unexpected digit '%c' in octal constant",
		      cur_ch);

	result.data.ivalue = base * result.data.ivalue + (10 + cur_ch - 'a');
	break;
      case '.':
        if (base == 16)
	  raise_error(ERROR_EXPR, "hexadecimal constant must be an integer");
        else if (base == 2)
	  raise_error(ERROR_EXPR, "binary constant must be an integer");

	result.type = FLOAT;
	result.data.fvalue = (double) result.data.ivalue;
	break;
      }
    } else if (result.type == FLOAT && reading_exponent) {
      switch (cur_ch) {
      default:
	done = true;
	break;
      case '0': case '1': case '2': case '3': case '4':
      case '5': case '6': case '7': case '8': case '9':
	float_exponent = base * float_exponent + (cur_ch - '0');
      }
    } else { //float, no exponent yet
      switch (cur_ch) {
      default:
	done = true;
	break;
      case '0': case '1': case '2': case '3': case '4':
      case '5': case '6': case '7': case '8': case '9':
	result.data.fvalue += float_frac_factor * (cur_ch - '0');
	float_frac_factor /= base;
	break;
      case 'E': case 'e': //float has an exponent modifier
	reading_exponent = true;

	if (parse->expr[parse->pos + 1] == '+') {
	  parse->pos++;
	} else if (parse->expr[parse->pos + 1] == '-') {
	  negative_exponent = true;
	  parse->pos++;
	}
	break;
      }
    }

    if (!done)
      parse->pos++;
  }

  if (result.type == FLOAT && float_exponent) {
    if (negative_exponent)
      float_exponent = -float_exponent;

    result.data.fvalue *= pow(10, float_exponent);
  }

  parse->cur_token = TOKEN_LITERAL;
  parse->numeric_value.type = result.type;
  parse->numeric_value.data = result.data;

  //save string-form for use in error messages
  int length = parse->pos - start_pos;
  if (length >= MAX_IDENTIFIER_LENGTH)
    length = MAX_IDENTIFIER_LENGTH - 1;
  strncpy(parse->str_value, parse->expr + start_pos, length);
  parse->str_value[length] = '\0';
}

void get_identifier(parser *parse) {
  int length = 0;
  
  while (length <= MAX_IDENTIFIER_LENGTH) {
    char cur_ch = parse->expr[parse->pos];

    if ((cur_ch >= 'A' && cur_ch <= 'Z')
	|| (cur_ch >= 'a' && cur_ch <= 'z')
	|| (cur_ch >= '0' && cur_ch <= '9')
	|| cur_ch == '_') {
      parse->str_value[length] = cur_ch;
      length++;
      parse->pos++;
    } else {
      break;
    }
  }

  parse->cur_token = TOKEN_IDENTIFIER;
  parse->str_value[length] = '\0';
}

void parse_expression(parser *parse, value *val) {  
  parse_conditional_expression(parse, val);

  bool done = false;
  while (!done) {
    switch (peek_token(parse)) {
    default:
      done = true;
      break;

    case TOKEN_OP_COMMA:
      get_token(parse);

      //evaluate right side, discard everything to the left of the comma
      parse_conditional_expression(parse, val);

      break;
    }
  }
}

void parse_conditional_expression(parser *parse, value *val) {
  value left;

  parse_logical_or_expression(parse, val);
  left = *val;

  if (peek_token(parse) == TOKEN_QMARK) {
    //this is a conditional expression
    value on_true, on_false;
    get_token(parse);

    //read first value
    parse_expression(parse, &on_true);

    //read colon
    get_token(parse);

    switch (parse->cur_token) {
    case TOKEN_COLON:
      break;
    case TOKEN_END:
      raise_error(ERROR_EXPR, "unexpected end of input");
    default:
      raise_error(ERROR_EXPR, "unexpected token '%s'", parse->str_value);
    }

    //read second value
    parse_conditional_expression(parse, &on_false);

    conditional(&left, &on_true, &on_false, val);
  }
}

void parse_logical_or_expression(parser *parse, value *val) {
  value left, right;
  
  parse_logical_and_expression(parse, val);
  left = *val;

  bool done = false;
  while (!done) {
    switch (peek_token(parse)) {
    default:
      done = true;
      break;
    case TOKEN_OP_OR:
      get_token(parse);
      parse_logical_and_expression(parse, &right);
      or(&left, &right, val);
      
      left = *val;
      break;
    }
  }  
}

void parse_logical_and_expression(parser *parse, value *val) {
  value left, right;

  parse_inclusive_or_expression(parse, val);
  left = *val;

  bool done = false;
  while (!done) {
    switch (peek_token(parse)) {
    default:
      done = true;
      break;
    case TOKEN_OP_AND:
      get_token(parse);
      parse_inclusive_or_expression(parse, &right);
      and(&left, &right, val);
      
      left = *val;
      break;
    }
  }
}

void parse_inclusive_or_expression(parser *parse, value *val) {
  value left, right;
  
  parse_exclusive_or_expression(parse, val);
  left = *val;

  bool done = false;
  while (!done) {
    switch (peek_token(parse)) {
    default:
      done = true;
      break;
    case TOKEN_OP_BIT_OR:
      get_token(parse);
      parse_exclusive_or_expression(parse, &right);
      bit_or(&left, &right, val);
      
      left = *val;
      break;
    }
  }
}

void parse_exclusive_or_expression(parser *parse, value *val) {
  value left, right;

  parse_and_expression(parse, val);
  left = *val;

  bool done = false;
  while (!done) {
    switch (peek_token(parse)) {
    default:
      done = true;
      break;
    case TOKEN_OP_BIT_XOR:
      get_token(parse);
      parse_and_expression(parse, &right);
      bit_xor(&left, &right, val);
      
      left = *val;
      break;
    }
  }
}

void parse_and_expression(parser *parse, value *val) {
  value left, right;

  parse_equality_expression(parse, val);
  left = *val;

  bool done = false;
  while (!done) {
    switch (peek_token(parse)) {
    default:
      done = true;
      break;
    case TOKEN_OP_BIT_AND:
      get_token(parse);
      parse_equality_expression(parse, &right);
      bit_and(&left, &right, val);
      
      left = *val;
      break;
    }
  }
}

void parse_equality_expression(parser *parse, value *val) {
  value left, right;

  parse_relational_expression(parse, val);
  left = *val;

  bool done = false;
  while (!done) {
    switch (peek_token(parse)) {
    default:
      done = true;
      break;

    case TOKEN_OP_EQUAL:
      get_token(parse);
      parse_relational_expression(parse, &right);
      equal(&left, &right, val);
      
      left = *val;
      break;

    case TOKEN_OP_NOT_EQUAL:
      get_token(parse);
      parse_relational_expression(parse, &right);
      not_equal(&left, &right, val);
      
      left = *val;
      break;
    }
  }
}

void parse_relational_expression(parser *parse, value *val) {
  value left, right;

  parse_shift_expression(parse, val);
  left = *val;

  bool done = false;
  while (!done) {
    switch (peek_token(parse)) {
    default:
      done = true;
      break;

    case TOKEN_OP_LESS_THAN:
      get_token(parse);
      parse_shift_expression(parse, &right);
      less_than(&left, &right, val);
      
      left = *val;
      break;

    case TOKEN_OP_LESS_THAN_EQ:
      get_token(parse);
      parse_shift_expression(parse, &right);
      less_than_eq(&left, &right, val);
      
      left = *val;
      break;

    case TOKEN_OP_GREATER_THAN:
      get_token(parse);
      parse_shift_expression(parse, &right);
      greater_than(&left, &right, val);
      
      left = *val;
      break;

    case TOKEN_OP_GREATER_THAN_EQ:
      get_token(parse);
      parse_shift_expression(parse, &right);
      greater_than_eq(&left, &right, val);
      
      left = *val;
      break;
    }
  }
}

void parse_shift_expression(parser *parse, value *val) {
  value left, right;

  parse_additive_expression(parse, val);
  left = *val;
  
  bool done = false;
  while (!done) {
    switch (peek_token(parse)) {
    default:
      done = true;
      break;

    case TOKEN_OP_BIT_SHIFT_LEFT:
      get_token(parse);
      parse_additive_expression(parse, &right);
      bit_shift_left(&left, &right, val);
      
      left = *val;
      break;

    case TOKEN_OP_BIT_SHIFT_RIGHT:
      get_token(parse);
      parse_additive_expression(parse, &right);
      bit_shift_right(&left, &right, val);
      
      left = *val;
      break;
    }
  }
}

void parse_additive_expression(parser *parse, value *val) {
  value left, right;
  
  parse_multiplicative_expression(parse, val);
  left = *val;

  bool done = false;
  while (!done) {
    switch (peek_token(parse)) {
    default:
      done = true;
      break;

    case TOKEN_OP_PLUS:
      get_token(parse);
      parse_multiplicative_expression(parse, &right);
      add(&left, &right, val);
      
      left = *val;
      break;

    case TOKEN_OP_MINUS:
      get_token(parse);
      parse_multiplicative_expression(parse, &right);
      subtract(&left, &right, val);
      
      left = *val;
      break;
    }
  }
}

void parse_multiplicative_expression(parser *parse, value *val) {
  value left, right;

  parse_unary_expression(parse, val);
  left = *val;

  bool done = false;
  while (!done) {
    switch (peek_token(parse)) {
    default:
      done = true;
      break;

    case TOKEN_OP_TIMES:
      get_token(parse);
      parse_unary_expression(parse, &right);
      multiply(&left, &right, val);
      
      left = *val;
      break;

    case TOKEN_OP_DIVIDE:
      get_token(parse);
      parse_unary_expression(parse, &right);
      divide(&left, &right, val);
      
      left = *val;
      break;

    case TOKEN_OP_IDIVIDE:
      get_token(parse);
      parse_unary_expression(parse, &right);
      int_divide(&left, &right, val);

      left = *val;
      break;

    case TOKEN_OP_MOD:
      get_token(parse);
      parse_unary_expression(parse, &right);
      modulo(&left, &right, val);
      
      left = *val;
      break;
    }
  }
}

void parse_unary_expression(parser *parse, value *val) {
  value operand;

  switch (peek_token(parse)) {
  default:
    parse_exponential_expression(parse, val);
    break;
    
  case TOKEN_OP_PLUS:
    get_token(parse);
    parse_unary_expression(parse, val);
    break;
    
  case TOKEN_OP_MINUS:
    get_token(parse);
    parse_unary_expression(parse, &operand);
    negate(&operand, val);
    break;

  case TOKEN_OP_BIT_NOT:
    get_token(parse);
    parse_unary_expression(parse, &operand);
    bit_not(&operand, val);
    break;

  case TOKEN_OP_NOT:
    get_token(parse);
    parse_unary_expression(parse, &operand);
    not(&operand, val);
    break;
  }
}

void parse_exponential_expression(parser *parse, value *val) {
  value left, right;

  parse_primary(parse, val);
  left = *val;

  bool done = false;
  while (!done) {
    switch (peek_token(parse)) {
    default:
      done = true;
      break;
    case TOKEN_OP_POW:
      get_token(parse);
      parse_unary_expression(parse, &right);
      power(&left, &right, val);
      
      left = *val;
      break;
    }
  }
}

void parse_primary(parser *parse, value *val) {
  get_token(parse);

  switch (parse->cur_token) {
  default:
    raise_error(ERROR_EXPR, "unexpected token '%s'", parse->str_value);
    break;

  case TOKEN_END:
    raise_error(ERROR_EXPR, "unexpected end of input");
    break;
    
  case TOKEN_IDENTIFIER:
    //is this a function or a constant?
    if (peek_token(parse) == TOKEN_LEFT_PAREN) {      
      int argc = 0;
      value argv[MAX_ARGUMENTS];
      char id[MAX_IDENTIFIER_LENGTH + 1];

      strncpy(id, parse->str_value, MAX_IDENTIFIER_LENGTH + 1);

      //eat parenthesis
      get_token(parse);

      //is there an argument list?
      if (peek_token(parse) == TOKEN_RIGHT_PAREN) {
	argc = 0;
      } else {
	bool done = false;
	while (!done) {
	  if (argc >= MAX_ARGUMENTS)
            raise_error(ERROR_EXPR, "too many arguments to function '%s'", id);
	  
          parse_conditional_expression(parse, &argv[argc]);
	  argc++;

	  switch (peek_token(parse)) {
          default:
	    done = true;
	    break;
          case TOKEN_OP_COMMA:
	    get_token(parse);
	    break;
	  }
	}
      }

      //read closing parenthesis
      get_token(parse);

      if (parse->cur_token != TOKEN_RIGHT_PAREN)
	raise_error(ERROR_EXPR, "unmatched parenthesis '('");

      //call the function
      call_function(id, val, argc, argv, parse->program_opts->degrees);
    } else {    
      get_constant(parse->str_value, val);
    }
    break;

  case TOKEN_LITERAL:
    *val = parse->numeric_value;
    break;

  case TOKEN_LEFT_PAREN:
    parse_expression(parse, val);

    //closing parenthesis should be next
    get_token(parse);

    if (parse->cur_token != TOKEN_RIGHT_PAREN)
      raise_error(ERROR_EXPR, "unmatched parenthesis '('");
    
    break;
  }
}
