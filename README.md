# data-structures-algorithms-c

A set of generic hashing-based data structures and algorithms were implemented from first principles in straight C, providing i) potential space and speed advantages relative to the use of C++ abstractions in settings with limited memory resources, ii) measurements of performance bottlenecks, and iii) parameters for hashing and parallelism optimization across input ranges and hardware settings (work in progress).

Additional information relating to the style of the provided implementations is available at https://wiki.sei.cmu.edu/confluence/display/c/3+Recommendations.

In each directory with a Makefile, run:

`make`

`./executable-name`

`make clean` or `make clean-all`

**Design and implementation highlights:**

`./algorithms-mthread/mergesort-mthread/`

A merge sort algorithm with parallel sorting and parallel merging for sorting arrays of generic elements, providing \Theta(n/log^{2}n) theoretical parallelism within the dynamic multithreading model, and demonstrating a parametrization approach to optimizing the parallelism of a recursive algorithm.

The implementation provides a set of parameters for setting the constant base case upper bounds for switching from parallel sorting to serial sorting and from parallel merging to serial merging during recursion, thereby enabling the optimization of the parallelism and concurrency-associated overhead across input ranges and hardware settings. On a 4-core machine, the optimization of the base case upper bound parameters, demonstrated in the accompanying tests, resulted in a speedup of approximately 2.6X in comparison to serial qsort (stdlib.h) on arrays of 10M random integer or double elements.

`./data-structures/heap/`

A generic (min) heap with a hash table parameter. The implementation provides a dynamic set in the min heap form for any element objects in memory associated with priority values of basic type (e.g. char, int, long, double).

The hash table parameter specifies a hash table used for in-heap search and modifications, and enables the optimization of space and time resources associated with heap operations by choice of a hash table and its load factor upper bound. Tests across i) division- and mutliplication-based hash tables, ii) contiguous and noncontiguous elements, and iii) priority types are provided.

`./data-structures/ht-mul-uint64/`

A hash table with generic hash keys and generic elements, where a hash key is an object within a contiguous memory block (e.g. a basic type, array, struct), and an element is any object in memory.

The implementation is based on a multiplication method for hashing into upto 2^63 slots and double hashing as an open addressing strategy for resolving collisions. The expected number of probes in a search is upper-bounded by 1/(1 - alpha) under the uniform hashing assumption, where alpha is a load factor upper bound. For every delete and remove operation at most one (re)insert operation is performed, resulting in a probing overhead per delete and remove operation bounded by 1/(1 - alpha) in expectation, and thus a 2/(1 – alpha) overall upper bound for the expected number of probes per delete and remove operation by linearity of expectation. Tests across load factor upper bounds and key sizes exceeding the basic types are provided with respect to element objects within contiguous and noncontiguous memory blocks.

`./data-structures/ht-div-{uint32, uint64}/`

A hash table with generic hash keys and generic elements, where a hash key is an object within a contiguous memory block (e.g. a basic type, array, struct), and an element is any object in memory. 

The implementation is based on a division method for hashing into upto > {2^31, 2^63} slots (largest provided prime number) and a chaining method for resolving collisions. Due to chaining, the number of keys and elements that can be inserted is not limited by the hash table implementation. Tests across load factor upper bounds and key sizes exceeding the basic types are provided with respect to element objects within contiguous and noncontiguous memory blocks.


`./utilities/utilities-rand-mod/`

random_range_{uint32, uint64} demonstrates the construction of uniformly random numbers exceeding the range of a random number generator by conditioning the construction process with a Bernoulli random variable on the lowest bit in the high bits outside the range of the generator.

mem_mod_{uint32, uint64} computes the modulo operation on a memory block by treating each byte of the block in the little-endian order and inductively applying modular arithmetic relations, without requiring a little-endian machine. fast_mem_mod_{uint32, uint64} treats a memory block in 8-byte increments in the little-endian order. Given a little-endian machine, the result is equal to the return value of mem_mod_{uint32, uint64}.
