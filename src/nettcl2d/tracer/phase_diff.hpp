/*
 * tracer/phase_diff.hpp --
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

#ifndef PHASE_DIFF_TRACER_HPP_
#define PHASE_DIFF_TRACER_HPP_

#include <utility>
#include <vector>

namespace tracer {

	class PhaseDifference : public FileIteration {

		typedef FileIteration Base;
		typedef std::pair<Network::index_type, Network::index_type> IndexPair;
		typedef std::vector<IndexPair> IndexPairVector;

		virtual void doBeforeRun(const Network& network, double const startTime, double const endTime, double const dt) {
			buildIndices(network);
			Base::doBeforeRun(network, startTime, endTime, dt);
		}

		virtual void doTrace(const Network& network, double const time, std::ostream& s) {
			bool first = true;
			for (IndexPairVector::const_iterator i = indices.begin(), last = indices.end(); i != last; ++i) {
				s << network.contact(i->first).phase - network.contact(i->second).phase;
				if (first) {
					first = false;
				} else {
					s << '\t';
				}
			}
		}

		virtual void writeHeader(std::ostream& s) {
			s << "# time";
			for (IndexPairVector::const_iterator i = indices.begin(), last = indices.end(); i != last; ++i) {
				s << "\tph" << i->first << "-ph" << i->second;
			}
			s << '\n';
		}

	public:

		struct Params : public IterationParams {
			std::string tagExpr1, tagExpr2;

			Params() : IterationParams("pd") {}
		};

		PhaseDifference(const Params& params) :	Base(new Params(params)) {}

	private:

		IndexPairVector indices;

		void buildIndices(const Network& network) {
			const Network::IndexVector& first = network.buildContactIndices(dynamic_cast<const Params*>(params.get())->tagExpr1);
			const Network::IndexVector& second = network.buildContactIndices(dynamic_cast<const Params*>(params.get())->tagExpr2);

			indices.clear();

			for (Network::IndexVector::const_iterator f = first.begin(), fe = first.end(); f != fe; ++f) {
				for (Network::IndexVector::const_iterator s = second.begin(), se = second.end(); s != se; ++s) {
					indices.push_back(IndexPair(*f, *s));
				}
			}
		}

	};

}

#endif /* PHASE_DIFF_TRACER_HPP_ */
