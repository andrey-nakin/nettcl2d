/*
 * perturbator/null.hpp --
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

#ifndef PERTURBATOR_NULL_HPP_
#define PERTURBATOR_NULL_HPP_

#include "../calc/abstract_perturbator.hpp"
#include "helper.hpp"

namespace perturbator {

	class Null : public Helper {

		Null(const Null& src) {}

		virtual phlib::Cloneable* doClone() const {
			return new Null(*this);
		}

	public:

		Null() {}

	private:

	};

}

#endif /* PERTURBATOR_NULL_HPP_ */
