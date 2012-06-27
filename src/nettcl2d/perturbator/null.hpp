/*
 * null.hpp
 *
 *  Created on: 22.06.2012
 *      Author: andrey
 */

#ifndef PERTURBATOR_NULL_HPP_
#define PERTURBATOR_NULL_HPP_

#include "../calc/abstract_perturbator.hpp"
#include "helper.hpp"

namespace perturbator {

	class Null : public Helper {

		Null(const Null& src) {}

		virtual phlib::Cloneable* doClone() const {
			return new Null(*this);
		}

	public:

		Null() {}

	private:

	};

}

#endif /* PERTURBATOR_NULL_HPP_ */
