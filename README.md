ccalc
=====

**ccalc** is a command-line calculator. ccalc takes a C expression as an
argument, evaluates it, and then prints the result. For quick calculations,
this can be faster and easier than invoking an interactive or graphical
program.


License
-------
ccalc is *free software*, free in the sense that it
[respects the user's freedom] (http://www.gnu.org/philosophy/free-sw.html).

ccalc is copyright &copy; 2015-2016 Gregory Kikola. ccalc is licensed under
the terms of the
[GNU General Public License] (http://www.gnu.org/licenses/gpl.html) as
published by the [Free Software Foundation] (http://www.fsf.org/), either
version 3 of the License or (at your option) any later version. See the file
[COPYING] (COPYING) for details.


Examples
--------

Here are a few example calculations using ccalc. For detailed usage
information, see the **Usage** section below.

    $ ccalc 2 + 2
    4

    $ ccalc PI / 2
    1.570796

    $ ccalc "sin(PI / 3)"
    0.866025

    $ ccalc "0x10ff << 3"
    34808

    $ ccalc "(log(3) > 1.5) ? 1 : -1"
    -1


Usage
-----

    ccalc [OPTION...] EXPRESSION

Evaluate the C-style *EXPRESSION* and display the result. If no expression is
given, read one line from standard input.

| Option | Long name             | Description                                |
|--------|-----------------------|--------------------------------------------|
| -b     | --binary              | Print integer results in binary (base 2)
|        | --bool                | Interpret the result as a boolean value and print true or false
| -c     | --caret-exp           | Use caret ^ for exponentiation rather than for bitwise XOR
| -d     | --degrees             | Use degrees instead of radians for trigonometric functions
| -g     | --grouping=*DIGITS*   | Group each set of *DIGITS* digits and separate each group with spaces (use 0 for no grouping)
| -o     | --octal               | Print integer results in octal (base 8)
| -p     | --precision=*DIGITS*  | Print floating-point results with *DIGITS* digits after the decimal point (default 6)
| -r     | --radix=*RADIX*       | Print integer results in base *RADIX*
| -s     | --scientific-notation | Always print floating-point results in scientific notation, [-]d.ddde±dd
| -t     | --time                | Show how much time the computation took
| -u     | --uppercase           | Use uppercase rather than lowercase letters for digits in bases greater than 10
| -x     | --hexadecimal         | Print integer results in hexadecimal (base 16)
| -?     | --help                | Give detailed usage information
|        | --usage               | Give a short usage message
|        | --version             | Display version information and exit

Mandatory or optional arguments to long options are also mandatory or optional
for any corresponding short options.

The *EXPRESSION* is interpreted as a C expression and evaluated. Integer
operations are used when possible. If a floating-point value is given
explicitly, or if an operation results in a non-integer value, then
floating-point arithmetic is used for the remainder of the calculation.

**WARNING:** The caret ^ indicates bitwise XOR, like in C, and not
exponentiation. This behavior can be changed with the '-c' option.
Exponentiation can also be performed using the **pow** function or (unlike in
C) the ** operator.

Many mathematical functions from the C standard library are available. Common
mathematical constants like PI and E are also defined. Consult the program
documentation for a complete list.
