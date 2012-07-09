/*
 * tracer/voltage.hpp --
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

#ifndef TRACER_VOLTAGE_HPP_
#define TRACER_VOLTAGE_HPP_

#include <set>
#include <iterator>
#include <boost/scoped_ptr.hpp>
#include <boost/bind.hpp>
#include "null.hpp"

namespace tracer {

	class Voltage : public Null, TimeTracer {

		virtual void doBeforeRun(const Network& network, double const startTime, double const endTime, double const dt) {
			close();
			detectPrecision(dt);
			open(network);
			nextTime = startTime;
		}

		virtual void doAfterRun(const Network& network) {
			close();
		}

		virtual void doAfterIteration(const Network& network, double const time) {
			if (params.startTime <= time && nextTime <= time) {
				std::for_each(entries.begin(), entries.end(), boost::bind(&Entry::trace, _1, &network, time));
				nextTime = time + params.interval;
			}
		}

	public:

		struct Params {

			std::string fileNameFormat;
			double startTime;
			double interval;
			int precision;
			std::set<Network::index_type> indices;

			Params() :
				fileNameFormat("u.%u"),
				startTime(0.0),
				interval(0.0),
				precision(6)
			{}

			std::string makeFileName(const std::size_t index) const {
				char buf[256];
				sprintf(buf, fileNameFormat.c_str(), index);
				return buf;
			}

		};

		Voltage(const Params params) :
			params(params), nextTime(0.0) {
		}

	private:

		struct Entry {
			Network::index_type index;
			boost::scoped_ptr<phlib::TraceStream> s;
			int timePrecision;
			int precision;

			Entry(const Network::index_type index, const Params& params, const int timePrecision) :
					index(index),
					s(new phlib::TraceStream(params.makeFileName(index))),
					timePrecision(timePrecision),
					precision(params.precision) {

				if (s->is_open() && s->isHeaderRequired()) {
					*s << "# time\tvoltage[" << index << "]\n";
				}
				if (!s->is_open()) {
					close();
				}
			}

			void trace(const Network* source, const double time) {
				if (s) {
					*s	<< std::fixed << std::setprecision(timePrecision) << time
						<< std::scientific << std::setprecision(precision)
						<< '\t'  << source->contact(index).voltage
						<< '\n';
				}
			}

		private:
			Entry(const Entry& src);
			Entry& operator=(const Entry& src);

			void close() {
				s.reset();
			}

		};

		typedef boost::shared_ptr<Entry> EntryPointer;
		typedef std::vector<EntryPointer> EntryVector;

		struct IndexGenerator {

			IndexGenerator() : index(0) {}

			Network::index_type operator()() {
				return index++;
			}

		private:

			Network::index_type index;

		};

		Params params;
		double nextTime;
		EntryVector entries;

		EntryPointer makeEntry(const Network::index_type index) const {
			return EntryPointer(new Entry(index, params, timePrecision));
		}

		void open(const Network& source) {
			// populate vector of node indices
			Network::IndexVector indices;
			if (params.indices.size() > 0) {
				// indices are explicitly specified
				std::copy(params.indices.begin(), params.indices.end(), std::back_insert_iterator<Network::IndexVector>(indices));
			}
			if (indices.empty()) {
				// trace all network nodes
				std::generate_n(std::back_insert_iterator<Network::IndexVector>(indices), source.getNumOfContacts(), IndexGenerator());
			}

			// create trace streams
			std::transform(
				indices.begin(),
				indices.end(),
				std::back_insert_iterator<EntryVector>(entries),
				boost::bind(&Voltage::makeEntry, this, _1));
		}

		void close() {
			entries.clear();
		}
	};

}

#endif /* TRACER_VOLTAGE_HPP_ */
