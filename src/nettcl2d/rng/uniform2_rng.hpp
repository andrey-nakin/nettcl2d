/*
 * rng/uniform_rng2.hpp --
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

#ifndef RNG_UNIFORM2_RNG_HPP_
#define RNG_UNIFORM2_RNG_HPP_

#include <boost/random/uniform_real.hpp>
#include <boost/random/linear_congruential.hpp>
#include "../calc/abstract_rng.hpp"

namespace rng {

	class Uniform2 : public AbstractRng {

		typedef boost::rand48 Generator;

		virtual double doGenerate() {
			return distr(generator);
		}

		virtual void doSeed(long seedValue) {
			generator.seed(static_cast<uint64_t>(seedValue));
		}

		virtual phlib::Cloneable* doClone() const {
			return new Uniform(min, max);
		}

	public:

		Uniform2(const double min, const double max) :
			min(min), max(max),
			generator(static_cast<uint64_t>(12345L)),
			distr(min, max) {}

	private:

		const double min;
		const double max;
		Generator generator;
		boost::uniform_real<double> distr;

	};

}

#endif /* RNG_UNIFORM2_RNG_HPP_ */
