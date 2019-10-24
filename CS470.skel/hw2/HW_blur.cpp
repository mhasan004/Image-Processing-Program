#include "IP.h"
using namespace IP;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_blur:
//
// Blur image I1 with a box filter (unweighted averaging).
// The filter has width filterW and height filterH.
// We force the kernel dimensions to be odd.
// Output is in I2.
//
void
HW_blur(ImagePtr I1, int filterW, int filterH, ImagePtr I2)
{
	ImagePtr I3;
	IP_copyImageHeader(I1, I2);
	IP_copyImageHeader(I1, I3); //copy I1 header files to I3
	
	int w = I1->width();
	int h = I1->height();
	int total = w * h;

	// declarations for image channel pointers and datatype
	ChannelPtr<uchar> in, out, buffer; //what image is it?
	int type;					

	//bluring causes smaller image so i will pad the image with zeroes. 1padd for 3pixel windows, 2padd for 6
	int paddingNumberH = (filterH - 1) / 2;		//number of padding needed on sides of rows for a given filter size
	int paddingNumberW = (filterW - 1) / 2;
	//int calcArrayRow[w + paddingNumberH];
	for (int ch = 0; IP_getChannel(I1, ch, in, type); ch++) 	// get input  pointer for channel ch
	{
		IP_getChannel(I3, ch, buffer, type);		
		for (int i = 0; i < total; i++) {
			//if 
		}
			
	}




}
