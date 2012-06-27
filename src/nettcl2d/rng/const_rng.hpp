/*
 * const_rng.hpp
 *
 *  Created on: 18.06.2012
 *      Author: andrey
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
