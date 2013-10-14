//Algorithm Header
#include <algorithm>

//File Utility Header
#include "msl/file_util.hpp"

//IO Stream Header
#include <iostream>

//Object Tracking Header
#include "object_tracker.hpp"

//OpenCV Header
#include <opencv2/opencv.hpp>

//Thread Header
#include <pthread.h>

//Socket Header
#include "msl/socket.hpp"

//String Header
#include <string>

//String Header (memset)
#include <string.h>

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

//Create Tracked Object
object_tracker tracked(pink_color,lime_color);

//Mouse Callback Variables
int mouse_x=0;
int mouse_y=0;
int mouse_flags=0;

//Server Variables
std::string server_ip_address="127.0.0.1:9999";

//Load Configuration Function Declaration
bool load_configuration();

//Save Configuration Function Declaration
void save_configuration();

//Server Thread Function Declaration
void* server_thread_function(void*);

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
	cv::VideoCapture cap(cv::VideoCapture(0));

	//Create Server Thread
	pthread_t server_thread;

	//Camera Opened
	if(cap.isOpened()&&pthread_create(&server_thread,NULL,&server_thread_function,NULL)==0)
	{
		pthread_detach(server_thread);
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

	//Track Objects...For Forever And Ever...
	while(true)
	{
		//Exit Key
		if(cv::waitKey(1)!=-1)
		{
			//Save Current Configuration
			save_configuration();

			//Exit Program
			exit(0);
		}

		//Get Current Frame
		cv::Mat current_frame;
		cap>>current_frame;

		//If The Current Frame Has Data
		if(!current_frame.empty())
		{
			//Set Tolerances For Tracked Object
			tracked.set_hsv_tolerances(h_tolerance,s_tolerance,v_tolerance);
			tracked.set_area_tolerances(area_min_tolerance,area_box_tolerance);

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
					tracked.set_pink_hsv_color(pink_color);
				}

				//Right Mouse Button Sets Lime Object Color
				if(mouse_flags&CV_EVENT_FLAG_RBUTTON)
				{
					lime_color=pixel;
					tracked.set_lime_hsv_color(lime_color);
				}
			}

			//Add Tracked Object Colors and Lines to Current Frame
			current_frame=current_frame*0.5+tracked.update(current_frame_hsv);

			//Debug
			//std::cout<<"x:"<<tracked.x()<<"\ty:"<<tracked.y()<<"\tdirection:"<<tracked.direction()<<std::endl;

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
		std::vector<std::string> values;

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
					values.push_back(temp);

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
				h_tolerance=msl::to_int(values[ii]);
			else if(variables[ii]=="s_tolerance")
				s_tolerance=msl::to_int(values[ii]);
			else if(variables[ii]=="v_tolerance")
				v_tolerance=msl::to_int(values[ii]);

			//Check For Area Tolerances
			else if(variables[ii]=="area_min_tolerance")
				area_min_tolerance=msl::to_int(values[ii]);
			else if(variables[ii]=="area_max_tolerance")
				area_box_tolerance=msl::to_int(values[ii]);

			//Check For Pink Color
			else if(variables[ii]=="pink_color_h")
				pink_color[0]=msl::to_int(values[ii]);
			else if(variables[ii]=="pink_color_s")
				pink_color[1]=msl::to_int(values[ii]);
			else if(variables[ii]=="pink_color_v")
				pink_color[2]=msl::to_int(values[ii]);

			//Check For Lime Color
			else if(variables[ii]=="lime_color_h")
				lime_color[0]=msl::to_int(values[ii]);
			else if(variables[ii]=="lime_color_s")
				lime_color[1]=msl::to_int(values[ii]);
			else if(variables[ii]=="lime_color_v")
				lime_color[2]=msl::to_int(values[ii]);

			//Check For Server IP Address
			else if(variables[ii]=="server_ip_address")
				server_ip_address=values[ii];
		}

		//Update Tracked Object Colors
		tracked.set_pink_hsv_color(pink_color);
		tracked.set_lime_hsv_color(lime_color);

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

	//Add Server IP Address
	configuration_data+="server_ip_address\t"+server_ip_address+"\n";

	//Save Configuration File
	msl::string_to_file(configuration_data,configuration_filename);
}

//Server Thread Function Definition
void* server_thread_function(void*)
{
	//Client Socket
	msl::socket client(server_ip_address);

	//Send Updates...Forever...
	while(true)
	{
		//If Disconnected
		while(!client.good())
		{
			//Wait On Server a Bit...
			usleep(10000);

			//Try to Connect to Server
			client.connect_tcp();
		}

		//Get Current Tracked Object's Position and Direction
		float x=tracked.x();
		float y=tracked.y();
		float direction=tracked.direction();

		//Create Packet to Send
		char packet[17];

		//Package Packet
		memcpy(packet+0,"uaf",4);
		packet[3]=12;
		memcpy(packet+4,&x,4);
		memcpy(packet+8,&y,4);
		memcpy(packet+12,&direction,4);
		packet[16]=0;

		//Calculate Packet CRC
		for(int ii=0;ii<16;++ii)
			packet[16]^=packet[ii];

		//Send Packet
		client.write(packet,17);

		//Give OS a Break...
		usleep(0);
	}
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