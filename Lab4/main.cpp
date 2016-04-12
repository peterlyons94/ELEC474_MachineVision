#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sstream>
#include <time.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/legacy/legacy.hpp>

using namespace std;
using namespace cv;

void imageDisplay(String, String);
void sequentialSearch();
void findBest(Mat, Mat);
void part2(Mat, vector<Mat>, String);
void groupingMethod();
vector<Mat> InitMakeMat();
Mat current_best;
int num_matches = 0;
int inliers = 0;

void imageDisplay(String photo1, String photo2){
    Mat im2 = imread(photo1, 0);
    Mat im1 = imread(photo2, 0);

    SurfFeatureDetector detector(400);
    vector<KeyPoint> keypoints1, keypoints2;
    detector.detect(im1, keypoints1);
    detector.detect(im2, keypoints2);

    SurfDescriptorExtractor extractor;
    Mat desc1, desc2;
    extractor.compute(im1, keypoints1, desc1);
    extractor.compute(im2, keypoints2, desc2);

    FlannBasedMatcher matcher;
    vector<DMatch> matches;
    matcher.match(desc1, desc2, matches);

    vector<Point2f> obj, scene;
    for(int n = 0; n < matches.size(); n++ ) {
        obj.push_back(keypoints1[matches[n].queryIdx ].pt);
        scene.push_back(keypoints2[matches[n].trainIdx ].pt);
    }
    vector<uchar> mask;
    Mat H = findHomography( obj, scene, CV_RANSAC, 3, mask);
    int inliers = 0;
    for( int y = 0; y <= mask.size(); y++){
        inliers += mask[y];
    }
    double inlierRatio = (double)inliers/matches.size();
    cout << "Inliers: " << inliers << ", Outliers: " << matches.size()-inliers << endl;
///
    if ( (double)inliers/matches.size() < 0.5) {
        cout << "No good match found." << endl;
        return;
    }

    vector<Point2f> obj_corners(4);
    obj_corners[0] = cvPoint(0,0); obj_corners[1] = cvPoint( im1.cols, 0 );
    obj_corners[2] = cvPoint( im1.cols, im1.rows ); obj_corners[3] = cvPoint( 0, im1.rows );
    vector<Point2f> scene_corners(4);

    perspectiveTransform( obj_corners, scene_corners, H);

    // display results
    namedWindow("matches " + photo1 + " Matched with: " + photo2, 1);
    Mat img_matches;
    drawMatches(im1, keypoints1, im2, keypoints2, matches, img_matches);
    imshow("matches " + photo1 + " Matched with: " +photo2, img_matches);

    line(img_matches, scene_corners[0] + Point2f(im1.cols, 0), scene_corners[1] + Point2f(im1.cols, 0), Scalar(0, 255, 0), 4 );
    line(img_matches, scene_corners[1] + Point2f(im1.cols, 0), scene_corners[2] + Point2f(im1.cols, 0), Scalar( 0, 255, 0), 4 );
    line(img_matches, scene_corners[2] + Point2f(im1.cols, 0), scene_corners[3] + Point2f(im1.cols, 0), Scalar( 0, 255, 0), 4 );
    line(img_matches, scene_corners[3] + Point2f(im1.cols, 0), scene_corners[0] + Point2f(im1.cols, 0), Scalar( 0, 255, 0), 4 );

    namedWindow("Homography Matched: " + photo2, CV_WINDOW_NORMAL);
    imshow("Homography Matched: " + photo2, img_matches );

    cout<< "Inlier Ratio: " << inlierRatio << endl;
}

// Sequential search

