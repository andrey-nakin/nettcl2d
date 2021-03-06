/*
 * perturbator/static.hpp --
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

#ifndef PERTURBATOR_STATIC_HPP_
#define PERTURBATOR_STATIC_HPP_

#include "../calc/abstract_rng.hpp"
#include "helper.hpp"

namespace perturbator {

	class Static : public Helper {

		Static(const Static& src) : params(src.params) {}

		virtual phlib::Cloneable* doClone() const {
			return new Static(*this);
		}

		virtual void doBeforeRun(Network& network, double const startTime, double const endTime, double const dt) {
			Network::IndexVector indices = network.buildContactIndices(params.tagExpr);
			const std::size_t numOfContacts = indices.size();

			if (0 == numOfContacts) {
				return;
			}

			std::vector<double> zs(numOfContacts);
			double sum = 0.0;
			for (Network::index_type i = 0, last = numOfContacts; i < last; ++i) {
				sum += (zs[i] = params.rng());
			}
			sum /= numOfContacts;
			sum -= params.average;

			std::vector<double>::const_iterator z= zs.begin();
			for (
					Network::IndexVector::const_iterator i = indices.begin(), last = indices.end();
					i != last;
					++i, ++z) {

				network.contact(*i).z = *z - sum;
			}
		}

	public:

		struct Params {
			double average;
			AbstractRng& rng;
			std::string tagExpr;

			Params(double const average, AbstractRng& rng) :
				average(average), rng(rng) {}
		};

		Static(const Params& params) : params(params) {}

	private:

		const Params params;

	};

}

#endif /* PERTURBATOR_STATIC_HPP_ */
