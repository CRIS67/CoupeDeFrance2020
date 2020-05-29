//define necessaire
#define VALEUR_TAG_GIROUETTE 17
#define ERR 424242

//Include des biblio pour l'objet vision
#include <raspicam/raspicam_cv.h>
#include <opencv2/aruco.hpp>
#include <string>
#include <opencv2/opencv.hpp>


enum Orientation
{
    NORD, SUD, ERROR
};

typedef Orientation Orientation;

class Vision
{
    public:
    void init_vision();
		Orientation recuperer_sens_girouette();
		void prendre_photo(std::string name);
    void stop_vision();
		
    protected:
		raspicam::RaspiCam_Cv Camera;
		cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_250);
    float matDist[3][3] = {{11166.9065,0,393.296681},{0, 11166.9065, 712.958149},{0, 0, 1}};
    float Coeff[14][1] = {{39.8681247},{-27052.856},{1.95980792},{-1.17693528},{92079.8228},{-47.3561629},{-13565.8581},{-106854.607},{0},{0},{0},{0},{0},{0}};
		cv::Mat distCoeffs = cv::Mat(14,1,CV_32F,Coeff);
		cv::Mat cameraMatrix = cv::Mat(3,3,CV_32F,matDist);
		
    private:
};
