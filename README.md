# CS 3512 - Programming Languages  
**Programming Project 01**  
**Group Number:** 03  
**Group Members:**  
- Harikishna N. 210206B  
- Harismnenan J. 210207E  

## Project Overview  
This project involves creating a C++ program to parse and execute code written in the RPAL language. The task is to develop a lexical analyzer and a parser that converts RPAL code into an Abstract Syntax Tree (AST). This AST is then transformed into a Standardized Tree (ST) for execution by a CSE machine.

## Problem Description  
The project requires implementing the following components:
- **Lexical Analyzer:** Breaks down RPAL code into tokens.
- **Parser:** Constructs an AST from tokens.
- **AST to ST Conversion:** Transforms the AST into a ST.
- **CSE Machine:** Executes the code based on the ST.

The output of the program should be consistent with the results of "rpal.exe".

## Project Structure  
The project is implemented in C++ and consists of the following key files:

- **`main.cpp`:** The entry point of the program that handles file reading and initialization of the parser.
- **`parser.h`:** Contains the Recursive Descent Parser that tokenizes the input and builds the AST. It also handles conversion to the ST and executes code using the CSE machine.
- **`tree.h`:** Defines the `tree` class used to represent nodes in the AST and ST.
- **`token.h`:** Defines the `token` class for representing and managing tokens during lexical analysis.
- **`environment.h`:** Implements the `environment` class for managing variable bindings and execution contexts in the CSE machine.

## Key Functions and Classes  
- **`main.cpp` Functions:** Parses command-line arguments, reads the input file, and initializes the parser.
- **`parser.h` Functions:** Includes tokenization (`getToken`), AST construction (`buildTree`), ST conversion (`makeST`), and execution (`cse_machine`).
- **`tree.h` Class:** Represents tree nodes with functions for setting and getting node values and types, and printing the tree.
- **`token.h` Class:** Manages token attributes and comparisons.
- **`environment.h` Class:** Handles variable bindings in the execution environment.

## Conclusion  
This project demonstrates a comprehensive approach to implementing a programming language parser and executor. By creating a lexical analyzer, parser, and CSE machine, the project effectively processes and executes RPAL code, showcasing skills in compiler design and language processing.
