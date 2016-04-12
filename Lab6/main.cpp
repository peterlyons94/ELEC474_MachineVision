#include <opencv2/core/core.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/contrib/contrib.hpp>
#include <opencv2/ml/ml.hpp>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <time.h>

using namespace std;
using namespace cv;

void computeFlann();
void loadImages(String, String);
void computeFund(); // takes in the output of the FLANN based Matcher
void display(); // show resulting images
void computeEpiLines(); // takes the results of computeFund
int num_matches = 0;

Mat fMat, qry, img;

static void onMouse(int event, int x, int y, int, void*) {
    if (event != cv::EVENT_LBUTTONDOWN)
        return;

    vector<Point2f> point;
    vector<Vec3f> lines;
    point.push_back(Point2f(x,y));

    computeCorrespondEpilines(Mat(point), 1, fMat, lines);

    line(img, Point(0, -lines[0][2]/lines[0][1]),
            Point(img.cols, -((lines[0][2]+lines[0][0]*img.cols)/lines[0][1])),
            Scalar(255,255,255));
    imshow("Image2", img);
}

int main()
{
    while(true)
    {
        cout << "Which set of images would you like to compare?" << endl;
        cout << "Set 1" << endl;
        cout << "Set 2" << endl;
        string x = "";
        getline(cin, x);
        if(x == "1")
        {
            loadImages("img2.jpg", "img1.jpg");
        } else if ( x== "2")
        {
            loadImages("img3.png", "img4.png");
        }
        waitKey(0);
    }
    return 0;
}

// Loads images and calls the comute FLANN function
void loadImages(String left, String right)
{
    qry = imread(left, 0);
    img = imread(right, 0);

    computeFlann();
}

// find the matches for the two images
void computeFlann()
{
    SiftFeatureDetector detector(400);
    vector<KeyPoint> keypoints1, keypoints2;
    detector.detect(qry, keypoints1);
    detector.detect(img, keypoints2);

    SiftDescriptorExtractor extractor;
    Mat desc1, desc2;
    extractor.compute(qry, keypoints1, desc1);
    extractor.compute(img, keypoints2, desc2);

    BFMatcher matcher(NORM_L2);
    vector<DMatch> matches;
    matcher.match(desc1, desc2, matches);

    vector<Point2f> points1, points2;

    for(int n = 0; n < matches.size(); n++ ) {
        points1.push_back(keypoints1[matches[n].queryIdx ].pt);
        points2.push_back(keypoints2[matches[n].trainIdx ].pt);
    }

    // Calc fund matrix
    fMat = findFundamentalMat(points1, points2, FM_LMEDS);

    cout << "Fundamental Matrix = " << fMat << endl;

    namedWindow("Image1", WINDOW_AUTOSIZE);
    imshow("Image1",qry);

    namedWindow("Image2", WINDOW_AUTOSIZE);
    imshow("Image2",img);

    //setup mouse callback
    setMouseCallback("Image1", onMouse, 0);

}



