#ifndef OPENCV_COLOR_TRACKER
#define OPENCV_COLOR_TRACKER

#include "color_matcher.hpp"

#include <opencv2/opencv.hpp>

class color_tracker
{
	public:
		color_tracker(const cv::Vec3b& hsv_color);

		void update(const cv::Mat& hsv_frame);

		float x() const;
		float y() const;
		float area() const;
		int h_tolerance() const;
		int s_tolerance() const;
		int v_tolerance() const;
		int area_min_tolerance() const;
		int area_box_tolerance() const;

		cv::Mat threshed() const;

		void set_hsv_color(const cv::Vec3b& hsv_color);

		void set_hsv_tolerances(const int h_tolerance,const int s_tolerance,const int v_tolerance);
		void set_area_tolerances(const int min,const int box);

	private:
		cv::Vec3b _hsv_color;
		color_matcher _matcher;
};

#endif