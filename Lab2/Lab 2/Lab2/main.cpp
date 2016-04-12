#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace cv;
using namespace std;

Mat blue, red, green;
int thresh_blue = 100;
int thresh_green = 90;
int thresh_red = 110;

int max_thresh = 255;
RNG rng(12345);
void process_image(Mat, String);
void play_video(String);
Mat process_frame(Mat);

void process_image(Mat image, String name){

    vector<Mat> channel(3);
    split(image, channel);
    Mat output = Mat::zeros(image.rows, image.cols, CV_8UC1);

    for(int i = 0; i<image.rows; i++){
        for(int j = 0; j<image.cols; j++){
            if(channel[0].at<uchar>(i,j) < thresh_blue &&
               channel[1].at<uchar>(i,j) < thresh_green &&
               channel[2].at<uchar>(i,j) > thresh_red){
                output.at<uchar>(i,j) = 255;
            }else{
               // output.at<uchar>(i,j) = 0;
            }
        }
    }
    dilate(output, output, Mat(), Point(-1, -1), 6, 2, 2);
    erode(output, output, Mat(), Point(-1, -1), 2, 1, 1);

    for (int i = 0; i < image.rows; i++) {
        for (int j = 0; j < image.cols; j++) {
            if (output.at<unsigned char>(i,j) == 0) {
                channel[0].at<unsigned char>(i,j) = 0;
                channel[1].at<unsigned char>(i,j) = 0;
                channel[2].at<unsigned char>(i,j) = 0;
            }
        }
    }

    Mat channels[] = {channel[0], channel[1], channel[2]};
    merge(channels, 3, image);


    namedWindow(name, CV_WINDOW_AUTOSIZE);
    imshow(name, image);

    namedWindow("Display Window", CV_WINDOW_AUTOSIZE);
    imshow("Display Window", image);

}

Mat process_frame(Mat image){
    vector<Mat> channel(3);
    split(image, channel);
    Mat output = Mat::zeros(image.rows, image.cols, CV_8UC1);
    for(int i = 0; i<image.rows; i++){
        for(int j = 0; j<image.cols; j++){
            if(channel[0].at<uchar>(i,j) < thresh_blue &&
               channel[1].at<uchar>(i,j) < thresh_green &&
               channel[2].at<uchar>(i,j) > thresh_red){
                output.at<uchar>(i,j) = 255;
            }else{
                output.at<uchar>(i,j) = 0;
            }
        }
    }
    dilate(output, output, Mat(), Point(-1, -1), 6, 2, 2);
    erode(output, output, Mat(), Point(-1, -1), 2, 1, 1);

    for (int i = 0; i < image.rows; i++) {
        for (int j = 0; j < image.cols; j++) {
            if (output.at<unsigned char>(i,j) == 0) {
                channel[0].at<unsigned char>(i,j) = 0;
                channel[1].at<unsigned char>(i,j) = 0;
                channel[2].at<unsigned char>(i,j) = 0;
            }
        }
    }

    Mat channels[] = {channel[0], channel[1], channel[2]};
    merge(channels, 3, image);

    return image;
}

void play_video(String vid_name)
{
    VideoCapture video(vid_name);
	if(!video.isOpened())
		throw "Error when reading stream";

    Mat frame; // creates frame
    video >> frame; // gets frame from video
    int rows = frame.rows, cols = frame.cols;

    while (video.grab()) {

        video >> frame;

        Mat output = process_frame(frame);

        namedWindow("Output", CV_WINDOW_AUTOSIZE);
        imshow("Output", output);
        waitKey(3);
    }

}

int main( int argc, char** argv )
{
    Mat image;
    String names[5]={"apple1", "apple4", "apple5", "apple6", "apple8"};

    for(int i=0; i < 5; i++){
        image = cv::imread(names[i] + ".jpg",1);
        process_image(image, names[i]);
    }

    //play_video("apple_garden.wmv");
    waitKey(0);
    return(0);
}


