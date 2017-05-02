#include "stdafx.h"
#include "Track.h"

#include <iomanip>
#include <iostream>

#define fspX 400 //x positon of fsp
#define fspY 450 //y positon of fsp
#define fontsize_fps 2 //size of the font for displaying fps 

#define erodeSize cv::Size(3,3) //size of pixel to erode x,y
#define dilateSize cv::Size(15, 15) //size of pixel to deilate x,y

#define erodeElement cv::getStructuringElement(cv::MORPH_RECT, erodeSize)
#define dilateElement cv::getStructuringElement(cv::MORPH_RECT, dilateSize)

#define Red cv::Scalar(0, 0, 255) //colour blue in RGB formate -> (Blue, Green, Red)
#define Red_lower_bound cv::Scalar{ 0, 160, 247 } //colour blue lower bound in hsv formate
#define Red_upper_bound cv::Scalar{ 12, 255, 255 } //colour blue upper bound in hsv formate

#define Green cv::Scalar(0, 255, 0) //colour blue in RGB formate -> (Blue, Green, Red)
#define Green_lower_bound cv::Scalar{ 49, 86, 166 } //colour blue lower bound in hsv formate
#define Green_upper_bound cv::Scalar{ 69, 173, 199 } //colour blue upper bound in hsv formate

#define White cv::Scalar(255, 255, 255) //colour blue in RGB formate -> (Blue, Green, Red)

using namespace std;

Track::Track()
{
#ifdef WIN4618
	//open external cam;
	vid.open(2);
#endif

#ifdef PI4618
	//open external cam
	vid.open(0);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 
#endif 
}


Track::~Track()
{
}

void Track::update()
{
	//get frequency
	freq = cv::getTickFrequency();
	//start clock
	start_tic = cv::getTickCount();

	//store vid image into matrix
	vid >> canvas;

	//convert to HSV
	cv::cvtColor(canvas, HSV, cv::COLOR_BGR2HSV);

	//appy mask
	cv::inRange(HSV, Red_lower_bound, Red_upper_bound, canvasR);
	cv::inRange(HSV, Green_lower_bound, Green_upper_bound, canvasG);

	//kernel for erode
	//cv::Mat erodeElement = cv::getStructuringElement(cv::MORPH_RECT, erodeSize);
	//kernel for erode
	//cv::Mat dilateElement = cv::getStructuringElement(cv::MORPH_RECT, dilateSize);

	//erode then dilate
	cv::erode(canvasR, canvasR, erodeElement);
	cv::erode(canvasG, canvasG, erodeElement);
	cv::dilate(canvasR, canvasR, dilateElement);
	cv::dilate(canvasG, canvasG, dilateElement);

	//find the shape of the object
	vector<cv::Vec4i> hierarchy;
	vector<vector<cv::Point>> contoursR;
	vector<vector<cv::Point>> contoursG;
	findContours(canvasR, contoursR, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
	findContours(canvasG, contoursG, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

	//draw a box around the shape, find the middle and follow it 
	for (unsigned int i = 0; i < contoursR.size(); i++)
	{
		r = cv::boundingRect(contoursR.at(i));

		//display on the ball
		cv::rectangle(canvas, r, Red, 3);
	}

	for (unsigned int i = 0; i < contoursG.size(); i++)
	{
		r = cv::boundingRect(contoursG.at(i));

		//display on the ball
		cv::rectangle(canvas, r, Green, 3);
	}
	

	//stop clock
	stop_tic = cv::getTickCount();

	//calculates fps
	elapsed_time = (freq / (stop_tic - start_tic));
}

void Track::draw()
{
		//display fsp on canvas
		string fsp = to_string(elapsed_time);
		cv::putText(canvas, fsp, cv::Point(fspX, fspY), cv::FONT_HERSHEY_COMPLEX_SMALL, fontsize_fps, White, 1);

		//display canvas
		cv::imshow("Image", canvas);

		cv::waitKey(1);
}

