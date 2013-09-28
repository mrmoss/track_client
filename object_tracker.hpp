#ifndef OPENCV_OBJECT_TRACKER
#define OPENCV_OBJECT_TRACKER

#include "color_tracker.hpp"

#include <opencv2/opencv.hpp>

class object_tracker
{
	public:
		object_tracker(const cv::Vec3b& pink_hsv_color,const cv::Vec3b& lime_hsv_color);

		cv::Mat update(const cv::Mat& hsv_frame);

		float x() const;
		float y() const;
		float direction() const;

		void set_hsv_tolerances(const int h_tolerance,const int s_tolerance,const int v_tolerance);
		void set_area_tolerances(const int min,const int box);

		void set_pink_hsv_color(const cv::Vec3b& pink_hsv_color);
		void set_lime_hsv_color(const cv::Vec3b& lime_hsv_color);

	private:
		float _x;
		float _y;
		float _direction;
		color_tracker _pink;
		color_tracker _lime;
};

#endif