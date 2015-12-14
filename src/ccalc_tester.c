/* ccalc-tester -- run tests on ccalc.
   Copyright (C) 2015 Gregory Kikola.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/* Written by Gregory Kikola <gkikola@gmail.com>. */

#include <assert.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE 256

#define EXPECT_INT(EXPR) expect_int(#EXPR, "", (EXPR))
#define EXPECT_FLOAT(EXPR) expect_float(#EXPR, "", (EXPR))

bool expect(char *expr, char *opts, char *expected);
bool expect_int(char *expr, char *opts, long expected);
bool expect_float(char *expr, char *opts, double expected);
bool expect_error(char *expr, char *opts, char *message);

bool call_ccalc(char *expr, char *opts, char *output, int output_size);

int num_tests = 0;

int main() {
  double PI = M_PI;
  double E = M_E;
  double PHI = (1 + sqrt(5)) / 2;

  assert(EXPECT_INT(42));
  assert(EXPECT_INT(0713));
  assert(EXPECT_INT(-0713));
  assert(EXPECT_INT(-35));
  assert(EXPECT_INT(0));
  assert(EXPECT_INT(1));
  assert(EXPECT_INT(-1));
  assert(expect_int("0b11011011", "", 0xDB));
  assert(expect_int("-0b1100111", "", -0x67));
  assert(expect_int("0b0", "", 0));
  assert(expect_int("0b1001001", "", 0x49));
  assert(EXPECT_INT(0xDEAD));
  assert(EXPECT_INT(000));
  assert(EXPECT_INT(010));
  assert(EXPECT_INT(021));
  assert(EXPECT_INT(003234));
  assert(EXPECT_INT(0xA));
  assert(EXPECT_INT(0xF1));
  assert(EXPECT_INT(0x234F3));
  
  assert(EXPECT_INT(3 + 2));
  assert(EXPECT_INT(2 + (5 + 8)));
  assert(EXPECT_INT((2 + 5) + 8));
  assert(EXPECT_INT(1 + 1 + 1 + 1 + 1 + 1 + 1));
  assert(EXPECT_INT((2 + 4) + (7 + 9)));
  assert(EXPECT_INT(2 + (4 + 7) + 9));
  assert(EXPECT_INT(+2 + +(+4 + +7) + +9));
  assert(EXPECT_INT(+7));

  assert(EXPECT_INT(4 - 7));
  assert(EXPECT_INT(7 - 4));
  assert(EXPECT_INT(4 - (2 + 5)));
  assert(EXPECT_INT(3 + (5 + 2 - 7 + (7 - 1) - 2 + -3) - 8 + (5 + (3 + 2))));
  
  assert(EXPECT_INT(3 * 49));
  assert(EXPECT_INT(3 * 7 + 1 - 2 * 4));
  assert(EXPECT_INT(3 * 3 * 3 * 3 * 3));
  assert(EXPECT_INT((3 * 3) * (3 * 3) * 3));
  assert(EXPECT_INT((3 * 3 * 3) * (3 * 3)));
  assert(EXPECT_INT(4 * (5 + 7 * (8 - 1 * (4 + 1 + 3)) * 2)));
  assert(EXPECT_INT(3 - -4 * -7));
  assert(EXPECT_INT((3 - -4) * -7));
  
  assert(EXPECT_INT(4 / 2));
  assert(EXPECT_INT(64 / 16));
  assert(EXPECT_INT(343 / 7));
  assert(EXPECT_INT(343 / 7 / 7));
  assert(EXPECT_INT(343 / 7 / 7 / 7));
  assert(EXPECT_INT(450 / 10 / 9));
  assert(EXPECT_INT(450 / (70 / 7)));
  assert(EXPECT_INT(4 + (7 / (3 * 2 + 1)) - 4 / (2 + 3 - 4)));
  assert(EXPECT_INT(4 + 7 - 36 / 12 + 4 - 13 * 96 / 13 + 2 - 4));
  assert(EXPECT_INT((4 + 7 - 36) / 5 + (4 - 13) * 96 / (2 * 13 - 8)));

  assert(expect_int("4 // 2", "", 4 / 2));
  assert(expect_int("64 // 16", "", 64 / 16));
  assert(expect_int("343 // 7", "", 343 / 7));
  assert(expect_int("343 // 7 // 7", "", 343 / 7 / 7));
  assert(expect_int("343 // 7 // 7 // 7", "", 343 / 7 / 7 / 7));
  assert(expect_int("5 // 3", "", 5 / 3));
  assert(expect_int("9 // 4 // 2", "", 9 / 4 / 2));
  assert(expect_int("5 // (3 + 4) // 2", "", 5 / (3 + 4) / 2));
  assert(expect_int("5 // 3 + 4 // 2", "", 5 / 3 + 4 / 2));
  assert(expect_int("343 // 15 // 12", "", 343 / 15 / 12));
  assert(expect_int("82 // 9", "", 82 / 9));
  assert(expect_int("10 / (5 // 2)", "", 10 / (5 / 2)));
  
  assert(EXPECT_INT(17 % 3));
  assert(EXPECT_INT(20 % 7));
  assert(EXPECT_INT(3 * 4 / 3 % 10));
  assert(EXPECT_INT(3 % 4 + 7 % 4 + 16 % 4));
  assert(EXPECT_INT(3 % (4 + 7) % (4 + 16) % 4));
  assert(EXPECT_INT(2 * 5 % 3));
  assert(EXPECT_INT(2 * (5 % 3)));
  assert(EXPECT_INT(3 * 3 % 3 * 3 - 4));
  assert(EXPECT_INT(3 * (3 % 3 * 3) - 4));
  assert(EXPECT_INT(81 % 7 + 13 % 7 + 42 % 7));
  assert(EXPECT_INT(81 % 7 * 13 % 7 * 42 % 7));
  assert(EXPECT_INT(13 % -5));
  assert(EXPECT_INT(-13 % 5));
  assert(EXPECT_INT(-13 % -5));

  assert(expect_int("7 ** 3", "", 343));
  assert(expect_int("4 * 3 ** 2", "", 4 * 9));
  assert(expect_int("2 ** 2 ** 2", "", 16));
  assert(expect_int("-13 ** 2", "", -169));
  assert(expect_int("(-13) ** 2", "", 169));
  assert(expect_int("+3 ** 2", "", 9));
  assert(expect_int("-3 ** 2", "", -9));
  assert(expect_int("!0 ** !0", "", 1));
  assert(expect_int("1 + -3 ** 4 + ~ 2 ** 3", "", 1 - 81 + ~8));
  assert(expect_int("1 + 3 ** 4 + 9 ** 2 + 1 ** 4 - 1", "", 1 + 81 + 81));
  assert(expect_int("(1 + 3) ** 4 - 9 ** (2 + 1)", "", 256 - 729));
  assert(expect_int("2 + 2048 / 4 ** 5 - 7 * 2 ** 8", "", 2 + 2 - 7 * 256));
  assert(expect_int("2 ** 2 ** 2 ** 2", "", 1 << 16));
  assert(expect_int("((2 ** 2) ** 2) ** 2", "", 256));
  assert(expect_int("2 ** (2 ** (2 ** 2))", "", 1 << 16));

  assert(expect_int("7 ^ 3", "-c", 343));
  assert(expect_int("4 * 3 ^ 2", "-c", 4 * 9));
  assert(expect_int("2 ^ 2 ^ 2", "-c", 16));
  assert(expect_int("-13 ^ 2", "-c", -169));
  assert(expect_int("(-13) ^ 2", "-c", 169));
  assert(expect_int("+3 ^ 2", "-c", 9));
  assert(expect_int("-3 ^ 2", "-c", -9));
  
  assert(EXPECT_INT(035 + 010 * (-0324 + 0112) - (07 * 010) % 05));
  assert(EXPECT_INT(0x11 + 0xa2 * (-0x33 + 0xdd) - (0xF * 0x1) % 0xB));
  
  assert(EXPECT_INT(4 == 4));
  assert(EXPECT_INT(4 == 5));
  assert(EXPECT_INT(4 == 4 == 5));
  assert(EXPECT_INT(11 == 4 + 7));
  assert(EXPECT_INT((11 == 4) + 7));
  assert(EXPECT_INT((14 == 14) == (3 == 3)));
  assert(EXPECT_INT(14 == (14 == 3) == 3));
  assert(EXPECT_INT(4 != 4));
  assert(EXPECT_INT(4 != 5));
  assert(EXPECT_INT(4 == 4 != 5));
  assert(EXPECT_INT(4 == (4 != 5)));
  assert(EXPECT_INT(4 / 4 == (4 != 5)));
  assert(EXPECT_INT(4 + (7 / (3 * 2 + 1) == (4 != 20)) - 13 * 4));

  assert(EXPECT_INT(4 > 7));
  assert(EXPECT_INT(7 > 4));
  assert(EXPECT_INT(4 > 7 == 0));
  assert(EXPECT_INT(7 > 4 == 15 > 12));
  assert(EXPECT_INT(7 > 4 == 12 > 15));
  assert(EXPECT_INT((4 > 3 == 1) * 42));
  assert(EXPECT_INT(1 + ((3 == 3) + 7) * 4));
  assert(EXPECT_INT(4 >= 7));
  assert(EXPECT_INT(4 <= 7));
  assert(EXPECT_INT(7 >= 4));
  assert(EXPECT_INT(7 <= 4));
  assert(EXPECT_INT(7 >= 7));
  assert(EXPECT_INT(7 <= 7));
  assert(EXPECT_INT(3 * (2 > 3) * 319));
  assert(EXPECT_INT(2 * ((3 <= 4) + (5 != 52) * (4 > 0))));
  assert(EXPECT_INT(4 < 6 < 8 < 8 < 8 < 8));
  assert(EXPECT_INT(4 > 3 < 2));
  assert(EXPECT_INT(4 >= 3 <= 2));
  assert(EXPECT_INT(5 >= 1 >= 1 >= 1));
  assert(EXPECT_INT(14 % ((3 <= 17) + 3)));
  
  assert(EXPECT_INT(!1));
  assert(EXPECT_INT(!0));
  assert(EXPECT_INT(!(7 == 7)));
  assert(EXPECT_INT(!(7 != 7)));
  assert(EXPECT_INT(!(!(!(!(!(!(1))))))));
  assert(EXPECT_INT(!(!(!(!(!(!(0))))))));
  assert(EXPECT_INT(!13 == !4));
  assert(EXPECT_INT(!(13 == 4)));
  assert(EXPECT_INT(4 == 4 && 7 >= 3 && 7 < 52));
  assert(EXPECT_INT(4 == (4 && 7) >= (3 && 7) < 52));
  assert(EXPECT_INT(4 != 4 && 6 == 6 && 1));
  assert(EXPECT_INT(4 != 4 || 6 == 6 || 1));
  assert(EXPECT_INT(4 != 4 || 6 == 6 && 1));
  assert(EXPECT_INT((4 != 4 || 6 == 6) && 1));
  assert(EXPECT_INT(4 != 4 && 6 == 6 || 1));
  assert(EXPECT_INT(!(4 != 4) && !(7 < 0) || !1));
  
  assert(EXPECT_INT(1 + ((4 > 0) ? 5 : 3) + ((5 == 10) ? 12 : 6) - 4));
  assert(EXPECT_INT(17 % 3 != 0 ? 4 + 3 < 5 ? 1 : 2 : 13));
  assert(EXPECT_INT(1 ? 5 : (7 == 4) ? 3 : 2));
  assert(EXPECT_INT(0 ? 5 : (7 == 4) ? 3 : 2));
  assert(EXPECT_INT(10 % 4 != 0 ? 7 > 4 ? 12 : 11 : 10));
  assert(EXPECT_INT(10 % 4 != 0 ? 7 > 4 ? 12 : 11 : 8 > 3 ? 10 : 9));
  assert(EXPECT_INT(10 % 4 == 0 ? 7 > 4 ? 12 : 11 : 8 > 3 ? 10 : 9));
  assert(EXPECT_INT(10 % 4 != 0 ? 7 < 4 ? 12 : 11 : 8 > 3 ? 10 : 9));
  assert(EXPECT_INT(14 % 5 == 0 ? 1 : 14 % 5 == 1 ? 2 : 14 % 5 == 2 ? 3 : 14 %
		    5 == 3 ? 4 : 14 % 5 == 4 ? 5 : -1));
  
  assert(EXPECT_INT((3, 4, 12, 0, -1, 12)));
  assert(EXPECT_INT((3, 5 ? 3 , 2 : 7, 2)));
  assert(EXPECT_INT((3, 5 + 7 * 8)));

  assert(EXPECT_INT(12 & 7));
  assert(EXPECT_INT(0 | 2 | 4 | 16));
  assert(EXPECT_INT(9 & 17 | 8));
  assert(EXPECT_INT(~17));
  assert(EXPECT_INT(~(9 | 17)));
  assert(EXPECT_INT(9 & 17 ^ 8));
  assert(EXPECT_INT(9 & 17 | 5 ^ 9));
  assert(EXPECT_INT(4 == 4 | 2 * (7 > 3)));
  assert(EXPECT_INT(7 & 8 > 3 & 3));
  assert(EXPECT_INT(7 ^ 8 > 3 ^ 3));
  assert(EXPECT_INT(7 | 8 > 3 | 3));
  assert(EXPECT_INT(16 | 5 ^ 7 & 7 | 7 ^ 7));
  
  assert(EXPECT_INT(1 << 4));
  assert(EXPECT_INT(256 >> 4));
  assert(EXPECT_INT(1 >> 2));
  assert(EXPECT_INT(73 << 3));
  assert(EXPECT_INT(15 >> 2));
  assert(EXPECT_INT(7 > 3 << 4));
  assert(EXPECT_INT(7 != 3 >> 9));
  assert(EXPECT_INT(7 >> 2 & 7 << 1));
  assert(EXPECT_INT(63 << 1 & 0xFF));
  assert(EXPECT_INT(0xcdcdcd | 710));
  assert(EXPECT_INT(0xcdcdcd & 710));
  assert(EXPECT_INT(0xcdcdcd ^ 710));
  assert(EXPECT_INT(3 << 12 / 4));
  assert(EXPECT_INT(33 / 11 >> 9 / 9));

  assert(EXPECT_INT(21 * 49 + 42 + -85 + 13 + 76 * 24 * -42 + 60 + -33 + 78 + 
		    -5 - 56 * 91));
  assert(EXPECT_INT(21 * 49 + 42 + -85 + 13 + 76 * 24 * -42 + 60 + -33 + 78 + 
		    -5 - 56 * 91));
  assert(EXPECT_INT(21 * 49 + 42 + (-85 + 13 + 76 * 24 * -42) + 60 + -33 + 
		    78 + -5 - 56 * 91));
  assert(EXPECT_INT(21 * (49 + 42 + -85) + 13 + 76 * (24 * -42 + 60) + -33 + 
		    78 + -5 - (56 * 91)));
  assert(EXPECT_INT(21 * (49 + (42 + (-85 + (13 + (76 * (24 * (-42 + (60 + 
		    (-33 + (78 + (-5 - (56 * 91))))))))))))));

  assert(EXPECT_FLOAT(30.));
  assert(EXPECT_FLOAT(.713));
  assert(EXPECT_FLOAT(1.2e47));
  assert(EXPECT_FLOAT(0.0));
  assert(EXPECT_FLOAT(-3.4e-12));
  assert(EXPECT_FLOAT(-3.4e+12));
  assert(EXPECT_FLOAT(0.00001));
  assert(EXPECT_FLOAT(1000.001));
  assert(EXPECT_FLOAT(12.34e-5));
  assert(EXPECT_FLOAT(6.e9));
  assert(EXPECT_FLOAT(.2e-2));
  assert(EXPECT_FLOAT(0219.));
  assert(EXPECT_FLOAT(00187.3));
  assert(EXPECT_FLOAT(219.00));
  assert(EXPECT_FLOAT(219.912));
  assert(EXPECT_FLOAT(87.41592));
  
  assert(EXPECT_FLOAT(3.0 + 2));
  assert(EXPECT_FLOAT(3 + 2.0));
  assert(EXPECT_FLOAT(3.0 + 2.0));
  assert(EXPECT_FLOAT(2 + (5.0 + 8)));
  assert(EXPECT_FLOAT((2 + 5.0) + 8));
  assert(EXPECT_FLOAT(1 + 1 + 1 + 1 + 1 + 1. + 1));
  assert(EXPECT_FLOAT((2 + 4.4) + (7 + 9)));
  assert(EXPECT_FLOAT(2 + (4 + 7) + 9.15));
  assert(EXPECT_FLOAT(+2.1 + +(+4.2 + +7.9) + +9.5));
  assert(EXPECT_FLOAT(+7.));

  assert(EXPECT_FLOAT(4 - 7.2));
  assert(EXPECT_FLOAT(7.2 - 4));
  assert(EXPECT_FLOAT(4 - (2.1 + 5)));
  assert(EXPECT_FLOAT(3 + (5.15 + 2 - 7 + (7.99 - 1) - 2 + -3.) - 8 + (5.87 +
		      (3 + 2.009))));
  
  assert(EXPECT_FLOAT(3 * 49.1));
  assert(EXPECT_FLOAT(3.14 * 49));
  assert(EXPECT_FLOAT(3 * 7 + 1 - 2.0 * 4));
  assert(EXPECT_FLOAT(3 * 3 * 3 * 3.33 * 3));
  assert(EXPECT_FLOAT((3 * 3) * (3 * 3.33) * 3));
  assert(EXPECT_FLOAT((3 * 3 * 3) * (3.33 * 3)));
  assert(EXPECT_FLOAT(4 * (5 + 7 * (8 - 1 * (4 + .1 + 3)) * 2)));
  assert(EXPECT_FLOAT(3.1 - -4.9 * -7.8));
  assert(EXPECT_FLOAT((3 - -4.0) * -7));

  assert(expect_float("5 / 3", "", 5.0 / 3));
  assert(expect_float("9 / 4 / 2", "", 9.0 / 4 / 2));
  assert(expect_float("5 / (3 + 4) / 2", "", 5.0 / (3 + 4) / 2));
  assert(expect_float("5 / 3 + 4 / 2", "", 5.0 / 3 + 4 / 2));
  assert(expect_float("343 / 15 / 12", "", 343.0 / 15 / 12));
  assert(expect_float("82 / 9", "", 82 / 9.0));
  assert(expect_float("10 / (2 / 5)", "", 10 / (2.0 / 5)));
  
  assert(EXPECT_FLOAT(4.0 / 2));
  assert(EXPECT_FLOAT(4 / 2.0));
  assert(EXPECT_FLOAT(300 / 12.5));
  assert(EXPECT_FLOAT(343 / 2.0));
  assert(EXPECT_FLOAT(343 / 15.0 / 12.0));
  assert(EXPECT_FLOAT(450 / 10 / 9.1));
  assert(EXPECT_FLOAT(4.50 / (70 / 7)));
  assert(EXPECT_FLOAT(4.38 + (7.91 / (3.26 * 2.01 + 1.94)) - 4.11 / (2.08
		      + 3.39 - 4.0)));
  assert(EXPECT_FLOAT(4 + 7 - 36 / 12 + 4 - 13. * 96 / 13 + 2 - 4));
  assert(EXPECT_FLOAT((4.816 + 7.285 - 36.09) / 5.184 + (4.009 - 13.04) *
		      96.11 / (2.927 * 13.07 - 8.888)));
  
  assert(EXPECT_FLOAT(3.8 * 4 / 3));
  assert(EXPECT_FLOAT(3 % 4 + 0.14));
  assert(EXPECT_FLOAT(2.5 * (5 % 3)));
  assert(EXPECT_FLOAT(3 * 3 % 3 * 3.9 - 4));
  assert(EXPECT_FLOAT(3 * (3 % 3 * 3.9) - 4));

  assert(expect_float("7.0 ** 3", "", pow(7.0, 3)));
  assert(expect_float("7 ** 3.0", "", pow(7, 3.0)));
  assert(expect_float("4.12 * 3.72 ** 2.15", "", 4.12 * pow(3.72, 2.15)));
  assert(expect_float("2.1 ** 2 ** 2", "", pow(2.1, 4)));
  assert(expect_float("(2.1 ** 2) ** 2", "", pow(pow(2.1, 2), 2)));
  assert(expect_float("-87.315 ** 4", "", -pow(87.315, 4)));
  assert(expect_float("(-87.315) ** 4", "", pow(87.315, 4)));
  assert(expect_float("3 ** -2", "", 1.0 / 9));
  assert(expect_float("-18 ** -3", "", -pow(18, -3)));
  assert(expect_float("(-18) ** (-3)", "", pow(-18, -3)));
  assert(expect_float("1 + 3.15 ** 4 + 9 ** 2.3 + 1 ** 4.5 - 1", "",
		      1 + pow(3.15, 4) + pow(9, 2.3) + pow(1, 4.5) - 1));
  assert(expect_float("(1 + 3.15) ** 4 - 9 ** (2.3 + 1)", "",
		      pow(1 + 3.15, 4) - pow(9, 2.3 + 1)));
  assert(expect_float("2 + 2048 / 3 ** 5 - 7 * 2 ** 8", "",
		      2 + 2048 / pow(3.0, 5) - 7 * 256));
  assert(expect_float("2.3 ** 2.3 ** 2.3 ** 2.3", "",
		      pow(2.3, pow(2.3, pow(2.3, 2.3)))));
  assert(expect_float("((2.3 ** 2.3) ** 2.3) ** 2.3", "",
		      pow(pow(pow(2.3, 2.3), 2.3), 2.3)));
  assert(expect_float("2.3 ** (2.3 ** (2.3 ** 2.3))", "",
		      pow(2.3, pow(2.3, pow(2.3, 2.3)))));
  assert(expect_float("-2.2 ** -2.2 ** -2.2 ** -2.2", "",
		      -pow(2.2, -pow(2.2, -pow(2.2, -2.2)))));
  
  assert(EXPECT_INT(4.0 == 4.0));
  assert(EXPECT_INT(4.0 == 5.0));
  assert(EXPECT_INT(4.0 == 4.0 == 5));
  assert(EXPECT_INT(11 == 4.0 + 7.0));
  assert(EXPECT_FLOAT((11 == 4) + 7.0));
  assert(EXPECT_INT(4.5 != 5.5));
  assert(EXPECT_INT(4 == 4.0 != 5));
  assert(EXPECT_INT(4.05 == (4.72 != 5.18)));
  assert(EXPECT_INT(10.0 / 4 == (4.0 != 5)));

  assert(EXPECT_INT(4.3 > 7.8));
  assert(EXPECT_INT(7.8 > 4.3));
  assert(EXPECT_INT(4.3 > 7.8 == 0));
  assert(EXPECT_INT(7.0 > 4.0 == 15.0 > 12.0));
  assert(EXPECT_INT(7.0 > 4.0 == 12.0 > 15.0));
  assert(EXPECT_FLOAT((4.0 > 3.5 == 1) * 42.7));
  assert(EXPECT_FLOAT(1 + ((3.0 == 3.0) + 7) * 4.18));
  assert(EXPECT_INT(4.1 >= 7.05));
  assert(EXPECT_INT(4.1 <= 7.05));
  assert(EXPECT_INT(7.05 >= 4.1));
  assert(EXPECT_INT(7.05 <= 4.1));
  assert(EXPECT_INT(7.0 >= 7.0));
  assert(EXPECT_INT(7.0 <= 7.0));
  assert(EXPECT_INT(3 * (2.0 > 3.0) * 319));
  assert(EXPECT_INT(2 * ((3.5 <= 4.2) + (5 != 52.1) * (4 > 0.0))));
  assert(EXPECT_INT(4.8 < 6.1 < 8. < 8. < 8. < 8.));
  assert(EXPECT_INT(4.5 > 3.5 < 2.5));
  assert(EXPECT_INT(4.5 >= 3.5 <= 2.5));
  assert(EXPECT_INT(5.0 >= 1.0 >= 1.0 >= 1.0));
  assert(EXPECT_INT(14 % ((3 <= 17) + 3)));
  
  assert(EXPECT_INT(!1.0));
  assert(EXPECT_INT(!0.0));
  assert(EXPECT_INT(!(7.0 == 7.0)));
  assert(EXPECT_INT(!(7.0 != 7.0)));
  assert(EXPECT_INT(!(!(!(!(!(!(1.87))))))));
  assert(EXPECT_INT(!(!(!(!(!(!(0.00))))))));
  assert(EXPECT_INT(!13.8 == !4.5));
  assert(EXPECT_INT(!(13.8 == 4.5)));
  assert(EXPECT_INT(4.2 == 4.2 && 7.0 >= 3.9 && 7.0 < 52.4));
  assert(EXPECT_INT(4.2 == (4.2 && 7.0) >= (3.9 && 7.0) < 52.4));
  assert(EXPECT_INT(4.0 != 4.0 && 6.0 == 6.0 && 1.));
  assert(EXPECT_INT(4.0 != 4.0 || 6.0 == 6.0 || 1.));
  assert(EXPECT_INT(4.0 != 4.0 || 6.0 == 6.0 && 1.));
  assert(EXPECT_INT((4.0 != 4.0 || 6.0 == 6.0) && 1.));
  assert(EXPECT_INT(4.0 != 4.0 && 6.0 == 6.0 || 1.));
  assert(EXPECT_INT(!(4.0 != 4.0) && !(7.12 < 0.0) || !1.));
  
  assert(EXPECT_FLOAT(1 + ((4 > 0) ? 5.5 : 3.5) + ((5 == 10) ? 12.4 : 6.) - 4));
  assert(EXPECT_FLOAT(17 % 3 != 0 ? 4 + 3 < 5 ? 1.2 : 2.2 : 13.2));
  assert(EXPECT_FLOAT(1.87 ? 5.4 : (7.0 == 4.0) ? 3.4 : 2.4));
  assert(EXPECT_FLOAT(0.0 ? 5.1 : (7.0 == 4.0) ? 3.1 : 2.1));
  
  assert(EXPECT_FLOAT((3.5, 4, 12.9, 0, -1, 12.5)));
  assert(EXPECT_FLOAT((3, 5 ? 3 , 2.5 : 7.0, 2.12)));
  assert(EXPECT_FLOAT((3, 5 + 7.1 * 8)));

  assert(EXPECT_FLOAT(2181.e-2 * 49.51 + 42.71 + -85.04 + 0.1348e2 + 76.21 *
		      24.43 * -42.93 + 60.40 + -33.07 + 78.40e+0 + -5.62 -
		      56.41 * 91.28));
  assert(EXPECT_FLOAT(21.81 * 49.51 + 42.71 + (-85.04 + 13.48 + 76.21 * 24.43
		      * -42.93) + 60.40 + -33.07 + 78.40 + -5.62 - 56.41 *
		      91.28));
  assert(EXPECT_FLOAT(21 * (49.0 / 42 + -85.2) + 13 + 76 * (24 * -4.2 + 60) +
		      -33 / 78.4 + -5.1 / (56 * 91)));
  assert(EXPECT_FLOAT(21 * (49 + (42 + (-85 + (13 + (76 * (24 * (-42 + (60 +
		      (-33 + (78 + (-5 - (56.0 * 91))))))))))))));

  assert(EXPECT_FLOAT(sin(4.2) - cos(1.0)));
  assert(EXPECT_FLOAT(log(1.8)));
  assert(EXPECT_FLOAT(log((7, 2))));
  assert(EXPECT_FLOAT(PI));
  assert(EXPECT_FLOAT(E));
  assert(EXPECT_FLOAT(PI * (E + PI)));
  assert(EXPECT_FLOAT(sin ( (2 * PI > 6) ? PI / 2 : PI / 3 )));
  assert(EXPECT_FLOAT(sin(sin(sin(1.0)))));
  assert(EXPECT_FLOAT(atan2(sin(PI / 3), cos(PI / 4))));
  assert(EXPECT_FLOAT(4 + atan2(hypot(3.0 * 2, sqrt(64)),
				hypot(59 % 4, 2 * 2))));
  assert(EXPECT_FLOAT(hypot(hypot(hypot(1.1, 1.2), hypot(1.3, 1.4)),
			    hypot(hypot(1.5, 1.6), hypot(1.7, 1.8)))));
  assert(EXPECT_FLOAT(sin(log(cos(tan(sqrt(exp(30.0))))))));
  assert(EXPECT_FLOAT(  E    +  cos   (    7.2    )  ));

  assert(EXPECT_FLOAT(PI));
  assert(EXPECT_FLOAT(E));
  assert(EXPECT_FLOAT(PHI));
  assert(EXPECT_FLOAT(PI * E + PI));
  assert(EXPECT_FLOAT(E * (PI / 7)));
  assert(EXPECT_FLOAT(9 * PI / 2));
  assert(EXPECT_FLOAT(-PI));
  assert(EXPECT_FLOAT(-3 * PI / 2));
  assert(EXPECT_FLOAT(1 + sqrt(PI) - pow(E, 2.34)));

  assert(EXPECT_INT(CHAR_MAX));
  assert(EXPECT_INT(CHAR_MIN));
  assert(EXPECT_INT(DBL_DIG));
  assert(EXPECT_FLOAT(DBL_EPSILON));
  assert(EXPECT_INT(DBL_MANT_DIG));
  assert(EXPECT_FLOAT(DBL_MAX));
  assert(EXPECT_INT(DBL_MAX_EXP));
  assert(EXPECT_FLOAT(DBL_MIN));
  assert(EXPECT_INT(DBL_MIN_EXP));
  assert(EXPECT_INT(FLT_DIG));
  assert(EXPECT_FLOAT(FLT_EPSILON));
  assert(EXPECT_INT(FLT_MANT_DIG));
  assert(EXPECT_FLOAT(FLT_MAX));
  assert(EXPECT_INT(FLT_MAX_EXP));
  assert(EXPECT_FLOAT(FLT_MIN));
  assert(EXPECT_INT(FLT_MIN_EXP));
  assert(EXPECT_INT(FLT_RADIX));
  assert(EXPECT_INT(INT_MAX));
  assert(EXPECT_INT(INT_MIN));
  assert(EXPECT_INT(LONG_MAX));
  assert(EXPECT_INT(LONG_MIN));
  assert(EXPECT_INT(RAND_MAX));
  assert(EXPECT_INT(SCHAR_MAX));
  assert(EXPECT_INT(SCHAR_MIN));
  assert(EXPECT_INT(SHRT_MAX));
  assert(EXPECT_INT(SHRT_MIN));
  assert(EXPECT_INT(UCHAR_MAX));
  assert(EXPECT_INT(USHRT_MAX));
  
  assert(EXPECT_INT(abs(0)));
  assert(EXPECT_INT(abs(3)));
  assert(EXPECT_INT(abs(-072)));
  assert(EXPECT_INT( abs ( abs ( - 386 ))));
  assert(expect_float("abs(0.0)", "", 0.0));
  assert(expect_float("abs(3.715)", "", 3.715));
  assert(expect_float("abs(-74.3e2)", "", 74.3e2));
  assert(expect_float("abs(abs(abs(abs(-3.14159))))", "", 3.14159));

  assert(EXPECT_FLOAT(sqrt(0)));
  assert(EXPECT_FLOAT(sqrt(0.1234)));
  assert(expect_float("(-82.87 + sqrt(82.87 ** 2 - 4 * 14 * 3)) / (2 * 14)", "",
		      (-82.87 + sqrt(pow(82.87, 2) - 4 * 14 * 3)) / (2 * 14)));
  assert(EXPECT_FLOAT(cbrt((1 + sqrt(5))/2)));
  assert(EXPECT_FLOAT(sqrt(sqrt(sqrt(47.89)))));
  assert(EXPECT_FLOAT(sqrt(2 + 3 * sqrt(3))));

  assert(EXPECT_FLOAT(cbrt(0)));
  assert(EXPECT_FLOAT(cbrt(0.1234)));
  assert(EXPECT_FLOAT(cbrt(-0.1234)));
  assert(EXPECT_FLOAT(3 * cbrt(1 + 5 * sqrt(5))));

  assert(expect_int("floor(0)", "", 0));
  assert(expect_int("floor(0.1234)", "", 0));
  assert(expect_int("floor(-0.1234)", "", -1));
  assert(expect_int("floor(382.71828)", "", 382));
  assert(expect_int("floor(12.0)", "", 12));
  assert(expect_int("floor(12)", "", 12));
  assert(expect_int("floor(-12)", "", -12));
  assert(expect_int("floor(-12.3)", "", -13));

  assert(expect_int("ceil(0)", "", 0));
  assert(expect_int("ceil(0.1234)", "", 1));
  assert(expect_int("ceil(-0.1234)", "", 0));
  assert(expect_int("ceil(382.71828)", "", 383));
  assert(expect_int("ceil(12.0)", "", 12));
  assert(expect_int("ceil(12)", "", 12));
  assert(expect_int("ceil(-12)", "", -12));
  assert(expect_int("ceil(12.3)", "", 13));
  assert(expect_int("ceil(-12.3)", "", -12));

  assert(expect_int("round(0)", "", 0));
  assert(expect_int("round(0.1234)", "", 0));
  assert(expect_int("round(-0.1234)", "", 0));
  assert(expect_int("round(1 - 0.1234)", "", 1));
  assert(expect_int("round(1.1234)", "", 1));
  assert(expect_int("round(382.2484)", "", 382));
  assert(expect_int("round(382.71828)", "", 383));
  assert(expect_int("round(-382.2484)", "", -382));
  assert(expect_int("round(-382.71828)", "", -383));
  assert(expect_int("round(12.0)", "", 12));
  assert(expect_int("round(12)", "", 12));
  assert(expect_int("round(-12)", "", -12));
  assert(expect_int("round(-12.3)", "", -12));
  assert(expect_int("round(-12.7)", "", -13));

  assert(expect_int("trunc(0)", "", 0));
  assert(expect_int("trunc(0.1234)", "", 0));
  assert(expect_int("trunc(-0.1234)", "", 0));
  assert(expect_int("trunc(1 - 0.1234)", "", 0));
  assert(expect_int("trunc(1.1234)", "", 1));
  assert(expect_int("trunc(382.2484)", "", 382));
  assert(expect_int("trunc(382.71828)", "", 382));
  assert(expect_int("trunc(-382.2484)", "", -382));
  assert(expect_int("trunc(-382.71828)", "", -382));
  assert(expect_int("trunc(12.0)", "", 12));
  assert(expect_int("trunc(12)", "", 12));
  assert(expect_int("trunc(-12)", "", -12));
  assert(expect_int("trunc(-12.3)", "", -12));
  assert(expect_int("trunc(-12.7)", "", -12));

  assert(expect_int("remainder(0, 1)", "", 0));
  assert(expect_int("remainder(12, 13)", "", -1));
  assert(expect_int("remainder(-12, 13)", "", 1));
  assert(expect_int("remainder(12, -13)", "", -1));
  assert(expect_int("remainder(-12, -13)", "", 1));
  assert(EXPECT_FLOAT(remainder(482.5, 73.6)));
  assert(EXPECT_FLOAT(remainder(-482.5, 73.6)));
  assert(EXPECT_FLOAT(remainder(482.5, -73.6)));
  assert(EXPECT_FLOAT(remainder(-482.5, -73.6)));
  assert(EXPECT_FLOAT(remainder(100, 3.78)));
  assert(EXPECT_FLOAT(remainder(3.78, 100)));
  assert(EXPECT_FLOAT(remainder(PI, 1 / E)));
  assert(EXPECT_FLOAT(remainder(exp(5), PHI)));

  assert(expect_int("fmod(0, 1)", "", 0 % 1));
  assert(expect_int("fmod(12, 13)", "", 12 % 13));
  assert(expect_int("fmod(-12, 13)", "", -12 % 13));
  assert(expect_int("fmod(12, -13)", "", 12 % -13));
  assert(expect_int("fmod(-12, -13)", "", -12 % -13));
  assert(EXPECT_FLOAT(fmod(482.5, 73.6)));
  assert(EXPECT_FLOAT(fmod(-482.5, 73.6)));
  assert(EXPECT_FLOAT(fmod(482.5, -73.6)));
  assert(EXPECT_FLOAT(fmod(-482.5, -73.6)));
  assert(EXPECT_FLOAT(fmod(100, 3.78)));
  assert(EXPECT_FLOAT(fmod(3.78, 100)));
  assert(EXPECT_FLOAT(fmod(PI, 1 / E)));
  assert(EXPECT_FLOAT(fmod(exp(5), PHI)));

  assert(expect_int("pow(12, 3)", "", 12 * 12 * 12));
  assert(expect_int("pow(3, 12)", "", (int)round(pow(3, 12))));
  assert(expect_int("pow(0, 1)", "", 0));
  assert(expect_int("pow(2, 1)", "", 2));
  assert(expect_int("pow(0, 14)", "", 0));
  assert(expect_int("pow(2, 10)", "", 1 << 10));
  assert(expect_int("pow(-2, 10)", "", 1 << 10));
  assert(expect_int("pow(2, 11)", "", 1 << 11));
  assert(expect_int("pow(-2, 11)", "", -(1 << 11)));
  assert(expect_int("pow(7, 3)", "", 343));
  assert(expect_int("pow(-7, 3)", "", -343));
  assert(EXPECT_FLOAT(pow(12, -3)));
  assert(EXPECT_FLOAT(pow(3, -12)));
  assert(EXPECT_FLOAT(pow(2, -1)));
  assert(EXPECT_FLOAT(pow(2, -10)));
  assert(EXPECT_FLOAT(pow(-2, -10)));
  assert(EXPECT_FLOAT(pow(2, -11)));
  assert(EXPECT_FLOAT(pow(-2, -11)));
  assert(EXPECT_FLOAT(pow(7, -3)));
  assert(EXPECT_FLOAT(pow(-7, -3)));
  assert(EXPECT_FLOAT(pow(0.1234, 5)));
  assert(EXPECT_FLOAT(pow(5, 0.1234)));
  assert(EXPECT_FLOAT(pow(-0.1234, 5)));
  assert(EXPECT_FLOAT(pow(5, -0.1234)));
  assert(EXPECT_FLOAT(pow(0.1234, 0.5678)));
  assert(EXPECT_FLOAT(pow(0.5678, 0.1234)));
  assert(EXPECT_FLOAT(pow(-0.1234, 0.5678)));
  assert(EXPECT_FLOAT(pow(PI, E)));
  assert(EXPECT_FLOAT(pow(pow(2, 3.5), pow(4.2, 3))));
  assert(EXPECT_FLOAT(pow(pow(2.18, 3.5), pow(4, 3))));
  
  assert(EXPECT_FLOAT(exp(0)));
  assert(EXPECT_FLOAT(exp(0.1234)));
  assert(EXPECT_FLOAT(exp(-0.1234)));
  assert(EXPECT_FLOAT(exp(sqrt(PI))));
  assert(EXPECT_FLOAT(exp(1)));
  assert(EXPECT_FLOAT(exp(-1)));
  assert(EXPECT_FLOAT(exp(2.375)));
  assert(EXPECT_FLOAT(exp(-PHI)));
  assert(EXPECT_FLOAT(pow(2.5, exp(7.5))));

  assert(expect_int("exp2(0)", "", 1));
  assert(EXPECT_FLOAT(exp2(0.1234)));
  assert(EXPECT_FLOAT(exp2(-0.1234)));
  assert(EXPECT_FLOAT(exp2(sqrt(PI))));
  assert(expect_int("exp2(1)", "", 2));
  assert(EXPECT_FLOAT(exp2(-1)));
  assert(EXPECT_FLOAT(exp2(2.375)));
  assert(EXPECT_FLOAT(exp2(-PHI)));
  assert(EXPECT_FLOAT(pow(2.5, exp2(7.5))));

  assert(EXPECT_FLOAT(expm1(0)));
  assert(EXPECT_FLOAT(expm1(0.1234)));
  assert(EXPECT_FLOAT(expm1(-0.1234)));
  assert(EXPECT_FLOAT(expm1(sqrt(PI))));
  assert(EXPECT_FLOAT(expm1(1)));
  assert(EXPECT_FLOAT(expm1(-1)));
  assert(EXPECT_FLOAT(expm1(2.375)));
  assert(EXPECT_FLOAT(expm1(-PHI)));
  assert(EXPECT_FLOAT(pow(2.5, expm1(7.5))));

  assert(EXPECT_FLOAT(log(0.1234)));
  assert(EXPECT_FLOAT(log(1 - 0.1234)));
  assert(EXPECT_FLOAT(log(sqrt(PI))));
  assert(EXPECT_FLOAT(log(1)));
  assert(EXPECT_FLOAT(log(E)));
  assert(EXPECT_FLOAT(log(1 / E)));
  assert(EXPECT_FLOAT(log(E * E * E)));
  assert(EXPECT_FLOAT(log(pow(E, 2.5))));
  assert(EXPECT_FLOAT(log(pow(PI, 2.5))));
  assert(EXPECT_FLOAT(log(2.375)));
  assert(EXPECT_FLOAT(pow(2.5, log(7.5))));

  assert(EXPECT_FLOAT(log10(0.1234)));
  assert(EXPECT_FLOAT(log10(1 - 0.1234)));
  assert(EXPECT_FLOAT(log10(sqrt(PI))));
  assert(EXPECT_FLOAT(log10(1)));
  assert(EXPECT_FLOAT(log10(10)));
  assert(EXPECT_FLOAT(log10(1000)));
  assert(EXPECT_FLOAT(log10(1.0 / 100)));
  assert(EXPECT_FLOAT(log10(E)));
  assert(EXPECT_FLOAT(log10(E * E * E)));
  assert(EXPECT_FLOAT(log10(pow(E, 2.5))));
  assert(EXPECT_FLOAT(log10(pow(PI, 2.5))));
  assert(EXPECT_FLOAT(log10(2.375)));
  assert(EXPECT_FLOAT(pow(2.5, log10(7.5))));

  assert(EXPECT_FLOAT(log1p(-0.1234)));
  assert(EXPECT_FLOAT(log1p(0)));
  assert(EXPECT_FLOAT(log1p(0.1234)));
  assert(EXPECT_FLOAT(log1p(1 - 0.1234)));
  assert(EXPECT_FLOAT(log1p(sqrt(PI))));
  assert(EXPECT_FLOAT(log1p(1)));
  assert(EXPECT_FLOAT(log1p(1 / E)));
  assert(EXPECT_FLOAT(log1p(E)));
  assert(EXPECT_FLOAT(log1p(E * E * E)));
  assert(EXPECT_FLOAT(log1p(pow(E, 2.5))));
  assert(EXPECT_FLOAT(log1p(pow(PI, 2.5))));
  assert(EXPECT_FLOAT(log1p(2.375)));
  assert(EXPECT_FLOAT(pow(2.5, log1p(7.5))));

  assert(EXPECT_FLOAT(log2(0.1234)));
  assert(EXPECT_FLOAT(log2(1 - 0.1234)));
  assert(EXPECT_FLOAT(log2(sqrt(PI))));
  assert(EXPECT_FLOAT(log2(1)));
  assert(EXPECT_FLOAT(log2(2)));
  assert(EXPECT_FLOAT(log2(1.0 / 2)));
  assert(EXPECT_FLOAT(log2(8)));
  assert(EXPECT_FLOAT(log2(E)));
  assert(EXPECT_FLOAT(log2(E * E * E)));
  assert(EXPECT_FLOAT(log2(pow(E, 2.5))));
  assert(EXPECT_FLOAT(log2(pow(PI, 2.5))));
  assert(EXPECT_FLOAT(log2(2.375)));
  assert(EXPECT_FLOAT(pow(2.5, log2(7.5))));

  assert(expect_int("min(3, 7)", "", 3));
  assert(expect_int("min(7, 3)", "", 3));
  assert(expect_int("min(0, 0)", "", 0));
  assert(expect_int("min(-5, -2)", "", -5));
  assert(expect_int("min(-2, -5)", "", -5));
  assert(expect_int("min(3, -3)", "", -3));
  assert(expect_int("min(-3, 3)", "", -3));
  assert(expect_int("min(-3, -3)", "", -3));
  assert(expect_int("min(12, 62)", "", 12));
  assert(expect_int("min(12, -62)", "", -62));
  assert(expect_int("min(-12, 62)", "", -12));
  assert(expect_float("min(3.5, 7.5)", "", 3.5));
  assert(expect_float("min(7.5, 3.5)", "", 3.5));
  assert(expect_float("min(0.0, 0)", "", 0.0));
  assert(expect_float("min(2, 3.1)", "", 2.0));
  assert(expect_float("min(0.1234, 0.5678)", "", 0.1234));
  assert(expect_float("min(-0.1234, -0.5678)", "", -0.5678));

  assert(expect_int("max(3, 7)", "", 7));
  assert(expect_int("max(7, 3)", "", 7));
  assert(expect_int("max(0, 0)", "", 0));
  assert(expect_int("max(-5, -2)", "", -2));
  assert(expect_int("max(-2, -5)", "", -2));
  assert(expect_int("max(3, -3)", "", 3));
  assert(expect_int("max(-3, 3)", "", 3));
  assert(expect_int("max(-3, -3)", "", -3));
  assert(expect_int("max(12, 62)", "", 62));
  assert(expect_int("max(12, -62)", "", 12));
  assert(expect_int("max(-12, 62)", "", 62));
  assert(expect_float("max(3.5, 7.5)", "", 7.5));
  assert(expect_float("max(7.5, 3.5)", "", 7.5));
  assert(expect_float("max(0.0, 0)", "", 0.0));
  assert(expect_float("max(2, 3.1)", "", 3.1));
  assert(expect_float("max(0.1234, 0.5678)", "", 0.5678));
  assert(expect_float("max(-0.1234, -0.5678)", "", -0.1234));

  assert(EXPECT_FLOAT(nextafter(31.25, 100)));
  assert(EXPECT_FLOAT(nextafter(1.0, 2)));
  assert(EXPECT_FLOAT(nextafter(1.0, 0)));
  assert(EXPECT_FLOAT(nextafter(-31.25, -100)));
  assert(EXPECT_FLOAT(nextafter(-31.25, 0)));
  assert(EXPECT_FLOAT(nextafter(0, 1)));
  assert(EXPECT_FLOAT(nextafter(0, -1)));

  assert(EXPECT_FLOAT(hypot(3, 4)));
  assert(EXPECT_FLOAT(hypot(3.0, 4)));
  assert(EXPECT_FLOAT(hypot(3, 4.0)));
  assert(EXPECT_FLOAT(hypot(12.3, 14.8)));
  assert(EXPECT_FLOAT(hypot(0, 0)));
  assert(EXPECT_FLOAT(hypot(5.5, 0)));
  assert(EXPECT_FLOAT(hypot(0, 5.5)));
  assert(EXPECT_FLOAT(hypot(5.5, 5.5)));
  assert(EXPECT_FLOAT(hypot(PI, E)));

  assert(EXPECT_FLOAT(sin(0)));
  assert(EXPECT_FLOAT(sin(0.1234)));
  assert(EXPECT_FLOAT(sin(-0.1234)));
  assert(EXPECT_FLOAT(sin(PI / 6)));
  assert(EXPECT_FLOAT(sin(-PI / 6)));
  assert(EXPECT_FLOAT(sin(PI / 3)));
  assert(EXPECT_FLOAT(sin(-PI / 3)));
  assert(EXPECT_FLOAT(sin(PI / 4)));
  assert(EXPECT_FLOAT(sin(sqrt(7+2) + 7 * PI - 47.05)));
  assert(EXPECT_FLOAT(sin(sin(sin(sin(sin(PI / 2)))))));

  assert(EXPECT_FLOAT(cos(0)));
  assert(EXPECT_FLOAT(cos(0.1234)));
  assert(EXPECT_FLOAT(cos(-0.1234)));
  assert(EXPECT_FLOAT(cos(PI / 6)));
  assert(EXPECT_FLOAT(cos(-PI / 6)));
  assert(EXPECT_FLOAT(cos(PI / 3)));
  assert(EXPECT_FLOAT(cos(-PI / 3)));
  assert(EXPECT_FLOAT(cos(PI / 4)));
  assert(EXPECT_FLOAT(cos(sqrt(7+2) + 7 * PI - 47.05)));
  assert(EXPECT_FLOAT(cos(cos(cos(cos(cos(PI / 2)))))));

  assert(EXPECT_FLOAT(tan(0)));
  assert(EXPECT_FLOAT(tan(0.1234)));
  assert(EXPECT_FLOAT(tan(-0.1234)));
  assert(EXPECT_FLOAT(tan(PI / 6)));
  assert(EXPECT_FLOAT(tan(-PI / 6)));
  assert(EXPECT_FLOAT(tan(PI / 3)));
  assert(EXPECT_FLOAT(tan(-PI / 3)));
  assert(EXPECT_FLOAT(tan(PI / 4)));
  assert(EXPECT_FLOAT(tan(sqrt(7+2) + 7 * PI - 47.05)));
  assert(EXPECT_FLOAT(tan(tan(tan(tan(tan(PI / 2)))))));

  assert(expect_float("-3 ** sin(1 - sqrt(3) * tan(10.0)) + 1", "",
		      -pow(3, sin(1 - sqrt(3) * tan(10))) + 1));
  assert(EXPECT_FLOAT(sin(cos(atan(37.0) + tan(PI / 6)))));

  assert(EXPECT_FLOAT(acos(0.1234)));
  assert(EXPECT_FLOAT(acos(-0.1234)));
  assert(EXPECT_FLOAT(acos(0)));
  assert(EXPECT_FLOAT(acos(1)));
  assert(EXPECT_FLOAT(acos(-1)));
  assert(EXPECT_FLOAT(acos(sqrt(2) / 2)));
  assert(EXPECT_FLOAT(acos(1 / sqrt(3))));

  assert(EXPECT_FLOAT(asin(0.1234)));
  assert(EXPECT_FLOAT(asin(-0.1234)));
  assert(EXPECT_FLOAT(asin(0)));
  assert(EXPECT_FLOAT(asin(1)));
  assert(EXPECT_FLOAT(asin(-1)));
  assert(EXPECT_FLOAT(asin(sqrt(2) / 2)));
  assert(EXPECT_FLOAT(asin(1 / sqrt(3))));

  assert(EXPECT_FLOAT(atan(0.1234)));
  assert(EXPECT_FLOAT(atan(-0.1234)));
  assert(EXPECT_FLOAT(atan(0)));
  assert(EXPECT_FLOAT(atan(1)));
  assert(EXPECT_FLOAT(atan(-1)));
  assert(EXPECT_FLOAT(atan(PI)));
  assert(EXPECT_FLOAT(atan( (1 + sqrt(5)) / 2 )));
  assert(EXPECT_FLOAT(atan(-867.326)));
  assert(EXPECT_FLOAT(atan(sqrt(2) / 2)));
  assert(EXPECT_FLOAT(atan(1 / sqrt(3))));

  assert(EXPECT_FLOAT(atan2(0.1234, 0.5678)));
  assert(EXPECT_FLOAT(atan2(-0.1234, 0.5678)));
  assert(EXPECT_FLOAT(atan2(0.1234, -0.5678)));
  assert(EXPECT_FLOAT(atan2(-0.1234, -0.5678)));
  assert(EXPECT_FLOAT(atan2(53, 386)));
  assert(EXPECT_FLOAT(atan2(0, 1)));
  assert(EXPECT_FLOAT(atan2(-sqrt(6), sqrt(55))));
  assert(EXPECT_FLOAT(atan2(1.0 / 2, sqrt(3) / 2)));
  assert(EXPECT_FLOAT(atan2(-1.0 / 2, sqrt(3) / 2)));
  assert(EXPECT_FLOAT(atan2(1 / 2.0, -sqrt(3) / 2)));
  assert(EXPECT_FLOAT(atan2(-1 / 2.0, -sqrt(3) / 2)));

  assert(EXPECT_FLOAT(sinh(0)));
  assert(EXPECT_FLOAT(sinh(0.1234)));
  assert(EXPECT_FLOAT(sinh(-0.1234)));
  assert(EXPECT_FLOAT(sinh(1 + PI + E)));
  assert(EXPECT_FLOAT(sinh(-PI / 12)));
  assert(EXPECT_FLOAT(sinh(-sinh(3.37))));

  assert(EXPECT_FLOAT(cosh(0)));
  assert(EXPECT_FLOAT(cosh(0.1234)));
  assert(EXPECT_FLOAT(cosh(-0.1234)));
  assert(EXPECT_FLOAT(cosh(1 + PI + E)));
  assert(EXPECT_FLOAT(cosh(-PI / 12)));
  assert(EXPECT_FLOAT(cosh(-cosh(3.37))));

  assert(EXPECT_FLOAT(tanh(0)));
  assert(EXPECT_FLOAT(tanh(0.1234)));
  assert(EXPECT_FLOAT(tanh(-0.1234)));
  assert(EXPECT_FLOAT(tanh(1 + PI + E)));
  assert(EXPECT_FLOAT(tanh(-PI / 12)));
  assert(EXPECT_FLOAT(tanh(-tanh(3.37))));

  assert(EXPECT_FLOAT(asinh(0)));
  assert(EXPECT_FLOAT(asinh(0.1234)));
  assert(EXPECT_FLOAT(asinh(-0.1234)));
  assert(EXPECT_FLOAT(asinh(1 + PI + E)));
  assert(EXPECT_FLOAT(asinh(-PI / 12)));
  assert(EXPECT_FLOAT(asinh(sinh(3.37))));

  assert(EXPECT_FLOAT(acosh(1)));
  assert(EXPECT_FLOAT(acosh(1.1234)));
  assert(EXPECT_FLOAT(acosh(2 - 0.1234)));
  assert(EXPECT_FLOAT(acosh(1 + PI + E)));
  assert(EXPECT_FLOAT(acosh(1 + PI / 12)));
  assert(EXPECT_FLOAT(acosh(cosh(3.37))));

  assert(EXPECT_FLOAT(atanh(0)));
  assert(EXPECT_FLOAT(atanh(0.1234)));
  assert(EXPECT_FLOAT(atanh(-0.1234)));
  assert(EXPECT_FLOAT(atanh(1 - 0.1234)));
  assert(EXPECT_FLOAT(atanh(tanh(3.37))));

  assert(EXPECT_FLOAT(erf(0)));
  assert(EXPECT_FLOAT(erf(0.1234)));
  assert(EXPECT_FLOAT(erf(0.5678)));
  assert(EXPECT_FLOAT(erf(1)));
  assert(EXPECT_FLOAT(erf(1.75)));
  assert(EXPECT_FLOAT(erf(13.8)));

  assert(EXPECT_FLOAT(erfc(0)));
  assert(EXPECT_FLOAT(erfc(0.1234)));
  assert(EXPECT_FLOAT(erfc(0.5678)));
  assert(EXPECT_FLOAT(erfc(1)));
  assert(EXPECT_FLOAT(erfc(1.75)));
  assert(EXPECT_FLOAT(erfc(13.8)));

  assert(EXPECT_FLOAT(lgamma(1)));
  assert(EXPECT_FLOAT(lgamma(0.1234)));
  assert(EXPECT_FLOAT(lgamma(2)));
  assert(EXPECT_FLOAT(lgamma(5)));
  assert(EXPECT_FLOAT(lgamma(-3.0/2)));
  assert(EXPECT_FLOAT(lgamma(1.0/2)));
  assert(EXPECT_FLOAT(lgamma(23.5128)));

  assert(EXPECT_FLOAT(tgamma(1)));
  assert(EXPECT_FLOAT(tgamma(0.1234)));
  assert(EXPECT_FLOAT(tgamma(2)));
  assert(EXPECT_FLOAT(tgamma(5)));
  assert(EXPECT_FLOAT(tgamma(-3.0/2)));
  assert(EXPECT_FLOAT(tgamma(1.0/2)));
  assert(EXPECT_FLOAT(tgamma(12.46)));

  assert(expect("42", "-b", "0b101010"));
  assert(expect("42", "-r 2", "0b101010"));
  assert(expect("42", "-o", "052"));
  assert(expect("42", "-r 8", "052"));
  assert(expect("42", "-x", "0x2a"));
  assert(expect("42", "-ux", "0x2A"));
  assert(expect("42", "-r 16", "0x2a"));
  assert(expect("42", "-r 16 -u", "0x2A"));
  assert(expect("-42", "-b", "-0b101010"));
  assert(expect("-42", "-o", "-052"));
  assert(expect("-42", "-x", "-0x2a"));
  assert(expect("42", "-r 12", "36"));
  assert(expect("42", "-r 20", "2:2"));
  assert(expect("-42", "-r 20", "-2:2"));

  assert(expect("4 > 2", "--bool", "true"));
  assert(expect("4 > 2", "-u --bool", "TRUE"));
  assert(expect("4 < 2", "--bool", "false"));
  assert(expect("4 < 2", "--bool -u", "FALSE"));

  assert(expect("783.123456789", "-p 0", "783"));
  assert(expect("783.123456789", "-p 1", "783.1"));
  assert(expect("783.123456789", "-p 2", "783.12"));
  assert(expect("783.123456789", "-p 8", "783.12345679"));

  assert(expect_float("sin(90)", "-d", sin(PI / 2)));
  assert(expect_float("cos(30)", "-d", cos(PI / 6)));
  assert(expect_float("acos(sqrt(2)/2)", "-d", acos(sqrt(2)/2) * 180 / PI));
  
  assert(expect_error("4 * (1 + 2", "", "unmatched parenthesis '('"));
  assert(expect_error("(5 + (1 - 2) / 3", "", "unmatched parenthesis '('"));
  assert(expect_error("3 + 4)", "", "unmatched parenthesis ')'"));
  assert(expect_error("(((47 + 3) * 8) + 2", "", "unmatched parenthesis '('"));
  
  assert(expect_error("37 + 8 +", "", "unexpected end of input"));
  assert(expect_error("-2 ** ", "", "unexpected end of input"));
  assert(expect_error("37 + (8 - 4 +) + 3", "", "unexpected token ')'"));
  assert(expect_error("12 * (-)", "", "unexpected token ')'"));
  assert(expect_error("* 13", "", "unexpected token '*'"));
  assert(expect_error("53 37", "", "unexpected token '37'"));
  assert(expect_error("5 ^ ^ 2", "", "unexpected token '^'"));

  assert(expect_error("5 > 3 ? 12", "", "unexpected end of input"));
  assert(expect_error("(5 > 3 ? 12)", "", "unexpected token ')'"));

  assert(expect_error("13 / 0", "", "division by zero"));
  assert(expect_error("13 // 0", "", "division by zero"));
  assert(expect_error("13 % 0", "", "mod by zero"));
  assert(expect_error("13.5 / 0.0", "", "division by zero"));

  assert(expect_error("PI = 37", "",
		      "assignment operator '=' is not supported"));
  assert(expect_error("12 = 13", "",
		      "assignment operator '=' is not supported"));
  assert(expect_error("E = (41 + 7) / 3", "",
		      "assignment operator '=' is not supported"));
  assert(expect_error("PI += 4", "",
		      "assignment operator '=' is not supported"));
  assert(expect_error("37 -= 8", "",
		      "assignment operator '=' is not supported"));

  assert(expect_error("2 + 53C4", "",
		      "unexpected digit 'C' in constant"));
  assert(expect_error("1 + 0182", "",
		      "unexpected digit '8' in octal constant"));
  assert(expect_error("02ff", "",
		      "unexpected digit 'f' in octal constant"));
  assert(expect_error("5 * 0b11020011", "",
		      "unexpected digit '2' in binary constant"));
  assert(expect_error("0x35a1.302", "",
		      "hexadecimal constant must be an integer"));
  assert(expect_error("0b110110.101011", "",
		      "binary constant must be an integer"));

  assert(expect_error("sin(13, 4)", "",
		      "function 'sin' does not take 2 arguments"));
  assert(expect_error("hypot(7)", "",
		      "function 'hypot' requires more than 1 argument"));
  assert(expect_error("hypot(7, 8, 9)", "",
		      "function 'hypot' does not take 3 arguments"));

  assert(expect_error("sine(13)", "",
		      "unknown function identifier 'sine'"));
  assert(expect_error("1 + magic(7, 12) - 3", "",
		      "unknown function identifier 'magic'"));
  assert(expect_error("47 + PIE", "",
		      "unknown identifier 'PIE'"));
  assert(expect_error("3 * apple + 1", "",
		      "unknown identifier 'apple'"));

  assert(expect_error("asin(1.5)", "",
		      "domain error in function 'asin'"));
  assert(expect_error("acos(-1.5)", "",
		      "domain error in function 'acos'"));
  assert(expect_error("log(-13)", "",
		      "domain error in function 'log'"));

  assert(expect_error("1 + 1", "-r 1", "radix cannot be less than 2"));
  assert(expect_error("1 + 1", "-r 0", "radix cannot be less than 2"));
  assert(expect_error("1 + 1", "-p -2", "precision cannot be less than 0"));

  assert(expect_error("5 // 1.5", "",
		      "integer division operator '//' requires integer "
		      "operands"));
  assert(expect_error("5.5 // 2", "",
		      "integer division operator '//' requires integer "
		      "operands"));
  assert(expect_error("1.5 % 4", "",
		      "modulo operator '%' requires integer operands"));
  assert(expect_error("10 % 2.5", "",
		      "modulo operator '%' requires integer operands"));

  assert(expect_error("3 & 4.5", "",
		      "bitwise AND operator '&' requires integer operands"));
  assert(expect_error("3.5 & 4", "",
		      "bitwise AND operator '&' requires integer operands"));
  assert(expect_error("-3 | 2.5", "",
		      "bitwise OR operator '|' requires integer operands"));
  assert(expect_error("-3.2 | 2", "",
		      "bitwise OR operator '|' requires integer operands"));
  assert(expect_error("3 ^ 0.5", "",
		      "bitwise XOR operator '^' requires integer operands"));
  assert(expect_error("3.5 ^ 2", "",
		      "bitwise XOR operator '^' requires integer operands"));
  assert(expect_error("1 + ~13.5", "",
		      "bitwise NOT operator '~' requires an integer operand"));
  assert(expect_error("1 << 3.5", "",
		      "bit shift operator '<<' requires integer operands"));
  assert(expect_error("1.5 << 3", "",
		      "bit shift operator '<<' requires integer operands"));
  assert(expect_error("100 >> 3.5", "",
		      "bit shift operator '>>' requires integer operands"));
  assert(expect_error("10.5 >> 3", "",
		      "bit shift operator '>>' requires integer operands"));  
  
  printf("%d tests completed successfully.\n", num_tests);
  return 0;
}

