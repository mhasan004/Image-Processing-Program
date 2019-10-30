#include <algorithm>
#include <vector>
#include "IP.h"
using namespace IP;
using std::vector;
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_median:
//
// Apply median filter of size filter x filter to I1.
// Clamp filter to 9.
// Output is in I2.
//

//copy a row in vector A to another row in the same vector A
void copyVectorRow(vector<vector<int> > &v, int a, int b) { //copy row a to row b
	v[b].clear();
	for (int i = 0; i < v[a].size(); ++i)
		v[b].push_back(v[a].at(i));
}


void HW_median(ImagePtr I1, int filter, ImagePtr I2)
{
	if (filter < 9) filter = 9;
	IP_copyImageHeader(I1, I2);
	ChannelPtr<uchar> in, out;																// Image channel pointers and datatype
	int type;
	const int w = I1->width();
	const int h = I1->height();
	int total = w * h;

	if (filter % 2 == 0 && filter != 1) 													// Bluring window must be odd so that the averaged of pixel in window goes to middle pxl. WITH OUT THIS: THE PICTURE GETS DARKER EVERY EVEN FILTER SIZE!
		filter--;

	int paddingNumber = (filter - 1) / 2;													// Number of padding needed on sides of rows for a given filter size
	int bufferW = w + 2 * w;
	
	vector<vector<int> > buff(filter);														// making a 2d vector matrix that can only have 'filter' rows. This will be the circular buffer. will just overwrite old rows.
	vector<vector<int> > paddedImage(h + 2 * paddingNumber);

	for (int ch = 0; IP_getChannel(I1, ch, in, type); ch++)
	{
		//1) paddedImage Vector Matrix = fully padding Image
		IP_getChannel(I1, ch, in, type);
		int rowIncrement = 0;
		for (int row = 0; row < h; ++row) {
			//copy the row + pads:
			for (int col = 0; col < w; ++col) {
				int pixel = CLIP(*in++, 0, 255);
				if (col == 0) {																//left padding = first pixel 
					for (int i = 0; i < paddingNumber; ++i)
						paddedImage[row + rowIncrement].push_back(pixel);
					paddedImage[row + rowIncrement].push_back(pixel);
				}
				if (col == w - 1) {															//right padding = last pixel 
					paddedImage[row + rowIncrement].push_back(pixel);
					for (int i = 0; i < paddingNumber; ++i)
						paddedImage[row + rowIncrement].push_back(pixel);
				}
				else
					paddedImage[row + rowIncrement].push_back(pixel);
			}
			//If its the first row, copy top padding: ex 1 padding means 1 top padding: paddedImage[row=0] = first row of image + padding. paddedImage[row=0] = copy of padded row0 and rowIncrement++ so that when row =1, this wont be overwritten
			if (row == 0) {
				for (int paddRows = 0; paddRows < paddingNumber; paddRows++) {
					rowIncrement++;
					copyVectorRow(paddedImage, 0, row + rowIncrement);                      //copy row 0 (the first padded row) to the next row <- creating top padding and first row of image padded.
				}
			}
			if (row == h - 1) {
				for (int paddRows = 0; paddRows < paddingNumber; paddRows++) {
					copyVectorRow(paddedImage, row + rowIncrement, row + rowIncrement + 1);   //copy row h-1 (the last padded row) to the next row <- creating bottom padding and first row of image padded.
					rowIncrement++;
				}
			}
		}



		////1) Prefill the filter rows. Ex: if the filter is 3x3, -> buffer will have 3 rows -> prefill the first 3 rows with the rows from the image BUT need to add top padding. So 3x3 filter has 1padding: buffer[0]=image first row+padding. buffer[1] = copy buffer[0] because top row is a padding
		////1a) buff[0] = first row of image with left and right padding 
		////1b) for x=1 to x=paddingNumber -> buff[x] = copyvectorRow(buff, 0, x)
		//for (int bIndex = 0; bIndex < filter; ++bIndex)
		//{
		//	for (int col = 0; col < w; ++col) {
		//		if (bIndex == 0) {	//im at the first index of the buffer. since im prefilling, im also at the 

		//		}

		//	}
		//}


	}

}
