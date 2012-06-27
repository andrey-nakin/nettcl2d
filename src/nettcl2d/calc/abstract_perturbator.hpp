/*
 * abstract_perturbator.hpp
 *
 *  Created on: 22.06.2012
 *      Author: andrey
 */

#ifndef CALC_ABSTRACT_PERTURBATOR_HPP_
#define CALC_ABSTRACT_PERTURBATOR_HPP_

#include <phlib/cloneable.hpp>

class AbstractPerturbator : public phlib::Cloneable {

	virtual void doBeforeRun(Network& network, double const startTime, double const endTime, double const dt) = 0;

	virtual void doAfterRun(Network& network) = 0;

	virtual void doSetZ(Network& network, Network::index_type index, double const time) = 0;

public:

	void beforeRun(Network& network, double const startTime, double const endTime, double const dt) {
		doBeforeRun(network, startTime, endTime, dt);
	}

	void afterRun(Network& network) {
		doAfterRun(network);
	}

	void setZ(Network& network, Network::index_type index, double const time) {
		doSetZ(network, index, time);
	}

};

#endif /* CALC_ABSTRACT_PERTURBATOR_HPP_ */
