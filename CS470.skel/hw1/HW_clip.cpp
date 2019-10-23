//Mahmudul Hasan and Aninda
#include "IP.h"
using namespace IP;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_clip:
//
// Clip intensities of image I1 to [t1,t2] range. Output is in I2.
// If    input<t1: output = t1;
// If t1<input<t2: output = input;
// If      val>t2: output = t2;
//
void
HW_clip(ImagePtr I1, int t1, int t2, ImagePtr I2)
{
	// copy image header (width, height) of input image I1 to output image I2
	IP_copyImageHeader(I1, I2);

	// init vars for width, height, and total number of pixels
	int w = I1->width();
	int h = I1->height();
	int total = w * h;

	// init lookup table: for all graylevels less than t1 -> LUT is t1. greather than t2 -> LUT is t2
	//int i, lut[MXGRAY]; //MXGRAY = 256 (all graylevels 0 to 255)
	//for (i = 0; i < t1 && i < MXGRAY; ++i) 
	//	lut[i] = t1;
	//for (i = 0; i > t2 && i < MXGRAY; ++i)
	//	lut[i] = t2;

	int lut[MXGRAY];
	for (int i = 0; i < MXGRAY; ++i) {
		if (i < t1)
			lut[i] = t1;
		else if (i > t2)
			lut[i] = t2;
		else
			lut[i] = i;
	}

	// declarations for image channel pointers and datatype
	ChannelPtr<uchar> p1, p2;
	int type;

	// visit all image channels and evaluate output image
	for (int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {	// get input  pointer for channel ch
		IP_getChannel(I2, ch, p2, type);						// get output pointer for channel ch
		for (int i = 0; i < total; i++)
			*p2++ = lut[*p1++];									// use lut[] to eval output
	}
}


