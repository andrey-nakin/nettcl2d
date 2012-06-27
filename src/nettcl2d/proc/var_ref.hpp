/*
 * var_ref.hpp
 *
 *  Created on: 18.06.2012
 *      Author: andrey
 */

#ifndef PROC_VAR_REF_HPP_
#define PROC_VAR_REF_HPP_

#include <tcl.h>

class VarRef {

	Tcl_Obj* var;

public:

	VarRef(Tcl_Obj* const var) : var(var) {
		Tcl_IncrRefCount(var);
	}

	VarRef(const VarRef& src) : var(src.var) {
		Tcl_IncrRefCount(var);
	}

	~VarRef() {
		Tcl_DecrRefCount(var);
	}

	VarRef& operator=(const VarRef& src) {
		Tcl_DecrRefCount(var);
		this->var = src.var;
		Tcl_IncrRefCount(var);
		return *this;
	}
};

#endif /* PROC_VAR_REF_HPP_ */
