### HCC (Hack Compiler Collection) ###

This is composed of 3 parts, a compiler, a vm translator and an assembler. Applied one after the other, they'll convert a program written in "Jack" down to a hack binary file composed of zeros and ones, capable of running on the Hack CPU.

If you wanted, you could write a compiler for java to get to this vm intermediary language, and then java programs would run on the hack cpu too.

Check the root folder README for instructions on how to compile and use it.

If you want to contribute, you need Boost on your path somewhere and
you can run the tests with:
```bash
$ make all test
```
