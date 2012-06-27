/*
 * calc/abstract_populator.hpp --
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

#ifndef ABSTRACT_POPULATOR_HPP_
#define ABSTRACT_POPULATOR_HPP_

#include <phlib/cloneable.hpp>
#include "../calc/network.hpp"

class AbstractPopulator : public phlib::Cloneable {

	virtual void doPopulate(Network& dest) = 0;

protected:

	AbstractPopulator() {}

public:

	void populate(Network& dest) {
		doPopulate(dest);
	}

};

#endif /* ABSTRACT_POPULATOR_HPP_ */
