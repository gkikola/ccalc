ccalc
=====

**ccalc** is a fast, lightweight, and cross-platform command-line calculator,
ideal for performing quick calculations from the terminal. This is often
faster and easier than invoking an interactive or graphical program.


Examples
--------

Here are a few example calculations using **ccalc**. For detailed usage
information, see the **Usage** section below.

Basic operations:

    $ ccalc "2 + 2"
    4
    
    $ ccalc "21 - 3 * 5"
    6
    
    $ ccalc "8146 % 7"
    5

Built-in functions and constants:

    $ ccalc "1.4 * E"
    3.805595
    
    $ ccalc "max(12, 15)"
    15
    
    $ ccalc "sin(PI / 3)"
    0.866025
    
    $ ccalc "exp(3.2)"
    24.532530

Base conversion:

    $ ccalc --radix=13 "54"
    42
    
    $ ccalc --binary "0x4a"
    1001010
    
    $ ccalc --radix=60 "82709"
    22:58:29

Boolean tests:

    $ ccalc --bool "10 > 5"
    true
    
    $ ccalc --bool "PI == 3"
    false 


Copyright
---------

Copyright &copy; 2015-2017 Gregory Kikola. License GPLv3+: [GNU GPL version 3
or later](http://www.gnu.org/licenses/gpl.html).

**ccalc** is free software: you are free to change and redistribute it. There
is NO WARRANTY, to the extent permitted by law. See the file
[COPYING](COPYING) for more details.


Installation
------------

**From git repository:**

To clone the repository, run

    $ git clone https://github.com/gkikola/ccalc.git
    $ cd ccalc/

To generate the configure script, run

    $ ./bootstrap

Then run

    $ ./configure && make

to build the project and

    $ sudo make install

to install it.


**From source distribution:**

Download ccalc-*version*.tar.gz and run

    $ tar -xvzf ccalc-1.xx.tar.gz
    $ cd ccalc-1.xx/
    $ ./configure && make
    $ sudo make install


**From Windows binary distribution (Windows only):**

Download ccalc-*version*_w32.zip and extract it anywhere. You may then run
**ccalc** from the command prompt. You may need to modify your Path
environment variable if you want to run **ccalc** from a directory different
than the one you installed it in.


Usage
-----

    ccalc [OPTION...] [EXPRESSION]

**ccalc** takes an arithmetic expression as an argument, it evaluates this
expression, and then it prints the result. If no expression is given,
**ccalc** will read lines from standard input. Any program options must be
specified before the start of an expression.

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

Mandatory arguments to long options are also mandatory for the corresponding
short options.

The input expression is assumed to follow the syntax of the C programming
language. All C operators are supported except for those with side-effects
(namely assignment, increment, and decrement). In addition, an exponentiation
operator `**` and an integer division operator `//` are provided. A few
mathematical constants like `PI` and `E` are defined, and most of the math
functions in the C standard library are available.

Note that **ccalc** treats the `^` operator as C does: as the bitwise XOR
operator and not as exponentiation. This behavior can be changed with the
--caret-exp option. Otherwise the `pow` function or `**` operator may be
used.

Note also that, unlike in C, the division operator `/` may
produce a floating-point result even when both operands are integers. To
force integer division, you may use the `//` operator.

Both integer and floating-point values may be used in the input expression.
**ccalc** will perform conversions where necessary. Integer values may also
be specified in binary, octal, or hexadecimal. Binary values should be
prefixed with `0b`, octal values with `0`, and hexadecimal values with `0x`.
