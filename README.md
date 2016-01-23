BPF Assembler
Copyright (c) 2003, John T Criswell

===============================================================================

What is the BPF Assembler?
--------------------------
The BPF Assembler is a program that eases the development of filter programs
for the Berkeley Packet Filter.  It takes as input a program written with
mnenomics representing BPF filter machine operations and produces an output C
header file containing a struct bpf_program structure that can be passed to
libpcap functions.

What platforms does the BPF Assembler support?
----------------------------------------------
The BPF Assembler has had rudimentary testing on MkLinux on PowerPC, but it
should compile and run on any UNIX like operating system with little/no
effort.

Note that there might be dependencies on features of Flex, Bison, and GNU Make.
Any such dependencies should be minimal.

Where can I get it?
-------------------
The BPF Assembler can be downloaded from the following website in the
"Portfolio" section:

http://www.bigw.org/~jcriswel

What is the instruction set?
-----------------------------
Instructions to the assembler begin with an optional label, an opcode, and zero
or more operands.  Labels begin with a percent sign (%).  Comments begin with a
pound sign (#) and go until the end of the line.

A program starts with the following special line:

	STRUCT <name>

The value <name> is used to define the name of the generated structure in the
generated C header file.  For example, the line:

	STRUCT Pooh

generates a structure named "Pooh" that refers to a structure named
"Pooh_array."

Values are any unrecognized alphanumeric characters, decimal numbers, and
hexadecimal numbers.  Values in brackets ([]) are taken literally, allowing for
preprocessor macros and C expressions to be used as values in operands.

The instruction set details can be understood by examining bpf.y, and it is
generally assumed that the user has read the bpf(4) man page.  However, the
following overview should make things a little easier to understand:

Loads
-----
LDA  <Value> - Load Accumulator with word from memory.
LDAI <Value> - Load Accumulator with immediate value.
LDAB <Value> - Load Accumulator with byte from packet.
LDAH <Value> - Load Accumulator with halfword from packet.
LDAW <Value> - Load Accumulator with word from packet.
LDAL         - Load Accumulator with packet's length.

LDAH, LDAH, and LDAW may also be used with X register indexing:

LDAH <Value>,X

LDX   <Value> - Load X Register with word from memory.
LDXI  <Value> - Load X Register with immediate value.
LDXL          - Load X Register with packet's length.
LDXIP         - Load X Register with length of IP header.

Stores
------
STA <Value> - Store Accumulator word in memory.
STX <Value> - Store X Register word in memory.

Transfers
---------
TXA - Transfer X register to Accumulator.
TAX - Transfer Accumulator to X register.

Arithmetic
----------
Arithmetic operations (except NEG) are of the form:

<optional label> <opcode>

or

<optional label> <opcode> <Value>

The first form uses the value in the X Register as the operand.  The second
form uses the specified constant as the operand.  All results are placed in the
Accumulator.

The NEG opcode has no operand.

ADD - Add.
SUB - Subtract.
MUL - Multiply.
DIV - Divide.
AND - Bitwise AND.
ORA - Bitwise OR.
LSH - Left Shift.
RSH - Right Shift.
NEG - Negate Accumulator.

Jumps
-----
JMP <label> - Jump to label.

Branches
--------
All branches are of the form:

<optional label> <opcode> <Value> <true label> <false label>

or

<optional label> <opcode> <true label> <false label>

The first form compares the Accumulator against a constant.  The second form
compares the Accumulator against the X register.

If the comparison is true, control will flow to the <true label>.  Otherwise,
it will transfer to <false label>.

Note that some of these operations are actually pseudo-ops; the BPF assembler
will arrange the true/false destinations and pick an actual BPF opcode to
represent the branch.

BGT   - Branch if greater than.
BGE   - Branch if greater than or equal to.
BLT   - Branch if less than.
BLE   - Branch if less than or equal to.
BEQ   - Branch if equal.
BNE   - Branch if not equal.
BSET  - Branch if Accumulator has bits set that are set in the compared value.
BNSET - Branch if Accumulator does not have bits set that are set in the
        compared value.

Return
------
RET         - Exit, returning the value in the accumulator.
RET <Value> - Exit, returning the specified value.

Are there any example programs?
-------------------------------
Example programs can be found in the samples directory.

