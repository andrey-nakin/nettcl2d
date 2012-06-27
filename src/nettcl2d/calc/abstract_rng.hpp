/*
 * abstract_rng.hpp
 *
 *  Created on: 18.06.2012
 *      Author: andrey
 */

#ifndef ABSTRACT_RNG_HPP_
#define ABSTRACT_RNG_HPP_

#include <phlib/cloneable.hpp>

class AbstractRng : public phlib::Cloneable {

	virtual double doGenerate() = 0;
	virtual void doSeed(long seedValue) = 0;

public:

	double operator()() {
		return generate();
	}

	double generate() {
		return doGenerate();
	}

	void seed(long seedValue) {
		doSeed(seedValue);
	}

};

#endif /* ABSTRACT_RNG_HPP_ */
