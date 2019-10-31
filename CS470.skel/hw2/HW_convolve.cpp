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
	IP_copyImageHeader(I1, I2);
	int w = I1->width();
	int h = I1->height();
	int total = w * h;
	ChannelPtr<uchar> input, out;
	ChannelPtr<uchar> point;
	
	
	int type;
	int filter = Ikernel->width();
	if (filter == 1) {
		for (int ch = 0; IP_getChannel(I1, ch, input, type); ch++) {
			IP_getChannel(I2, ch, out, type);
			for (int i = 0; i<total ; i++) *out++ = *input++;
		}
	}
	else if (filter > 1) {

		ChannelPtr<float> pKernel; 
		IP_getChannel(Ikernel, 0, pKernel, type);

		int padded_bufsize = filter + w - 1; 
		
	    // Couldn't dynamically allocate
		short* buffers[30]; 
		for (int i = 0; i < filter; i++)
		{
		buffers[i] = new short[padded_bufsize];
		}
		
		for (int ch = 0; IP_getChannel(I1, ch, input, type); ch++) {
			IP_getChannel(I2, ch, out, type);
			point = input + total; // pointing to the total + input

			// Here copying the top padded row 1 to buffer.
			for (int i = 0; i < filter / 2; i++)
			{
				copyRowTo_Buffer(input, buffers[i], w, filter);
			}
			
			for (int i = filter / 2; i < filter; i++) {
				copyRowTo_Buffer(input, buffers[i], w, filter);
				if (input < point - w) {
					input = input + w;
				}
				}

			for (int col = 0; col < h; col++) {  
				for (int r = 0; r < w; r++) {
					float sum = 0.0;
					for (int i = 0; i < filter; i++) {
						for (int j = 0; j < filter; j++) {
							sum += (buffers[i][j + r] * *pKernel++);
						}
					}
					pKernel -= filter * filter;
					*out++ = (int)CLIP(sum, 0, 255);
				}

				// circular quque
				// remove buffers[0] and push all other buffers up by one index
				for (int i = 0; i < filter - 1; i++) {
					for (int j = 0; j < padded_bufsize; j++) {
						buffers[i][j] = buffers[i + 1][j];
					}
				}
				// copy next row to last buffer
				copyRowTo_Buffer(input, buffers[filter - 1], w, filter); // copy new row to the last buffer
				if (input < point - w) input += w;
			}
		}
	
		for (int i = 0; i < 30; i++) delete[] buffers[i];
	
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
