/*
 * rng/uniform_rng.hpp --
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

#ifndef UNIFORM_RNG_HPP_
#define UNIFORM_RNG_HPP_

#include <boost/random/uniform_real.hpp>
#include <boost/random/linear_congruential.hpp>
#include "../calc/abstract_rng.hpp"

namespace rng {

	class Uniform : public AbstractRng {

		typedef boost::rand48 Generator;

		virtual double doGenerate() {
			return distr(generator);
		}

		virtual void doSeed(long seedValue) {
			generator.seed(static_cast<uint64_t>(seedValue));
		}

		virtual phlib::Cloneable* doClone() const {
			return new Uniform(mean, range);
		}

	public:

		Uniform(const double mean, const double range) :
			mean(mean), range(range),
			generator(static_cast<uint64_t>(12345L)),
			distr(mean - 0.5 * range, mean + 0.5 * range) {}

	private:

		const double mean;
		const double range;
		Generator generator;
		boost::uniform_real<double> distr;

	};

}

#endif /* UNIFORM_RNG_HPP_ */
