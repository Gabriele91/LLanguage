# L Language Interpreter

This repository contains the interpreter for the L language, a dynamically typed interpreted language. L language syntax and features are described using Extended Backus-Naur Form (EBNF) in this README.

## Table of Contents
- [L Language Interpreter](#l-language-interpreter)
  - [Table of Contents](#table-of-contents)
  - [Syntax Overview](#syntax-overview)
  - [Constants](#constants)
  - [Operators](#operators)
  - [Statements](#statements)
    - [If Statement](#if-statement)
    - [Loop Statements](#loop-statements)
    - [Function and Method Definitions](#function-and-method-definitions)
    - [Return Statement](#return-statement)
    - [Context and Scope](#context-and-scope)
    - [Object-Oriented Programming (OOP)](#object-oriented-programming-oop)
      - [Class Definition](#class-definition)
      - [Inheritance](#inheritance)
      - [Access Modifiers](#access-modifiers)
  - [Contributing](#contributing)
  - [License](#license)

## Syntax Overview

L language is designed to be expressive and easy to understand. Here's a brief overview of its syntax:

- **Constants**: Variables, integers, strings, and double values.
- **Operators**: Logical operators (`and`, `or`), comparison operators (`<`, `>`, `==`), arithmetic operators (`+`, `-`, `*`, `/`), and more.
- **Statements**: Includes if statements, loops (while and for), function and method definitions, and return statements.
- **Context and Scope**: Defines global and super contexts for variables and their assignments.

## Constants

- **variable**: Any alphanumeric character.
- **string**: A sequence of characters enclosed in double quotes.
- **constant**: Can be a double, integer, or string.

## Operators

- **Logical Operators**: `and`, `or`, `&&`, `||`.
- **Comparison Operators**: `<`, `>`, `==`, `<=`, `>=`.
- **Unary Operators**: `!` (negation).
- **Arithmetic Operators**: `+`, `-`, `*`, `/`, `%`.

## Statements

### If Statement

```
if exp stamentsblock [else_if] [else]
else_if: else if exp stamentsblock
else: else stamentsblock
```

### Loop Statements

- **While Loop**:
```
while exp stamentsblock
```

- **For Loop**:
```
for ( (for_each_args | for_c_args) [';'] | '(' (for_each_args | for_c_args ) ')') stamentsblock
```

### Function and Method Definitions

- **Function Definition**:
```
def [variable] '(' ([ variable {[',' variable]} [',' '...' variable] ] | ['...'variable]) ')' stamentsblock
```

- **Operator Overloading**:
```
operator ['right'] ('+'|'-'|'*'|'/'|'%'|logic_comp|logic_op|logic_one)
                   '(' ([ variable {[',' variable]} [',' '...' variable] ] | ['...'variable]) ')'
                    stamentsblock
```

### Return Statement

```
return [exp]
```

### Context and Scope

- **Type Context**: Defines global or super contexts for variables and their assignments.
```
type context = ('global' | 'super') [ variable [assignament exp] {[ ',' variable [assignament exp] ]} ]
```

### Object-Oriented Programming (OOP)

L language supports Object-Oriented Programming (OOP) with classes and inheritance.

#### Class Definition

```
class ClassName
{
    // Class fields and methods
}
```

#### Inheritance

```
class SubClassName : SuperClassName
{
    // Additional fields and methods
}
```

#### Access Modifiers

- **public**: Accessible from anywhere.
- **protected**: Accessible within the class and its subclasses.
- **private**: Accessible only within the class.


## Contributing

Feel free to contribute to the development of the L language interpreter by opening issues or submitting pull requests.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.