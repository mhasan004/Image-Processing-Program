#include <iostream>
#include<cstdlib>
#include<cmath>
#include "math.h"
#include <fstream>
#include <string>
#define PI		3.1415926535897931160
#define SGN(A)		((A) > 0 ? 1 : ((A) < 0 ? -1 : 0 ))
#define FLOOR(A)	((int) (A))
#define CEILING(A)	((A)==FLOOR(A) ? FLOOR(A) : SGN(A)+FLOOR(A))
#define CLAMP(A,L,H)	((A)<=(L) ? (L) : (A)<=(H) ? (A) : (H))

using namespace std;

// global variable for param
//double g_param;


double boxFilter(double t, double) {
  if ((t > -0.5) && (t <= 0.5))
    return 1.0;
  return 0.0;
}

double triFilter(double t, double) {
  if (t < 0)
    t = -t;
  if (t < 1.0)
    return 1.0 - t;
  return 0.0;
}

double cubicConv(double t, double param) {
  double g_param, t2, t3;
  if (t < 0)
    t = -t;
  t2 = t*t;
  t3 = t2 * t;
  if (t < 1.0)
    return (g_param + 2)*t3 - (g_param + 3)*t2 +1;
  if (t < 2.0)
    return g_param * (t3 - 5 * t2 + 8 * t - 4);
  return 0.0;
}

double sinc(double t) {
  t *= PI;
  if (t != 0)
    return sin(t)/t;
  return 1.0;
}

double hann(double t, double param) {
  if (t < 0)
    t = -t;
  if (t < (int)param)
    return sinc(t) * (.5 + .5 * cos(PI * t / (int)param));
  return 0.0;
}

double hamming(double t, double param = 3) {
  if (t < 0)
    t = -t;
  if (t < (int)param)
    return sinc(t) * (0.54 + 0.46 * cos(PI * t / (int)param));
  return 0.0;
}

double lanczos(double t, double param) {
  if (t < 0)
    t = -t;
  if (t < (int)param)
    return sinc(t)* sinc(t/(int)param);
  return 0.0;
}

void resize1D(float *IN, float *OUT, int INlen, int OUTlen, int kernel_type, double param) {
  
  if(INlen == OUTlen) {
    for(int i = 0; i < INlen; i++) {
      *IN = *OUT;
        IN++;
        OUT++;
    }
    return;

  }

  int i;
  int left, right;
  int pixel;
  double u,x;
  double scale;
  double (*filter)(double, double);
  double fwidth;
  double fscale;
  double weight;
  double acc;

  //g_param = param;
  scale = (double)OUTlen/INlen;

  switch (kernel_type) {
    case 0: {
      filter = boxFilter;
      fwidth = .5;
      break;
    }
    case 1: {
      filter = triFilter;
      fwidth = 1.0;
      break;
    }
    case 2: {
      filter = cubicConv;
      fwidth = 2;
      break;
    }
    case 3: {
      filter = hann;
      fwidth = param;
      break;
    }
    case 4: {
      filter = hamming;
      fwidth = param;
      break;
    }
    case 5: {
      filter = lanczos;
      fwidth = param;
      break;
    }
    default:
      cout << "Wrong kernel_type Selected. Check kernel_type again.\n";
      return;
  }

  if (scale < 1.0) {
    fwidth = fwidth/scale;
    fscale = scale;

    if (kernel_type == 0) {
      fwidth = CEILING(fwidth);
      fscale = 1.0/(2*fwidth);
    }
  }
  else
    fscale = 1.0;


  // evaluate size of padding and buffer length
  int padlen = CEILING(fwidth);   // buffer pad length
  int buflen = INlen + 2 * padlen;    // buffer length

  // allocate buffer memory if necessary
  float*   Ibuf;
  float* buf;
  if(buflen > 0)
    Ibuf = new float[buflen];

  // copy src into padded buffer
  buf = Ibuf;

  // copy src into dst; save space for padding
  float* p1 = IN;
  float* p2 = buf + padlen;

  for(int x=0; x<INlen; ++x)
    p2[x] = *p1++;

  // pad left and right columns
  int  v1, v2;
  p1 = buf + padlen;
  p2 = p1  + INlen - 1;

  // replicate border
  v1 = p1[0];
  v2 = p2[0];
  for(int x=1; x<= padlen; ++x) {
    p1[-x] = v1;
    p2[ x] = v2;
  }


  // init srcp to point to first non-padded pixel in padded buffer
  float* srcp = buf + padlen;
  
  for (x = 0; x < OUTlen; x++) {
      u = x/scale;
      if (u - fwidth < 0)
        left = FLOOR(u - fwidth);
      else
        left = CEILING(u - fwidth);
      right = FLOOR(u + fwidth);
      
      acc = 0;
      double sumWeight = 0;
      for (i = left; i <= right; i++) {
      //  pixel = IN[CLAMP(i, 0, INlen-1)];
        pixel = srcp[i];
        weight = (*filter)((u - i)*fscale, param);
        sumWeight += weight;

        acc += pixel * weight;
      }    
      OUT[(int)x] = CLAMP(acc/sumWeight,0,255);
  }
}

int main(int argc, char** argv) {

	FILE *in, *out;

	// if(argc != 7) {
  //       printf("Invalid argument parameter");
  //       printf("Command argument:\n");
  //       printf("\tprogram file:\n");
  //       printf("\tInput file:\n");
  //       printf("\tOuput file:\n");
  //       printf("\tInput length:\n");
  //       printf("\tOutput length:\n");
  //       printf("\tkernel type:\n");
  //       printf("\tparam:\n");
  //       return 1;
  //   }

  if (argc == 7) {
    char* inputFile = argv[1];
    char* outputFile = argv[2];
    int INlen = atoi(argv[3]);  
    int OUTlen = atoi(argv[4]);
    int kernel_type = atoi(argv[5]);
    double param = atof(argv[6]);
    if(kernel_type >=2)
		param = -1;

        // open files
        in = fopen(inputFile, "r");
        out = fopen(outputFile, "w");

        float *input = (float*)malloc(sizeof(float)*INlen);
        float *output = (float*)malloc(sizeof(float)*OUTlen);


        for(int i = 0; i < INlen; i++)
            fscanf(in, "%f", &input[i]);

        resize1D(input, output,INlen, OUTlen, kernel_type, param);

        for(int i = 0; i < OUTlen; i++) {
        	int ferr = fprintf(out, "%.2f\n", output[i]);
        	if(ferr < 0)
        		return ferr;
        }

    }
  
  return 0;
}
