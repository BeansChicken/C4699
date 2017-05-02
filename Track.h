#pragma once

#include "Base4618.h"

#include <vector>
#include <thread>

#ifdef WIN4618
#define Motor_down_LIMIT 10
#define Motor_up_LIMIT 95
#define Motor_right_LIMIT 5
#define Motor_left_LIMIT 175
#define Motor_xdir_MIDDLE 90
#define Motor_UpDown 0 //channel for controingl tilt of servo motor on msp430 (p3.5)
#define Motor_LeftRight 1	//channel for contorling pane of servo motor on msp430 (p4.1)
#endif

#ifdef PI4618
#define Motor_down_LIMIT 505
#define Motor_up_LIMIT 1500
#define Motor_right_LIMIT 505
#define Motor_left_LIMIT 2455
#define Motor_xdir_MIDDLE 1500
#define Motor_UpDown 2
#define Motor_LeftRight 3
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
class Track : public CBase4618
{
private:
	int flag_firsttime = 1; //see if program as run before
	int flag_colour = 0; //keep track of which colour was choosen

	cv::Scalar Colour; //chosen colour 
	cv::Scalar Colour_lower_bound; //chosen colour lower bound in hsv
	cv::Scalar Colour_upper_bound; //chosen colour uppper bound in hsv

	double start_tic;  //start time 
	double stop_tic;  //stop time
	double freq;	//freq of the system
	double elapsed_time; //time passed (fps)

	cv::VideoCapture vid; //creates the vid object for video capture
	cv::Mat HSV;	//image after converting to HSV formate
	cv::Mat mask;	//mask apply to the image
	cv::Mat erode;	//image after erode
	cv::Mat dilate; //image after dilate

	int Image_sizeX; //x axis size of image
	int Image_sizeY; //y axis size of image
	cv::Rect r; //box drawn around the ball
	int area = 0; //area of the box

	int BallPositionX; //x coord of the ball
	int BallPositionY; //y coord of the ball
	int MotorX = Motor_xdir_MIDDLE; //servo x direction 
	int MotorY = Motor_down_LIMIT; //servo y direction 
	
	int AdjustX = 20; //step constant in X position 
	int AdjustY = 10; //step constant in y position 

#ifdef WIN4618
	int AdjustX_Motor = 1; //step constant for motor in X position 
	int AdjustY_Motor = 1; //step constant for motor in y position 
#endif

#ifdef PI4618
	int AdjustX_Motor = 10; //step constant for motor in X position 
	int AdjustY_Motor = 10; //step constant for motor in y position 
#endif 

	//multi-threading
	bool _thread_exit = 0; // thread safe?
	static void update_thread(Track* ptr);
	static void draw_thread(Track* ptr);
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

	/** @brief Record & process the webcam image and determine the location of the colored object.
	*			Also, send the servo motor commands to orient the pan/tilt system to center the object in the field of view
	* @param N/A
	* @return N/A
	*/
	void update();

	/** @brief Display the processed image
	* @param matrix of the image wanted to display
	* @return N/A
	*/
	void draw();

	/** @brief Start the colour tracking
	* @param matrix of the image wanted to display
	* @return N/A
	*/
	void start();
};

