# Max-median-filter-with-memory-optimisations
Implementation of a max-median filter in C with various optimisation techniques in effect.
A max median filter is a non-liner filter commonly used for redusing noise in grayscale images.
Each folders contains:
- A variant of the implemented filter using the loop fusion optimization technique. Other techniques were previously tested, with this yielding the best results.
- A memory map file.
- A scatter file.
- A stack file.

The software ARMulator by Codeworks is required in order to run the code. 

The three folder variations are:
1. no cache: No buffer is used. A buffer is a kernel which temporarily stores data in cache memory, resulting in faster execution.
2. one buffer: One buffer and one L1 cache memory is implemented.
3. two buffers: Two buffers and two levels of cache memory are implemented.

Collaborator: 
