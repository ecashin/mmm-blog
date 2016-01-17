.. title: Quick Start for BLAS
.. slug: quick-start-for-blas
.. date: 2016-01-16 23:34:54 UTC-05:00
.. tags: numerical-programming
.. category: Software Development
.. link: 
.. description: Getting started with BLAS examples
.. type: text

Anyone who has used R or Python's numpy extensively has been pleased
with the speed of operations on large vectors of numbers.  I wanted to
get the speedy vector operations in a very lean program, so I looked
under the hood and surveyed some of the libraries that do calculations
on large vectors and matrices.

It turns out that BLAS (Basic Linear Algebra Subroutines) is
everywhere.  It lies beneath more sophisticated operations in
libraries like LAPACK, and it has many implementations, ranging from
Fortran (don't laugh---they're fast and reliable) code bases that
started in the Seventies to implementations like OpenBLAS that use the
latest low-level instructions from Intel's newest processors.

By gaining a basic familiarity with BLAS, you can gain access to all
those new SIMD instructions, like AVX, MMX, and SSE2.  There are
bindings in C, Go, and many other languages.

The routines do take some getting used to, though.  They have lots of
parameters.  The names of the parameters are very short coded strings
that you have to learn to decypher.  Fortunately, there are only a few
tricks you need to master, and they're all explained in a few papers.

http://www.netlib.org/blas/blas2-paper.ps

There are many helpful references online that don't go into all the
details discussed in the papers.

http://web.stanford.edu/class/me200c/tutorial_77/18.1_blas.html

So how to get started?  I'm going to talk about a specific example,
using the OpenBLAS implementation on Ubuntu 14.04 x86_64 in C and Go.

First I install the libopenblas packages from APT.

::

    sudo apt-get update
    sudo apt-get install libblas-doc libopenblas-{dev,base}

A simple Makefile will build the C program.  Even though Ubuntu used
the "alternatives" mechanism to make OpenBLAS appear as libblas.so, I
like to use the more explicit "-lopenblas" linker option.  Note that
there has to be a real tab character before "gcc" in the Makefile
contents.

::

    dist: dist.c
    	gcc -Wall -O2 -o $@ $< -lopenblas

The C program starts off like this::

    #include <stdio.h>
    #include <stdlib.h>
    #include <cblas.h>
    
    int main(void)
    {
    	const int p = 10;
    	float *x = malloc(p * sizeof *x);
    	float *y = malloc(p * sizeof *y);
    	float *z = malloc(p * sizeof *y);
    	float *orig = malloc(p * sizeof *orig);
    	int i;
    
    	if (!(x && y && orig && z)) {
    		perror("malloc");
    		exit(1);
    	}
    
    	for (i = 0; i < p; ++i) {
    		y[i] = i;
    		x[i] = 2 * i;
    		z[i] = 0;
    	}
    	cblas_scopy(p, y, 1, orig, 1);

Continue it here.
