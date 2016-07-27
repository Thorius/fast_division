#Fast Division

A simple library for integer division through shifts and multiplications.

##About

Most integers operations on modern CPUs can be thought of as essentially free from
a computational standpoint. While this is certainly true for addition and multiplication, 
the same does not hold for integer division. Integer division can have a latency around 10-20
greater than multiplication and the difference is even more significant compared to simpler 
operations such as shifts.

A way to improve performance is to use combinations of multiplications and shifts that would
produce the exact same result as division by some integer. The technique for doing this is
described in [Division by Invariant Integers Using Multiplication](https://gmplib.org/~tege/divcnst-pldi94.pdf). 
This technique is used by most compiler optimizers when you divide using a compile time constant.
However, optimizers cannot in general do this as the calculation of the invariant integers for the 
multiplication and the shifts has overhead that outweighs the benefits.

Still, a lot of the time you would want to have a runtime variable and use it as a divider in several
expressions. In this case, it is beneficial to compute the invariant values beforehand and use them 
instead of simple division. This is where this library comes in. The goal is to provide this interface
without too much trouble.

##Usage
This is a simple header only library. Simply clone it and include the fast_division.hpp file in your project.

##Future Directions
This implementation is very bare-bones at the moment. It only currently supports division by unsigned 32-bit
integers. I plan to add support for other formats in the future.     

