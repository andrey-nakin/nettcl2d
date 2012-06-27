/*
 * circuit.hpp
 *
 *  Created on: 18.06.2012
 *      Author: andrey
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
