# C- Compiler

Student:
- Bruno Sampaio Leite

C- Compiler built based on Kenneth C. Louden's Tiny Compiler available at: "Compiler Construction: Principles and Practice". Louden, Kenneth C. 1997.

Target machine's HDL Code (Verilog) available at: https://github.com/brunosampaio08/acc_proccessor

Up-to-date version of this compiler's code available at: https://github.com/brunosampaio08/compiler_c-

## Usage:

Terminal Commands:
- cd compilador_c-
- make
- ./compiler \<input-file-name>

## Attention:

Outputs on folder: compilador_c-/output \
Inputs on folder: compilador_c-/input

Sinthesys will only run if there are no syntax or semantic errors on input file.
- This can be changed in main.

After compiling 'make clean' will clean all unnecessary files.
- After make clean it's necessary to make again in order to run ./compiler.

The first part of this compiler (lexical, syntax and semantic analysis) was built along with @ludmilalima. The second part, (intermediary, assembly and binary code generation) was built as a solo project.
