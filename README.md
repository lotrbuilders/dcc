# DCC
A C compiler with a relatively easy to modify backend. Under hosted conditions it can compile itself as long as all the header files functions are also defined somewhere.
## Limitations
This compiler does not handle unions or typedefs and does not have unsigned integers or floats at present. The operation or, xor, shift left, shift right and increment and decrement are currently unsupported.
Most anything else is present though K&R style declarations are not allowed and will produce errors.
The compiler only turns preprocessed C into assembly. The top-level manager can call other programs to do preprocessing, assembling and linking. It uses the standard GCC tools for that and the x86 backend also uses glibc for library functions. 
### Known errors
Function pointers are available though calling the return value of a function returning a function pointer does not work presently.
## Backend
Two backends are available: one for x86-32 and one for the mod5 processor. The generator produces stack machine calls, which can be processed by the backend. The backend itself is responsible for correctly calling functions via it's own standard. Register allocation is the responsibility of the backend and quite inefficient at the moment. x86 does not bother with register allocation, whereas mod5 uses the first 8 registers as temporaries.
## Optimization
All operations on two constants are optimized to a single constant. Optimizition is non optional.
## Installation
Currently installation targets linux only. For windows users WSL works.
First create a builddirectory(standard is ./obj). You can then either make all or execute run.sh. Both will compile everything, but the second will run a test script. 
After the makefile has been run there is an install.sh script, which can put the files in the correct location for a linux setup. To edit the locations have a look at /src/etc/dcc.c and the install.sh script.
## Licensing
The compiler is licensed under GPL as shown by the license. The testsuite under /testing/ is licensed under MIT. 
Including the header files in the /include/ in a project is considered normal use of the software. Software that uses those header files as is normal in the C programming language does not automatically have to be GPL licensed.
