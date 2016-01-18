#include <stdio.h>
#include <cblas.h>

int main(void)
{
	float a[] = { 1, 2, 3, 4, 5, 6 };
	float b[] = { 0, 0, 0, 0, 0, 0 };
	const int n = sizeof a / sizeof a[0];
	int i;

	cblas_scopy(n/2, a, 2, b, 2);	/* copy every other element */
	for (i = 0; i < n; ++i)
		printf("%f ", b[i]);
	putchar('\n');

	return 0;
}
