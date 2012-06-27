/*
 * null.hpp
 *
 *  Created on: 21.06.2012
 *      Author: andrey
 */

#ifndef TRACER_NULL_HPP_
#define TRACER_NULL_HPP_

#include "../calc/abstract_tracer.hpp"

namespace tracer {

	class Null : public AbstractTracer {

		Null(const Null& src) {}

		virtual phlib::Cloneable* doClone() const {
			return new Null(*this);
		}

		virtual void doBeforeRun(const Network& network, double const startTime, double const endTime, double const dt) {}
		virtual void doAfterRun(const Network& network) {}
		virtual void doAfterIteration(const Network& network, double const time) {}

	public:

		Null() {}

	private:

	};

}

#endif /* TRACER_NULL_HPP_ */
