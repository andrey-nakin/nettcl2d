/*
 * helper.hpp
 *
 *  Created on: 22.06.2012
 *      Author: andrey
 */

#ifndef HELPER_HPP_
#define HELPER_HPP_

#include "../calc/abstract_perturbator.hpp"

namespace perturbator {

	class Helper : public AbstractPerturbator {

		virtual void doBeforeRun(Network& network, double const startTime, double const endTime, double const dt) {

		}

		virtual void doAfterRun(Network& network) {

		}

		virtual void doSetZ(Network& network, Network::index_type index, double const time) {

		}

	protected:

		void updateZ(Network& network, Network::index_type index, double const z) {
			network.contact(index).z = z;
		}

	};
}

#endif /* HELPER_HPP_ */
