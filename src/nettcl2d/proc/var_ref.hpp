/*
 * proc/var_ref.hpp --
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
