/*
 * calc/abstract_rng.hpp --
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
