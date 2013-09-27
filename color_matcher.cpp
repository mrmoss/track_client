#include "color_matcher.hpp"

color_matcher::color_matcher():_x(0.0),_y(0.0),_area(0.0),_h_tolerance(10),_s_tolerance(100),_v_tolerance(100),
	_area_min_tolerance(10),_area_box_tolerance(100)
{}

void color_matcher::calculate_moments(const cv::Mat& img)
{
	cv::Moments mu=cv::moments(img);
	_area=mu.m00;
	_x=mu.m10/_area;
	_y=mu.m01/_area;
	_area/=255;
}

void color_matcher::match(const cv::Mat& hsv_img,const cv::Vec3b& target)
{
	_threshed=cv::Mat(hsv_img.size(),1);

	cv::inRange(hsv_img,
		cv::Scalar(target[0]-_h_tolerance,target[1]-_s_tolerance,target[2]-_v_tolerance),
		cv::Scalar(target[0]+_h_tolerance,target[1]+_s_tolerance,target[2]+_v_tolerance),
		_threshed);

	calculate_moments(_threshed);

	if(_area>_area_min_tolerance*_area_min_tolerance)
	{
		cv::Rect roi(-_area_box_tolerance,-_area_box_tolerance,2*_area_box_tolerance,2*_area_box_tolerance);
		roi+=cv::Point(_x,_y);
		roi&=cv::Rect(cv::Point(0,0),_threshed.size());
		calculate_moments(_threshed(roi));
		_x+=roi.x;
		_y+=roi.y;
	}
}

float color_matcher::x() const
{
	return _x;
}

float color_matcher::y() const
{
	return _y;
}

float color_matcher::area() const
{
	return _area;
}

cv::Mat color_matcher::threshed() const
{
	return _threshed;
}

void color_matcher::set_hsv_tolerances(const int h_tolerance,const int s_tolerance,const int v_tolerance)
{
	_h_tolerance=h_tolerance;
	_s_tolerance=s_tolerance;
	_v_tolerance=v_tolerance;
}

void color_matcher::set_area_tolerances(const int min,const int box)
{
	_area_min_tolerance=min;
	_area_box_tolerance=box;
}