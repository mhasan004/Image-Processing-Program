
#include "IP.h"
#include <math.h>    // pow
using namespace IP;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_gammaCorrect:
// Gamma correct image I1. Output is in I2.
// Written by Mahmudul Hasan and Aninda Halder

void
HW_gammaCorrect(ImagePtr I1, double gamma, ImagePtr I2)
{
	IP_copyImageHeader(I1, I2);
	ChannelPtr<uchar> p1, p2;
	int type;

	int w = I1->width();
	int h = I1->height();
	int total = w * h;

	int lut[MXGRAY];
	int c = 255;
	for (int i = 0; i < MXGRAY; i++) {
		int output = c * pow((float)i / c, 1 / gamma);
		if (output < 0) output = 0;
		if (output > 255) output = 255;
		lut[i] = output;
	}
																// visit all image channels and evaluate output image
	for (int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {	// get input  pointer for channel ch
		IP_getChannel(I2, ch, p2, type);						// get output pointer for channel ch
		for (int i = 0; i < total; i++)
			*p2++ = lut[*p1++];									// use lut[] to eval output
	}

}

