#include "IP.h"
#include <vector>	
#include <algorithm>	//sort	

using namespace IP;
using std::vector;
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_median:
//
// Apply median filter of size filter x filter to I1.
// Clamp filter to 9.
// Output is in I2.
//

//copy row a in vector A to row b in the same vector A
void copyVectorRow(vector<vector<int> > &v, int a, int b) { //copy row a to row b
	v[b].clear();
	for (int i = 0; i < v[a].size(); ++i)
		v[b].push_back(v[a].at(i));
}
void copyVectorRow(vector<vector<int> > &copyFrom, vector<vector<int> > &copyTo, int fromRow, int toRow) { //copy row a from vector a to row b of vector b
	copyTo[toRow].clear();
	for (int fromCol = 0; fromCol < copyFrom[fromRow].size(); ++fromCol)
		copyTo[toRow].push_back(copyFrom[fromRow].at(fromCol));
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
		//1) Make Fully Padded Input Image (paddedImage Vector Matrix) for the circular buffer
		IP_getChannel(I1, ch, in, type);
		IP_getChannel(I2, ch, out, type);
		int rowIncrement = 0;
		for (int row = 0; row < h; ++row) {
			//copy the row + pads:
			for (int col = 0; col < w; ++col) {
				int pixel = *in++;
				if (col == 0) {																//left padding = first pixel 
					for (int i = 0; i < paddingNumber; ++i)
						paddedImage[row + rowIncrement].push_back(pixel);
					//paddedImage[row + rowIncrement].push_back(pixel);
				}
				if (col == w - 1) {															//right padding = last pixel 
					paddedImage[row + rowIncrement].push_back(pixel);
					for (int i = 0; i < paddingNumber; ++i)
						paddedImage[row + rowIncrement].push_back(pixel);
				}
				else
					paddedImage[row + rowIncrement].push_back(pixel);
			}
			//First row of image -> Copy the first row 'paddingNumber' number of times to the bottom. (just added the top paddings + first row)  Ex: 1 padding means 1 top padding: paddedImage[row=0] = first row of image + padding. paddedImage[row=0] = copy of padded row0 and rowIncrement++ so that when row =1, this wont be overwritten
			if (row == 0) {
				for (int paddRows = 0; paddRows < paddingNumber; paddRows++) {
					rowIncrement++;
					copyVectorRow(paddedImage, 0, row + rowIncrement);                      //copy row 0 (the first padded row) to the next row <- creating top padding and first row of image padded.
				}
			}
			//Last row of image -> Copy the last row 'paddingNumber' number of times to the bottom of the matrix. (just added the final row + bottom paddings)
			if (row == h - 1) {
				for (int paddRows = 0; paddRows < paddingNumber; paddRows++) {
					copyVectorRow(paddedImage, row + rowIncrement, row + rowIncrement + 1);   //copy row h-1 (the last padded row) to the next row <- creating bottom padding and first row of image padded.
					rowIncrement++;
				}
			}
		}


		//2) Circular buffer: initial buffer setup: add the first 'filter' number of rows from the Padded Image to the buffer.
		for (int bIndex = 0; bIndex < filter; ++bIndex)	
			for (int col = 0; col < paddedImage[bIndex].size(); ++col)
				buff[bIndex].push_back(paddedImage[bIndex].at(col));

		//3) As the filter increments down, replace the oldest row index with the newer row. Ex: filter 3x3: buff[0,1,2] -> window down -> buff[3,1,2] -> window down -> buff[3,4,2] -> window down -> buff[3,4,5] -> window down -> buff[6,4,5]
		vector<vector<int> > output(h);
		int medianIndex = filter * filter / 2;
		int k = 5; //what numbers to add from medium
		for (int yIncrement = 0; yIncrement < h; ++yIncrement)												// Sliding filter windows vertically direction
		{
			for (int xIncrement = 0; xIncrement < w; ++xIncrement)											// Sliding filter windows horizontally direction
			{
				vector<int> sorted;
				int count = 1;
				int sum = 0;

				// A1) buffer is filled atm with the correct rows so add everything to the 'sorted' 1D vector and then sort the vector
				for (int buffRow = 0; buffRow < filter; ++buffRow)											// Going through rows of buffer 9shifted the rows at the end)
					for (int buffCol = xIncrement; buffCol < filter + xIncrement; ++buffCol)					// Going through cols of buffer (using the shift from the for loop)
						sorted.push_back(buff[buffRow].at(buffCol));
				sort(sorted.begin(), sorted.end());                                 //sort cevotr of the window

				// A2) add the median values with the k val from left to right
				sum = sum + sorted[medianIndex];
				for (int i = 1; i < k; ++i)
				{
					count += 2;
					sum = sum + sorted[medianIndex - i];
					sum = sum + sorted[medianIndex + i];

				}
				//*out++ = CLIP(sum / count, 0, 255);
			}
			//B) replace old row in buffer with the next row in opaddedImage
			if (yIncrement < h - 1) {
				copyVectorRow(paddedImage, buff, filter + yIncrement, yIncrement);
			}
		}

	}

}
