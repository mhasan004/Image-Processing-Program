//makidn a Padded Image
#include <vector>
#include <iostream>
#include<algorithm> // for sort() 
using namespace std;

void copyVectorRow(vector<vector<int> >& v, int a, int b) { //copy row a to row b
	v[b].clear();
	for (int i = 0; i < v[a].size(); ++i)
		v[b].push_back(v[a].at(i));
}
void copyVectorRow(vector<vector<int> >& copyFrom, vector<vector<int> >& copyTo, int fromRow, int toRow) { //copy row a from vector a to row b of vector b
	copyTo[toRow].clear();
	for (int fromCol = 0; fromCol < copyFrom[fromRow].size(); ++fromCol)
		copyTo[toRow].push_back(copyFrom[fromRow].at(fromCol));
}
void printV(vector<vector<int> >& v) {
	for (int i = 0; i < v.size(); i++) {
		for (int j = 0; j < v[i].size(); j++)
			cout << v[i][j] << " ";
		cout << endl;
	}
}
void printV(vector<int>& v) {
	for (int j = 0; j < v.size(); j++)
		cout << v[j] << " ";
}

int main()
{
	int filter = 3;
	int paddingNumber = (filter - 1) / 2;
	int h = 5;
	int w = 4;
	vector<vector<int> > paddedImage(h + 2 * paddingNumber);
	vector<vector<int> > v(h); //data

	v[0].push_back(2); v[0].push_back(1); v[0].push_back(9); v[0].push_back(4);
	v[1].push_back(9); v[1].push_back(6); v[1].push_back(4); v[1].push_back(8);
	v[2].push_back(8); v[2].push_back(7); v[2].push_back(6); v[2].push_back(3);
	v[3].push_back(8); v[3].push_back(7); v[3].push_back(6); v[3].push_back(3);
	v[4].push_back(8); v[4].push_back(7); v[4].push_back(6); v[4].push_back(3);
	//v[5].push_back(8);v[5].push_back(7);v[5].push_back(6);v[5].push_back(3);

//******h=5 gives seg fault



	cout << "\noriginal Image: " << endl;
	printV(v);

	cout << "\npadd: " << paddingNumber << endl;
	int rowIncrement = 0;
	for (int row = 0; row < h; row++) {
		//copy the row + pads:
		for (int col = 0; col < w; col++) {
			int pixel = v[row][col];
			if (col == 0) {									//left padding = first pixel 
				for (int i = 0; i < paddingNumber; ++i)
					paddedImage[row + rowIncrement].push_back(pixel);
				//paddedImage[row + rowIncrement].push_back(pixel);
			}
			if (col == w - 1) {								//right padding = last pixel 
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
	cout << "Padded Image: " << endl;
	printV(paddedImage);


	vector<vector<int> > buff(filter);														// making a 2d vector matrix that can only have 'filter' rows. This will be the circular buffer. will just overwrite old rows.
	for (int bIndex = 0; bIndex < filter; ++bIndex)
		for (int col = 0; col < paddedImage[bIndex].size(); ++col)
			buff[bIndex].push_back(paddedImage[bIndex].at(col));
	cout << "\nComplete Circular Buffer Rows: " << endl;
	printV(buff);



	vector<vector<int> > output(h);
	int medianIndex = filter * filter / 2;
	int k = 5; //what numbers to add from medium
	for (int yIncrement = 0; yIncrement < h; ++yIncrement)												// Sliding filter windows vertically direction
	{
		for (int xIncrement = 0; xIncrement < w; ++xIncrement)											// Sliding filter windows horizontally direction
		{
			cout << "\nyIncrement: " << yIncrement << "  xIncrement: " << xIncrement << endl;
			vector<int> sorted;
			int count = 1;
			int sum = 0;

			// A1) buffer is filled atm with the correct rows so add everything to the 'sorted' 1D vector and then sort the vector
			for (int buffRow = 0; buffRow < filter; ++buffRow)											// Going through rows of buffer 9shifted the rows at the end)
				for (int buffCol = xIncrement; buffCol < filter + xIncrement; ++buffCol)					// Going through cols of buffer (using the shift from the for loop)
					sorted.push_back(buff[buffRow].at(buffCol));
			sort(sorted.begin(), sorted.end());                                 //sort cevotr of the window
			cout << "          Sorted Vector: " << endl << "           ";
			printV(sorted);

			// A2) add the median values with the k val from left to right
			sum = sum + sorted[medianIndex];
			//cout<<"\n          medianIndex = "<<medianIndex<<endl;
			//cout<<"\n          .....added: "<<sorted[medianIndex]<<"     sum: "<<sum<<endl;
			for (int i = 1; i < k; ++i)
			{
				count += 2;
				sum = sum + sorted[medianIndex - i];
				sum = sum + sorted[medianIndex + i];
				//cout<<"\n          .....added: "<<sorted[medianIndex-i]<<"  and   "<<sorted[medianIndex+i]<<"     sum: "<<sum<<"         -i: "<<medianIndex-i<<"     +i: "<<medianIndex+i<<endl;


			}
			output[yIncrement].push_back(sum / count);
			//cout<<"\n          output["<<yIncrement<<"] pushed: "<<sum/count<<"            sum: "<<sum<<"   count: "<<count<<"    sum/count= "<<sum/count<< endl;

		}
		//B) replace old row in buffer with the next row in opaddedImage
		if (yIncrement < h - 1) {
			cout << "          aaaaaaaaaaaaaaaaaaaaaaaaaa  filter: " << filter << "    yIncrement: " << yIncrement << "........copy from row: " << filter + yIncrement << "  to row: " << yIncrement << endl;;
			copyVectorRow(paddedImage, buff, filter + yIncrement, yIncrement);
		}
	}
	cout << endl;
	printV(output);

}




































