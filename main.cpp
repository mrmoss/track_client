//Algorithm Header
#include <algorithm>

//Color Tracking Header
#include "color_tracker.hpp"

//File Utility Header
#include "msl/file_util.hpp"

//IO Stream Header
#include <iostream>

//OpenCV Header
#include <opencv2/opencv.hpp>

//String Header
#include <string>

//String Stream Header
#include <sstream>

//String Utility Header
#include "msl/string_util.hpp"

//Configuration File Contents
std::string configuration_filename="track_client.conf";
std::string configuration_data="";

//Matching Tolerances For Sliders
int h_tolerance=10;
int s_tolerance=100;
int v_tolerance=100;
int area_min_tolerance=10;
int area_box_tolerance=100;

//Tracking Colors
cv::Vec3b pink_color(170,200,200);
cv::Vec3b lime_color(70,200,200);

//Mouse Callback Variables
int mouse_x=0;
int mouse_y=0;
int mouse_flags=0;
const int kb_escape=1048603;

//Load Configuration Function Declaration
bool load_configuration();

//Save Configuration Function Declaration
void save_configuration();

//Mouse Callback Function Declaration
void mouse_callback(int event,int x,int y,int flags,void *param);

//Mouse Callback Clear Function Declaration
void mouse_callback_clear();

//Main
int main()
{
	//Try To Load Configuration File, If Fail Create Default
	if(!load_configuration())
		save_configuration();

	//Get Camera
	cv::VideoCapture cap=cv::VideoCapture(0);

	//Camera Opened
	if(cap.isOpened())
	{
		std::cout<<":)"<<std::endl;
	}

	//Camera Failed to Open
	else
	{
		std::cout<<":("<<std::endl;
		exit(0);
	}

	//Name OpenCV Window
	cv::namedWindow("video",CV_WINDOW_AUTOSIZE);

	//Set Mouse Callback
	cv::setMouseCallback("video",mouse_callback,NULL);

	//Create Tolerance Sliders
	cv::createTrackbar("Hue Tolerance","video",&h_tolerance,180,NULL);
	cv::createTrackbar("Saturation Tolerance","video",&s_tolerance,255,NULL);
	cv::createTrackbar("Value Tolerance","video",&v_tolerance,255,NULL);
	cv::createTrackbar("Area Minimum","video",&area_min_tolerance,200,NULL);

	//Create Color Trackers
	color_tracker pink(pink_color);
	color_tracker lime(lime_color);

	//Track For Forever
	while(true)
	{
		//Exit Key
		if(cv::waitKey(1)==kb_escape)
		{
			save_configuration();
			exit(0);
		}

		//Get Current Frame
		cv::Mat current_frame;
		cap>>current_frame;

		//If The Current Frame Has Data
		if(!current_frame.empty())
		{
			//Get HSV Version of Frame
			cv::Mat current_frame_hsv;
			cv::cvtColor(current_frame,current_frame_hsv,CV_BGR2HSV);

			//Check If Mouse Position Is Within Window Boundry
			if(mouse_x>=0&&mouse_x<current_frame.cols&&mouse_y>=0&&mouse_y<current_frame.rows)
			{
				//Pixel Variable
				cv::Vec3b pixel;

				//Get Color At Mouse Position If A Mouse Button Is Pressed
				if(mouse_flags&CV_EVENT_FLAG_LBUTTON||mouse_flags&CV_EVENT_FLAG_RBUTTON)
					pixel=current_frame_hsv.at<cv::Vec3b>(mouse_y,mouse_x);

				//Left Mouse Button Sets Pink Object Color
				if(mouse_flags&CV_EVENT_FLAG_LBUTTON)
				{
					pink_color=pixel;
					pink.set_hsv_color(pink_color);
				}

				//Right Mouse Button Sets Lime Object Color
				if(mouse_flags&CV_EVENT_FLAG_RBUTTON)
				{
					lime_color=pixel;
					lime.set_hsv_color(lime_color);
				}
			}

			//Set Tolerances For Pink Object
			pink.set_hsv_tolerances(h_tolerance,s_tolerance,v_tolerance);
			pink.set_area_tolerances(area_min_tolerance,area_box_tolerance);

			//Set Tolerances For Lime Object
			lime.set_hsv_tolerances(h_tolerance,s_tolerance,v_tolerance);
			lime.set_area_tolerances(area_min_tolerance,area_box_tolerance);

			//Update Pink Object
			pink.update(current_frame_hsv);

			//Update Lime Object
			lime.update(current_frame_hsv);

			//Create a Canvas to Draw On
			cv::Mat canvas(cv::Mat::zeros(current_frame.size(),CV_8UC3));

			//If There Are Two Object To Track
			if(pink.area()>area_min_tolerance*area_min_tolerance&&lime.area()>area_min_tolerance*area_min_tolerance)
			{
				//DEBUG
				std::cout<<"pink\tx:"<<pink.x()<<"\ty:"<<pink.y()<<"\tarea:"<<pink.area()<<std::endl;
				std::cout<<"lime\tx:"<<lime.x()<<"\ty:"<<lime.y()<<"\tarea:"<<lime.area()<<std::endl;

				//Draw Line Between Objects
				cv::line(canvas,cv::Point(pink.y(),pink.x()),cv::Point(lime.y(),lime.x()),cv::Scalar(255,128,128),5);
			}

			//Create Tracked Object Colors Canvas
			std::vector<cv::Mat> tracked_object_colors_channels;
			tracked_object_colors_channels.push_back(lime.threshed());
			tracked_object_colors_channels.push_back(lime.threshed());
			tracked_object_colors_channels.push_back(pink.threshed());
			cv::Mat tracked_object_colors;
			cv::merge(tracked_object_colors_channels,tracked_object_colors);

			//Add Tracked Object Colors and Lines to Current Frame
			current_frame=current_frame*0.5+tracked_object_colors+canvas;

			//Draw Current Frame
			cv::imshow("video",current_frame);
		}

		//Clear Mouse Button Flags
		mouse_callback_clear();
	}

	//Call Me Plz T_T
	return 0;
}

