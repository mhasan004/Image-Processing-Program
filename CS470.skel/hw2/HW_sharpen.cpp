#include #include "IP.h"
using namespace IP;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_sharpen: Written By Mahmudul Hasan and Aninda
// Sharpen image I1. Output is in I2.
// 1) Blur the image with with 'size' filter window
// 2) Edge Image = (Original - Blurred Image)*factor
// 3) Original + Edge Image

void HW_blur(ImagePtr I1, int filterW, int filterH, ImagePtr I2);					// Will be using the blur function so define it externally here

void HW_sharpen(ImagePtr I1, int size, double factor, ImagePtr I2)
{
	ImagePtr BlurredI;																// This is the pointer to the pixels in the Blurred Image
	ImagePtr EdgesI;																// This is the pointer to the pixels in the Edges Image
	IP_copyImageHeader(I1, I2);
	IP_copyImageHeader(I1, BlurredI);
	IP_copyImageHeader(I1, EdgesI);

	ChannelPtr<uchar> in, out, blur, edge;
	int type;
	const int w = I1->width();
	const int h = I1->height();
	int total = w * h;

	HW_blur(I1, size, size, BlurredI);												// 1) Get the Blurred Image

	for (int ch = 0; IP_getChannel(I1, ch, in, type); ch++)
	{
		IP_getChannel(I1, ch, in, type);
		IP_getChannel(BlurredI, ch, blur, type);
		IP_getChannel(EdgesI, ch, edge, type);
		for (int row = 0; row < h; ++row) {											// 2) Edged image = (Original Image - Blurred image)*factor
			for (int col = 0; col < w; ++col) {
				int inputPixel = *in++;
				int blurPixel = *blur++;
				*edge++ = CLIP((inputPixel - blurPixel) * factor, 0, 255);
			}
		}

		IP_getChannel(I1, ch, in, type);
		IP_getChannel(I2, ch, out, type);
		IP_getChannel(EdgesI, ch, edge, type);
		for (int row = 0; row < h; ++row) {											// 3) Sharpened Image = Original + Edge Image
			for (int col = 0; col < w; ++col) {
				int inputPixel = *in++;
				int edgePixel = *edge++;
				*out++ = CLIP(inputPixel + edgePixel, 0, 255);
			}
		}
	}
}
