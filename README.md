# GenStruct

## Philosophy

Thie repo contains general purpose data structures for fast project prototyping and copy-pasting. 

Source files in this repo are NOT meant to be compiled as standalone libraries or object files. Instead,
treat them as a template for a data structure you want to implement. Copy-paste the template to your project
and do some quick tweak, if needed, to make it work with as little effort as possible.

Source files in this repo are NOT meant to be super fast, highly optimized, coded with high-IQ language hacks,
showing off personal skills, etc. Expect only ordinary and canonical stuff. 
Coding principles and standards for readability, however, are fully imposed and carried out throughout the entire repo.
In fact, the motivation of opening this repo is simply because I no longer want to reinvent the wheel for each new
project I started. I will also keep this repo constantly updated as soon as I reinvented another wheel.

## Directory Structure

This repo is only loosely organized. Each directory under src/ds/ is an independent data structure implementation.
Each sub-directory also has their own README files for detailed information on the usage and interface. 
I will strive to keep the source file organization as uniform as possible. In most cases, the sub-directory will
have the following files:

- A ds_name.c source file as the implementation
- A ds_name.h header file for declarations and macros
- A `test.c` file containing `main()` and tests.
- A `Makefile` that compiles the source file and the test
- An optional README.md if I feel it is necessary
