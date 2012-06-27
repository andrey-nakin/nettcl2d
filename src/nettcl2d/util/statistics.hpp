/*
 * statistics.hpp
 *
 *  Created on: 21.06.2012
 *      Author: andrey
 */

#ifndef STATISTICS_HPP_
#define STATISTICS_HPP_

#include <math.h>

class Statistics {

	double sum, sum2, mn, mx;
	unsigned n;
	bool minMaxInitialized;

public:

	Statistics() :
		sum(0.0), sum2(0.0), mn(0.0), mx(0.0),
		n(0),
		minMaxInitialized(false) {}

	void accum(double const v) {
		sum += v;
		sum2 += v * v;
		++n;

		if (minMaxInitialized) {
			mn = std::min(mn, v);
			mx = std::max(mx, v);
		} else {
			mn = mx = v;
			minMaxInitialized = true;
		}
	}

	double getMean() const {
		return n > 0 ? sum / n : 0.0;
	}

	double getStd() const {
		return n > 0 ? ::sqrt((sum2 - sum * sum / n) / n) : 0.0;
	}

	double getMin() const {
		return mn;
	}

	double getMax() const {
		return mx;
	}

	unsigned getCount() {
		return n;
	}
};

#endif /* STATISTICS_HPP_ */