// build a string with the image name and pass the two images to the imageDisplay functin
// return a metric with how close the match is
// at the end of loop, the best match will be found
void sequentialSearch(){
    // creating the 4 query images
    clock_t compTime = clock();
    Mat query[4];

    query[0] = imread("qry_1.jpg", 0);
    query[1] = imread("qry_2.jpg", 0);
    query[2] = imread("qry_3.jpg", 0);
    query[3] = imread("qry_4.jpg", 0);

    // calls the matcher and loads the comparison images
    for(int x = 0; x < 4; x++){
        String e;
        stringstream convert;
        convert << x;
        e = convert.str();
        namedWindow("Query " + e, 1);
        imshow("Query " + e, query[x]);

        num_matches = 0;    // reset number of matches
        Mat target[30];
        for(int y = 0; y < 30; y++){
            String name;
            stringstream convert;
            convert << y;
            name = convert.str();

            target[y] = imread("img_" + name + ".jpg" , 0);
            findBest(query[x], target[y]);

        }
        namedWindow("Best Match " + e, 1);
        imshow("Best Match " + e, current_best);
        cout << "Inliers: " << inliers << endl;

    }
    compTime = clock() - compTime;
    cout << "Time: " << compTime << "ms" << endl;
}

void findBest(Mat qry, Mat img){

    SurfFeatureDetector detector(400);
    vector<KeyPoint> keypoints1, keypoints2;
    detector.detect(qry, keypoints1);
    detector.detect(img, keypoints2);

    SurfDescriptorExtractor extractor;
    Mat desc1, desc2;
    extractor.compute(qry, keypoints1, desc1);
    extractor.compute(img, keypoints2, desc2);

    FlannBasedMatcher matcher;
    vector<DMatch> matches;
    matcher.match(desc1, desc2, matches);

    // store only good matches, and then return image name and number of good matches
    double max_dist = 0;
    double min_dist = 100;

    for( int i = 0; i < desc1.rows; i++ )
    {
        double dist = matches[i].distance;
        if( dist < min_dist ) min_dist = dist;
        if( dist > max_dist ) max_dist = dist;
    }
    vector< DMatch > good_matches;
    for( int i = 0; i < desc1.rows; i++ )
    {
        if( matches[i].distance <= max(2*min_dist, 0.02) )
            { good_matches.push_back( matches[i]); }
    }

    if(good_matches.size() > num_matches)
    {
        num_matches = good_matches.size();
        current_best = img;
        vector<Point2f> obj, scene;
        for(int n = 0; n < matches.size(); n++ ) {
            obj.push_back(keypoints1[matches[n].queryIdx ].pt);
            scene.push_back(keypoints2[matches[n].trainIdx ].pt);
        }
        vector<uchar> mask;
        Mat H = findHomography( obj, scene, CV_RANSAC, 3, mask);

        for( int y = 0; y <= mask.size(); y++){
            inliers += mask[y];
        }
        double inlierRatio = (double)inliers/matches.size();
    }



}

void groupingMethod(){
        // creating the 4 query images
    Mat query[4];
    vector<Mat> trainDesc = InitMakeMat();
    query[0] = imread("qry_1.jpg", 0);
    query[1] = imread("qry_2.jpg", 0);
    query[2] = imread("qry_3.jpg", 0);
    query[3] = imread("qry_4.jpg", 0);

    // calls the matcher and loads the comparison images
    for(int x = 0; x < 4; x++){
        String e;
        stringstream convert;
        convert << (x + 1);
        e = convert.str();
//        namedWindow("Query " + e, 1);
//        imshow("Query " + e, query[x]);

        part2(query[x], trainDesc, e);
    }
}

vector<Mat> InitMakeMat(){
    //
    clock_t compTime = clock();
    vector<KeyPoint> k1;
    Mat descriptors;
    SurfDescriptorExtractor extractor;
    SiftFeatureDetector detector;
    vector<Mat> trainDesc;
    String name;

    // calculate the descriptors and push them back on the the vector
    for(int x = 0; x <= 30; x++){
        stringstream convert;
        convert << x ;
        name = convert.str();
        Mat target = imread("img_" + name + ".jpg" , 0);
        convert.clear();

        detector.detect(target, k1);
        extractor.compute(target, k1, descriptors);

        trainDesc.push_back(descriptors);
        k1.clear();
    }
    compTime = clock() - compTime;
    cout << "Keypoints Compute Time: " << compTime  << "ms" << endl;

    return trainDesc;
}

