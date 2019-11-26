#include "stdlib.h"
#include "math.h"
#define PI 3.141592653589793
#include <stdio.h>
#include <stdlib.h>
#include<stdbool.h> 

typedef struct {
	int len;	// length of complex number list
	float * real;	// pointer to real number list
	float * imag;	// pointer to imaginary number list
}
complexS, *complexP;


void fft1D(complexP q1, int dir, complexP q2)
{
	int i, N, N2;
	float * r1, *i1, *r2, *i2, *ra, *ia, *rb, *ib;
	float FCTR, fctr, a, b, c, s, num[2];
	complexP qa, qb;

	N = q1 -> len;
	r1 = (float *)q1 -> real;
	i1 = (float *)q1 -> imag;
	r2 = (float *)q2 -> real;
	i2 = (float *)q2 -> imag;

	if (N == 2) {
		a = r1[0] + r1[1];	// F(0)=f(0)+f(1);F(1)=f(0)-f(1)
		b = i1[0] + i1[1];	// a,b needed when r1=r2
		r2[1] = r1[0] - r1[1];
		i2[1] = i1[0] - i1[1];
		r2[0] = a;
		i2[0] = b;

	}
	else {
		N2 = N / 2;
		qa = (complexP)malloc(sizeof(complexS));
	//	qa = malloc(sizeof(*qa));
		qa -> len = N2;
		qa -> real = (float *)malloc(sizeof(float) * qa -> len);
		qa -> imag = (float *)malloc(sizeof(float) * qa -> len);

		qb = (complexP)malloc(sizeof(complexS));
	//	qb = malloc(sizeof(*qb));
		qb -> len = N2;
		qb -> real = (float *)malloc(sizeof(float) * qb -> len);
		qb -> imag = (float *)malloc(sizeof(float) * qb -> len);

		ra = (float *)qa -> real;
		ia = (float *)qa -> imag;
		rb = (float *)qb -> real;
		ib = (float *)qb -> imag;

		// split list into 2 halves; even and odd
		for (i = 0; i < N2; i++) {
			ra[i] = *r1++;
			ia[i] = *i1++;
			rb[i] = *r1++;
			ib[i] = *i1++;
		}

		// compute fft on both lists
		fft1D(qa, dir, qa);
		fft1D(qb, dir, qb);

		// build up coefficients
		if (!dir)	// forward
			FCTR = -2 * PI / N;
		else	FCTR = 2 * PI / N;

		for (fctr = i = 0; i < N2; i++, fctr += FCTR) {
			c = cos(fctr);
			s = sin(fctr);
			a = c * rb[i] - s * ib[i];
			r2[i] = ra[i] + a;
			r2[i + N2] = ra[i] - a;

			a = s * rb[i] + c * ib[i];
			i2[i] = ia[i] + a;
			i2[i + N2] = ia[i] - a;
		}

		free(qa);
		free(qb);
	}

	if (!dir) {	// inverse : divide by logN
		for (i = 0; i < N; i++) {
			q2 -> real[i] = q2 -> real[i] / 2;
			q2 -> imag[i] = q2 -> imag[i] / 2;
		}
	}
}
bool isPowerof_Two(int n) {
	if (n == 0)
		return false;

	return (ceil(log2(n)) == floor(log2(n)));
}
int main(int argc, char* argv[]) {
	FILE *IN;
	FILE *OUT;
	// Take just 4 argc
	if (argc == 4) {
		char *in = argv[1]; // 1st arguement Input file;
		int dir = atoi(argv[2]);// 2nd arguement dir
		char *out = argv[3]; //  3rd arguement OUTPUT

		IN = fopen(in, "r"); OUT = fopen(out, "w");

		int w, h; //reading height and width from the input
		int u, x, N;
		double c, s, real, imag, cos(), sin();
		fscanf(IN, "%d %d", &w, &h);
		N = h;

		//Length of the list must be a power of 2.
		if (!isPowerof_Two(N)) {
			printf("N is not power of 2.");
			return 0;
		}
		// init q1 
		complexP q1 = malloc(sizeof(*q1));
		q1->len = N;
		q1->real = malloc(N * sizeof(float));
		q1->imag = malloc(N * sizeof(float));
		//Init q2
		complexP q2 = malloc(sizeof(*q2));
		q2->len = h;
		q2->real = malloc(N * sizeof(float));
		q2->imag = malloc(N * sizeof(float));
		
		// input
		for (int i = 0; i < N; i++) {
			fscanf(IN, "%f %f", &q1->real[i], &q1->imag[i]);
		}// Fast FT
		fft1D(q1, dir, q2);
		// outstream
		fprintf(OUT, "%d %d\n", w, h);
		
		int N_q2 = q2->len;
		for (int i = 0; i < N_q2; i++) {
			fprintf(OUT, "%f %f\n", q2->real[i], q2->imag[i]);
		}
	
	}
	else {
	
		printf("Failed-> Wrong Number of Arguments Issued");
		return 0;
	}



	return 0;

}