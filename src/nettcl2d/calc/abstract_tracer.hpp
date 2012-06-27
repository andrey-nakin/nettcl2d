/*
 * abstract_tracer.hpp
 *
 *  Created on: 21.06.2012
 *      Author: andrey
 */

#ifndef TRACER_ABSTRACT_TRACER_HPP_
#define TRACER_ABSTRACT_TRACER_HPP_

#include <phlib/cloneable.hpp>
#include "../calc/network.hpp"

class AbstractTracer : public phlib::Cloneable {

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
