# C Compiler tests
SCCC uses the compiler tests as written by Nora Sandler. These have been modified to suit the limitations and extra features of this compiler. Stage 1-10 have been modified and expanded a bit, whereas Stage 11 and 12 were created specifically for this project.
Note this file has been edited for SCCC purposes, as has the testing script.


# Write a C Compiler!

This is a set of C test programs to help you write your own compiler. They were written to accompany [this tutorial](https://norasandler.com/2017/11/29/Write-a-Compiler.html).

## Usage

### test all
```
./test_compiler.sh sccc
```

### test specific stages
To test stage 1 and stage 3,
```
./test_compiler.sh sccc 1 3
```
To test from stage 1 to stage 6,
```
./test_compiler.sh sccc `seq 1 6`
```

In order to use this script, your compiler needs to follow this spec:

1. It can be invoked from the command line, taking only a C source file as an argument, e.g.: `./YOUR_COMPILER /path/to/program.c`

2. When passed `program.c`, it generates executable `a.out` in the top directory

3. It doesn’t generate assembly or an executable if parsing fails (this is what the test script checks for invalid test programs).

The script doesn’t check whether your compiler outputs sensible error messages, but you can use the invalid test programs to test that manually.

## Contribute

Additional test cases welcome! You can also file issues here, either about the test suite itself or about the content of the tutorial.
