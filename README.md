
# Julia++
This is my simple programming language created for the semester work. It has no dependence with the already existing language *Julia* and is named only in honor of the author.

## Features:
* comment support;
* label and *goto* support;
* several operators for output;
* increment and decrement support;
* double (and more) assignment;
* function support and automatic type detection (*void* or *int*);
* recursion support;
* detailed error messages;
* local and global scopes.

## How to use it?
0. Use this command to build the library:
```
make lib
```

1. Put the **julia.a** archive in the project directory and include it:
```c++
#include "julia.a"
```
2. Library supports two modes.
* File mode (the text of your code):
```c++
Julia julia;
std::ifstream file("code.jul");
julia.run(file, debug_mode);
```
* String mode:
```c++
Julia julia;
// some function to get a string with the code
// (use ';' as a sepatator for lines)
std::string code = read_code();
julia.run(code, debug_mode);
```
`debug_mode` is a boolean value. Set **true** if you want to print debug information and **false** otherwise. 
3. Put the **include** folder in the project directory and compile:
```bash
g++ main.cpp julia.a -I include -o main
```
4. Success!

## How to learn it?
Use the **examples** directory as a tutorial. Every file contains all necessary language information and features description.
