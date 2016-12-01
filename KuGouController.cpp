/**********************************************************************
    
  KuGouController
  Release Date: 2010/12/31
  Copyright (C) 2010	Zhijie Lee
                        email: onezeros.lee@gmail.com 
                        web: http://blog.csdn.net/onezeros

  modified by zhyh2010 in 2016-12-01
						web: http://blog.csdn.net/onezeros

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
  
**********************************************************************/

#include "stdafx.h"
#include "CVObject.h"
#include <exception>

void sendHotKey(BYTE key){
	keybd_event(VK_CONTROL, 0, 0, 0);
	keybd_event(VK_MENU, 0, 0, 0);
	keybd_event(key, 0, 0, 0);
	keybd_event(key, 0, KEYEVENTF_KEYUP, 0);
	keybd_event(VK_MENU, 0, KEYEVENTF_KEYUP, 0);
	keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);
}

void postCommand(Gesture g){
	switch (g){
		case NOOPERATION:
			return;
		case PREVIOUS:
			sendHotKey(VK_RIGHT);	
			cout << "previous" << endl;
			break;
		case NEXT:
			sendHotKey(VK_LEFT);					
			cout << "next" << endl;
			break;
		case PAUSEPLAY:
			sendHotKey('X');
			cout << "pause/play" << endl;
			break;
		default:
			return;
	}
}

bool PreSet(int & threshold_lower, int & threshold_higher){
	namedWindow("skin", CV_WINDOW_NORMAL);
	namedWindow("video", CV_WINDOW_NORMAL);

	//create trackbar for skin window
	createTrackbar("Lower Threshold", "skin", &threshold_lower, 255, NULL);
	createTrackbar("Higher Threshold2", "skin", &threshold_higher, 255, NULL);

	moveWindow("skin", 0, 320);
	moveWindow("video", 0, 0);

	return true;
}

VideoCapture openCamera(int cameraIndex){
	VideoCapture cap(cameraIndex);
	if (!cap.isOpened())
		throw exception("初始化相机失败");

	return cap;
}

Mat getFrame(VideoCapture & cap){
	Mat img;
	bool ret = cap.read(img);
	if (!ret)
		throw exception("读取相机图像失败");

	return img;
}

void FramePreHandle(Mat ImgFrame, int & threshold_lower, int & threshold_higher, Mat & cb, Mat & ImgFrameSmall){
	Mat ImgSkin;
	vector<Mat> channels;
	resize(ImgFrame, ImgFrameSmall, winSize);
	cvtColor(ImgFrameSmall, ImgSkin, CV_BGR2YCrCb);
	split(ImgSkin, channels);
	ThresholdBidirection(channels[2], threshold_lower, threshold_higher);

	Mat element = getStructuringElement(MORPH_RECT, Size(5, 5));
	morphologyEx(channels[2], channels[2], MORPH_OPEN, element);
	cb = channels[2];

#ifdef DEBUG
	imshow("cb", cb);
#endif
}

bool SetControlUI(const Size & winSize, Mat & ImgSkin){
	ImgSkin = Mat::zeros(winSize.height, winSize.width, CV_8UC3);

	rectangle(ImgSkin, Point(0, 0), Point(winSize.width / 6, winSize.height), Scalar(0, 255, 255), CV_FILLED);
	rectangle(ImgSkin, Point(winSize.width * 5 / 6, 0), Point(winSize.width, winSize.height), Scalar(0, 255, 255), CV_FILLED);

	return true;
}

int main(int argc, char**argv){
	try{
		int cameraIndex = 0;
		if (argc == 2){
			cameraIndex = argv[1][0] - '0';
		}

		int threshold_lower = 0, threshold_higher = 122;
		PreSet(threshold_lower, threshold_higher);
		VideoCapture cap = openCamera(cameraIndex);
		Sleep(1000);
		Mat ImgFrame;

		Locus locusAnalyser;

		while (true){
			ImgFrame = getFrame(cap);
			if (ImgFrame.empty())
				break;			

			Mat cb, ImgSkin, ImgFrameSmall, ImgSkin_s;
			FramePreHandle(ImgFrame, threshold_lower, threshold_higher, cb, ImgFrameSmall);
			SetControlUI(winSize, ImgSkin);
			Rect target_rect;
			FindTargets(cb, 1000, target_rect, ImgSkin_s);			// findContours 会改变原图

			if (target_rect.width > 0){
				rectangle(ImgFrameSmall, target_rect, Scalar(255, 0, 0), 3);
				Point center(target_rect.x + target_rect.width / 2, target_rect.y + target_rect.height / 2);
				ImgSkin += ImgSkin_s;

				//draw cross at target center
				line(ImgSkin, Point(center.x - 5, center.y - 5), Point(center.x + 5, center.y + 5), Scalar(0, 0, 255), 3);
				line(ImgSkin, Point(center.x + 5, center.y - 5), Point(center.x - 5, center.y + 5), Scalar(0, 0, 255), 3);

				// locus
				locusAnalyser.addPoint(center, winSize);
				Gesture g = locusAnalyser.analyseLocus();

				// post gesture
				postCommand(g);
			}
			else{
				locusAnalyser.reset();
			}
			

			imshow("skin", ImgSkin);
			imshow("video", ImgFrameSmall);

			if (waitKey(3) == 27)
				break;
		}

		return 0;
	}
	catch (exception & e){
		cerr << e.what() << endl;
		return -1;
	}	
}

