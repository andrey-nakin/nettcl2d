/*
 * tracer/circuit_tracer.hpp --
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

#ifndef TRACER_CIRCUIT_TRACER_HPP_
#define TRACER_CIRCUIT_TRACER_HPP_

#include "file_iteration.hpp"

namespace tracer {

	class CircuitTracer : public FileIteration {

		typedef FileIteration Base;

	protected:

		struct CircuitTracerParams : public IterationParams {
			std::string tagExpr;

			CircuitTracerParams(const char* fileName) :
				IterationParams(fileName)
			{}
		};

		CircuitTracer(const CircuitTracerParams* p) : FileIteration(p) {}

		virtual void doBeforeRun(const Network& network, double const startTime, double const endTime, double const dt) {
			Base::doBeforeRun(network, startTime, endTime, dt);
			indices = network.buildCircuitIndices(dynamic_cast<const CircuitTracerParams*>(params.get())->tagExpr);
		}

		Network::IndexVector indices;

	};

}

#endif /* TRACER_CIRCUIT_TRACER_HPP_ */
