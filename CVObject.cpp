/**********************************************************************
    
  CVObject
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

#include <cmath>
#include <queue>
#include <algorithm>
using namespace std;

void ThresholdBidirection(Mat & img, int lower, int upper){
	if (img.channels() != 1)
		throw exception("图像通道数目不为1");

	for (int h = 0; h < img.rows; h++){
		for (int w = 0; w < img.cols; w++){
			uchar * data = &img.at<uchar>(w, h);
			if (*data <= upper && *data >= lower){
				*data = 255;
			}
			else{
				*data = 0;
			}
		}
	}
}

void FindTargets(const Mat & img, const int erea_threshold, Rect & rect, Mat & filtered){
	if (img.channels() != 1)
		throw exception("FindTargets : 通道数必须为 1");
	vector<vector<Point>> contours;
	findContours(img, contours, CV_RETR_EXTERNAL, CHAIN_APPROX_NONE);
	vector<Point> max_contour;
	double max_area = 0;
	for (auto item : contours){
		auto area = contourArea(item);
		if (area > max_area && area > erea_threshold){
			max_area = area;
			max_contour = item;
		}
	}

	rect = Rect();
	if (max_contour.size() > 0)
		rect = boundingRect(max_contour);

	filtered = Mat::zeros(img.rows, img.cols, CV_8UC3);
	drawContours(filtered, vector<vector<Point>>{max_contour}, -1, Scalar(255, 255, 255), CV_FILLED);
	rectangle(filtered, rect, Scalar(0, 255, 255), 3);

#ifdef DEBUG	
	imshow("contours", filtered);
#endif	
}

bool Locus::InRegionLeftOfTurn(Point p, Size winSize){
	if (p.x < winSize.width / 3){
		return true;
	}
	return false;
}

bool Locus::InRegionRightOfTurn(Point p, Size winSize){
	if (p.x>winSize.width * 2 / 3){
		return true;
	}
	return false;
}

void Locus::addPoint(Point p, Size winSize){
	tracks.push_back(p);
	newPoint = p;
}

Gesture Locus::analyseLocus(){
	Gesture g = NOOPERATION;

	Point first = tracks.front();
	if (InRegionRightOfTurn(first, winSize) && InRegionLeftOfTurn(newPoint, winSize)){
		g = PREVIOUS;
		tracks.clear();
		tracks.push_back(newPoint);
	}
	else if (InRegionLeftOfTurn(first, winSize) && InRegionRightOfTurn(newPoint, winSize)){
		g = NEXT;
		tracks.clear();
		tracks.push_back(newPoint);
	}
	else if (InRegionTop(first, winSize) && InRegionBottom(newPoint, winSize)){
		g = PAUSEPLAY;
		tracks.clear();
		tracks.push_back(newPoint);
	}
	else if (InRegionBottom(first, winSize) && InRegionTop(newPoint, winSize)){
		g = PAUSEPLAY;
		tracks.clear();
		tracks.push_back(newPoint);
	}
	
	return g;
}

bool Locus::InRegionTop(Point p, Size winSize){
	if (p.y < winSize.height  / 3){
		return true;
	}
	return false;
}

bool Locus::InRegionBottom(Point p, Size winSize){
	if (p.y > winSize.height * 2 / 3){
		return true;
	}
	return false;
}
