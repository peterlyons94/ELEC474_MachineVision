#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/ml/ml.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/legacy/legacy.hpp>
#include <opencv2/contrib/contrib.hpp>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <stdlib.h>
#include <string.h>
#include <sstream>
#include <time.h>

using namespace std;
using namespace cv;

/// Function Declarations
Mat detectFace(Mat, CascadeClassifier);
void makeSet(String);
void saveSet();
void mergeSet(String, String);
void lab5(String);

/// Global Variables
Mat samples;
vector<String> labels;

/// Functions from Lab 5 Sample Code
void addToDataSet(Mat &data, vector<string> &labels, Mat &newData, vector<string> &newLabels);
Mat norm_0_255(InputArray _src);
string recognizeFace(Mat query, Mat samples, vector<string> labels);

int main()
{
    while(true)
    {
        cout << "" << endl;
        cout << "1. Generate Data Set" << endl;
        cout << "2. Merge Data Sets" << endl;
        cout << "3. Visualization" << endl;

        string input = "";
        getline(cin, input);

        if(input == "1")
        {
            vector<string> fnames;
            fnames.push_back("img1.jpg");
            fnames.push_back("img2.jpg");
            fnames.push_back("img3.jpg");
            fnames.push_back("img4.jpg");
            fnames.push_back("img5.jpg");
            fnames.push_back("img6.jpg");

            for(unsigned int i = 0; i < fnames.size() ; i++)
            {
                makeSet(fnames[i]);
                waitKey();
            }
            cout << "Size of matrix: " << samples.rows << "     x: " << samples.cols << endl;
            saveSet();
            waitKey(0);
        }
        else if (input == "2")
        {
            cout << "Please enter name for file 1:" << endl;
            getline(cin, input);
            String f1 = input;

            cout << "Please enter name for file 2:" << endl;
            getline(cin, input);
            String f2 = input;

            mergeSet(f1, f2);
        }
        else if (input == "3")
        {
            cout << "Which file would you like to visualize?" << endl;
            getline(cin, input);
            String f1 = input + ".xml";

            lab5(f1);
            waitKey();
        }
        else
        {
            break;
        }
        waitKey(0);
    }
    return 0;
}

/// Lab 5
void lab5(String xmlFile)
{
    // Combine more sets
    String fname;
    Mat lab_sample;
    vector<String> lab_label;

    //Perform PCA
    FileStorage xmlData;
    xmlData.open(xmlFile, FileStorage::READ);
    xmlData["samples"] >> lab_sample;
    xmlData["labels"] >> lab_label;

    //PCA pca;
    PCA pca(lab_sample, noArray(), CV_PCA_DATA_AS_ROW, 0);
    xmlData.release();

    // Visualize Mean
    InputArray meanFace = pca.mean;

    // normalize and reshape mean
    Mat final;
    final = norm_0_255(meanFace);
    final = final.reshape(1, 200);

    imshow("meanFace", final);
    waitKey(0);

    // Visualize Eigenfaces
    for(int i = 0; i < pca.eigenvectors.rows; i++)
    {
        Mat eigenface;
        eigenface = pca.eigenvectors.row(i).clone();
        eigenface = norm_0_255(eigenface);
        eigenface = eigenface.reshape(1, 200);
        applyColorMap(eigenface, eigenface, COLORMAP_JET);
        imshow(format("eigenface_%d", i), eigenface);

        waitKey();
    }

    // Project all samples into the Eigenspace
    Mat projection = pca.project(lab_sample);

	// ID Faces
    cout << "Which image would you like to compare?" << endl;
    string input = "";
    getline(cin, input);
    String f1 = input + ".jpg";

    cout << "What is the persons name?" << endl;
    input = "";
    getline(cin, input);
    String person = input;


    // Make a set for the query images, run it through the makeSet and detectFaces functions
    makeSet(f1);

    string label;

    for (unsigned int k = 0; k < lab_label.size(); k++)
    {
        label = recognizeFace(samples, lab_sample, lab_label);
    }
    cout << "The closest match we found for " + person + " was " + label << endl;

    samples = Mat();
    labels.clear();
}

