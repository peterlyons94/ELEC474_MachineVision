#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

void part1(String, String, int);
void part2(Mat, String);
void equalize();
void CreateCDF(Mat, int);
//String names[];
Mat src;
Mat src_gray;
int thresh = 100;
int max_thresh = 255;
RNG rng(12345);
double lengths[5];
double areas[5];
int counter = 0;
void contour_method(String, int);
void classify_loop();
void compare(Mat, double[], double[], int);

void part1(String background,String foreground,int num)
{
    VideoCapture video(background);
	if(!video.isOpened())
		throw "Error when reading stream";

	Mat M;
    Mat S;
    Mat frame; // creates frame

    video >> frame; // gets frame from video
    int rows = frame.rows, cols = frame.cols;
    M = Mat::zeros(rows, cols, CV_32F);
    S = M.clone();

    int N = 1; // counter for num frames to sample
    while(N<=50){
        video >> frame;
        cvtColor(frame, frame, CV_BGR2GRAY); // converts to rgb
        cv::accumulate(frame, M);
        N++;
    }
    M = M/N; // gives the mean
    N = 1; // reset counter
    video.set(CV_CAP_PROP_POS_FRAMES, 0);
    while(N<=50){
        video >> frame;
        cvtColor(frame , frame, CV_RGB2GRAY);
        Mat tmp = frame.clone();
        tmp.convertTo(tmp, CV_32F);
        accumulateSquare(tmp - M, S);
        N++;
    }
    S = S / N;
    for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++)
                S.at<float>(i,j) = sqrt(S.at<float>(i,j));
    }

    M.convertTo(M, CV_8UC1);
    S.convertTo(S, CV_8UC1);

    VideoCapture I(foreground);
    if (!I.isOpened())
        return;
    VideoWriter output = VideoWriter("output.wmv", CV_FOURCC('W','M','V','1'), I.get(CV_CAP_PROP_FPS), cvSize(I.get(CV_CAP_PROP_FRAME_WIDTH), I.get(CV_CAP_PROP_FRAME_HEIGHT)), 0);
    if ( !output.isOpened() )
        cout << "Fail" << endl;

    Mat out = frame.clone();
    int everyFew = 0;
    while (I.grab()) {
        I >> frame;
        cvtColor(frame , frame, CV_RGB2GRAY);

        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                if (abs(frame.at<unsigned char>(i,j) - M.at<unsigned char>(i,j)) > (num * S.at<unsigned char>(i,j)))
                    out.at<unsigned char>(i,j) = 255;// foreground pixel
                else
                    out.at<unsigned char>(i,j) = 0;// background pixel
            }
        }

        output.write(out);

        //namedWindow("Output", CV_WINDOW_AUTOSIZE);
       // imshow("Output", out);
        waitKey(3);
    }
    cout << "ended" << endl;
}

/// This method just builds two arrays with the arclength and area of each image
void contour_method(String img, int j)
{
    src = imread( img + ".bmp", 1 );
    cvtColor( src, src_gray, CV_BGR2GRAY );
    vector<vector<Point> > contours;
    findContours( src_gray, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);


    Mat drawing = Mat::zeros( src_gray.size(), CV_8UC3 );
    for( int i = 0; i< contours.size(); i++ )
     {
       Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
       drawContours( drawing, contours, i, color, 2, 8, false, 0, Point() );
     }
    /// Show in a window
    namedWindow( img, CV_WINDOW_AUTOSIZE );
    imshow( img, drawing );

    double length = arcLength(contours[0], 1);
    double area = contourArea(contours[0], 0);

    lengths[j] = length;
    areas[j] = area;
    //cout << "arc length:" << length << " contour area:" << area << endl;
}
/// Loop method
void classify_loop(){

    String names[] = {"nut3", "peg3", "pipe4", "q1", "prong5" };
    for(int i = 0; i < 5; i++){
        contour_method(names[i], i);
    }
}


