BPF Assembler
Copyright (c) 2003, 2016 John T Criswell
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
The BPF Assembler has had rudimentary testing on Mac OS X, Linux, and
(a very long time ago) on MkLinux on PowerPC.  It should compile and run
on any UNIX like operating system with little/no effort.

Note that you will need GNU Flex, GNU Bison, and Make to compile the code.
The Makefile does not use GNU Make extensions, so it should work with most
versions of Make.

Where can I get it?
-------------------
The BPF Assembler can be downloaded from Github at the following URL:

https://github.com/jtcriswell/bpfa

How do I compile it?
--------------------

Run the "make" program to compile the code.

How do I run the BPF Assembler?
-------------------------------
Run the bpfa program to run the assembler. The file containing the BPF
assembler code input should be specified on the command line.  The output will
be written to standard output by default:

	% ./bpfa file.bpf

The -o option can be used to specify an output file:

	% ./bpfa -o file.h file.bpf

What is the instruction set?
----------------------------
Instructions to the assembler begin with an optional label, an opcode, and zero
or more operands.  Labels begin with a percent sign (%) and either define the
location of an instruction or a location in the scratch memory.  Comments begin
with a pound sign (#) and run until the end of the line.

A program starts with the following special line:

	STRUCT <name>

... and ends with the following special line:

	END

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

Memory
------
The keyword VAR create a label that denotes a location within the scratch
memory.  The following directive reserves a memory location with the label
%port:

	VAR %port

Instructions that take memory locations as operand (LDA, STA, LDX, STX)
take memory location labels as arguments.

Loads
-----

The accumlator register may be loaded using the following instructions:

	LDA  <Label> - Load Accumulator with word from memory.
	LDAI <Value> - Load Accumulator with immediate value.
	LDAB <Value> - Load Accumulator with byte from packet.
	LDAH <Value> - Load Accumulator with halfword from packet.
	LDAW <Value> - Load Accumulator with word from packet.
	LDAL         - Load Accumulator with packet's length.

LDAH, LDAH, and LDAW may also be used with X register indexing:

	LDAH <Value>,X

The index register (X) may be loaded using the following instructions:

	LDX   <Label> - Load X Register with word from memory.
	LDXI  <Value> - Load X Register with immediate value.
	LDXL          - Load X Register with packet's length.
	LDXIPV4LEN    - Load X Register with length of IPv4 header.

Stores
------
	STA <Label> - Store Accumulator word in memory.
	STX <Label> - Store X Register word in memory.

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

	[optional label] <opcode> <Value> <true label> [false label]

The first form compares the Accumulator against a constant.  The second form
compares the Accumulator against the X register.

If the comparison is true, control will flow to the true label.  Otherwise,
it will transfer to the false label if it is specified or to the next
instruction if no false label is specified.

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

