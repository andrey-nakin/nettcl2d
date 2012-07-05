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
#include "time_tracer.hpp"

namespace tracer {

	class AverageVoltage : public Null, TimeTracer {

		virtual void doBeforeRun(const Network& network, double const startTime, double const endTime, double const dt) {
			close();
			detectPrecision(dt);
			open();
			nextTime = startTime;
		}

		virtual void doAfterRun(const Network& network) {
			close();
		}

		virtual void doAfterIteration(const Network& network, double const time) {
			if (s && params.startTime <= time && nextTime <= time) {
				const Statistics stat = calcStat(network);
				*s	<< std::fixed << std::setprecision(timePrecision) << time
					<< std::scientific << std::setprecision(params.precision)
					<< '\t'  << stat.getMean()
					<< '\n';
				nextTime = time + params.interval;
			}
		}

	public:

		struct Params {
			std::string fileName;
			double interval;
			double startTime;
			unsigned precision;
			std::string tagExpr;

			Params() :
				fileName("s"),
				interval(0.0),
				startTime(0.0),
				precision(6) {}
		};

		AverageVoltage(const Params& params) : params(params), nextTime(0.0) {}

	private:

		Params params;
		double nextTime;
		boost::scoped_ptr<phlib::TraceStream> s;

		void open() {
			if (!params.fileName.empty()) {
				// open a file
				s.reset(new phlib::TraceStream(params.fileName));

				if (s->is_open() && s->isHeaderRequired()) {
					*s << "# time\t<voltage>\n";
				}
				if (!s->is_open()) {
					close();
				} else {
					*s << std::setprecision(params.precision);
				}
			}
		}

		void close() {
			s.reset();
		}

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
