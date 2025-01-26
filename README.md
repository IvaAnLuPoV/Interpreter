## Interpreter
Developed an interpreter for a simple programming language as coursework for "Data Structures in Programming" in FMI, Sofia University

## Commands
# READ <variable>
Reads an integer value and stores it in <variable>.

# PRINT <expression>
Prints the value of the expression.

# GOTO <label> / LABEL <label>
Introduces goto functionality, similar to that of C++.

# IF <expression> [ELSE]/ENDIF
Allows branching of the program, dependant on the result of <expression>.

# WHILE <expression> / DONE
Allows loops, dependant on the result of <expression>.

## Compilation
The interpreter supports both written files and REPL.
For written file with name <file>, compile the main:
```
g++ -o main main.cpp
```
and pass the files name as first parameter:
```
./main <file>
```
