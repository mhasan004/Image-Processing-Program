#include <algorithm>
#include <vector>
#include "IP.h"
using namespace IP;
using std::vector;
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_median:
//
// Apply median filter of size sz x sz to I1.
// Clamp sz to 9.
// Output is in I2.
//
void HW_median(ImagePtr I1, int sz, ImagePtr I2)
{
	if (sz < 9) sz = 9;
	IP_copyImageHeader(I1, I2);
	ChannelPtr<uchar> in, out;															// Image channel pointers and datatype
	int type;
	const int w = I1->width();
	const int h = I1->height();
	int total = w * h;

	if (sz % 2 == 0 && sz != 1) 														// Bluring window must be odd so that the averaged of pixel in window goes to middle pxl. WITH OUT THIS: THE PICTURE GETS DARKER EVERY EVEN FILTER SIZE!
		sz--;

	vector<vector<int> > copiedImage(h);																// a) making a h x w 2D vector matrix of all zeroes
	for (int i = 0; i < h; i++)
		copiedImage[i].resize(w);

	int paddingNumber = (sz - 1) / 2;													// Number of padding needed on sides of rows for a given filter size
	int bufferW = w + 2 * w;

	vector<vector<int> > paddedMatrix(h + paddingNumber);													// a) making a h x w 2D vector matrix of all zeroes
	for (int i = 0; i < h + paddingNumber; i++)
		paddedMatrix[i].resize(w + 2 * paddingNumber);


	for (int ch = 0; IP_getChannel(I1, ch, in, type); ch++)
	{
		IP_getChannel(I1, ch, in, type);
		// making a matrix copy to acces last row easier
		for (int i = 0; i < h; i++)
			for (int j = 0; j < w; j++)
				copiedImage[i][j] = CLIP(*in++, 0, 255);

		// 1) go through the first row and last row of the image and making a top and bottom padding (which will also the first padded row)
		vector<int> paddedTopBuffer;
		vector<int> paddedBottomBuffer;
		for (int col = 0; col < w; col++) {
			int pixel = copiedImage[0][col];
			if (col == 0) {									//left padding = first pixel 
				for (int i = 0; i < paddingNumber; ++i)
					paddedTopBuffer.push_back(pixel);
				paddedTopBuffer.push_back(pixel);
			}
			if (col == w - 1) {								//right padding = last pixel 
				paddedTopBuffer.push_back(pixel);
				for (int i = 0; i < paddingNumber; ++i)
					paddedTopBuffer.push_back(pixel);
			}
			else
				paddedTopBuffer.push_back(pixel);
		}
		for (int col = 0; col < w; col++) {
			int pixel = copiedImage[h - 1][col];
			if (col == 0) {									//left padding = first pixel 
				for (int i = 0; i < paddingNumber; ++i)
					paddedBottomBuffer.push_back(pixel);
				paddedBottomBuffer.push_back(pixel);
			}
			if (col == w - 1) {								//right padding = last pixel 
				paddedBottomBuffer.push_back(pixel);
				for (int i = 0; i < paddingNumber; ++i)
					paddedBottomBuffer.push_back(pixel);
			}
			else
				paddedBottomBuffer.push_back(pixel);
		}


		// 2) make the padded matrix
		IP_getChannel(I1, ch, in, type);
		for (int row = 0; row < h; row++) {
			for (int col = 0; col < w; col++) {
				// a) If im on the first row of the image, add the padding on the top and then add the first row and its side padding itself (Ex: if padding is 2, copy the 'paddedTopBuffer' vector 3 times)
				if (row == 0 && col == 0 && paddingNumber != 0) {											// Im on the first row, i also have the vector that is the padding for the top of image. Ex: for a 5x5 filter window, will add 2 padding on left and right side of the image and will add two rows of padding on the top and bottom:  Will add 2 rows of padding on top here:
					for (int paddedRow = 0; paddedRow <= paddingNumber; ++paddedRow)						// Will add 'paddingNumber' rows of padding on top
						for (int paddedRowCols = 0; paddedRowCols < bufferW; ++paddedRowCols)				// goign through the 'paddedTopBuffer' vector and pushign each index to 'paddedBuffer' vector
							paddedMatrix[row].push_back(paddedTopBuffer.at(paddedRowCols));
				}


				/////^^^^^^^^^^^^^^^^^^^^^^^^ERROR HERE ON 2A on vector sizing is wrong



				//// b) If im on the last row copy that last row and the padding rows (Ex: padding is 2, copy last row 3 times)
				//if (row == h-1 && col == 0 && paddingNumber != 0) {
				//	for (int paddedRow = 0; paddedRow <= paddingNumber; ++paddedRow)			
				//		for (int paddedRowCols = 0; paddedRowCols < bufferW; ++paddedRowCols)				
				//			paddedMatrix[row].push_back(paddedBottomBuffer.at(paddedRowCols));
				//}

				//// c) If its the middle rows, add the padding at the left and right and the pixel themselves
				//else {
				//	int pixel = copiedImage[row][col];
				//	if (col == 0) {									//left padding = first pixel 
				//		for (int i = 0; i < paddingNumber; ++i)
				//			paddedMatrix[row].push_back(pixel);
				//		paddedMatrix[row].push_back(pixel);
				//	}
				//	if (col == w - 1) {								//right padding = last pixel 
				//		paddedMatrix[row].push_back(pixel);
				//		for (int i = 0; i < paddingNumber; ++i)
				//			paddedMatrix[row].push_back(pixel);
				//	}
				//	else
				//		paddedMatrix[row].push_back(pixel);
				//}
			}
		}

		//// 3) Do the median filter on the copiedMatrix 2D vector and copy to I2
		//IP_getChannel(I2, ch, out, type);




	}

}
