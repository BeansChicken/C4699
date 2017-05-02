#include "stdafx.h"
#include "Track.h"

#include <iomanip>
#include <iostream>

#define fspX 400 //x positon of fsp
#define fspY 450 //y positon of fsp
#define fontsize_fps 2 //size of the font for displaying fps 

#define erodeSize cv::Size(3,3) //size of pixel to erode x,y
#define dilateSize cv::Size(15, 15) //size of pixel to deilate x,y

#define Blue cv::Scalar(255, 0, 0) //colour blue in RGB formate -> (Blue, Green, Red)
#define Blue_lower_bound cv::Scalar{ 99, 139, 62 }; //colour blue lower bound in hsv formate
#define Blue_upper_bound cv::Scalar{ 117, 255, 197 }; //colour blue upper bound in hsv formate

#define Yellow cv::Scalar(0, 255, 255) //colour blue in RGB formate -> (Blue, Green, Red)
#define Yellow_lower_bound cv::Scalar{ 0, 103, 216 }; //colour blue lower bound in hsv formate
#define Yellow_upper_bound cv::Scalar{ 35, 162, 255 }; //colour blue upper bound in hsv formate

#define Green cv::Scalar(0, 255, 0) //colour blue in RGB formate -> (Blue, Green, Red)
#define Green_lower_bound cv::Scalar{ 75, 125, 158 }; //colour blue lower bound in hsv formate
#define Green_upper_bound cv::Scalar{ 92, 255, 255 }; //colour blue upper bound in hsv formate

#define White cv::Scalar(255, 255, 255) //colour blue in RGB formate -> (Blue, Green, Red)


using namespace std;

