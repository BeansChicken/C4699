#pragma once
#include <vector>
#include <thread>

#define WIN4618 //define for running on a windows machine
//#define PI4618 //define for running on a rasbpian machine

#ifdef WIN4618
#include "opencv.hpp"
#endif

#ifdef PI4618
#include <opencv2/opencv.hpp>
#endif

/**
*
* @brief This class contains functions used for tracking color objects
*
* This class contains the update and draw function to determine the location of the colored object
* Require connection w/ servo (brown = GND, red = VCC, orange = data)
*
* @author Ben Yang
*
*/
class Track 
{
private:
	double start_tic;  //start time 
	double stop_tic;  //stop time
	double freq;	//freq of the system
	double elapsed_time; //time passed (fps)

	cv::VideoCapture vid; //creates the vid object for video capture
	cv::Mat canvas; //matrix for storing video capture images for red
	cv::Mat canvasR; //matrix for storing video capture images for red
	cv::Mat canvasG; //matrix for storing video capture images for green
	cv::Mat HSV;	//image after converting to HSV formate

	//cv::Rect r; //box drawn around the ball

	int distance = 0;
	int height = 0;

public:

	/** @brief Constructor
	* @param N/A
	* @return N/A
	*/
	Track();

	/** @brief Destructor
	* @param N/A
	* @return N/A
	*/
	~Track();

	/** @brief Determine the colour and distance seen on the webcam image
	* @param address of red distance (0 = red not seen)
	* @param address of green distance (0 = green not seen)
	* @return N/A
	*/
	int update(void);

	/** @brief Display the processed image
	* @param N/A
	* @return N/A
	*/
	void draw();
};

