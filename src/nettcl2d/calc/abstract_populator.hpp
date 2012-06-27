/*
 * abstract_populator.hpp
 *
 *  Created on: 18.06.2012
 *      Author: andrey
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
