/*
 * voltage.hpp
 *
 *  Created on: 22.06.2012
 *      Author: andrey
 */

#ifndef TRACER_VOLTAGE_HPP_
#define TRACER_VOLTAGE_HPP_

#include <set>
#include <boost/scoped_ptr.hpp>
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
				std::for_each(entries.begin(), entries.end(), Tracer(network, time));
				nextTime = time + params.interval;
			}
		}

	public:

		typedef std::size_t index_type;
		typedef std::vector<index_type> IndexVector;

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

		virtual ~Voltage() {
			close();
		}

	private:

		struct Entry {
			index_type index;
			boost::scoped_ptr<phlib::TraceStream> s;
			int timePrecision;
			int precision;

			Entry(const index_type index, const Params& params, const int timePrecision) :
					index(index),
					s(new phlib::TraceStream(params.makeFileName(index))),
					timePrecision(timePrecision),
					precision(params.precision) {

				if (s->is_open() && s->isHeaderRequired()) {
					*s << "# time\tvoltage[" << index << "]\n";
				}
				if (!s->is_open()) {
					close();
				} else {
					s->precision(params.precision);
				}
			}

			void trace(const Network& source, const double time) {
				if (s) {
					*s	<< std::fixed << std::setprecision(timePrecision) << time
						<< std::scientific << std::setprecision(precision)
						<< '\t'  << source.contact(index).voltage
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

			index_type operator()() {
				return index++;
			}

		private:

			index_type index;

		};

		struct EntryCreator {

			EntryCreator(const Params& params, const int timePrecision) :
				params(params), timePrecision(timePrecision) {}

			EntryPointer operator()(const index_type index) const {
				return EntryPointer(new Entry(index, params, timePrecision));
			}

		private:

			const Params& params;
			const int timePrecision;

		};

		struct Tracer {

			Tracer(const Network& source, double const time) :
				source(source), time(time) {}

			void operator()(EntryPointer& e) {
				e->trace(source, time);
			}

		private:

			const Network& source;
			const double time;

		};

		Params params;
		double nextTime;
		EntryVector entries;

		void open(const Network& source) {
			// populate vector of node indices
			IndexVector indices;
			if (params.indices.size() > 0) {
				// indices are explicitly specified
				std::copy(params.indices.begin(), params.indices.end(), std::back_insert_iterator<IndexVector>(indices));
			}
			if (indices.empty()) {
				// trace all network nodes
				std::generate_n(std::back_insert_iterator<IndexVector>(indices), source.getNumOfContacts(), IndexGenerator());
			}

			// create trace streams
			std::transform(
				indices.begin(),
				indices.end(),
				std::back_insert_iterator<EntryVector>(entries),
				EntryCreator(params, timePrecision));
		}

		void close() {
			entries.clear();
		}
	};

}

#endif /* TRACER_VOLTAGE_HPP_ */
