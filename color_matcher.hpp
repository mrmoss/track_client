#ifndef OPENCV_COLOR_MATCHER
#define OPENCV_COLOR_MATCHER

#include <opencv2/opencv.hpp>

class color_matcher
{
	public:
		color_matcher();

		void calculate_moments(const cv::Mat& img);
		void match(const cv::Mat& hsv_img,const cv::Vec3b& target);

		float x() const;
		float y() const;
		float area() const;
		cv::Mat threshed() const;

		void set_hsv_tolerances(const int h_tolerance,const int s_tolerance,const int v_tolerance);
		void set_area_tolerances(const int min,const int box);

	private:
		cv::Mat _threshed;
		float _x;
		float _y;
		float _area;
		int _h_tolerance;
		int _s_tolerance;
		int _v_tolerance;
		int _area_min_tolerance;
		int _area_box_tolerance;
};

#endif