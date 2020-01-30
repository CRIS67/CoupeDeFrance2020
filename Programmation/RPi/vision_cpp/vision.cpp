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


 	std::cout<<"OpenCV Version used:"<<CV_MAJOR_VERSION<<"."<<CV_MINOR_VERSION<<std::endl;
 
	int num_girouette; //id dans la liste des tags détécté du tag arruco de la girouette
	float angle = 1;
	//Récupération de l'image
	cv::Mat image;
	Camera.grab();
    	Camera.retrieve(image);
     
      imwrite("raw_img_orient.jpg", image);
    	
    	std::vector<int> ids;
    	std::vector<std::vector<cv::Point2f> > corners, rejecteds;
    	cv::Ptr<cv::aruco::DetectorParameters> parameters = cv::aruco::DetectorParameters::create();
    	cv::aruco::detectMarkers(image, dictionary, corners, ids, parameters, rejecteds);
    	
    	//affichage de l'image traité
    	cv::Mat outputImage = image.clone();
	cv::aruco::drawDetectedMarkers(outputImage, corners, ids);
	
	//enregistrement du log
	imwrite("./detected_markers.jpg",outputImage);
 
 
      int size = ids.size();
    	// if at least one marker detected
    	if (size > 0){
    	    //On récupère le tag correspondant à la girouette
          
          for(int i = 0; i < size; i++){
			      if(ids[i] == VALEUR_TAG_GIROUETTE){
      				num_girouette = i;
	      			break;
			      }
		      }
          
	    	//On récupère l'orientation du tag dans l'espace
             std::vector<cv::Vec3d> rvecs, tvecs;             
             std::vector<std::vector<cv::Point2f> > cornered;
             cornered.push_back(corners[num_girouette]);
		  cv::aruco::estimatePoseSingleMarkers(cornered, 0.05, cameraMatrix, distCoeffs, rvecs, tvecs);
		  //On récupère à partir de rvec la rotation du tag de la girouette
		  angle = sqrt(rvecs[0][0]*rvecs[0][0] + rvecs[0][1]*rvecs[0][1] + rvecs[0][2]*rvecs[0][2]);
    	}
 	//On traite l'angle obtenu afin d'en sortir l'orientation de la girouette
	if(sin(angle) >= 0 && size>0){
		  return NORD;
	}else if(sin(angle) <= 0 && size>0){ //cas ou l'on a une rotation tel que le panneau est tête en bas
		return SUD;
	}else{
     return ERROR;
	}
}