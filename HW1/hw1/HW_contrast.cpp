#include "IP.h"
using namespace IP;
using namespace std;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_contrast:
//
// Apply contrast enhancement to I1. Output is in I2.
// Stretch intensity difference from reference value (128) by multiplying difference by "contrast" and adding it back to 128. 
// Shift result by adding "brightness" value.

void
HW_contrast(ImagePtr I1, double brightness, double contrast, ImagePtr I2)
{
	// copy image header (width, height) of input image I1 to output image I2
	IP_copyImageHeader(I1, I2);

	// init vars for width, height, and total number of pixels
	int w = I1->width();
	int h = I1->height();
	int total = w * h;


	// declarations for image channel pointers and datatype
	ChannelPtr<uchar> p1, p2;
	int type;

	// Go through image and get smallest and largest pixel intensity:
	int g_min = 100;
	int g_max = 0;
	for (int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {	// get input  pointer for channel ch
		for (int i = 0; i < total; i++) {		// go through image pixels
			int pixel = *p1++;	// go to the next pixel every iteration of the for loop
			if (pixel <= g_min)
				g_min = pixel;
			if (pixel >= g_max)
				g_max = pixel;
		}
	}

	// visit all image channels and evaluate output image
	for (int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {	// get input  pointer for channel ch
		IP_getChannel(I2, ch, p2, type);	// get output pointer for channel ch
		for (int i = 0; i < total; i++) {
			/*int output = contrast * (*p1++ - g_min);
			output += brightness;

			if (output < 0) output = 0;
			if (output > 255) output = 255;
			*p2++ = output;*/



			/*int output = contrast * (128 - *p1++);
			output += brightness + 128;
			if (output < 0) output = 0;
			if (output > 255) output = 255;
			*p2++ = output;*/


			int output = contrast * (*p1++ - 128);	// subtracting the middle intensity value from the pixel value
			output += brightness + 128;				// shift the graph by the brightness slider AND readding that 128 we took away
			if (output < 0) output = 0;
			if (output > 255) output = 255;
			*p2++ = output;


		}
	}

}










