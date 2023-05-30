#include "opencv2/aruco.hpp"
#include "opencv2/opencv.hpp"
#include <iostream>

using namespace cv;
using namespace std;

class BottomCamera
{
public:
    cv::VideoCapture inputVideo;
    cv::Mat image;
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
    std::vector<float> getLocationFromCplusplus()
    {
        this->inputVideo.retrieve(image);
        std::vector<int> ids;
        std::vector<std::vector<cv::Point2f>> corners;
        cv::aruco::detectMarkers(image, this->dictionary, corners, ids);
        // If at least one marker detected
        std::vector<float> location ;
        if (ids.size() > 0)
        {
            std::vector<cv::Vec3d> rvecs, tvecs;
            cv::aruco::estimatePoseSingleMarkers(corners, 0.181, cameraMatrix, distCoeffs, rvecs, tvecs);
            float x = tvecs.at(0)[0];
            float y = tvecs.at(0)[1];
            float z = tvecs.at(0)[2];
            location = {x, y, z};
            for (int i = 0; i < ids.size(); i++)
                cv::aruco::drawAxis(image, cameraMatrix, distCoeffs, rvecs[i], tvecs[i], 0.1);
        }
        else
        {
            location = {0, 0, 0};
        }
        return location;
    }

private:
    cv::Mat cameraMatrix, distCoeffs;
    float markerLength = 0.05;
    cv::aruco::DetectorParameters detectorParams = cv::aruco::DetectorParameters();
    cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
};

int main()
{
    BottomCamera bottomCamera;
    std::vector<float> location;
    while (1)
    {
        location = bottomCamera.getLocationFromCplusplus();
        cv::imshow("image", bottomCamera.image);
        cv::waitKey(1);
        std::cout << "x: " << location[0] << " y: " << location[1] << " z: " << location[2] << std::endl;
    }
    return 0;
}