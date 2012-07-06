/*
 * tracer/avg_voltage.hpp --
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

#ifndef AVG_VOLTAGE_HPP_
#define AVG_VOLTAGE_HPP_

#include <math.h>
#include <string>
#include <fstream>
#include <iomanip>
#include <boost/scoped_ptr.hpp>
#include <phlib/tracestream.h>
#include "../calc/abstract_tracer.hpp"
#include "../util/statistics.hpp"
#include "file_iteration.hpp"

namespace tracer {

	class AverageVoltage : public FileIteration {

		virtual void doTrace(const Network& network, double const time, std::ostream& s) {
			const Statistics stat = calcStat(network);
			s << stat.getMean();
		}

		virtual void writeHeader(std::ostream& s) {
			s << "# time\t<voltage>\n";
		}

	public:

		struct Params : public IterationParams {
			Params() : IterationParams("s") {}
		};

		AverageVoltage(const Params& params) : FileIteration(new Params(params)) {}

	private:

		Statistics calcStat(const Network& network) {
			Statistics stat;

			for (Network::contact_const_iterator i = network.contactBegin(), last = network.contactEnd(); i != last; ++i) {
				stat.accum(i->voltage);
			}

			return stat;
		}

	};

}

#endif /* AVG_VOLTAGE_HPP_ */
