/*
 * calc/network.hpp --
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

#ifndef CALC_NETWORK_HPP_
#define CALC_NETWORK_HPP_

#include <vector>
#include <boost/shared_ptr.hpp>
#include <phlib/cloneable.hpp>
#include "contact.hpp"
#include "circuit.hpp"

class Network : public phlib::Cloneable {

	virtual phlib::Cloneable* doClone() const {
		return new Network(*this);
	}

	Network(const Network& src) : contacts(src.contacts), circuits(src.circuits) {}

public:

	typedef std::size_t index_type;

	typedef std::vector<Contact> ContactVector;
	typedef ContactVector::iterator contact_iterator;
	typedef ContactVector::const_iterator contact_const_iterator;

	typedef std::vector<Circuit> CircuitVector;
	typedef CircuitVector::iterator circuit_iterator;
	typedef CircuitVector::const_iterator circuit_const_iterator;

	Network() {

	}

	ContactVector::size_type getNumOfContacts() const {
		return contacts.size();
	}

	CircuitVector::size_type getNumOfCircuits() const {
		return circuits.size();
	}

	Contact& contact(std::size_t const index) {
		return contacts[index];
	}

	const Contact& contact(std::size_t const index) const {
		return contacts[index];
	}

	Circuit& circuit(std::size_t const index) {
		return circuits[index];
	}

	const Circuit& circuit(std::size_t const index) const {
		return circuits[index];
	}

	contact_const_iterator contactBegin() const {
		return contacts.begin();
	}

	contact_iterator contactBegin() {
		return contacts.begin();
	}

	contact_const_iterator contactEnd() const {
		return contacts.end();
	}

	contact_iterator contactEnd() {
		return contacts.end();
	}

	circuit_const_iterator circuitBegin() const {
		return circuits.begin();
	}

	circuit_iterator circuitBegin() {
		return circuits.begin();
	}

	circuit_const_iterator circuitEnd() const {
		return circuits.end();
	}

	circuit_iterator circuitEnd() {
		return circuits.end();
	}

	std::size_t addContact(const Contact& c) {
		const std::size_t index = contacts.size();
		contacts.push_back(c);
		return index;
	}

	std::size_t addCircuit(const Circuit& c) {
		const std::size_t index = circuits.size();
		circuits.push_back(c);
		return index;
	}

private:

	ContactVector contacts;
	CircuitVector circuits;

};

#endif /* CALC_NETWORK_HPP_ */
