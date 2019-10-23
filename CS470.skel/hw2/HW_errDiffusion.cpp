#include "IP.h"
using namespace IP;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_errDiffusion:
//
// Apply error diffusion algorithm to image I1.
//
// This procedure produces a black-and-white dithered version of I1.
// Each pixel is visited and if it + any error that has been diffused to it
// is greater than the threshold, the output pixel is white, otherwise it is black.
// The difference between this new value of the pixel from what it used to be
// (somewhere in between black and white) is diffused to the surrounding pixel
// intensities using different weighting systems.
//
// Use Floyd-Steinberg     weights if method=0.
// Use Jarvis-Judice-Ninke weights if method=1.
//
// Use raster scan (left-to-right) if serpentine=0.
// Use serpentine order (alternating left-to-right and right-to-left) if serpentine=1.
// Serpentine scan prevents errors from always being diffused in the same direction.
//
// A circular buffer is used to pad the edges of the image.
// Since a pixel + its error can exceed the 255 limit of uchar, shorts are used.
//
// Apply gamma correction to I1 prior to error diffusion.
// Output is saved in I2.
//
void
HW_errDiffusion(ImagePtr I1, int method, bool serpentine, double gamma, ImagePtr I2)
{





}

void copyRowToCircBuffer(ChannelPtr<uchar> p1, short* buffer, int w, int size) {
	int i = 0;
	for (i = 0; i < size / 2;    i++) {
		buffer[i] = *p1; }
    for (i = 0; i < size + w - 1; i++) {
		buffer[i] = *p1;
	}

	for (i = 0; i < size / 2 + w - 1; i++) {
		buffer[i] = *p1++;
	}
}

void
HW_gammaCorrect(ImagePtr I1, double gamma, ImagePtr I2)
{
	// s = c * r^gamma. c=1, r=image input
	// gamma correction: s = c * r^(1/gamma).

	// copy image header (width, height) of input image I1 to output image I2
	IP_copyImageHeader(I1, I2);
	// declarations for image channel pointers and datatype
	ChannelPtr<uchar> p1, p2;
	int type;

	// init vars for width, height, and total number of pixels
	int w = I1->width();
	int h = I1->height();
	int total = w * h;


	// init lookup table

	int i, lut[MXGRAY];
	for (i = 0; i < MXGRAY; ++i) {
		lut[i] = (int)CLIP((double)MaxGray*(pow((double)i / MaxGray, (1 / gamma))), 0, MaxGray);

	}

	// visit all image channels and evaluate output image
	for (int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {	// get input  pointer for channel ch
		IP_getChannel(I2, ch, p2, type);						// get output pointer for channel ch
		for (int i = 0; i < total; i++)
			*p2++ = lut[*p1++];			                        // use lut[] to eval output
	}
}
