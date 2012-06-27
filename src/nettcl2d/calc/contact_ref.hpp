/*
 * calc/contact_ref.hpp --
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
