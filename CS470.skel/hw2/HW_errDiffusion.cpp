#include "IP.h"
using namespace IP;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_errDiffusion:
//
// Apply error diffusion algorithm to image I1.
//
// This procedure produces a black-and-white dithered version of I1.
// Each pixel is visited and if it + any error that has been diffused to it
// is greater than the threshold, the output pixel is white, otherwise it is black.
// The difference between this new value of the pixel from what it used to be
// (somewhere in between black and white) is diffused to the surrounding pixel
// intensities using different weighting systems.
//
// Use Floyd-Steinberg     weights if method=0.
// Use Jarvis-Judice-Ninke weights if method=1.
//
// Use raster scan (left-to-right) if serpentine=0.
// Use serpentine order (alternating left-to-right and right-to-left) if serpentine=1.
// Serpentine scan prevents errors from always being diffused in the same direction.
//
// A circular buffer is used to pad the edges of the image.
// Since a pixel + its error can exceed the 255 limit of uchar, shorts are used.
//
// Apply gamma correction to I1 prior to error diffusion.
// Output is saved in I2.
//
void copyRowToCircBuffer(ChannelPtr<uchar>, short*, int, int);
void gammaCorrect(ImagePtr, double, ImagePtr);



void HW_errDiffusion(ImagePtr I1, int method, bool serpentine, double gamma, ImagePtr I2)
{

	IP_copyImageHeader(I1, I2);
	ImagePtr I3; // The temp image pointer for gamma
	IP_copyImageHeader(I1, I3);
	gammaCorrect(I1, gamma, I3);
	int type;
	int w = I1->width();
	int h = I1->height();

	int thr = MXGRAY / 2;

	ChannelPtr<uchar> in, out;

	if (method == 0) {
		// method 0 for floyd-steinberg method
		short* in1;
		short* in2;
		short error;

		// top of buffer
		short* buf0 = new short[w + 2];
		// bottom of buffer
		short* buf1 = new short[w + 2];
		
		for (int ch = 0; IP_getChannel(I3, ch, in, type); ch++) {
			IP_getChannel(I2, ch, out, type);

			copyRowToCircBuffer(in, buf0, w, 3);  // kernel size is 3
			in += w;

			for (int y = 1; y < h; y++) {
				if (serpentine) {
					// serpentine scan needed to be done
					if (y % 2 == 0) {
						copyRowToCircBuffer(in, buf0, w, 3);
						in += w;
						in1 = buf1 + w + 1; // +1 for the padding
						in2 = buf0 + w + 1; // same skip for top buffer 
						out += w - 1;
						for (int i = 0; i < w; i++) {
							*out = (*in1 < thr) ? 0 : 255;
							error = *in1 - *out;
							*(in1 - 1) += (error*7/16.0);
							*(in2 - 1) += (error*3/16.0);
							*(in2 + 0) += (error*5/16.0);
							*(in2 - 1) += (error*1/16.0);
							in1--; in2--;
							out--;
						}
						out += w + 1;
					}
					else { // else dtstement for Odd Rows
						copyRowToCircBuffer(in, buf1, w, 3);
						in += w;
						in1 = buf0 + 1;
						in2 = buf1 + 1;

						for (int x = 0; x < w; x++) {
							*out = (*in1 < thr) ? 0 : 255; // maxgray or, 255
							error = *in1 - *out;
							*(in1 + 1) += (error*7/16.0);
							*(in2 - 1) += (error*3/16.0);
							*(in2 + 0) += (error*5/16.0);
							*(in2 + 1) += (error*1/16.0);
							in1++; in2++; // advance circ buffer ptrs
							out++;

						}
					}
				}
				else {
					// raster scan down below
					// y%2 == 0 perform ops on even rows and else for odds
					if (y % 2 == 0) { //even rows
						copyRowToCircBuffer(in, buf0, w, 3); // in to buf0
						in1 = buf1 + 1; // +1 for the padding
						in2 = buf0 + 1; // same skip for top buffer 
					}
					else { // odd rows
						copyRowToCircBuffer(in, buf1, w, 3);
						in1 = buf0 + 1; // +1 skip for padding
						in2 = buf1 + 1; // same
					}
					in += w;
					for (int x = 0; x < w; x++) {
						*out = (*in1 < thr) ? 0 : 255; // maxgray or, 255
						error = *in1 - *out;
						*(in1 + 1) += (error*7/16.0);
						*(in2 - 1) += (error*3/16.0);
						*(in2 + 0) += (error*5/16.0);
						*(in2 + 1) += (error*1/16.0);
						in1++; in2++; // advance circ buffer ptrs
						out++;        // same with outputs


					}
				}
			}
		}
		delete[] buf0;  // clearing the buffer0 
		delete[] buf1;  // clearing the buffer1


	}
	else if (method == 1) {
		// Jarvis_judice_ninke Method == 1
		
		
		
		int buffersize = w + 4;
		int inputsze=3;
		short** input = new short*[inputsze];
		short** buff = new short*[inputsze];
		int error;
		for (int i = 0; i < 3; i++) buff[i] = new short[buffersize];
		
		int type;
		for (int ch = 0; IP_getChannel(I3, ch, in, type); ch++) {
			IP_getChannel(I2, ch, out, type);

			copyRowToCircBuffer(in, buff[0], w, 5); // Kernel SIze 5 for JJN method
			// Here copying row 1 to first two buffers.
			copyRowToCircBuffer(in, buff[1], w, 5);

			in += w;
			for (int y = 2; y < h; y++){
				if (y % 3 == 2) {
					copyRowToCircBuffer(in, buff[2], w, 5); 
				}
				else if (y % 3 == 1) {
					copyRowToCircBuffer(in, buff[1], w, 5); 
				}
				else if (y % 3 == 0) { 
					copyRowToCircBuffer(in, buff[0], w, 5); 
				}
			in += w;

				if (serpentine) {
					// serpentine for JJN
					if (y % 2 == 0) { 
						// for the even rows
						if (y % 3 == 0) {
							input[0] = buff[1] + w + 2;
							input[1] = buff[2] + w + 2;
							input[2] = buff[0] + w + 2;
						}
						else if (y % 3 == 1) {
							input[0] = buff[2] + w + 2;
							input[1] = buff[0] + w + 2;
							input[2] = buff[1] + w + 2;
						}
						else {
							input[0] = buff[0] + w + 2;
							input[1] = buff[1] + w + 2;
							input[2] = buff[2] + w + 2;
						}

						out += w - 1;
						for (int i = 0; i < w; i++) {
							*out = (*input[0] < thr) ? 0 : 255;
							error = *input[0] - *out;
							*(input[0] - 1) += (error*7/48.0);     // row[0] + 1  flipped
							*(input[0] - 2) += (error*5/48.0);     // row[0] + 2  flipped
							*(input[1] + 0) += (error*7/48.0);     // row[1] + 0
							*(input[1] + 1) += (error*5/48.0);     // row[1] + 1  
							*(input[1] + 2) += (error*3/48.0);     // row[1] + 2
							*(input[1] - 1) += (error*5/48.0);     // row[1] - 1
							*(input[1] - 2) += (error*3/48.0);     // row[1] - 2
							*(input[2] + 0) += (error*5/48.0);     // row[2] + 0
							*(input[2] + 1) += (error*3/48.0);     // row[2] + 1
							*(input[2] + 2) += (error*1/48.0);     // row[2] + 2
							*(input[2] - 1) += (error*3/48.0);     // row[2] - 1
							*(input[2] - 2) += (error*1/48.0);     // row[2] - 2
							
							input[0]--;  input[1]--; input[2]--; // Walk back(flipped) for even
							out--;
						}
						out += w + 1;

					}
					else {
						//  else OPs on Odd rows 
						if (y % 3 == 1) {
							input[0] = buff[2] + 2;
							input[1] = buff[0] + 2;
							input[2] = buff[1] + 2;
						}
						else if (y % 3 == 0) {
							input[0] = buff[1] + 2;
							input[1] = buff[2] + 2;
							input[2] = buff[0] + 2;
						}
						else {
							input[0] = buff[0] + 2;
							input[1] = buff[1] + 2;
							input[2] = buff[2] + 2;
						}
						for (int i = 0; i < w; i++) {
							*out = (*input[0] < thr) ? 0 : 255;
							error = *input[0] - *out;
							*(input[0] + 1) += (error*7/48.0);     // row[0] + 1  
							*(input[0] + 2) += (error*5/48.0);     // row[0] + 2
							*(input[1] + 0) += (error*7/48.0);     // row[1] + 0
							*(input[1] + 1) += (error*5/48.0);     // row[1] + 1  
							*(input[1] + 2) += (error*3/48.0);     // row[1] + 2
							*(input[1] - 1) += (error*5/48.0);     // row[1] - 1
							*(input[1] - 2) += (error*3/48.0);     // row[1] - 2
							*(input[2] + 0) += (error*5/48.0);     // row[2] + 0
							*(input[2] + 1) += (error*3/48.0);     // row[2] + 1
							*(input[2] + 2) += (error*1/48.0);     // row[2] + 2
							*(input[2] - 1) += (error*3/48.0);     // row[2] - 1
							*(input[2] - 2) += (error*1/48.0);     // row[2] - 2
							
							input[0]++; input[1]++; input[2]++;
							out++; // Advancing on Odd rows



						}




					}


				}
				else {
					// else jarvis-judis method by Raster 
					//to be continued
					if (y % 3 == 1) {
						input[0] = buff[2] + 2;
						input[1] = buff[0] + 2;
						input[2] = buff[1] + 2;
					}
					else if (y % 3 == 0) {
						input[0] = buff[1] + 2;
						input[1] = buff[2] + 2;
						input[2] = buff[0] + 2;
					}
					else {
						input[0] = buff[0] + 2;
						input[1] = buff[1] + 2;
						input[2] = buff[2] + 2;
					}
					for (int i = 0; i < w; i++) {
						*out = (*input[0] < thr) ? 0 : 255;
						error = *input[0] - *out;
						*(input[0] + 1) += (error * 7 / 48.0);     // row[0] + 1  
						*(input[0] + 2) += (error * 5 / 48.0);     // row[0] + 2
						*(input[1] + 0) += (error * 7 / 48.0);     // row[1] + 0
						*(input[1] + 1) += (error * 5 / 48.0);     // row[1] + 1  
						*(input[1] + 2) += (error * 3 / 48.0);     // row[1] + 2
						*(input[1] - 1) += (error * 5 / 48.0);     // row[1] - 1
						*(input[1] - 2) += (error * 3 / 48.0);     // row[1] - 2
						*(input[2] + 0) += (error * 5 / 48.0);     // row[2] + 0
						*(input[2] + 1) += (error * 3 / 48.0);     // row[2] + 1
						*(input[2] + 2) += (error * 1 / 48.0);     // row[2] + 2
						*(input[2] - 1) += (error * 3 / 48.0);     // row[2] - 1
						*(input[2] - 2) += (error * 1 / 48.0);     // row[2] - 2
						
						input[0]++; input[1]++; input[2]++;
						out++; // Advancing
						
						



					}
				}
			}

		}
		delete[] buff; // clearing buffer after the terations

	}


}







void copyRowToCircBuffer(ChannelPtr<uchar> in, short* buffer, int w, int size) {

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


void gammaCorrect(ImagePtr I1, double gamma, ImagePtr I2)
{
	// s = c * r^gamma. c=1, r=image input
	// gamma correction: s = c * r^(1/gamma).

	// copy image header (width, height) of input image I1 to output image I2
	IP_copyImageHeader(I1, I2);
	// declarations for image channel pointers and datatype
	ChannelPtr<uchar> p1, p2;
	int type;

	// init vars for width, height, and total number of pixels
	int w = I1->width();
	int h = I1->height();
	int total = w * h;


	// init lookup table

	int i, lut[MXGRAY];
	for (i = 0; i < MXGRAY; ++i) {
		lut[i] = (int)CLIP((double)MaxGray*(pow((double)i / MaxGray, (1 / gamma))), 0, MaxGray);

	}

	// visit all image channels and evaluate output image
	for (int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {	// get input  pointer for channel ch
		IP_getChannel(I2, ch, p2, type);						// get output pointer for channel ch
		for (int i = 0; i < total; i++)
			*p2++ = lut[*p1++];			                        // use lut[] to eval output
	}
}
