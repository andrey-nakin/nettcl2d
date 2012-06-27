/*
 * calc/circuit.hpp --
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

#ifndef CIRCUIT_HPP_
#define CIRCUIT_HPP_

#include <vector>
#include "contact_ref.hpp"

class Circuit {
public:

	typedef std::vector<ContactRef> ContactRefVector;
	typedef ContactRefVector::iterator iterator;
	typedef ContactRefVector::const_iterator const_iterator;

	double square;
	ContactRefVector contactRefs;

	Circuit(double const square) : square(square) {}

	iterator begin() {
		return contactRefs.begin();
	}

	const_iterator begin() const {
		return contactRefs.begin();
	}

	iterator end() {
		return contactRefs.end();
	}

	const_iterator end() const {
		return contactRefs.end();
	}

	std::size_t addContactRef(const ContactRef& cr) {
		const std::size_t index = contactRefs.size();
		contactRefs.push_back(cr);
		return index;
	}

};


#endif /* CIRCUIT_HPP_ */
