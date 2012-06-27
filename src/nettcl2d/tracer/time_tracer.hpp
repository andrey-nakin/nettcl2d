/*
 * tracer/time_tracer.hpp --
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

#ifndef TIME_TRACER_HPP_
#define TIME_TRACER_HPP_

namespace tracer {

	class TimeTracer {
	protected:

		int timePrecision;

		TimeTracer() : timePrecision(2) {}
		TimeTracer(const TimeTracer& src) : timePrecision(src.timePrecision) {}

		TimeTracer& operator=(const TimeTracer& src) {
			this->timePrecision = src.timePrecision;
			return *this;
		}

		void detectPrecision(double dt) {
			const double l = log10(dt);
			int result = 0;

			if (l < 0.0) {
				result = static_cast<int>(floor(-l + 0.95));
			}

			timePrecision = result;
		}

	};

}

#endif /* TIME_TRACER_HPP_ */