/// Create Face Dataset
void makeSet(String fname)
{
    String cascade_name = "lbpcascade_frontalface.xml";
    CascadeClassifier faceDetector;
    faceDetector.load(cascade_name);

    if( !faceDetector.load( cascade_name ) ){ printf("--(!)Error loading\n"); };

    Mat image = imread(fname, 0);
    imshow(fname, image);
    Mat face;

    vector<Rect> faces;
    faceDetector.detectMultiScale(image, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30,30));

    if(faces.size() == 0)
    {
        cerr << "error, no faces found" << endl;
        //return Mat();
    }

    if(faces.size() > 1)
    {
        cerr << "error, multiple faces found" << endl;
        //return Mat();
    }

    Mat detected = image.clone();

    for(unsigned int i = 0; i < faces.size(); i++)
    {
        rectangle(detected, faces[i].tl(), faces[i].br(), Scalar(255,0,0));
    }
    face = image(faces[0]).clone();
    resize(face, face, Size(200,200));

    imshow(fname + " face", face);
    samples.push_back(face.clone().reshape(1,1));
    //labels.push_back(fname);
}

/// Save to file
void saveSet()
{
    FileStorage fs;

    // Enter Label Names
    labels.push_back("Bill Gates");
    labels.push_back("Kim Jong Un");
    labels.push_back("Steve Jobs");
    labels.push_back("George Bush");
    labels.push_back("Angela Merkel");
    labels.push_back("Doug Lyons");

    fs.open("10042024.xml", FileStorage::WRITE);
    fs << "samples" << samples;
    fs << "labels" << labels;
    fs.release();

    // clear global variables
    samples = Mat();
    labels.clear();

    cout<< "Successfully saved file" << endl;
}

/// Gathering Face Data
Mat detectFace(Mat image, CascadeClassifier faceDetector)
{
    vector<Rect> faces;
    faceDetector.detectMultiScale(image, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30,30));

    if(faces.size() == 0)
    {
        cerr << "error, no faces found" << endl;
        return Mat();
    }

    if(faces.size() > 1)
    {
        cerr << "error, multiple faces found" << endl;
        return Mat();
    }

    Mat detected = image.clone();
    for(unsigned int i = 0; i < faces.size(); i++)
    {
        rectangle(detected, faces[i].tl(), faces[i].br(), Scalar(255,0,0));
    }
    return image(faces[0]).clone();
}

/// Merge Data Set
void mergeSet(String f1, String f2)
{
    // Create variables
    FileStorage fs1, fs2, combined;
    Mat samples1, samples2, samplesCombo;
    vector<String> label1, label2, labelCombo;

    // open and read files to be merged
    fs1.open(f1 + ".xml", FileStorage::READ);
    fs1["samples"] >> samples1;
    fs1["labels"] >> label1;
    fs1.release();

    fs2.open(f2 + ".xml", FileStorage::READ);
    fs2["samples"] >> samples2;
    fs2["labels"] >> label2;
    fs2.release();

    // iterate through fs1 and fs2 and push back onto new combined set
    samplesCombo.push_back(samples1);
    for(unsigned int i = 0; i < label1.size(); i++)
    {
        labelCombo.push_back(label1[i]);
    }

    samplesCombo.push_back(samples2);
    for(unsigned int j = 0; j < label2.size(); j++)
    {
        labelCombo.push_back(label2[j]);
    }

    // Finally add them to the combined file
    combined.open(f1 + "_" + f2 + ".xml", FileStorage::WRITE);
    combined << "samples" << samplesCombo;
    combined << "labels" << labelCombo;
    combined.release();
}

/// Normalize Image

Mat norm_0_255(InputArray _src)
{
    Mat src = _src.getMat();
    double min, max;
    minMaxLoc(src, &min, &max);

    // Create and return normalized image:
    Mat dst;
    switch(src.channels()) {
    case 1:
        normalize(_src, dst, 0, 255, NORM_MINMAX, CV_8UC1);
        break;
    case 3:
        normalize(_src, dst, 0, 255, NORM_MINMAX, CV_8UC3);
        break;
    default:
        src.copyTo(dst);
        break;
    }
    return dst;
}

/// Compare Faces

string recognizeFace(Mat query, Mat samples, vector<string> lab_labels)
{
    float Max = 1000000;
    float near;
    int index;

    for (int i = 0; i < samples.rows; i++)
    {
            near = norm(query, samples.row(i), NORM_L2);
            if(near < Max)
            {
                Max = near;
                index = i;
        }
    }
    Mat closest = samples.row(index);
    closest = closest.reshape(1, 200);
    imshow("Closest Match", closest);

    return lab_labels[index];
}
