
#include "IP.h"
using namespace IP;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_histoStretch:
// Apply histogram stretching to I1. Output is in I2.
// Stretch intensity values between t1 and t2 to fill the range [0,255].
// Written by: Mahmudul Hasan and Aninda Halder

void
HW_histoStretch(ImagePtr I1, int t1, int t2, ImagePtr I2)
{
	IP_copyImageHeader(I1, I2);
	ChannelPtr<uchar> p1, p2;
	int type;

	int w = I1->width();
	int h = I1->height();
	int total = w * h;

	int lut[MXGRAY];
	for (int i = 0; i < MXGRAY; ++i) { 	// 1) first need to clip the images before i stretch it out
		if (i < t1)
			lut[i] = t1;
		else if (i > t2)
			lut[i] = t2;
		else
			lut[i] = i;
	}
	
	for (int i = 0; i < MXGRAY; ++i) 	// 2) Use the histogram stretch function. Taking the clipped LUT and applying the formul
	{
		int max = 255;
		int MIN = 0;
		if (t1 < t2) {												// so that there is no divide by zero. t1 is the lower bound and t2 is the upper bound
			lut[i] = (lut[i] - t1) * (max - MIN) / (t2 - t1) + MIN; //normalizing histogram ranfge from MIn to MAX
		}
	}

	for (int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {	// visit all image channels and evaluate output image // get input  pointer for channel ch
		IP_getChannel(I2, ch, p2, type);						// get output pointer for channel ch
		for (int i = 0; i < total; i++)
			*p2++ = lut[*p1++];									// use lut[] to eval output
	}
}
