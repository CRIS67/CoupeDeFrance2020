//define necessaire
#define VALEUR_TAG_GIROUETTE 17
#define ERROR 424242

//Include des biblio pour l'objet vision
#include <opencv2/aruco.hpp>
#include <opencv2/videoio.hpp>

typedef enum Orientation Orientation;

enum Orientation
{
    NORD, SUD, ERROR
};

class Vision
{
    public:
        void init_vision();
		Orientation récuperer_sens_girouette();
		
		
    protected:
		cv::VideoCapture inputVideo;
		cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_250);
		cv::Mat cameraMatrix = ;
		cv::Mat distCoeffs = ;
		std::vector<cv::Vec3d> rvecs, tvecs;
    private:
};
