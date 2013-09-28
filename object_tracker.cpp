#include "object_tracker.hpp"

#include "math.h"

object_tracker::object_tracker(const cv::Vec3b& pink_hsv_color,const cv::Vec3b& lime_hsv_color):
	_x(0.0),_y(0.0),_direction(0.0),_pink(pink_hsv_color),_lime(lime_hsv_color)
{}

cv::Mat object_tracker::update(const cv::Mat& hsv_frame)
{
	//Update Pink Object
	_pink.update(hsv_frame);

	//Update Lime Object
	_lime.update(hsv_frame);

	//Create a Canvas to Draw On
	cv::Mat canvas(cv::Mat::zeros(hsv_frame.size(),CV_8UC3));

	//If There Are Two Object To Track
	if(_pink.area()>_pink.area_min_tolerance()*_pink.area_min_tolerance()&&
		_lime.area()>_lime.area_min_tolerance()*_lime.area_min_tolerance())
	{
		//Determine X Position
		_x=(_pink.x()+_lime.x())/2.0;

		//Determine Y Position
		_y=(_pink.y()+_lime.y())/2.0;

		//Determine Direction Position
		_direction=-atan2(_lime.y()-_pink.y(),_lime.x()-_pink.x());

		if(_direction<0.0)
			_direction+=M_PI*2;

		//Draw Line Between Objects
		cv::line(canvas,cv::Point(_pink.x(),_pink.y()),cv::Point(_lime.x(),_lime.y()),cv::Scalar(255,128,128),5);
	}

	//Create Tracked Object Colors Canvas
	std::vector<cv::Mat> tracked_object_colors_channels;
	tracked_object_colors_channels.push_back(_lime.threshed());
	tracked_object_colors_channels.push_back(_lime.threshed());
	tracked_object_colors_channels.push_back(_pink.threshed());
	cv::Mat tracked_object_colors;
	cv::merge(tracked_object_colors_channels,tracked_object_colors);

	//Add Tracked Object Colors and Lines to Current Frame
	return tracked_object_colors+canvas;
}

float object_tracker::x() const
{
	return _x;
}

float object_tracker::y() const
{
	return _y;
}

float object_tracker::direction() const
{
	return _direction;
}

void object_tracker::set_hsv_tolerances(const int h_tolerance,const int s_tolerance,const int v_tolerance)
{
	_pink.set_hsv_tolerances(h_tolerance,s_tolerance,v_tolerance);
	_lime.set_hsv_tolerances(h_tolerance,s_tolerance,v_tolerance);
}

void object_tracker::set_area_tolerances(const int min,const int box)
{
	_pink.set_area_tolerances(min,box);
	_lime.set_area_tolerances(min,box);
}

void object_tracker::set_pink_hsv_color(const cv::Vec3b& pink_hsv_color)
{
	_pink.set_hsv_color(pink_hsv_color);
}

void object_tracker::set_lime_hsv_color(const cv::Vec3b& lime_hsv_color)
{
	_lime.set_hsv_color(lime_hsv_color);
}