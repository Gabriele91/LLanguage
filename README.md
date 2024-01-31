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
  - [Examples](#examples)
    - [Operators overloading](#operators-overloading)
    - [Inheritance, call the parent constructor](#inheritance-call-the-parent-constructor)
    - [Variadic arguments](#variadic-arguments)
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

## Examples
In this section, we present a couple of examples of L Language

### Operators overloading
Example of a vec2 class with the operators overload

```
class vec2
{
    var x,y
    
    def vec2(x,y)
    {
        this.x = to_float(x) or 0.
        this.y = to_float(y) or 0.
    }
    
    operator + (value)
    {
        if type_of(value) == "object"
            return vec2(this.x+value.x,this.y+value.y)
        else
            return vec2(this.x+to_float(value),this.y+to_float(value))
    }
    
    operator - (value)
    {
        if type_of(value) == "object"
            return vec2(this.x-value.x,this.y-value.y)
        else
            return vec2(this.x-to_float(value),this.y-to_float(value))
    }
    
    operator -
    {
        return vec2(-this.x,-this.y)
    }
    
    operator * (value)
    {
        if type_of(value) == "object"
            return vec2(this.x*value.x,this.y*value.y)
        else
            return vec2(this.x*to_float(value),this.y*to_float(value))
    }
    
    operator / (value)
    {
        if type_of(value) == "object"
            return vec2(this.x/value.x,this.y/value.y)
        else
            return vec2(this.x/to_float(value),this.y/to_float(value))
    }
    
    operator % (value)
    {
        if type_of(value) == "object"
            return vec2(this.x%value.x,this.y%value.y)
        else
            return vec2(this.x%to_float(value),this.y%to_float(value))
    }
    
    operator right * (value)
    {
        return vec2(to_float(value) * this.x,to_float(value) * this.y)
    }
    
    
    def print()
    {
        io.println("(",this.x, ", ", this.y,")")
    }
}

x = ( ( vec2(3,4) + vec2(3,4) - vec2(3,8) ) * vec2(4,1) / vec2(2,1) ) % vec2(4,1)
y = vec2(4,1) % 4
z = 3 * vec2(3,2)
w = -vec2(3,2)
x.print()
y.print()
z.print()
w.print()
```
### Inheritance, call the parent constructor
```
class User
{
	def print_name_and_sum()
	{
		println(this.name, " - ", this.sum)
	}
	def sum_inc()
	{
		this.sum += 1
	}
	
	operator + (right) //always public
	{
		u = User()
		u.name = this.name + right.name
		return u
    }

protected:
	
	var name = ""

private:

	var sum = 0
			
}

class Mario : User
{	
	def Mario()
	{
		this.User()
		this.name = "Mario"
	}
	def sum_to_0()
	{
		this.sum = 0 //<- error is private
	}
}
```
### Variadic arguments
```
class vector
{
    var data = []
    
    def vector(...args) this.data = args
    
    def get(i) return this.data[i]
    
    def set(i,value) this.data[i] = value
    
    operator + (value)
    {
        o = vector()
        //copy data
        o.data = copy(this.data)
        //add...
        if ( type_of(value)    == "object" &&
             class_name(value) == "vector" )
        {
            for arg_v of value.data
                o.data.push(arg_v)
        }
        else
        {
            o.data.push(value)
        }
        return o
    }
    
    def forall(call)
    {
        for (k in this.data) call(k,this.data[k])
    }
}

x = vector( 2, 3, 4 )
x+= vector(-5,-6,-7 )

x.forall(def (x,y) println("key: ",x+1,"\t value: ",y))
```

## Contributing

Feel free to contribute to the development of the L language interpreter by opening issues or submitting pull requests.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.