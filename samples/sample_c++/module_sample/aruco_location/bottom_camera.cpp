#include "opencv2/aruco.hpp"
#include "opencv2/opencv.hpp"
#include <iostream>

using namespace cv;
using namespace std;

extern "C" class BottomCamera
{
public:
    // constructor
    BottomCamera()
    {
        this->inputVideo.open(0);
        this->cameraMatrix = (Mat_<double>(3, 3) << 511.30105788, 0.0, 330.14455905,
                              0.0, 510.54688952, 247.69595943,
                              0.0, 0.0, 1.0);
        this->distCoeffs = (Mat_<double>(1, 5) << 0.18435012, -0.57693444, 0.00853163, 0.00982479, 0.46038521);
        // Set coordinate system
    }

    // main function
    void main() {}
    void getLocationFromCplusplus(float *location)
    {
        cv::Mat image, imageCopy;
        this->inputVideo.retrieve(image);
        image.copyTo(imageCopy);
        std::vector<int> ids;
        std::vector<std::vector<cv::Point2f>> corners;
        cv::aruco::detectMarkers(image, this->dictionary, corners, ids);
        // If at least one marker detected
        if (ids.size() > 0)
        {
            cv::aruco::drawDetectedMarkers(imageCopy, corners, ids);
            std::vector<cv::Vec3d> rvecs, tvecs;
            cv::aruco::estimatePoseSingleMarkers(corners, 0.181, cameraMatrix, distCoeffs, rvecs, tvecs);
            float x = tvecs.at(0)[0];
            float y = tvecs.at(0)[1];
            float z = tvecs.at(0)[2];
            location[0] = x;
            location[1] = y;
            location[2] = z;
        }
        else
        {
            location[0] = 0;
            location[1] = 0;
            location[2] = 0;
        }
    }

private:
    cv::VideoCapture inputVideo;
    cv::Mat cameraMatrix, distCoeffs;
    float markerLength = 0.05;
    cv::aruco::DetectorParameters detectorParams = cv::aruco::DetectorParameters();
    cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
};