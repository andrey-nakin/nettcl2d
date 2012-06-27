/*
 * Contact.hpp
 *
 *  Created on: 18.06.2012
 *      Author: andrey
 */

#ifndef CONTACT_HPP_
#define CONTACT_HPP_

#include "point.hpp"

struct Contact {

	double beta, tau, v;

	double z;
	double phase;
	double voltage;
	int tag;

	Contact() :
		beta(1.0), tau(1.0), v(40.0), z(0.0), phase(0.0), voltage(0.0), tag(0) {}

	Contact(double const beta, double const tau, double const v) :
		beta(beta), tau(tau), v(v), z(0.0), phase(0.0), voltage(0.0) {}

};

#endif /* CONTACT_HPP_ */
