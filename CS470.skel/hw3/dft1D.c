#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <complex.h>
#define PI 3.1415927
#define MAG(i)		(sqrt(Fr[i]*Fr[i] + Fi[i]*Fi[i]))
#define PHASE(i)	(atan2(Fi[i],Fr[i]))

int main(int argc, char *argv[])
{
	FILE *IN, *OUT;

	if (argc == 4) {
		// If the arguments are exactly 4
		char *in = argv[1];
		char dir = atoi(argv[2]);
		char *out = argv[3];

		IN = fopen(in,   "r");
		OUT = fopen(out, "w");

		int w, h;
		int u, x, N;
		double c, s, real, imag, cos(), sin();

		fscanf(IN, "%d %d", &w, &h);
		
		N = h; // complex numbers
		// Initializing dymanic array for F real and img/ f real and img
		float* Fr = malloc(N *(sizeof(float)));
		float* fr = malloc(N *(sizeof(float)));
		float* Fi = malloc(N *(sizeof(float)));
		float* fi = malloc(N *(sizeof(float)));
        
		// if dir=0 then Forward DFT is computed
		if (dir == 0) {
			for (x = 0; x < N; x++) {
				fscanf(IN, "%f %f", &fr[x], &fi[x]);
			}
			for (u = 0; u < N; u++) {
				real = imag = 0;
				for (x = 0; x < N; x++) {
					c = cos(2.*PI*u*x / N);
					s = -sin(2.*PI*u*x / N);
					real += (fr[x] * c - fi[x] * s);
					imag += (fr[x] * s + fi[x] * c);
				
				}
				Fr[u] = real / N;
				Fi[u] = imag / N;
				fprintf(OUT, "%f %f\n", Fr[u], Fi[u]); // output file print operation
			}
		
		}
		else if (dir == 1) {
		// if dir=1 , Inverse Fourier Tansform is performed
			for (x = 0; x < N; x++) {
				fscanf(IN, "%f %f", &fr[x], &fi[x]);
			}
			for (x = 0; x < N; x++) {
				//AS IFT 
				real = imag = 0;
				for (u = 0; u < N; u++) {
					c = cos(2.*PI*u*x / N);
					s = -sin(2.*PI*u*x / N);
					real += (fr[x] * c - fi[x] * s);
					imag += (fr[x] * s + fi[x] * c);
				
				
				}
				fr[x] = real;
				fi[x] = imag;
				fprintf(OUT, "%f %f\n", fr[x], fi[x]);
			}
			
		
		}

		free(Fr);
		free(fr);
		free(Fi);
		free(fi);
	}
	else {
	    printf("An error was thrown due to incorrect Arguement");
		return 0;
	}


	fclose(IN);
	fclose(OUT);
	return 0;

}