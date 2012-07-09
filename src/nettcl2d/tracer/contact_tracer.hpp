/*
 * tracer/contact_tracer.hpp --
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

#ifndef TRACER_CONTACT_TRACER_HPP_
#define TRACER_CONTACT_TRACER_HPP_

#include "file_iteration.hpp"

namespace tracer {

	class ContactTracer : public FileIteration {

		typedef FileIteration Base;

		virtual void doBeforeRun(const Network& network, double const startTime, double const endTime, double const dt) {
			Base::doBeforeRun(network, startTime, endTime, dt);
			indices = network.buildIndices(dynamic_cast<const ContactTracerParams*>(params.get())->tagExpr);
		}

	protected:

		struct ContactTracerParams : public IterationParams {
			std::string tagExpr;

			ContactTracerParams(const char* fileName) :
				IterationParams(fileName)
			{}
		};

		ContactTracer(const ContactTracerParams* p) : FileIteration(p) {}

		Network::IndexVector indices;

	};

}

#endif /* TRACER_CONTACT_TRACER_HPP_ */
