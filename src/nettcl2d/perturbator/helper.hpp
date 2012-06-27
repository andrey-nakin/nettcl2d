/*
 * perturbator/helper.hpp --
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

#ifndef PERT_HELPER_HPP_
#define PERT_HELPER_HPP_

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

#endif /* PERT_HELPER_HPP_ */
