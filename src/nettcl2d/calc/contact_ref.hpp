/*
 * contact_ref.hpp
 *
 *  Created on: 21.06.2012
 *      Author: andrey
 */

#ifndef CALC_CONTACT_REF_HPP_
#define CALC_CONTACT_REF_HPP_

#include "contact.hpp"

struct ContactRef {

	std::size_t index;
	double gain;
	double weight;

	ContactRef(std::size_t const index, double const gain, double const weight) :
		index(index), gain(gain), weight(weight) {}
};


#endif /* CALC_CONTACT_REF_HPP_ */