//Load Configuration Function Definition
bool load_configuration()
{
	//Try to Load File
	if(msl::file_exists(configuration_filename)&&msl::file_to_string(configuration_filename,configuration_data))
	{
		//Create a Parser
		std::istringstream istr(configuration_data,std::ios_base::in);

		//Create Vectors to Hold Parsed Variables and Values
		std::vector<std::string> variables;
		std::vector<int> values;

		//Temporary String Variable
		std::string temp="";

		//Parse Configuration Data
		while(true)
		{
			//Parse Variable
			if(istr>>temp)
			{
				//Add Variable to Vector
				variables.push_back(temp);

				//Parse Value
				if(istr>>temp)
					values.push_back(msl::to_int(temp));

				//Error Parsing Value, Break
				else
					break;
			}

			//Error Parsing Variable, Break
			else
			{
				break;
			}
		}

		//Go Through Variables and Values
		for(unsigned int ii=0;ii<std::min(variables.size(),values.size());++ii)
		{
			//Check For HSV Tolerances
			if(variables[ii]=="h_tolerance")
				h_tolerance=values[ii];
			else if(variables[ii]=="s_tolerance")
				s_tolerance=values[ii];
			else if(variables[ii]=="v_tolerance")
				v_tolerance=values[ii];

			//Check For Area Tolerances
			else if(variables[ii]=="area_min_tolerance")
				area_min_tolerance=values[ii];
			else if(variables[ii]=="area_max_tolerance")
				area_box_tolerance=values[ii];

			//Check For Pink Color
			else if(variables[ii]=="pink_color_h")
				pink_color[0]=values[ii];
			else if(variables[ii]=="pink_color_s")
				pink_color[1]=values[ii];
			else if(variables[ii]=="pink_color_v")
				pink_color[2]=values[ii];

			//Check For Lime Color
			else if(variables[ii]=="lime_color_h")
				lime_color[0]=values[ii];
			else if(variables[ii]=="lime_color_s")
				lime_color[1]=values[ii];
			else if(variables[ii]=="lime_color_v")
				lime_color[2]=values[ii];
		}

		//Load Successful
		return true;
	}

	//Load Failed
	return false;
}

//Save Configuration Function Definition
void save_configuration()
{
	//Clear Configuration Data
	configuration_data="";

	//Add HSV Tolerances
	configuration_data+="h_tolerance\t"+msl::to_string(h_tolerance)+"\n";
	configuration_data+="s_tolerance\t"+msl::to_string(s_tolerance)+"\n";
	configuration_data+="v_tolerance\t"+msl::to_string(v_tolerance)+"\n";

	//Add Area Tolerances
	configuration_data+="area_min_tolerance\t"+msl::to_string(area_min_tolerance)+"\n";
	configuration_data+="area_box_tolerance\t"+msl::to_string(area_box_tolerance)+"\n";

	//Pink Color
	configuration_data+="pink_color_h\t"+msl::to_string(static_cast<int>(pink_color[0]))+"\n";
	configuration_data+="pink_color_s\t"+msl::to_string(static_cast<int>(pink_color[1]))+"\n";
	configuration_data+="pink_color_v\t"+msl::to_string(static_cast<int>(pink_color[2]))+"\n";

	//Add Lime Color
	configuration_data+="lime_color_h\t"+msl::to_string(static_cast<int>(lime_color[0]))+"\n";
	configuration_data+="lime_color_s\t"+msl::to_string(static_cast<int>(lime_color[1]))+"\n";
	configuration_data+="lime_color_v\t"+msl::to_string(static_cast<int>(lime_color[2]))+"\n";

	//Save Configuration File
	msl::string_to_file(configuration_data,configuration_filename);
}

//Mouse Callback Function Definition
void mouse_callback(int event,int x,int y,int flags,void *param)
{
	//Update Mouse Position
	if(event==CV_EVENT_MOUSEMOVE)
	{
		mouse_x=x;
		mouse_y=y;
	}

	//Get Button Flags
	mouse_flags=flags;
}

//Mouse Callback Clear Function Definition
void mouse_callback_clear()
{
	mouse_flags=0;
}