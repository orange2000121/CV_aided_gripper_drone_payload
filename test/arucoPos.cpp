#include <opencv2/aruco.hpp>
// #include <opencv2/objdetect/aruco_detector.hpp>
#include "opencv2/opencv.hpp"
#include <iostream>

using namespace cv;
using namespace std;

int main(int argc, char **argv)
{
    cv::VideoCapture inputVideo;
    inputVideo.open(0);
    cv::Mat cameraMatrix, distCoeffs;
    // camera parameters are read from somewhere
    cameraMatrix = (Mat_<double>(3,3) << 511.30105788, 0.0         , 330.14455905, 
                                             0.0         , 510.54688952, 247.69595943, 
                                             0.0         , 0.0         , 1.0);
    distCoeffs = (Mat_<double>(1,5) << 0.18435012, -0.57693444, 0.00853163, 0.00982479, 0.46038521);
    cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
    while (inputVideo.grab())
    {
        cv::Mat image, imageCopy;
        inputVideo.retrieve(image);
        image.copyTo(imageCopy);
        std::vector<int> ids;
        std::vector<std::vector<cv::Point2f>> corners;
        cv::aruco::detectMarkers(image, dictionary, corners, ids);
        // if at least one marker detected
        if (ids.size() > 0)
        {
            cv::aruco::drawDetectedMarkers(imageCopy, corners, ids);
            std::vector<cv::Vec3d> rvecs, tvecs;
            cv::aruco::estimatePoseSingleMarkers(corners, 0.05, cameraMatrix, distCoeffs, rvecs, tvecs);
            double x = tvecs[0][0];
            double y = tvecs[0][1];
            double z = tvecs[0][2];
            cout << "x: " << x << " y: " << y << " z: " << z << endl;
            // draw axis for each marker
            for (int i = 0; i < ids.size(); i++)
                cv::aruco::drawAxis(imageCopy, cameraMatrix, distCoeffs, rvecs[i], tvecs[i], 0.1);
        }
        // cv::imshow("out", imageCopy);
        char key = (char)cv::waitKey(1);
        if (key == 27)
            break;
    }
}
