.. title: Quick Start for BLAS
.. slug: quick-start-for-blas
.. date: 2016-01-16 23:34:54 UTC-05:00
.. tags: numerical-programming, mathjax
.. category: Software Development
.. link: 
.. description: Getting started with BLAS examples
.. type: text

Anyone who has used R or Python's numpy extensively has been pleased
with the speed of operations on large vectors of numbers.  I wanted to
get the speedy vector operations in a very lean program, so I looked
under the hood and surveyed some of the libraries that do calculations
on large vectors and matrices.

It turns out that BLAS_ (Basic Linear Algebra Subroutines) is
everywhere.  It lies beneath more sophisticated operations in
libraries like LAPACK_, and it has many implementations, ranging from
Fortran (don't laugh---they're fast and reliable) code bases that
started in the Seventies to implementations like OpenBLAS_ that use
the latest low-level instructions from Intel's newest processors.

.. _BLAS: http://www.netlib.org/blas/
.. _LAPACK: http://www.netlib.org/lapack/
.. _OpenBLAS: http://www.openblas.net/

By gaining a basic familiarity with BLAS, you can gain access to all
those new SIMD_ instructions, like AVX_, MMX_, and SSE2_.  There are
bindings in C, Go_, and many other languages.

.. _SIMD: https://en.wikipedia.org/wiki/SIMD
.. _AVX: https://software.intel.com/en-us/articles/introduction-to-intel-advanced-vector-extensions
.. _MMX: https://software.intel.com/en-us/node/583183
.. _SSE2: https://software.intel.com/en-us/node/583138
.. _Go: https://golang.org/

The routines do take some getting used to, though.  They have lots of
parameters.  The names of the parameters are very short coded strings
that you have to learn to decypher.  Fortunately, there are only a few
tricks you need to master, and they're all explained in a few papers.

http://www.netlib.org/blas/blas2-paper.ps

There are many helpful references online that don't go into all the
details discussed in the papers.

http://web.stanford.edu/class/me200c/tutorial_77/18.1_blas.html

So how to get started?  I'm going to talk about a specific example,
using the OpenBLAS implementation on Ubuntu 14.04 x86_64 in C.

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
    	gcc -Wall -O3 -o $@ $< -lopenblas

