#include <opencv2/core/core.hpp>
#include <opencv2/contrib/contrib.hpp>
#include <opencv2/ml/ml.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>
#include <string>
#include <vector>

using namespace cv;
using namespace std;

void addToDataSet(Mat &data, vector<string> &labels, Mat &newData, vector<string> &newLabels);
Mat norm_0_255(Mat src);
string recognizeFace(Mat query, Mat samples, vector<string> labels);

int main()
{    
	// Load your data and combine it with the data set of several of your peers using:
	// addToDataSet
	
	// Perform PCA
	// cv::PCA pca(....);
	
	// Visualize Mean
    Mat meanFace = pca.mean;
    // normalize and reshape mean
    imshow("meanFace", meanFace);
    waitKey();
    
    // Visualize Eigenfaces
    for(unsigned int i = 0; i < pca.eigenvectors.rows; i++)
    {
        Mat eigenface;
        eigenface = pca.eigenvectors.row(i).clone();
        // normalize and reshape eigenface
        //applyColorMap(eigenface, eigenface, COLORMAP_JET);
        
        imshow(format("eigenface_%d", i), eigenface);
        waitKey();
    }
    
    // Project all samples into the Eigenspace
    // code..
    
	// ID Faces
	// code..
	   
    return 0;
}

void addToDataSet(Mat &samples, vector<string> &labels, Mat &newSamples, vector<string> &newLabels)
{
    // your code from the pre lab
}

Mat norm_0_255(Mat src) 
{
    // Create and return normalized image
    // should work for 1 and 3 channel images
}

string recognizeFace(Mat query, Mat samples, vector<string> labels)
{
    // given a query sample find the training sample it is closest to and return the proper label
    // implement a nearest neighbor algorithm to achieve this
}