/// Takes in the video frame, and performs the find Contour method
/// If the contours match within a range, then it stores it as an image.
void compare(Mat out, double lengths[], double areas[], int counter ){
    String name;

/*
    if(out.empty())
        return;
*/
    Mat out_clone = out.clone();

    vector<vector<Point> > contours;
    findContours( out_clone, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

    if(contours.size() >0){
        double length = arcLength(contours[0], 1);
        double area = contourArea(contours[0], 0);

        /*
        Mat drawing = Mat::zeros( out_clone.size(), CV_8UC3 );
        for( int i = 0; i< contours.size(); i++ )
         {
           Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
           drawContours( drawing, contours, i, color, 2, 8, false, 0, Point() );
         }
             /// Show in a window
        namedWindow( "test", CV_WINDOW_AUTOSIZE );
        imshow( "test", drawing );
        */
        cout << "arclength: " << length << " contour area: " << area << endl;

        int flag = 0;

        if(length > lengths[0] - 100 && length < 100 + lengths[0]){
            if(area > areas[0] - 1000 && area < 1000 + areas[0] ){
                name = "nut";
                flag = 1;
            }
        }

        if(length > lengths[1] - 50 && length < 50 + lengths[1]){
            if(area > areas[1] - 600 && area < 600 + areas[1] ){
                name = "peg";
                flag = 1;
            }
        }

        if(length > 70 && length < 90){
            if(area > 200 && area < 240 ){
                name = "pipe";
                flag = 1;
            }
        }

        if(length > lengths[3] - 50 && length < 50 + lengths[3]){
            if(area > areas[3] - 600 && area < 600 + areas[3] ){
                name = "q";
                flag = 1;
            }
        }

        if(length > 320 && length < 370){
            if(area > 2700 && area < 3000 ){
                name = "prong";
                flag = 1;
            }
        }

        //cout << flag << endl;

        if(flag == 1){
            Mat drawing = Mat::zeros( out.size(), CV_8UC3 );
            for( int i = 0; i< contours.size(); i++ )
             {
               Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
               drawContours( drawing, contours, i, color, 2, 8, false, 0, Point() );
             }
            namedWindow( name, CV_WINDOW_AUTOSIZE);
            imshow( name, drawing );
            flag = 0;
        }

    }


}

void play_video(String vid_name)
{
    VideoCapture video(vid_name);
	if(!video.isOpened())
		throw "Error when reading stream";

    Mat out;
    Mat frame; // creates frame
    video >> frame; // gets frame from video
    //int rows = frame.rows, cols = frame.cols;
    int everyFew = 0;

    while (video.grab()) {

        video >> frame;
        cvtColor(frame , frame, CV_RGB2GRAY);
        out = frame.clone();

        if(everyFew % 2 == 0){
            compare(out, lengths, areas, counter);

        }
        everyFew++;
        namedWindow("Output", CV_WINDOW_AUTOSIZE);
        imshow("Output", out);

        waitKey(3);
    }
    cout << "done" << endl;
    waitKey(1);
}

void part2(Mat image, String windownumber)
{
    // initialize parameters
    int histSize = 256; // size of the bin
    float range[] = {0,256};
    const float *ranges[] = {range};

    // calc historgram
    Mat hist;
    calcHist(&image, 1, 0, Mat(), hist, 1, &histSize, ranges, true, false);
    CreateCDF(hist, image.rows * image.cols);

    //Plot the histogram
    int hist_w = 512;
    int hist_h = 400;
    int bin_w = cvRound((double) hist_w/histSize);

    Mat histImage(hist_h, hist_w, CV_8UC1, Scalar(0,0,0));
    normalize(hist,hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());


    for( int i = 1; i < histSize; i++ )
    {
      line( histImage, Point( bin_w*(i-1), hist_h - cvRound(hist.at<float>(i-1)) ) ,
                       Point( bin_w*(i), hist_h - cvRound(hist.at<float>(i)) ),
                       Scalar( 255, 0, 0), 2, 8, 0  );
    }


    namedWindow( windownumber, CV_WINDOW_AUTOSIZE );
    imshow( windownumber, histImage );

}
 void CreateCDF(Mat cdfImg, int n)
{
    // initialize vars
    double cdf[256];
    int h = 200;

    cdf[0] = cdfImg.at<float>(0)/n;

    for(int i = 1; i<256; i++){
        cdf[i] = cdf[i-1]+ (double)(cdfImg.at<float>(i)/n);
    }

    //plot loop
    Mat cdfImage(h, 256, CV_8UC3, Scalar(0,0,0));

    for( int i = 1; i < 256; i++)
    {
        line(cdfImage, Point((i-1), h-cdf[i-1]*h), Point((i), h-cdf[i]*h),
             Scalar(255,0,0), 2, 8,0);
    }

        namedWindow("cdf", CV_WINDOW_AUTOSIZE);

    imshow("cdf", cdfImage);

}

void equalize(String img)
{
    Mat dst;
    Mat toeq;
    Mat gray;

    toeq = cv::imread(img,0);
    gray = cv::imread(img,0);
    namedWindow("Grey", CV_WINDOW_AUTOSIZE);
    imshow("Grey", gray);
    part2(gray, "One");

    equalizeHist(toeq, dst);
    namedWindow("Equalized", CV_WINDOW_AUTOSIZE);
    imshow("Equalized", dst);

    part2(dst, "Two");
}

int main(int, char**){
	cout << "Part 1 or 2?" << endl;
	string input = "";
	getline(cin, input);
	if(input == "1"){
        classify_loop();

        for (int i = 0; i < 5; i++)
            cout << lengths[i] << "     " << areas[i] <<endl;

        play_video("output.wmv");
	    //part1("belt2_bg.wmv", "belt2_fg.wmv", 7);
	} else if(input == "2"){
        cout<< "Input name of file" << endl;
        string poo = "";
        getline(cin, poo);
        equalize(poo);
	} else{
        return 0;
	}
	waitKey(0); // wait for keystroke in the window
	return 0;
}


