#include "color_tracker.hpp"

color_tracker::color_tracker(const cv::Vec3b& hsv_color):_hsv_color(hsv_color)
{}

void color_tracker::update(const cv::Mat& hsv_frame)
{
	_matcher.match(hsv_frame,_hsv_color);
}

float color_tracker::x() const
{
	return _matcher.y();
}

float color_tracker::y() const
{
	return _matcher.x();
}

float color_tracker::area() const
{
	return _matcher.area();
}

cv::Mat color_tracker::threshed() const
{
	return _matcher.threshed();
}

void color_tracker::set_hsv_color(const cv::Vec3b& hsv_color)
{
	_hsv_color=hsv_color;
}

void color_tracker::set_hsv_tolerances(const int h_tolerance,const int s_tolerance,const int v_tolerance)
{
	_matcher.set_hsv_tolerances(h_tolerance,s_tolerance,v_tolerance);
}

void color_tracker::set_area_tolerances(const int min,const int box)
{
	_matcher.set_area_tolerances(min,box);
}