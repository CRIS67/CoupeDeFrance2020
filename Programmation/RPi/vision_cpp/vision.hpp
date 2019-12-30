//define necessaire
#define VALEUR_TAG_GIROUETTE 17
#define ERR 424242

#include <string>
#include <stdio.h>
 #include <ctime>
#include <iostream>

//Include des biblio pour l'objet vision
#include </usr/local/include/opencv2/aruco.hpp>
#include </usr/local/include/opencv2/opencv.hpp>
#include </usr/local/include/opencv2/highgui/highgui.hpp>
#include </usr/local/include/opencv2/videoio.hpp>
#include </usr/local/include/opencv2/imgproc/imgproc.hpp>
#include <raspicam/raspicam_cv.h>
#include <raspicam/raspicam.h>

//using namespace std;

enum Orientation
{
    NORD, SUD, ERROR
};

typedef Orientation Orientation;

class Vision
{
    public:
        void init_vision();
        void prendre_photo(std::string nom);
        void stop_vision();
		Orientation recuperer_sens_girouette();
		
		
    protected:
		raspicam::RaspiCam_Cv Camera;
		cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
		cv::Ptr<cv::aruco::DetectorParameters> params = cv::aruco::DetectorParameters::create();
		cv::Mat cameraMatrix;
		cv::Mat distCoeffs;
		std::vector<cv::Vec3d> rvecs, tvecs;
    private:
};
