/*
 * rng/const_rng.hpp --
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

#ifndef CONST_RNG_HPP_
#define CONST_RNG_HPP_

#include "../calc/abstract_rng.hpp"

namespace rng {

	class Const : public AbstractRng {

		virtual double doGenerate() {
			return value;
		}

		virtual void doSeed(long seedValue) {
		}

		virtual Cloneable* doClone() const {
			return new Const(value);
		}

	public:

		Const(const double value) : value(value) {}

	private:

		const double value;

	};

}

#endif /* CONST_RNG_HPP_ */