void part2(Mat qry, vector<Mat> trainDesc, String qname){
    clock_t compTime = clock();
    // detect key points in Query image
    SiftFeatureDetector detector(400);
    vector<KeyPoint> keypoints;
    detector.detect(qry, keypoints);

    // Description extractor on query image and keypoints
    SurfDescriptorExtractor extractor;
    Mat desc1;
    extractor.compute(qry, keypoints, desc1);

    // Create a matcher and add all the points
    BFMatcher matcher(NORM_L2);
    matcher.add(trainDesc);
    matcher.train();

    // add matches to a vectors
    vector<DMatch> matches;
    matcher.match(desc1, matches);

    // store only good matches, and then return image name and number of good matches
    double max_dist = 0;
    double min_dist = 100;

    for( int i = 0; i < desc1.rows; i++ )
    {
        double dist = matches[i].distance;
        cout<< matches[i].imgIdx << endl;
        if( dist < min_dist ) min_dist = dist;
        if( dist > max_dist ) max_dist = dist;
    }
    vector< DMatch > good_matches;

    for( int i = 0; i < desc1.rows; i++ )
    {
        if( matches[i].distance <= max(2*min_dist, 0.02) )
            { good_matches.push_back( matches[i]); }
    }

    // create a string var to contain the index of the best match
    String c;
    stringstream convert;
    convert << good_matches[0].imgIdx;
    c = convert.str();

    // send to prelab function to draw image.
    String im1 = "qry_"+qname+".jpg";
    String im2 = "img_" + c + ".jpg";
    //imageDisplay(im1, im2);

//    // display results
    Mat Matched, Fobject;
    Matched = imread(im2, 0);
    Fobject = imread(im1, 0);
    Size sz1 = Fobject.size();
    Size sz2 = Matched.size();
    Mat com_img(sz2.height, sz1.width+sz2.width, CV_8UC1);
    Mat left(com_img, Rect(0, 0, sz1.width, sz1.height));
    Fobject.copyTo(left);
    Mat right(com_img, Rect(sz1.width, 0, sz2.width, sz2.height));
    Matched.copyTo(right);
    namedWindow("Query: " + qname + " Target Image: " + c ,1);
//    Mat img_matches = imread("img_" + c + ".jpg", 0);
//    //
    imshow("Query: " + qname + " Target Image: " + c, com_img);

    // clear vectors just in case
    good_matches.clear();
    matches.clear();
    keypoints.clear();
    compTime = clock() - compTime;
    // print name of query image and its matched image
    cout<< "qry_"+qname+".jpg" + " Matched with: img_" +c+".jpg" + " Execution Time: " << compTime << "ms"<< endl;
}

int main()
{
    String photo1, photo2;

    while(true){
        cout<< "Which Set of photos would you like to choose?" << endl;
        cout<< "1. Traffic 1" << endl;
        cout<< "2. Desk" << endl;
        cout<< "3. Traffic 2" << endl;
        cout<< "4. Sequential Query" << endl;
        cout<< "5. Descriptor Matcher" << endl;
        string x = "";
        getline(cin, x);
        if(x == "1"){
            photo1 = "traffic-1.bmp";
            photo2 = "traffic-2.bmp";
            imageDisplay(photo1, photo2);
        } else if (x == "3"){
            photo1 = "traffic-3.jpg";
            photo2 = "traffic-2.bmp";
            imageDisplay(photo1, photo2);
        } else if (x == "2"){
            photo2 = "calculator-2.bmp";
            photo1 = "messy_desk_E.jpg";
            imageDisplay(photo1, photo2);
        } else if (x == "4"){
            sequentialSearch();
        } else if (x == "5"){
            groupingMethod();
            cout<< endl;
        } else if (x == "6"){
            photo2 = "qry_1.jpg";
            photo1 = "img_3.jpg";
            imageDisplay(photo1, photo2);
        } else
            break;
        //imageDisplay(photo1, photo2);
        cout << endl;
        waitKey(0);
    }

    return 0;
}

