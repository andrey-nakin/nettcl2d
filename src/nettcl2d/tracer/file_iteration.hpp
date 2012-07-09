/*
 * tracer/file_iteration.hpp --
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

#ifndef TRACER_FILE_ITERATION_HPP_
#define TRACER_FILE_ITERATION_HPP_

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

	class FileIteration : public AbstractTracer, TimeTracer {

		virtual void doTrace(const Network& network, double const time, std::ostream&) = 0;
		virtual void writeHeader(std::ostream& s) {}

	protected:

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
			if (s && params->startTime <= time && nextTime <= time) {
				*s	<< std::fixed << std::setprecision(timePrecision) << time
					<< std::scientific << std::setprecision(params->precision)
					<< '\t';
				doTrace(network, time, *s);
				*s << '\n';
				nextTime = time + params->interval;
			}
		}

		struct IterationParams : public phlib::Polymorphic {
			std::string fileName;
			double interval;
			double startTime;
			unsigned precision;

			IterationParams(const char* fileName) :
				fileName(fileName),
				interval(0.0),
				startTime(0.0),
				precision(6) {}

		};

		boost::scoped_ptr<const IterationParams> params;
		double nextTime;
		boost::scoped_ptr<phlib::TraceStream> s;

		FileIteration(const IterationParams* p) : params(p), nextTime(0.0) {}

		void open() {
			if (!params->fileName.empty()) {
				// open a file
				s.reset(new phlib::TraceStream(params->fileName));

				if (s->is_open() && s->isHeaderRequired()) {
					writeHeader(*s);
				}
				if (!s->is_open()) {
					close();
				} else {
					*s << std::setprecision(params->precision);
				}
			}
		}

		void close() {
			s.reset();
		}

	};

}

#endif /* TRACER_FILE_ITERATION_HPP_ */
