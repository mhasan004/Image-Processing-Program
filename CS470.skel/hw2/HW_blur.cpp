#include "IP.h"
using namespace IP;
using namespace std;
#include <algorithm>
#include <vector>
#include <string> 

//This is to print debug messages to a window. Use DBOUT(L"string to print") or integers: DBOUT(x)
#include <Windows.h>
#include <iostream>
#include <sstream>
#define DBOUT( s )            \
{                             \
   std::wostringstream os_;    \
   os_ << s;                   \
   OutputDebugStringW( os_.str().c_str() );  \
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_blur: Written By mahmudul Hasan and Aninda
//
// Blur image I1 with a box filter (unweighted averaging).
// The filter has width filterW and height filterH.
// We force the kernel dimensions to be odd.
// Output is in I2.
//
//***NOTE****looks like theimage sizes, I1, I2, or I3 must be the size of I1
//Bluring causes smaller image so i will need to pad the image with zeroes on the left and right of the row (and top and bottom if its the top and last row). NumberOfPaddingOnSides = (FilterWindow-1)/2


void HW_blur(ImagePtr I1, int filterW, int filterH, ImagePtr I2)
{
	ImagePtr I3;																	// I1 -> I3 will be I1 with appriopiate padding -> blur I3 - > outoput to I2
	IP_copyImageHeader(I1, I2);
	IP_copyImageHeader(I1, I3);														// copy I1 header files to I3

	int w = I1->width();
	int h = I1->height();
	int total = w * h;

	ChannelPtr<uchar> in, out, buffer;												// image channel pointers and datatype
	int type;

	// Bluring causes smaller image so i will pad the image with zeroes. 1padd for 3pixel windows, 2padd for 6
	if (filterH % 2 == 0 && filterH != 1) {											// bluring window must be odd so will just subtract to previous window
		filterH--;
	}
	if (filterW % 2 == 0 && filterW != 1) {											// bluring window must be odd so will just subtract to previous window
		filterW--;
	}
	int paddingNumberH = (filterH - 1) / 2;											// number of padding needed on sides of rows for a given filter size
	int paddingNumberW = (filterW - 1) / 2;
	vector<int> paddedBuffer;														// the buffer vector to shore each row with appropiate pading to left or right. Will then blur row and output to output image I2


	for (int ch = 0; IP_getChannel(I1, ch, in, type); ch++) 						// get input  pointer for channel ch
	{
		IP_getChannel(I2, ch, out, type);

		for (int row = 0; row < h; ++row)
		{
			paddedBuffer.clear();
			for (int col = 0; col < w; ++col)										// 1) Put the input row into a buffer vector and add appropiate padding on sides
			{
				int pixel = *in++;													// ***NOTE*** if i didnt use pixel = *in++ and just used *p++, the image gets cut diagonally where the left side in on the right and the right on the left. it was weird
				if (col == 0) {														// 1a) if its the first pixel in the row...																		
					for (int k = 0; k < paddingNumberW; ++k) 						// 1b) put zero paddings into the buffer 
						paddedBuffer.push_back(0);
					paddedBuffer.push_back(pixel);									// 1c) after padding, insert the first pixel of input row into buffer and copy row pixels as normal
				}
				else if (col == w - 1) {											// 1d) at the end of the row add the last pixel to the buffer then add the padding to the buffer			
					paddedBuffer.push_back(pixel);
					for (int k = 0; k < paddingNumberW; ++k) {
						paddedBuffer.push_back(0);
					}
				}
				else {																// 1c) if not pixel at beginning or end of image, just copy pixels
					paddedBuffer.push_back(pixel);
				}
			}
			//2 and 3) now that i recorded the rows need to blur the row using the filter and output each blured pixel to output
			int counter = 0;
			for (int window = paddingNumberW; window < (paddedBuffer.size() - paddingNumberW); ++window) {
				int sum = 0;
				sum += paddedBuffer.at(paddingNumberW);
				for (int i = 0; i < paddingNumberW; ++i) {
					sum += paddedBuffer.at(window - i);
					sum += paddedBuffer.at(window + i);
				}
				*out++ = CLIP(sum / filterW, 0, 255);									//3) print blured pixel to output	
				counter++;
			}
		}
	}

}
