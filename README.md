# <img src="./robot_freedom_ai/assets/logo_small.png" width="40">  Triples-Lang 

Welcome to the official robotic langauage call Triples maintained by RobotFreedom.org. This code is designed to help students of all ages get hands on experience with AgenticAI platforms.  

Triples is a software language designed for AgenticAI Agents to use and create their own tools, integrated with a RDF database and communicate with other AIs.

The language is designed for portablity and has a Python, Arduino and CPP engines.

Triples-Lang is focused on interacting with Resource Description Framework (RDF) databases and the syntax follows a subject, verb, object (SVO) paradigm as much as possible. The SVO paradigm was chosen because it links directly to a RDF database meaning the returns for queries can be directly executed. Also LLMs are well suited parsing natural text to SVO and a non-LLM parser can be easily developed for deployment environments where LLMs are not an option like an Arduino. 

Triples-Lang is an explicit language reducing potential confusion and enabling it to be read out loud for interfacing verbally with a AgenticAI agent like a robot. However, its primary motivation is for an AI – not a human – to naturally connect to a RDF database.

## Table of Contents 

- [What's New](#what-is-new)
- [Installation](#installation)  
- [Getting Started](#getting-started)   
- [Operators](#operators)
- [Basic Commands](#basic-commands)

## What's New

This is the first alpha release, and there is still a lot of work to be done.  


## Installation

First, download this repro and unzip it on your Desktop or projects folder. Then, open a terminal and navigate to Triples-Lang. Next compile the code using g++ (this code will not compile using gcc). 

```bash 
 g++ -o triples_lang triples_lang.cpp
```

```bash 
 ./triples_lang

 set v 1;
 add v v;

```


## Getting Started

Triples-Lang is designed as a secure language for AI agents and not inteneded to compete other languages like Java, Python or C++. 

To run Triples-Lang you need to first install the software on a Arduino using the Arduino IDE. You can then inteact with Triples-Lang using either the Serial Monitor tool of running serial_com.py
 
Help provides a list of avaible commands.

```bash 
  
 help;

```
  

## Examples

Creating a variable uses the set command.

```bash 
set myar 1;
```

You can retrieve a variable using the get or echo commands.

```bash 
echo myvar;
```

Logic commands return a 1 or 0 (True /False). A number is return to make the output more portiable to Arduinos. 

```bash 
set myvar2 3;
more myvar1 myvar2;
```

Math functions return numeric (float) values

```bash 
add myvar1 myvar2;
```

Flow control is define between a start and end. After the end statement to cocde block is executed unless the block is inside another block.

```bash  
flow if start; 
less myvar1 myvar2; 
echo hello ; 
flow if end; 
``` 

User routines are run using the routines command. 

```bash 
routines create myfunction;
echo hello ;
routines create end;

routines execute myfunction;
``` 

## Operators

Triples-Lang does not have operators in a tradtional sinse becuase it is met to be spoken., Instead it maps seldom used early Modern English words to operators.

Put next output into prior statment (<-) 

```bash 
whense
```

Put prior ouput into next statment (->)
```bash 
whither
``` 
  
Comments are denoted by starting the line with the word annoatation 
```bash 
annotation
```

Other standard operators like + and - are called as functions add and subtract.

## Basic Commands
 
__VARIABLES__

*set* 

Stores a numeric value to memory

```bash 
set myvar 1;
```

*set* 

Stores a string value to memory

```bash 
set mytextvar  hello ;
```

*get*

Retrieve a variable from memeory

```bash 
get myvar;
```


__MATH__

*add*

Adds two variables together

```bash  
add myvar1 myvar2;
```

*subract*

Subtracts two variables together

```bash  
subtract myvar1 myvar2;
```

*divide*

Divides two variables together

```bash  
divide myvar1 myvar2;
```

*multiple*

Multiples two variables together

```bash  
multiple myvar1 myvar2;
```

*decrement*

Decreases the value of a variables  

```bash  
decrement myvar1 1;
```

*increment*

Increases the value of a variables  

```bash  
increment myvar1 1;
```
  

__LOGIC__

*more*

Returns 1/0 based on is the first varible is more than the second.

```bash  
more myvar1 myvar2;
```

*less*

Returns 1/0 based on is the first varible is less than the second.

```bash  
less myvar1 myvar2;
```
 
*different*

Returns 1/0 based on is the first varible is different than the second.

```bash  
different myvar1 myvar2;
```

*equal*

Returns 1/0 based on is the first varible is equal than the second.

```bash  
equal myvar1 myvar2;
```

__INPUT/OUTPUT__

*echo*

Returns a value to command stream.


```bash  
echo hello ;
```
  
__CONDITIONAL__

*flow if*
 
Define a conditional using the flow if command.

```bash  
flow if start;
more myvar1 myvar2;
echo "condition met";
flow if end;
``` 

__ROUTINES__

*create*

Define routine using the create directive.

```bash  
routine create my_funct;
echo Hello;
routine create  end;
```

*execute*

Run a routine using the execute directive.

```bash  
routine execute my_funct;
```

*create pass parameters*

Unlike other languages pass parameters are handled by create global values.

```bash  
set param1 1 ;
set param2 2  ;
routine create params ;
add param1 param2  ;
routine create end    ;
execute params   ;
'''