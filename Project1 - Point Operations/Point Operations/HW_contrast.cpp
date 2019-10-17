#include "IP.h"
using namespace IP;
using namespace std;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_contrast:
// Apply contrast enhancement to I1. Output is in I2.
// Stretch intensity difference from reference value (128) by multiplying difference by "contrast" and adding it back to 128. 
// Shift result by adding "brightness" value.
// Written by: Mahmudul Hasan and Aninda

void
HW_contrast(ImagePtr I1, double brightness, double contrast, ImagePtr I2)
{
	IP_copyImageHeader(I1, I2);					// copy image header (width, height) of input image I1 to output image I2
	int w = I1->width();						// init vars for width, height, and total number of pixels
	int h = I1->height();
	int total = w * h;

	ChannelPtr<uchar> p1, p2;					// declarations for image channel pointers and datatype
	int type;

	int lut[MXGRAY];							// init lookup table
	int mid = 128;
	for (int i = 0; i < MXGRAY; i++) {
		int output = contrast * (i - mid);		// subtracting the middle intensity value from the pixel value
		output += brightness + mid;				// shift the graph by the brightness slider AND readding that 128 we took away
		if (output < 0) output = 0;
		if (output > 255) output = 255;
		lut[i] = output;
	}

	
	for (int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) 	// visit all image channels and evaluate output image// get input  pointer for channel ch
	{
		IP_getChannel(I2, ch, p2, type);						// get output pointer for channel ch
		for (int i = 0; i < total; i++)
			*p2++ = lut[*p1++];
	}

}










