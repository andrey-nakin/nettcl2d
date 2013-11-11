/*
 * tracer/phase.hpp --
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

#ifndef TRACER_PHASE_HPP_
#define TRACER_PHASE_HPP_

#include "index_tracer.hpp"

namespace tracer {

	struct PhaseWorker {

		void writeHeader(std::ostream& s, const Network::index_type index) const {
			s << "# time\tphase[" << index << "]\n";
		}

		void trace(std::ostream& s, const Network& network, const Network::index_type index) const {
			s << network.contact(index).phase;
		}

		static const char* fileNameFormat() {
			return "phase.%u";
		}
	};

	class Phase : public IndexTracer<PhaseWorker> {

	public:

		Phase(const Params& params) : IndexTracer<PhaseWorker>(params) {}

	};

}

#endif /* TRACER_PHASE_HPP_ */