bool call_ccalc(char *expr, char *opts, char *output, int output_size) {
  char *prog = "./ccalc ";
  char *sep = " -- \"";
  char *end = "\" 2>&1";
  char command[BUF_SIZE];

  if (strlen(expr) + strlen(opts) + strlen(sep) + strlen(expr) + strlen(end)
      + 1 > BUF_SIZE) {
    fprintf(stderr, "Error: command string too big\n");
    return false;
  }
  
  strcpy(command, prog);
  strcat(command, opts);
  strcat(command, sep);
  strcat(command, expr);
  strcat(command, end);

  FILE *stream = popen(command, "r");

  if (!stream) {
    fprintf(stderr, "Error: could not create stream\n");
    return false;
  }

  fgets(output, output_size, stream);
  
  pclose(stream);

  ++num_tests;

  return true;
}

bool expect(char *expr, char *opts, char *expected) {
  char result[BUF_SIZE];
  char desired[BUF_SIZE];

  snprintf(desired, BUF_SIZE, "%s\n", expected);

  if (!call_ccalc(expr, opts, result, BUF_SIZE))
    return false;

  bool cmp = !strcmp(result, desired);

  if (!cmp) {
    printf("Test failed: ");
    printf("%s\nExpected: %sReceived: %s", expr, desired, result);
  }

  return cmp;
}

bool expect_int(char *expr, char *opts, long expected) {
  char value[BUF_SIZE];

  snprintf(value, BUF_SIZE, "%ld", expected);

  return expect(expr, opts, value);
}

bool expect_float(char *expr, char *opts, double expected) {
  char value[BUF_SIZE];

  snprintf(value, BUF_SIZE, "%f", expected);

  return expect(expr, opts, value);
}

bool expect_error(char *expr, char *opts, char *message) {
  char str[BUF_SIZE];

  snprintf(str, BUF_SIZE, "Error: %s", message);

  return expect(expr, opts, str);
}
