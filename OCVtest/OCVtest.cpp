#include <iostream>
#include<opencv2/opencv.hpp>
using namespace std;
using namespace cv;


//extern "C"
//{
//
//	__declspec(dllexport) void Show_Image(unsigned char*, long);
//	__declspec(dllexport) void detect_text(unsigned char*, long);
//
//}
//void Show_Image(unsigned char* img_pointer, long data_len)
//{
//	vector<unsigned char> inputImageBytes(img_pointer, img_pointer + data_len);
//	cv::Mat img = imdecode(inputImageBytes, IMREAD_COLOR);
//	cv::Mat image;
//	cv::bitwise_not(img, image);
//	cv::imshow("img just recieved from c#", image);
//	//std::vector<Prediction> result = classifier->Classify(img, top_n_results);
//	//...
//	//*length_of_out_result = ss.str().length();
//	
//}
//
//
//void detect_text(unsigned char* argv, long argc)
//{
//
//}
extern "C" {
  _declspec(dllexport) void detect_text(string);
}


int main(int argc, char* argv[]) {
    detect_text(string("C:\\Users\\Bikalpa\\Documents\\testdll\\testimg.png"));
}

void detect_text(string input) 
{
    Mat large = imread(input);

    Mat rgb;
    // downsample and use it for processing
    pyrDown(large, rgb);
    pyrDown(rgb, rgb);
    Mat small;
    cvtColor(rgb, small, COLOR_RGB2GRAY);
    // morphological gradient
    Mat grad;
    Mat morphKernel = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
    morphologyEx(small, grad, MORPH_GRADIENT, morphKernel);
    // binarize
    Mat bw;
    threshold(grad, bw, 0.0, 255.0, THRESH_BINARY | THRESH_OTSU);
    // connect horizontally oriented regions
    Mat connected;
    morphKernel = getStructuringElement(MORPH_RECT, Size(9, 1));
    morphologyEx(bw, connected, MORPH_CLOSE, morphKernel);
    // find contours
    Mat mask = Mat::zeros(bw.size(), CV_8UC1);
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    findContours(connected, contours, hierarchy,RETR_CCOMP, CHAIN_APPROX_SIMPLE, Point(0, 0));
    // filter contours
    for (int idx = 0; idx >= 0; idx = hierarchy[idx][0]) {
        Rect rect = boundingRect(contours[idx]);
        Mat maskROI(mask, rect);
        maskROI = Scalar(0, 0, 0);
        // fill the contour
        drawContours(mask, contours, idx, Scalar(255, 255, 255), FILLED);

        RotatedRect rrect = minAreaRect(contours[idx]);
        double r = (double)countNonZero(maskROI) / (rrect.size.width * rrect.size.height);

        Scalar color;
        int thickness = 1;
        // assume at least 25% of the area is filled if it contains text
        if (r > 0.25 &&
            (rrect.size.height > 8 && rrect.size.width > 8) // constraints on region size
            // these two conditions alone are not very robust. better to use something 
            //like the number of significant peaks in a horizontal projection as a third condition
            ) {
            thickness = 2;
            color = Scalar(0, 255, 0);
        }
        else
        {
            thickness = 1;
            color = Scalar(0, 0, 255);
        }

        Point2f pts[4];
        rrect.points(pts);
        for (int i = 0; i < 4; i++)
        {
            line(rgb, Point((int)pts[i].x, (int)pts[i].y), Point((int)pts[(i + 1) % 4].x, (int)pts[(i + 1) % 4].y), color, thickness);
        }
    }
    

    imwrite("cont.jpg", rgb);
    
    cv::imshow("We got", rgb);

    waitKey(0);
   
}

