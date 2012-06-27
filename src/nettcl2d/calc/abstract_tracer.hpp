/*
 * calc/abstract_tracer.hpp --
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

#ifndef TRACER_ABSTRACT_TRACER_HPP_
#define TRACER_ABSTRACT_TRACER_HPP_

#include <phlib/polymorphic.hpp>
#include "../calc/network.hpp"

class AbstractTracer : public phlib::Polymorphic {

	virtual void doBeforeRun(const Network& network, double const startTime, double const endTime, double const dt) = 0;
	virtual void doAfterRun(const Network& network) = 0;
	virtual void doAfterIteration(const Network& network, double const time) = 0;

public:

	void beforeRun(const Network& network, double const startTime, double const endTime, double const dt) {
		doBeforeRun(network, startTime, endTime, dt);
	}

	void afterRun(const Network& network) {
		doAfterRun(network);
	}

	void afterIteration(const Network& network, double const time) {
		doAfterIteration(network, time);
	}

};

#endif /* TRACER_ABSTRACT_TRACER_HPP_ */
