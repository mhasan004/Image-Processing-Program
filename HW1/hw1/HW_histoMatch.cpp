#include "IP.h"
using namespace IP;

void histoMatchApprox(ImagePtr, ImagePtr, ImagePtr);

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_histoMatch:
//
// Apply histogram matching to I1. Output is in I2.
//
void
HW_histoMatch(ImagePtr I1, ImagePtr targetHisto, bool approxAlg, ImagePtr I2)
{
	if (approxAlg) {
		histoMatchApprox(I1, targetHisto, I2);
		return;
	}

	
    int i, p, R, type;
	int left[MXGRAY], right[MXGRAY], temp[MXGRAY];
	int Hsum, Havg, h1[MXGRAY], save[MXGRAY];
	ChannelPtr<uchar> in, out;
	ChannelPtr<int> h2;
	double scale;
	

	IP_copyImageHeader(I1, I2);
	// intit var for width, height and total pixels
	int w = I1->width();
	int h = I1->height();
	int total = w * h;
	// total number of pixels in image

	



	for (i = 0; i < MXGRAY; i++) h1[i] = 0;	// clear histogram
	// for (i = 0; i < total; i++)  h1[in[i]]++;	// eval  histogram
		IP_getChannel(targetHisto, 0, h2, type);

	// target histogram
	// normalize h2 to conform with dimensions of I1
	for (i = Havg = 0; i < MXGRAY; i++) Havg += h2[i];
	scale = (double)total / Havg;
	//========================

	if (scale != 1) for (i = 0; i < MXGRAY; i++) {
	
	
		
		 h2[i] *= scale;
	}
	
	for (int ch = 0; IP_getChannel(I1, ch, in, type); ch++) {
		IP_getChannel(I2, ch, out, type);

		//already cleared and now evaluated
		for (i = 0; i < total; i++) h1[in[i]]++;

		R = 0;
		Hsum = 0;
		// evaluate remapping of all input gray levels;
		// Each input gray value maps to an interval of valid
		// output values. The endpoints of the intervals are
		// left[] and right[] */
		for (i = 0; i < MXGRAY; i++) {
			left[i] = R; // left end of interval
			save[i] = h2[R] - Hsum;
			temp[i] = left[i];
			Hsum += h1[i];		// cum. interval value

			// make interval wider, if necessary
			while (Hsum > h2[R] && R < MXGRAY - 1) {
				Hsum -= h2[R];		// adjust Hsum
				R++;			// update
			}
			right[i] = R;
		}

		// clear h1 and reuse it below
		for (i = 0; i < MXGRAY; i++) h1[i] = 0;

		// visit all input pixels
		for (i = 0; i < total; i++) {
			int inp = in[i];
			p = left[inp];
			/* if (h1[p] < h2[p])	// mapping satisfies h2
			//	out[i] = p;
			//else	out[i] = p = left[in[i]] = MIN(p + 1, right[in[i]]);
			//h1[p]++;
			*/
			while (p == right[inp]) save[inp++]--;
			if (( left[in[i]] != right[in[i] - 1]) && h1[p] < h2[p])
				out[i] = p;

			else if ((h1[p] + save[in[i] - 1]) < h2[p])
				out[i] = p;
			else out[i] = p = left[in[i]] = MIN(p + 1, right[in[i]]);
			h1[p]++;
		}



	}









	
}

void
histoMatchApprox(ImagePtr I1, ImagePtr targetHisto, ImagePtr I2)
{
	int i, p, R, type;
	int left[MXGRAY], right[MXGRAY];
	int Hsum, Havg, h1[MXGRAY], save[MXGRAY];
	ChannelPtr<uchar> in, out;
	ChannelPtr<int> h2;
	double scale;
	IP_copyImageHeader(I1, I2);
	// intit var for width, height and total pixels
	int w = I1->width();
	int h = I1->height();
	int total = w * h;
	// total number of pixels in image

	for (i = 0; i < MXGRAY; i++) h1[i] = 0;	// clear histogram
	IP_getChannel(targetHisto, 0, h2, type);

	// target histogram
	// normalize h2 to conform with dimensions of I1
	for (i = Havg = 0; i < MXGRAY; i++) Havg += h2[i];
	scale = (double)total / Havg;
	if (scale != 1) for (i = 0; i < MXGRAY; i++) {
		h2[i] *= scale;
	}

	for (int ch = 0; IP_getChannel(I1, ch, in, type); ch++) {
		IP_getChannel(I2, ch, out, type);

		//already cleared and now evaluated
		for (i = 0; i < total; i++) h1[in[i]]++;

		R = 0;
		Hsum = 0;
		// evaluate remapping of all input gray levels;
		// Each input gray value maps to an interval of valid
		// output values. The endpoints of the intervals are
		// left[] and right[] */
		for (i = 0; i < MXGRAY; i++) {
			left[i] = R;		// left end of interval
			Hsum += h1[i];		// cum. interval value

			// make interval wider, if necessary
			while (Hsum > h2[R] && R < MXGRAY - 1) {
				Hsum -= h2[R];		// adjust Hsum
				R++;			// update
			}

			right[i] = R;
		}

		// clear h1 and reuse it below
		for (i = 0; i < MXGRAY; i++) h1[i] = 0;

		// visit all input pixels
		for (i = 0; i < total; i++) {
			p = left[in[i]];
			if (h1[p] < h2[p])	// mapping satisfies h2
				out[i] = p;
			else	out[i] = p = left[in[i]] = MIN(p + 1, right[in[i]]);
			h1[p]++;





		}



	}



}
