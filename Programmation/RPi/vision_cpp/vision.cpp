include "vision.hpp"

void Vision::init_vision(){
	//On allume la caméra
	inputVideo.open(0);
}

void Vision::prendre_photo(){
	//On allume la caméra
	cv::Mat image;
    	inputVideo.retrieve(image);
	//enregistrement de l'image dans un réperoire quelconque
}

Orientation Vision::récuperer_sens_girouette(){
	int num_girouette; //id dans la liste des tags détécté du tag arruco de la girouette
	float angle = ERR;
	//Récupération de l'image
	cv::Mat image;
    inputVideo.retrieve(image);
    std::vector<int> ids;
    std::vector<std::vector<cv::Point2f> > corners;
    cv::aruco::detectMarkers(image, dictionary, corners, ids);
	
    // if at least one marker detected
    if (ids.size() > 0){
        //On récupère le tag correspondant à la girouette
		for(int i = 0; i < ids.size; i++){
			if(ids[i] == VALEUR_TAG_GIROUETTE){
				num_girouette = i;
				break;
			}
		}
	    	//On récupère l'orientation du tag dans l'espace
		cv::aruco::estimatePoseSingleMarkers(corners[num_girouette], 0.05, cameraMatrix, distCoeffs, rvecs, tvecs);
		//On récupère à partir de rvec la rotation du tag de la girouette
		angle = sqrt(rvecs[0]^2 + rvecs[1]^2 + rvecs[2]^2);
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