Track::Track()
{
#ifdef WIN4618
	//open external cam
	vid.open(2);
#endif

#ifdef PI4618
	//open external cam
	vid.open(0);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 
#endif 
	//Servo starting poistion
	Control.set_data(Servo, Motor_UpDown, MotorY); 
	Control.set_data(Servo, Motor_LeftRight, MotorX);
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

	vid >> canvas;

	//size of canvas
	Image_sizeX = canvas.cols;
	Image_sizeY = canvas.rows;

	//size of canvas
	BallPositionX = Image_sizeY / 2;
	BallPositionY = Image_sizeX / 2;

	//convert to HSV
	cv::cvtColor(canvas, HSV, cv::COLOR_BGR2HSV);

	//appy mask
	cv::inRange(HSV, Colour_lower_bound, Colour_upper_bound, mask);

	//kernel for erode
	cv::Mat erodeElement = cv::getStructuringElement(cv::MORPH_RECT, erodeSize);
	//kernel for erode
	cv::Mat dilateElement = cv::getStructuringElement(cv::MORPH_RECT, dilateSize);

	//erode then dilate
	cv::erode(mask, erode, erodeElement);
	cv::dilate(erode, dilate, dilateElement);

	//find the shape of the object
	vector<cv::Vec4i> hierarchy;
	vector<vector<cv::Point>> contours;
	findContours(dilate, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

	//draw a box around the shape, find the middle and follow it 
	for (unsigned int i = 0; i < contours.size(); i++)
	{
		r = cv::boundingRect(contours.at(i));
		BallPositionX = r.x + (r.width / 2);
		BallPositionY = r.y + (r.height / 2);
		string Position = to_string(BallPositionX) + ", " + to_string(BallPositionY);
		//display on the ball
		cv::putText(canvas, "+", cv::Point(BallPositionX, BallPositionY), cv::FONT_HERSHEY_COMPLEX_SMALL, fontsize_fps, Colour, 3);
		cv::putText(canvas, Position, cv::Point(BallPositionX + 10, BallPositionY + 20), cv::FONT_HERSHEY_COMPLEX_SMALL, 0.8, Colour, 1);
		cv::rectangle(canvas, r, Colour, 3);
	}

	//position calculation for Y
	//create a y boundary (160 to 320)
	//when ball position y is less than 160 move till it's above it (limit by motor up limit)
	//when ball position y is more than 320 move till it's below it (limit by motor down limit)
		if (BallPositionY < 150)
		{
			for (int i = 0; BallPositionY < 160; i = i + AdjustY)
			{
				if ((MotorY + AdjustY_Motor) < Motor_up_LIMIT)
				{
					MotorY = MotorY + AdjustY_Motor;
					Control.set_data(Servo, Motor_UpDown, MotorY);
				}
				BallPositionY = BallPositionY + i;
			}
		}
		if (BallPositionY > 330)
		{
			for (int i = 0; BallPositionY > 320; i = i + AdjustY)
			{
				if ((MotorY - AdjustY_Motor) > Motor_down_LIMIT)
				{
				MotorY = MotorY - AdjustY_Motor;
				Control.set_data(Servo, Motor_UpDown, MotorY);
				}
				BallPositionY = BallPositionY - i;
			}
		}

		//position calculation for X
		//create a x boundary (213 to 426)
		//when ball position x is less than 213 move till it's above it (limit by motor up limit)
		//when ball position x is more than 426 move till it's below it (limit by motor down limit)
	if (BallPositionX < 213)
	{
		for (int i = 0; BallPositionX < 213; i = i + AdjustX)
		{
			if ((MotorX + AdjustX_Motor) < Motor_left_LIMIT)
			{
				MotorX = MotorX + AdjustX_Motor;
				Control.set_data(Servo, Motor_LeftRight, MotorX);
			}
			BallPositionX = BallPositionX + i;
		}
	}
	if (BallPositionX > 426)
	{
		for (int i = 0; BallPositionX > 426; i = i + AdjustX)
		{
			if ((MotorX - AdjustX_Motor) > Motor_right_LIMIT)
			{
				MotorX = MotorX - AdjustX_Motor;
				Control.set_data(Servo, Motor_LeftRight, MotorX);
			}
			BallPositionX = BallPositionX - i;
		}
	}

	//stop clock
	stop_tic = cv::getTickCount();

	//calculates fps
	elapsed_time = (freq / (stop_tic - start_tic));
}

void Track::draw()
{
	if (flag_firsttime == 1)
	{
		//UI for choosing colour
		cout << "Choose which color to track" << endl;
		cout << setw(20) << "1 = Blue" << setw(20) << "2 = Yellow" << setw(20) << "3 = Green" << endl;
		cout << "Enter a number: ";
		cin >> flag_colour;

		//check if input is within bound
		if ((1 > flag_colour) && (flag_colour > 3))
		{
			cout << "Invalid choice, program will now terminate after a key press." << endl;
			cv::waitKey(0);
		}

		//set colour choices and colour boundaries
		if (flag_colour == 1)
		{
			Colour = Blue;
			Colour_lower_bound = Blue_lower_bound;
			Colour_upper_bound = Blue_upper_bound;
		}
		else if (flag_colour == 2)
		{
			Colour = Yellow;
			Colour_lower_bound = Yellow_lower_bound;
			Colour_upper_bound = Yellow_upper_bound;
		}
		else
		{
			Colour = Green;
			Colour_lower_bound = Green_lower_bound;
			Colour_upper_bound = Green_upper_bound;
		}
		flag_firsttime = 0;
	}
	else
	{
		//display fsp on canvas
		string fsp = to_string(elapsed_time);
		cv::putText(canvas, fsp, cv::Point(fspX, fspY), cv::FONT_HERSHEY_COMPLEX_SMALL, fontsize_fps, White, 1);

		//display center of the canvas
		cv::putText(canvas, "+", cv::Point(Image_sizeX / 2, Image_sizeY / 2), cv::FONT_HERSHEY_COMPLEX_SMALL, fontsize_fps, White, 3);

		if (flag_colour == 1)
		{
			cv::putText(canvas, "Blue", cv::Point(fspX, fspY - 100), cv::FONT_HERSHEY_COMPLEX_SMALL, fontsize_fps, White, 1);
		}
		else if (flag_colour == 2)
		{
			cv::putText(canvas, "Yellow", cv::Point(fspX, fspY - 100), cv::FONT_HERSHEY_COMPLEX_SMALL, fontsize_fps, White, 1);
		}
		else
		{
			string fsp = to_string(elapsed_time);
			cv::putText(canvas, "Green", cv::Point(fspX, fspY - 100), cv::FONT_HERSHEY_COMPLEX_SMALL, fontsize_fps, White, 1);
		}

		//tracking status
		/*
		if ( != 0)
			cv::putText(canvas, "Tracking", cv::Point(fspX, fspY - 50), cv::FONT_HERSHEY_COMPLEX_SMALL, fontsize_fps, White, 1);
		else
			cv::putText(canvas, "Not Found", cv::Point(fspX, fspY - 50), cv::FONT_HERSHEY_COMPLEX_SMALL, fontsize_fps, White, 1);
		*/

		//display canvas
		cv::imshow("Image", canvas);

		//display HSV
		//cv::imshow("HSV", HSV);

		//display mask
		cv::imshow("mask", mask);

		//display erode
		//cv::imshow("erode", erode);

		//display dilate
		//cv::imshow("dilate", dilate);

		cv::waitKey(1);
	}
}

void Track::start()
{
	thread t1(&Track::update_thread, this);
	thread t2(&Track::draw_thread, this);
	t1.join();
	t2.join();
}

void Track::update_thread(Track* ptr)
{
	while (ptr->_thread_exit == false)
	{
		cv::waitKey(100);
		ptr->update();
	}
}

void Track::draw_thread(Track* ptr)
{
	while (ptr->_thread_exit == false)
	{
		cv::waitKey(100);
		ptr->draw();
	}
}