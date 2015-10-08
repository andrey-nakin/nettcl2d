/*
 * tracer/avg_flux.hpp --
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

#ifndef TRACER_AVG_FLUX_HPP_
#define TRACER_AVG_FLUX_HPP_

#include "circuit_tracer.hpp"

namespace tracer {

	class AverageFlux : public CircuitTracer {

		virtual void doTrace(const Network& network, double const time, std::ostream& s) {
			const Statistics stat = calcStat(network);
			s << stat.getMean();
		}

		virtual void writeHeader(std::ostream& s) {
			s << "# time\t<flux>\n";
		}

	public:

		struct Params : public CircuitTracerParams {
			Params() : CircuitTracerParams("avg-flux") {}
		};

		AverageFlux(const Params& params) : CircuitTracer(new Params(params)) {}

	private:

		Statistics calcStat(const Network& network) {
			Statistics stat;

			for (Network::IndexVector::const_iterator i = indices.begin(), last = indices.end(); i != last; ++i) {
				stat.accum(network.flux(*i));
			}

			return stat;
		}

	};

}

#endif /* TRACER_AVG_FLUX_HPP_ */
