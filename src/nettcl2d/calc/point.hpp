/*
 * point.h
 *
 *  Created on: 18.06.2012
 *      Author: andrey
 */

#ifndef POINT_H_
#define POINT_H_

class Point {

public:

	double x, y;

	Point() : x(0.0), y(0.0) {}
	Point(const double x, const double y) : x(x), y(y) {}
	Point(const Point& src) : x(src.x), y(src.y) {}

	Point& operator=(const Point& src) {
		this->x = src.x;
		this->y = src.y;
		return *this;
	}

};

#endif /* POINT_H_ */
