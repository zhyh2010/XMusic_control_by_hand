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

#pragma once
#include <cv.h>
#include <vector>
using namespace std;

//constants
const Size winSize(300, 300);

enum Gesture{NOOPERATION,PAUSEPLAY,NEXT,PREVIOUS};

class Locus{
public:
	Locus() :newPoint{-1, -1}{}
	Gesture analyseLocus();
	void addPoint(Point p, Size winSize);
	void reset(){
		tracks.clear();
		newPoint = Point{ -1, -1 };
	}

private:
	static const int frameBuffer = 10;
	static const int volStep = 20;

	Point newPoint;
	vector<Point> tracks;

	bool InRegionLeftOfTurn(Point p, Size winSize);
	bool InRegionRightOfTurn(Point p, Size winSize);
	bool InRegionTop(Point p, Size winSize);
	bool InRegionBottom(Point p, Size winSize);
};

/*!
* \fn Ë«±ß¶þÖµ»¯
*   \brief
*   \param
*   \return
*/
void ThresholdBidirection(Mat & img, int lower, int upper);

void FindTargets(const Mat & img, const int erea_threshold, Rect & rect, Mat & filtered);

