/*
 * calc/point.hpp --
 *
 * This file is part of nettcl2d application.
 *
 * Copyright (c) 2012 Andrey V. Nakin <andrey.nakin@gmail.com>
 * All rights reserved.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 */

#ifndef CALC_POINT_HPP_
#define CALC_POINT_HPP_

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

#endif /* CALC_POINT_HPP_ */
