# DCC
A C compiler with a relatively easy to modify backend. Under hosted conditions it can compile itself as long as all the header files functions are also defined somewhere.
## Limitations
This compiler does not handle unions or typedefs and does not have unsigned integers or floats at present. Most anything else is present though K&R style declarations are not allowed and will produce errors.
The compiler only turns preprocessed C into assembly. The top-level manager can call other programs to do preprocessing, assembling and linking. It uses the standard GCC tools for that and the x86 backend also uses glibc for library functions. 
### Known errors
Function pointers are available though calling the return value of a function returning a function pointer does not work presently.
## Backend
Currently only an x86 backend is available. The generator produces stack machine calls, which can be processed by the backend. The backend itself is responsible for correctly calling functions via it's own standard. Comparison and jump combinations are very inefficient at this moment as are || and && currently, though a fix is planned. Register allocation is the responsibility of the backend and quite inefficient at the moment.
## Optimization
Currently not present
## Installation
After the makefile has been run there is an install.sh script, which can put the files in the correct location for a linux setup. To edit the locations have a look at /src/etc/dcc.c and the install.sh script.
## Licensing
The compiler is licensed under GPL as shown by the license. The testsuite under /testing/ is licensed under MIT. 
