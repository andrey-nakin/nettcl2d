/*
 * tracer/flux.hpp --
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

#ifndef TRACER_FLUX_HPP_
#define TRACER_FLUX_HPP_

#include "index_tracer.hpp"

namespace tracer {

	struct FluxWorker {

		void writeHeader(std::ostream& s, const Network::index_type index) const {
			s << "# time\tflux[" << index << "]\n";
		}

		void trace(std::ostream& s, const Network& network, const Network::index_type index) const {
			s << network.flux(index);
		}

		static const char* fileNameFormat() {
			return "flux.%u";
		}
	};

	class Flux : public IndexTracer<FluxWorker> {
	public:

		Flux(const Params& params) : IndexTracer<FluxWorker>(params) {}

	};

}

#endif /* TRACER_FLUX_HPP_ */
