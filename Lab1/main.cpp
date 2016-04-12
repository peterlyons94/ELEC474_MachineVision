#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
using namespace cv;
using namespace std;


void ImageStatistics(){
    Mat image;
    image = cv::imread("lena2.jpg",0);
    namedWindow("Display Window", CV_WINDOW_AUTOSIZE);
    imshow("Display Window", image);
    int min, max;
    float avg;
    int pix_val = 0;
    int pix_num = 0;

    for(int i = 0; i<image.rows; i++){
        for(int j = 0; j<image.cols; j++){
            pix_num++;
            if(image.at<uchar>(i,j) > max){
                max = image.at<uchar>(i,j);
            } else if (image.at<uchar>(i,j) < min){
                min = image.at<uchar>(i,j);
            }
            pix_val += image.at<uchar>(i,j);
            avg = pix_val/pix_num;
        }
    }

    cout << min << endl;
    cout << max << endl;
    cout << avg << endl;
}

void ImageInversion(){
    int val, new_val;
    Mat image;
    image = cv::imread("lena2.jpg",0);
    namedWindow("Display Window", CV_WINDOW_AUTOSIZE);
    imshow("Display Window", image);
    for(int i = 0; i<image.rows; i++){
        for(int j= 0; j<image.cols; j++){
            val = image.at<uchar>(i,j);
            new_val = 255 - val;
            image.at<uchar>(i,j) = new_val;
        }
    }

        imshow("Display Window", image);
}

void ImageFlipVertical(){
    Mat image2;
    Mat image;
    image = cv::imread("lena2.jpg",0);
    image2 = cv::imread("lena2.jpg",0);
    namedWindow("Display Window", CV_WINDOW_AUTOSIZE);
    //imshow("Display Window", image);
    int rows = image.rows;
    int cols = image.cols;

    //Vertical flip (x-axis)
    for(int i = 0; i<image.rows; i++){
        for(int j= 0; j<image.cols; j++){
            image2.at<uchar>(rows - i,j) = image.at<uchar>(i,j);
        }
    }
    imshow("Display Window", image2);
}

void ImageFlipHorizontal(){
    Mat image2;
    Mat image;
    image = cv::imread("lena2.jpg",0);
    image2 = cv::imread("lena2.jpg",0);
    namedWindow("Display Window", CV_WINDOW_AUTOSIZE);
    //imshow("Display Window", image);
    int rows = image.rows;
    int cols = image.cols;

    //Horizontal flip (y-axis)
    for(int i = 0; i<image.rows; i++){
        for(int j= 0; j<image.cols; j++){
            image2.at<uchar>(i, j) = image.at<uchar>(i, rows - j);
        }
    }
    imshow("Display Window", image2);
}

void ImageMerge(){
    Mat image2;
    Mat image;
    Mat image3;
    image = cv::imread("octo1.jpg",0);
    image2 = cv::imread("octo2.jpg",0);
    image3 = cv::imread("octo2.jpg", 0); //will get overwritten

    namedWindow("Display Window 1", CV_WINDOW_AUTOSIZE);
    namedWindow("Display Window 2", CV_WINDOW_AUTOSIZE);
    namedWindow("Display Window 3", CV_WINDOW_AUTOSIZE);

    int a, b;
    //images are same size so can use dimensions of the first image
    for(int i = 0; i<image.rows; i++){
        for(int j= 0; j<image.cols; j++){
            a = image.at<uchar>(i, j);
            b = image2.at<uchar>(i,j);
            if(a > b)   image3.at<uchar>(i,j) = a; // takes the max. image3 started as a clone of "b"
        }
    }
    imshow("Display Window 1", image);
    imshow("Display Window 2", image2);
    imshow("Display Window 3", image3);
}

void ImageDifference(){
    Mat image;
    Mat image2;
    Mat image3;
    image = cv::imread("messy_desk_A-1.jpg", 0);
    image2 = cv::imread("messy_desk_B-1.jpg", 0);
    image3 = cv::imread("messy_desk_B-1.jpg", 0);
    namedWindow("Display Window 1", CV_WINDOW_AUTOSIZE);
    namedWindow("Display Window 2", CV_WINDOW_AUTOSIZE);
    namedWindow("Display Window 3", CV_WINDOW_NORMAL);

    int a, b;
    int thresh = 10; // the threshold value for differencfe
    //images are same size so can use dimensions of the first image
    for(int i = 0; i<image.rows; i++){
        for(int j= 0; j<image.cols; j++){
            a = image.at<uchar>(i, j);
            b = image2.at<uchar>(i,j);
            if((a - b) > thresh)   image3.at<uchar>(i,j) = 255;
            else image3.at<uchar>(i,j) = 0;
        }
    }
    imshow("Display Window 1", image);
    imshow("Display Window 2", image2);
    imshow("Display Window 3", image3);

    /* Potential differences in the image could have occured to post processing of the image, ie. if any aspects were altered. Another differance could be the camera settings used
    to take it. If it was on manual then there may be slight differences in the ISO, exposure, shutter speed, aperture etc.. when taken. If the images were convertd from color to b&w
    then maybe the conversion algorithm returned slightly different results.*/

}
int main(){
    cout<< "Welcome to Lab 1, please select an option" << endl;
    cout<< "1. Image Statistics" << endl;
    cout<< "2. Image Inversion" << endl;
    cout<< "3. Image Flip Horizontal" <<endl;
    cout<< "4. Image Flip Vertical" <<endl;
    cout<< "5. Image Merge" << endl;
    cout<< "6. Image Difference" << endl;
    string input = "";
    getline(cin, input);
    if(input == "1"){
        ImageStatistics();
    } else if(input == "2"){
        ImageInversion();
    } else if (input == "3"){
        ImageFlipHorizontal();
    } else if (input == "4"){
        ImageFlipVertical();
    } else if (input == "5"){
        ImageMerge();
    } else if (input == "6"){
        ImageDifference();
    } else if (input != "1" || "2" || "3" || "4"){
        cout << "please select a number and press enter" << endl;
    }
    waitKey(0);
    return 0;
}


