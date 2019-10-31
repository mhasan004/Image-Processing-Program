#include "IP.h"
using namespace IP;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_convolve:
//
// Convolve I1 with filter kernel in Ikernel.
// Output is in I2.
//
#include<vector>

// Using the same the copyRowToBuffer

void copyRowTo_Buffer(ChannelPtr<uchar>, short*, int, int);

void HW_convolve(ImagePtr I1, ImagePtr Ikernel, ImagePtr I2)
{
	int filter = Ikernel->width();
	// if(filter % 2 == 0) filter++; // set filter to always be an odd number

	IP_copyImageHeader(I1, I2);
	int w = I1->width();
	int h = I1->height();
	int total = w * h;

	int type;
	ChannelPtr<uchar> input, out;
	ChannelPtr<uchar> point;

	if (filter == 1) {
		for (int ch = 0; IP_getChannel(I1, ch, input, type); ch++) {
			IP_getChannel(I2, ch, out, type);
			for (point = input + total; input < point;) {
				*out++ = *input++;
			}
		}
	}
	else if (filter > 1) {

		ChannelPtr<float> pointerKernl; // kernel value pointer
		IP_getChannel(Ikernel, 0, pointerKernl, type);

		int bufSz = filter + w - 1;
		short* buffers[12]; // array of 12 pointers , failed to dynamically allocate
		for (int i = 0; i < filter; i++) buffers[i] = new short[bufSz];

		for (int ch = 0; IP_getChannel(I1, ch, input, type); ch++) {
			IP_getChannel(I2, ch, out, type);
			point = input + total;

			// copying padded row1 to first buffers, 
			for (int i = 0; i < filter / 2; i++)
			{
				copyRowTo_Buffer(input, buffers[i], w, filter);
			}
			// finish the rest of buffers



			for (int i = filter / 2; i < filter; i++) {
				copyRowTo_Buffer(input, buffers[i], w, filter);
				if (input < point - w) {
					input += w;
				}
			}

			for (int y = 0; y < h; y++) {  // visit each row
				// visit each pixel in row
				for (int x = 0; x < w; x++) {
					float sum = 0.0;
					for (int i = 0; i < filter; i++) {
						for (int j = 0; j < filter; j++) {
							sum += (buffers[i][j + x] * *pointerKernl++);
						}
					}
					pointerKernl -= filter * filter;
					*out++ = (int)CLIP(sum, 0, 255);
				}

				
			
				for (int i = 0; i < filter - 1; i++) {
					for (int j = 0; j < bufSz; j++) {
						buffers[i][j] = buffers[i + 1][j];	// remove buffers[0] and push all other buffers up by one index
					}
				}
				// copy next row to last buffer
				copyRowTo_Buffer(input, buffers[filter - 1], w, filter); // copy new row to the last buffer
				if (input < point - w) input += w;
			}
		}
		for (int i = 0; i < filter; i++) delete[] buffers[i];
	}
	
	
}

void copyRowTo_Buffer(ChannelPtr<uchar> in, short* buffer, int w, int size) {

	for (int i = 0; i < size / 2; i++) {
		buffer[i] = *in;
	}

	for (int i = size / 2 + w - 1; i < size + w - 1; i++) {
		buffer[i] = *in;
	}
	for (int i = size / 2; i < size / 2 + w - 1; i++) {
		buffer[i] = *in++;
	}

}
