#include "IP.h"
using namespace IP;
using namespace std;
#include <algorithm>
#include <vector>
#include <string> 

//to output to the output scren
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
// HW_blur:
//
// Blur image I1 with a box filter (unweighted averaging).
// The filter has width filterW and height filterH.
// We force the kernel dimensions to be odd.
// Output is in I2.
//
//***NOTE****looks like theimage sizes, I1, I2, or I3 must be the size of I1


void HW_blur(ImagePtr I1, int filterW, int filterH, ImagePtr I2)
{
	ImagePtr I3;			     //I1 -> I3 will be I1 with appriopiate padding -> blur I3 - > outoput to I2
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
	vector<int> paddedRow(paddingNumberW * 2 + w, 0);		//make a vector of size paddingNumberW*2+w and set all to 0
	//fill(v.begin(), v.end(), 0)
	DBOUT(L"\nWIDTH     ");
	DBOUT(w);
	DBOUT(L"\nHEIGHT    ");
	DBOUT(h);
	DBOUT(L"\ntotal     ");
	DBOUT(total);
	DBOUT(L"\nfilterW:          ");
	DBOUT(filterW);
	DBOUT(L"\nfilterH:          ");
	DBOUT(filterH);
	DBOUT(L"\npaddingNumberW:  ");
	DBOUT(paddingNumberW);
	DBOUT(L"\npaddingNumberH:  ");
	DBOUT(paddingNumberH);
	DBOUT(L"\n");


	vector<int> paddedBuffer;// (paddingNumberW * 2 + w, 0);		//make a vector of size paddingNumberW*2+w and set all to 0

	for (int ch = 0; IP_getChannel(I1, ch, in, type); ch++) 	// get input  pointer for channel ch
	{
		IP_getChannel(I2, ch, out, type);

		for (int row = 0; row < h; ++row)
		{
			for (int col = 0; col < w; ++col)
			{
				int pixel = *in++;									// ***NOTE*** if i didnt use pixel and just put in *p++, the image gets slip diagonally where the left side in on the right and the right on the left. weird
				if (col == 0) {										// 1a) if its the first pixel in the row...
					for (int k = 0; k < paddingNumberW; ++k)		// 1b) put zero paddings into the buffer 
						paddedBuffer.push_back(0);
					paddedBuffer.push_back(pixel);					// 1c1) after padding, insert the first pixel of input row into buffer and copy row pixels as normal
				}
				if (col == w - 1) {									// 1d) at the end of the row add the last pixel to the buffer then add the padding to the buffer			
					paddedBuffer.push_back(pixel);
					for (int k = 0; k < paddingNumberW; ++k)
						paddedBuffer.push_back(0);
				}
				else {												// 1c2) if not pixel at beginning or end of image, just copy pixels
					paddedBuffer.push_back(pixel);
				}
			}

			//print the buffer vector for testing
			/*DBOUT(L"start: \n");
			for (vector<int>::size_type i = 0; i < paddedBuffer.size(); i++) {
				DBOUT(paddedBuffer.at(i));
				DBOUT(L" ");
			}
			DBOUT(L" - end\n");
*/
			DBOUT(L"\nvector size of row: ");
			DBOUT(paddedBuffer.size());
			DBOUT(L"\n ");

			//finished the row, now do this:
			int counter = 0;
			for (int colCopy = paddingNumberW; colCopy < (w + paddingNumberW); ++colCopy)
			{
				*out++ = CLIP(paddedBuffer.at(colCopy), 0, 255);
				counter++;
			}
			DBOUT(L"\n---------------------------------------------------------------- row of output: ");
			DBOUT(counter);
			DBOUT(L"\n---------------------------------------------------------------- row of input : ");
			DBOUT(w);

			//clear vectror to 0 
			paddedBuffer.clear();//fill(paddedBuffer.begin(), paddedBuffer.end(), 0);


		}

	}

}