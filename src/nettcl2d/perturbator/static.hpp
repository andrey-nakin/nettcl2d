/*
 * static.hpp
 *
 *  Created on: 22.06.2012
 *      Author: andrey
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
			std::size_t numOfContacts = 0;
			for (Network::contact_const_iterator i = network.contactBegin(), last = network.contactEnd();
					i != last; ++i) {
				if (tagMatches(params.tag, i->tag)) {
					++numOfContacts;
				}
			}

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
					Network::contact_iterator i = network.contactBegin(), last = network.contactEnd();
					i != last;
					++i) {
				if (tagMatches(params.tag, i->tag)) {
					i->z = *z - sum;
					++z;
				}
			}
		}

	public:

		struct Params {
			double average;
			AbstractRng& rng;
			int tag;

			Params(double const average, AbstractRng& rng, int tag) :
				average(average), rng(rng), tag(tag) {}
		};

		Static(const Params& params) : params(params) {}

	private:

		const Params params;

		static bool tagMatches(const int tag1, const int tag2) {
			return
					tag1 == tag2
					|| tag1 == 0
					|| tag2 == 0;
		}
	};

}

#endif /* PERTURBATOR_STATIC_HPP_ */
