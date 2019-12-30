#include "vision.hpp"

void Vision::init_vision(){
	//On allume la caméra
	//set camera params
    Camera.set( CV_CAP_PROP_FORMAT, CV_16U );
    //Open camera
    std::cout<<"Opening Camera..."<<std::endl;
    if (!Camera.open()) {std::cerr<<"Error opening the camera"<<std::endl;return;}
}

void Vision::stop_vision(){
	std::cout<<"Stop camera..."<<std::endl;
    Camera.release();
}

void Vision::prendre_photo(std::string nom){
	//On allume la caméra
	cv::Mat image;
	Camera.grab();
   	Camera.retrieve(image);
   	imwrite(nom + ".jpg", image);
	//enregistrement de l'image dans un réperoire quelconque
}

Orientation Vision::recuperer_sens_girouette(){
	int num_girouette; //id dans la liste des tags détécté du tag arruco de la girouette
	float angle = ERR; 
	//Récupération de l'image
	std::cout<<"1"<<std::endl;
	cv::Mat image;
	Camera.grab();
    Camera.retrieve(image);
    std::cout<<"2"<<std::endl;
    params->cornerRefinementMethod = cv::aruco::CORNER_REFINE_NONE;
    std::vector<int> ids;
    std::vector<std::vector<cv::Point2f>> corners;
    std::cout<<"3"<<std::endl;
    std::cout<<ids.size()<<std::endl;
    std::cout<<(corners.size())<<std::endl;
    cv::aruco::detectMarkers(image, dictionary, corners, ids, params);
	std::cout<<"4"<<std::endl;

    imwrite("retour_camera.jpg", image);
    // if at least one marker detected
    if (ids.size() > 0){
    	 std::cout<<"J'en ai trouvé !"<<std::endl;
        //On récupère le tag correspondant à la girouette
		for(int i = 0; i < ids.size(); i++){
			if(ids[i] == VALEUR_TAG_GIROUETTE){
				num_girouette = i;
				break;
			}
		}
	    	//On récupère l'orientation du tag dans l'espace
		//cv::aruco::estimatePoseSingleMarkers(corners[num_girouette], 0.05, cameraMatrix, distCoeffs, rvecs, tvecs);
		//On récupère à partir de rvec la rotation du tag de la girouette
		std::cout<<"5"<<std::endl;
		//angle = sqrt(rvecs[0][0]*rvecs[0][0] + rvecs[0][1]*rvecs[0][1] + rvecs[0][2]*rvecs[0][2]);
    }
 //On traite l'angle obtenu afin d'en sortir l'orientation de la girouette
	if(angle == ERR){
		return ERROR;
	}else if(sin(angle) <= 0){ //cas ou l'on a une rotation tel que le panneau est tête en bas
		return SUD;
	}else{
		return NORD;
	}
}
