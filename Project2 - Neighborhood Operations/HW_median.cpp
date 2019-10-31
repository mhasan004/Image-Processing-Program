#include "IP.h"
#include <vector>	
#include <algorithm>	//sort	
using namespace IP;
using std::vector;
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_median: by Mahmudul Hasan
//
// Apply median filter of size filter x filter to I1.
// Clamp filter to 9.
// Output is in I2.
//

void copyVectorRow(vector<vector<short> > &v, int a, int b) { 																			//copy row a in vector A to row b in the same vector A
	v[b].clear();
	for (int i = 0; i < v[a].size(); ++i)
		v[b].push_back(v[a].at(i));
}
void copyVectorRow(vector<vector<short> > &copyFrom, vector<vector<short> > &copyTo, int fromRow, int toRow) { 							//copy row a from vector a to row b of vector b
	copyTo[toRow].clear();
	for (int fromCol = 0; fromCol < copyFrom[fromRow].size(); ++fromCol)
		copyTo[toRow].push_back(copyFrom[fromRow].at(fromCol));
}


void HW_median(ImagePtr I1, int filter, ImagePtr I2)
{
	if (filter < 9) filter = 9;																											//limit filter to <9
	if (filter % 2 == 0) filter++;                                                               										//keep the filter odd
	int k = 0; //what numbers to add from medium
	IP_copyImageHeader(I1, I2);
	ChannelPtr<uchar> in, out;																											// Image channel pointers and datatype
	int type;
	const int w = I1->width();
	const int h = I1->height();
	int total = w * h;

	int paddingNumber = (filter - 1) / 2;																								// Number of padding needed on sides of rows and on top and bottom of image for a given filter size
	vector<vector<short> > paddedImage(h + 2 * paddingNumber);
	for (int ch = 0; IP_getChannel(I1, ch, in, type); ch++)
	{
		IP_getChannel(I1, ch, in, type);																				
		IP_getChannel(I2, ch, out, type);
		int rowIncrement = 0;

		// Padded Image: Better Implementation: use an array of pointers to vectors instead of the "paddedImage" 2D vector because then i wouldnt need to copy entire paded rows, i could just set each row to point to a padded row 
		for (int row = 0; row < h; ++row) {																				//1) Make Fully Padded Input Image (paddedImage Vector Matrix) for the circular buffer.
			for (int col = 0; col < w; ++col) {
				int pixel = *in++;
				if (col == 0) {																											//left padding = first pixel 
					for (int i = 0; i < paddingNumber; ++i)
						paddedImage[row + rowIncrement].push_back(pixel);
				}
				if (col == w - 1) {																										//right padding = last pixel 
					paddedImage[row + rowIncrement].push_back(pixel);
					for (int i = 0; i < paddingNumber; ++i)
						paddedImage[row + rowIncrement].push_back(pixel);
				}
				else
					paddedImage[row + rowIncrement].push_back(pixel);
			}
			if (row == 0) {																												//First row of image -> Copy the first row 'paddingNumber' number of times to the bottom. (just added the top paddings + first row)  Ex: 1 padding means 1 top padding: paddedImage[row=0] = first row of image + padding. paddedImage[row=0] = copy of padded row0 and rowIncrement++ so that when row =1, this wont be overwritten
				for (int paddRows = 0; paddRows < paddingNumber; paddRows++) {
					rowIncrement++;
					copyVectorRow(paddedImage, 0, row + rowIncrement);                      											//copy row 0 (the first padded row) to the next row <- creating top padding and first row of image padded.
				}
			}
			if (row == h - 1) {																											//Last row of image -> Copy the last row 'paddingNumber' number of times to the bottom of the matrix. (just added the final row + bottom paddings)
				for (int paddRows = 0; paddRows < paddingNumber; paddRows++) {
					copyVectorRow(paddedImage, row + rowIncrement, row + rowIncrement + 1);   											//copy row h-1 (the last padded row) to the next row <- creating bottom padding and first row of image padded.
					rowIncrement++;
				}
			}
		}

																														//2) Circular buffer: initial buffer setup: point the buffer's points to thje initial 'filter' number of rows of the padded image.
		short **buff = new short*[filter];								  																//***needs to be a dynamic array wont let me do short *buff[filter];	
		short **startOfKernal = new short*[filter];																						// this pointer will help me keep track of x shifts so that i dont have to constantly go to the correct column of an image
		for (int bIndex = 0; bIndex < filter; ++bIndex) {
			buff[bIndex] = paddedImage[bIndex].data();
			startOfKernal[bIndex] = buff[bIndex];                     																 	//***Set the reset pointer  
		}

																														//3) As the filter increments down, change the pointer from pointing at an old padded row to a newer padded row. Ex: filter 3x3: buff[0,1,2] (buff points to rows 0,1,2 of padded image)-> window down -> buff[3,1,2] -> kernal shifts down -> buff[3,4,2] -> kernal shifts down  -> buff[3,4,5] -> kernal shifts down  -> buff[6,4,5]
		int medianIndex = filter * filter / 2;
		for (int yIncrement = 0; yIncrement < h; ++yIncrement)																			// Sliding filter windows vertically direction
		{
			for (int xIncrement = 0; xIncrement < w; ++xIncrement)																		// Sliding filter windows horizontally direction
			{
				vector<short> sorted;
				int count = 1;
				int sum = 0;

				if (xIncrement == 0)                                  																	//A) buffer is filled atm with the correct rows so add everything to the 'sorted' 1D vector and then sort the vector
				{
					for (int buffRow = 0; buffRow < filter; ++buffRow)																	// Going through rows of buffer 9shifted the rows at the end)
					{
						for (int buffCol = xIncrement; buffCol < filter + xIncrement; ++buffCol)										// Going through cols of buffer (using the shift from the for loop)
						{
							if (buffCol == (xIncrement + 1)) {                                     										//***now i know where the first pixel of kernal will ne located in
								startOfKernal[buffRow] = buff[buffRow];
							}
							sorted.push_back(*buff[buffRow]++);
						}
					}
				}

				else																													//B)
				{
					for (int buffRow = 0; buffRow < filter; ++buffRow)																	// Going through rows of buffer 9shifted the rows at the end)
					{
						for (int buffCol = xIncrement; buffCol < filter + xIncrement; ++buffCol)										// Going through cols of buffer (using the shift from the for loop)
						{
							if (buffCol == xIncrement)
								buff[buffRow] = startOfKernal[buffRow];                                  								//***get back the position of the first pixel of kernal row
							if (buffCol == (xIncrement + 1))                                            								//***set pos for the next iteration!
								startOfKernal[buffRow] = buff[buffRow];
							sorted.push_back(*buff[buffRow]++);
						}
					}
				}
				sort(sorted.begin(), sorted.end());

				sum = sum + sorted[medianIndex];
				for (int i = 1; i < k; ++i)
				{
					count += 2;
					sum = sum + sorted[medianIndex - i];
					sum = sum + sorted[medianIndex + i];
				}
				*out++ = CLIP(sum / count, 0, 255);
			}
																														//4) replace old row in buffer with the next row in the padded Image
			if (yIncrement < h - 1) {
				for (int i = 0; i < filter; ++i) 
					buff[(yIncrement + i) % filter] = paddedImage[filter + yIncrement].data();
			}
		}
	}
}
