This is a monkey project for a monkey subject called SPO (Системное программное обеспечение , СПО  ). May this be a tribute to the health conditions I developed while trying to pass this subject, may this be a shield to you dear reader so you can pass it easier than I had to. 
﻿MonKeys Programming Language Compiler
This project is a custom compiler for a dynamically typed programming language with support for user-defined types, inheritance, method overriding, and templating. It compiles source code written in a high-level custom language into linear intermediate code, and then assembles it for execution on a stack-based virtual machine with shared memory (common RAM).

🚀 Features
Dynamic typing

Custom user-defined types (classes/structs)

Inheritance with member and method extension

Method overriding and polymorphism

Template support for generic programming

Linear intermediate code generation

Assembler for stack-based virtual machine instructions

Console-based runtime inspector

📦 Components
Parser: Supports extended syntax for types, inheritance, and member access

Control Flow Builder: Constructs an operation tree with type-aware instructions

Code Generator: Converts operations into linear stack-machine code

Assembler: Translates intermediate code into VM instructions

Inspector Tool: Displays runtime values including nested type fields and local variables

📄 Example Workflow
Write program using the custom language syntax

Compile: Parse → Build operation tree → Generate linear code

Assemble: Produce VM-executable code

Run: Execute using virtual machine backend

Inspect: Use console inspector to analyze runtime state

🔧 Build & Run
Requires Python/C++/your environment tools (update based on your actual tech stack)

bash
Copy
Edit
# Compile the source program
./compiler my_program.src -o my_program.lcode

# Assemble linear code
./assembler my_program.lcode -o my_program.bin

# Run in virtual machine
./vm my_program.bin

# Optional: Inspect during/after execution
./inspector my_program.bin
📁 Example Files
examples/inheritance.src – Demonstrates class extension and method overriding

examples/templates.src – Shows template usage with dynamic types

examples/inspector_demo.src – Program for interactive inspection

✅ Status
This project was developed as part of a lab assignment focused on building a compiler from scratch. All required subtasks have been successfully implemented and tested.
