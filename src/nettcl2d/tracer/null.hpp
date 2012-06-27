/*
 * tracer/null.hpp --
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
