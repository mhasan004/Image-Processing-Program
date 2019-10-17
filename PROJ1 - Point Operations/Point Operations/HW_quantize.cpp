#include "IP.h"
using namespace IP;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_quantize:
// Quantize I1 to specified number of levels. Apply dither if flag is set.
// Output is in I2.
// Written By: Mahmudul Hasan and Aninda


void
HW_quantize(ImagePtr I1, int levels, bool dither, ImagePtr I2)
{   
	IP_copyImageHeader(I1, I2);									// Copying image header of  input I1 to output I2
	int w = I1->width();										// intit var for width, height and total pixels
	int h = I1->height();
	int total = w * h;
	int scale = MXGRAY / levels;								// intitializing scale ref. Point OP page 12

	int i, lut[MXGRAY];											// init lookup table for Quantization
	for (i = 0; i < MXGRAY; ++i) {
		lut[i] = CLIP(int(scale*(i / scale) + (scale / 2)), 0, 255);// applying gain and Bias p:17 and Bias as scale/2
	}

	ChannelPtr<uchar> p1, p2;									//image channel pointers
	int type;
																// evaluation of output image and the Ditheing proccess
																// if the dithering is false execute the usual eval.
	if (!dither) {
		for (int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {	// get input  pointer for channel ch
			IP_getChannel(I2, ch, p2, type);						// get output pointer for channel ch
			for (i = 0; i < total; i++) *p2++ = lut[*p1++];			// use lut[] to eval output
		}
	}
	else
	{
		int noise;
		int j, k, s;
		for (int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {	// dithering evaluation ref: in class
			IP_getChannel(I2, ch, p2, type);
			for (int r = 0; r < h; r++) {							//visting all the rows and sign value alternates in each row
				s = (r % 2) ? 1 : -1;
				for (int x = 0; x < w; x++) {						// adding noise in the bias range [0,bias]
					j = ((double)rand() / RAND_MAX) * (scale / 2);
					k = *p1++ + j * s;								// signed noise
					s *= -1;										// alternate sign for the next pixel
					noise = lut[CLIP(k, 0, MXGRAY)];				// evaluating the output after adding noise
					*p2++ = lut[noise];
				}
			}
		}



	}

}