The C program starts off like this::

    #include <stdio.h>
    #include <stdlib.h>
    #include <cblas.h>
    
    int main(void)
    {
    	const int p = 10;
    	float *a = malloc(p * sizeof *a);
    	float *b = malloc(p * sizeof *b);
    	float *c = malloc(p * sizeof *c);
    	float *orig = malloc(p * sizeof *orig);
    	int i;
    
    	if (!(a && b && c && orig)) {
    		perror("malloc");
    		exit(1);
    	}

    	for (i = 0; i < p; ++i) {
    		b[i] = i;
    		a[i] = 2 * i;
    		c[i] = 0;
    	}
    	cblas_scopy(p, b, 1, orig, 1);

There are several items of note in this example:

1. The arrays used by BLAS are regular, space-efficient, C arrays.  They could just as well have been allocated on the stack.
2. You use BLAS as "cblas", and the routines have a `cblas_` prefix.
3. There is an additional `s` prefix before the part of the function name that describes what it does, `copy`.  That `s` stands for "single-precision floating point."  There are other prefixes for doubles or complex numbers.

The parameters in the original example are,

====== ============================== ==========
 name           description            argument
====== ============================== ==========
N      number of assignments to make  p
X      the source array               y
incX   the increment in X             1
Y      the destination array          orig
incY   the increment in Y             1
====== ============================== ==========

You might also be wondering, "What are the ones for in the parameter
list?"  They're for specifying what the increment should be on their
respective arrays in the parameter list.  I doubt you'll use them
much, but here's an illustrative example.

.. listing:: incdemo.c c

See how every other element remains zero in the run below::

    bash$ gcc -Wall -O3 incdemo.c -lopenblas
    bash$ ./a.out 
    1.000000 0.000000 3.000000 0.000000 5.000000 0.000000 
    bash$ 

That clears up what the parameters are for, but let's get back to the
original example and focus on adding and multiplying vectors together.

Continuing the original example, here below is the first arithmetical
vector operation.  The first time I heard the name of this routine, it
sounded really funny

::

    	cblas_saxpy(p, -1, x, 1, y, 1);

The name, "saxpy", means single-precision floating point `alpha` times
`x` plus `y`.  The `alpha` is a scalar that is multiplied with every
element in `x`.  The result is added element-wise to `y`.  I want to
subtract these two vectors, so I use -1 as `alpha`.

What if we want to square the result?  Hmm... There's no obvious
element-wise vector-vector multiply function to call.  And it seems
kind of clunky to use a matrix when it would be mostly empty space.

Happily, the creators of BLAS had a plan.  They chose to create a
small number of functions that are useful in several kinds of
circumstances.  There is a notion of "banded matrices", where only the
diagonal (or the diagonals above and below it) are non-zero.  These
banded matrices are represented efficiently.

For a single diagonal, you can just use an array of numbers with the
values of the diagonal.  So by using a banded matrix for one vector
and a vector for the other, you can multiply two vectors element wise.

The routine to use here is `cblas_sgbmv`, as shown below.  It does the
following operation.

.. math::

    y \gets \alpha*A*x + \beta*y

Here that is again in monospace, in case you read this before nikola_
fixes mathjax or I figure out what user error causes that math to fail
to render in this blog.

.. _nikola: https://getnikola.com/

::

    y <- alpha*A*x + beta*y

Note that there are a large number of parameters to use for tweaking
the way the function works.

::

    	cblas_sgbmv(CblasRowMajor,
    		    CblasNoTrans, /* Don't transpose A */
    		    p,          /* M is the number of rows in A */
    		    p,          /* N is the number of columns in A */
    		    0,		/* KL: bands below the diagonal */
    		    0,		/* KU: bands above the diagonal */
    		    1,		/* alpha: the scalar multiplier */
    		    b,		/* A: just the diagonal of A in this case */
    		    1,		/* LDA: 1st dimension of A */
    		    b,          /* X: the vector to multiply */
    		    1,          /* incX */
    		    1,          /* beta: the scalar multiplier for  */
    		    c,          /* Y: the results are stored here */
    		    1);         /* incY */

I find it handy to have the cblas.h file open for reference while
writing a call like this.

I guess one gets used to this kind of thing.  Maybe not.  Usually one
just uses numpy, julia, or something similarly convenient, so maybe
there's not enough time to get used to it.

Anyway, it could be worse, and that's a relatively small price to pay
for portable programs that get the absolute best in performance.

The rest of the file is easy::

    	for (i = 0; i < p; ++i) {
    		printf("b(%f) - a(%f) = %f\n", orig[i], a[i], b[i]);
    		printf("(b-a)^2 = %f\n", c[i]);
    	}
	free(a);
	free(b);
	free(c);
	free(orig);
    	return 0;
    }

And running it looks like this::

    bash$ ./dist 
    b(0.000000) - a(0.000000) = 0.000000
    (b-a)^2 = 0.000000
    b(1.000000) - a(2.000000) = -1.000000
    (b-a)^2 = 1.000000
    b(2.000000) - a(4.000000) = -2.000000
    (b-a)^2 = 4.000000
    b(3.000000) - a(6.000000) = -3.000000
    (b-a)^2 = 9.000000
    b(4.000000) - a(8.000000) = -4.000000
    (b-a)^2 = 16.000000
    b(5.000000) - a(10.000000) = -5.000000
    (b-a)^2 = 25.000000
    b(6.000000) - a(12.000000) = -6.000000
    (b-a)^2 = 36.000000
    b(7.000000) - a(14.000000) = -7.000000
    (b-a)^2 = 49.000000
    b(8.000000) - a(16.000000) = -8.000000
    (b-a)^2 = 64.000000
    b(9.000000) - a(18.000000) = -9.000000
    (b-a)^2 = 81.000000
    bash$ 

That's great, and now we have seen how to add and multiply vectors,
but to calculate the distance is even easier than using the explicit
steps like the ones we've seen so far.  Check out `cblas_snrm2` for
Euclidean distance and `cblas_sasum` for Manhattan distance.

Hopefully some of these examples has given you a head start in working
with BLAS tools and code.
