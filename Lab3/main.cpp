#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace std;
using namespace cv;

void thresh_callback(int, void* );
void load_image(String);

Mat image; Mat src_gray;

int thresh = 110;
int max_thresh = 255;
RNG rng(12345);

void load_image(String img)
{
    image = imread(img, 0);

    blur( image, src_gray, Size(3,3) );

    namedWindow( "Image", CV_WINDOW_AUTOSIZE );
    imshow( "Image", image );

    createTrackbar( " Canny thresh:", "Source", &thresh, max_thresh, thresh_callback );
    thresh_callback( 0, 0 );

}


void thresh_callback(int, void* )
{
  Mat canny_output;

  Canny( src_gray, canny_output, thresh, thresh*2, 3 );

  namedWindow( "Contours", CV_WINDOW_AUTOSIZE );
  imshow( "Contours", canny_output );
}


int main( int argc, char** argv )
{
    load_image("highway.jpg");

    waitKey(0);
    return(0);
}



