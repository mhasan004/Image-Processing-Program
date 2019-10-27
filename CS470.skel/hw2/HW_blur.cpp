#include "IP.h"
#include <algorithm>
#include <vector>
using namespace IP;
using namespace std;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_blur: Written By Mahmudul Hasan and Aninda
//
// Blur image I1 with a box filter (unweighted averaging).
// The filter has width filterW and height filterH.
// We force the kernel dimensions to be odd.
// Output is in I2.
//
// ***NOTE****looks like the image sizes, I1, I2, or I3 must be the size of I1
// Bluring causes smaller image so i will need to pad the image with zeroes on the left and right of the row (and top and bottom if its the top and last row). NumberOfPaddingOnSides = (FilterWindow-1)/2


void HW_blur(ImagePtr I1, int filterW, int filterH, ImagePtr I2)
{
	ImagePtr I3;																	// I1 -> I3 will be I1 with appriopiate padding -> blur I3 - > outoput to I2
	IP_copyImageHeader(I1, I2);
	IP_copyImageHeader(I1, I3);														// copy I1 header files to I3

	ChannelPtr<uchar> in, out, buffer;												// Image channel pointers and datatype
	int type; 
	const int w = I1->width();
	const int h = I1->height();
	int total = w * h;

	if (filterH % 2 == 0 && filterH!=1) 											// Bluring window must be odd so that the averaged of pixel in window goes to middle pxl. WITH OUT THIS: THE PICTURE GETS DARKER EVERY EVEN FILTER SIZE!
		filterH--;
	if (filterW % 2 == 0 && filterW != 1) 									
		filterW--;
																					// Bluring causes smaller image so i will pad the image with zeroes. 1padd for 3pixel windows, 2padd for 6
	int paddingNumberH = (filterH - 1) / 2;											// Number of padding needed on sides of rows for a given filter size
	int paddingNumberW = (filterW - 1) / 2;		
	vector<int> paddedBufferW;														// The buffer vector to shore each row with appropiate pading to left or right. Will then blur row and output to output image I2
	vector<int> paddedBufferH;

	for (int ch = 0; IP_getChannel(I1, ch, in, type); ch++) 												// get input  pointer for channel ch
	{
																											//***NEED TO PUT THE ORIGINAL IMAGE INTO A 2D VECTOR MATRIX TO ACCESS VERTICLE PIXELS WITH EASE. Dont want to constantly increment pixel pointers to get it to the right position, this pointer increment makes it very laggy! 
		vector<vector<int> > copy(h);																		// a) making a h x w 2D vector matrix of all zeroes
		for (int i = 0; i < h; i++)
			copy[i].resize(w);
		for (int i = 0; i < h; i++)																			// b) copy data to vector matrix
			for (int j = 0; j < w; j++)
				copy[i][j] = *in++;

																											///////VERTICAL BLURRING: I1 -> Copy Matrix -> Vertical Blur using Column Vector -> Copy Matrix -> I3///////
		IP_getChannel(I1, ch, in, type);
		int rowNumber;
		for (int col = 0; col < w; ++col)										
		{
			paddedBufferH.clear();
			for (int row = 0; row < h; ++row)
			{
				rowNumber = row;
				int pixel = copy[row][col];											
				if (row == 0) {																				// 1) Say ur going through the column, if its the first row, then there needs to be some padding on the top of the image depending on the filter window. Put into the vector (this vector contains the sppecific column + padding)
					for (int k = 0; k < paddingNumberH; ++k)
						paddedBufferH.push_back(pixel);
					paddedBufferH.push_back(pixel);
				}
				else if (row == h - 1) {																	// 2) In the column, if its the last row, there needs to be some padding at the bottom of the image depending on the filter window. Put into the vector (this vector contains the sppecific column + padding)
					paddedBufferH.push_back(pixel);
					for (int k = 0; k < paddingNumberH; ++k)
						paddedBufferH.push_back(pixel);
				}
				else
					paddedBufferH.push_back(pixel);									
			}

			for (int window = paddingNumberH; window < (paddedBufferH.size() - paddingNumberH); ++window)
			{
				int sum = 0;
				sum += paddedBufferH.at(window);
				for (int i = 1; i <= paddingNumberH; ++i) {													// 3) Ex: if the filter window is 5. Go through the padded vector that is the column pixels + padding. Add all pixels in the area and divide by the widnow size. Increment (h) times to average the column pixels
					sum += paddedBufferH.at(window - i);
					sum += paddedBufferH.at(window + i);
				}
				copy[window - paddingNumberH][col] = CLIP(sum / filterH, 0, 255);							// 4) output the result of the curent averging window and overwrite the pixel at copy matrix. Later, will print to output
			}
		}
		IP_getChannel(I3, ch, buffer, type);																// 5) copy matrix = now a fully vertically blured I1. Now output it to I3
		for (int i = 0; i < h; i++)
			for (int j = 0; j < w; j++)
				*buffer++ = copy[i][j];

																											///////HORIZONTAL BLURRING: Vertical Blurred I3 -> Horizontal blur using row vector -> I2 ///////
		IP_getChannel(I3, ch, buffer, type);
		IP_getChannel(I2, ch, out, type);
		for (int row = 0; row < h; ++row)
		{
			paddedBufferW.clear();
			for (int col = 0; col < w; ++col)																// 1) Put the input row into a buffer vector and add appropiate padding on sides
			{
				int pixel = *buffer++;																		// ***NOTE*** if i didnt use 'pixel = *buffer++' and just used '*buffer++', the image gets cut diagonally where the left side in on the right and the right on the left. it was weird
				if (col == 0) {																				// 2) if its the first pixel in the row need to add the appropiate padding																		
					for (int k = 0; k < paddingNumberW; ++k) 												// 3) put zero paddings into the buffer 
						paddedBufferW.push_back(pixel);
					paddedBufferW.push_back(pixel);															// 4) after padding, insert the first pixel of input row into buffer and copy row pixels as normal
				}
				else if (col == w - 1) {																	// 5) at the end of the row add the last pixel to the buffer then add the padding to the buffer			
					paddedBufferW.push_back(pixel);
					for (int k = 0; k < paddingNumberW; ++k) {
						paddedBufferW.push_back(pixel);
					}
				}
				else {																						// 6) if not pixel at beginning or end of image, just copy pixels
					paddedBufferW.push_back(pixel);
				}
			}
			for (int window = paddingNumberW; window < (paddedBufferW.size() - paddingNumberW); ++window)   // 7) now that i recorded the rows need to blur the row using the filter and output each blured pixel to output
			{
				int sum = 0;
				sum += paddedBufferW.at(window);									
				for (int i = 1; i <= paddingNumberW; ++i) {
					sum += paddedBufferW.at(window - i);
					sum += paddedBufferW.at(window + i);
				}
				*out++ = CLIP(sum / filterW, 0, 255);														// 8) output blured pixel to output image	
			}
		}
	}
}
