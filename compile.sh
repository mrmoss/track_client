#!/bin/sh
g++ -ggdb main.cpp color_matcher.cpp color_tracker.cpp object_tracker.cpp msl/file_util.cpp msl/string_util.cpp -o track_client -O -lopencv_core -lopencv_highgui -lopencv_features2d -lopencv_ts -lopencv_imgproc -lopencv_video -lm
