/*
 * tracer/index_tracer.hpp --
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

#ifndef TRACER_INDEX_TRACER_HPP_
#define TRACER_INDEX_TRACER_HPP_

#include <set>
#include <iterator>
#include <boost/scoped_ptr.hpp>
#include <boost/bind.hpp>
#include "null.hpp"

namespace tracer {

	template <bool useContacts>
	struct IndexBuilder {
		static Network::IndexVector buildIndices(const Network& source, const std::string tagExpr) {
			return source.buildContactIndices(tagExpr);
		}
	};

	template <>
	struct IndexBuilder<false> {
		static Network::IndexVector buildIndices(const Network& source, const std::string tagExpr) {
			return source.buildCircuitIndices(tagExpr);
		}
	};

	template <typename Worker, bool useContacts = true>
	class IndexTracer : public Null, TimeTracer {

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

			typedef std::set<Network::index_type> IndexContainer;

			std::string fileNameFormat;
			double startTime;
			double interval;
			int precision;
			std::string tagExpr;

			Params() :
				fileNameFormat(Worker::fileNameFormat()),
				startTime(0.0),
				interval(0.0),
				precision(6)
			{}

			std::string makeFileName(std::size_t index) const {
				char buf[256];
				sprintf(buf, fileNameFormat.c_str(), index);
				return buf;
			}

		};

		IndexTracer(const Params& params) :
			params(params), nextTime(0.0) {
		}

	private:

		struct Entry {
			Network::index_type index;
			boost::scoped_ptr<phlib::TraceStream> s;
			int timePrecision;
			int precision;
			Worker worker;

			Entry(const Network::index_type index, const Params& params, const int timePrecision) :
					index(index),
					s(new phlib::TraceStream(params.makeFileName(index))),
					timePrecision(timePrecision),
					precision(params.precision) {

				if (s->is_open() && s->isHeaderRequired()) {
					worker.writeHeader(*s, index);
				}
				if (!s->is_open()) {
					close();
				}
			}

			void trace(const Network* source, const double time) {
				if (s) {
					*s	<< std::fixed << std::setprecision(timePrecision) << time
						<< std::scientific << std::setprecision(precision)
						<< '\t';
					worker.trace(*s, *source, index);
					*s << '\n';
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
			if (!params.tagExpr.empty()) {
				// tag expression is specified
				indices = IndexBuilder<useContacts>::buildIndices(source, params.tagExpr);
			} else {
				// trace all network nodes
				std::generate_n(std::back_insert_iterator<Network::IndexVector>(indices), source.getNumOfContacts(), IndexGenerator());
			}

			// create trace streams
			std::transform(
				indices.begin(),
				indices.end(),
				std::back_insert_iterator<EntryVector>(entries),
				boost::bind(&IndexTracer::makeEntry, this, _1));
		}

		void close() {
			entries.clear();
		}
	};

}

#endif /* TRACER_INDEX_TRACER_HPP_ */
