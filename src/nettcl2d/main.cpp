/*
 * main.cpp --
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

#include <tcl.h>
#include <iostream>
#include "commands.hpp"

namespace proc {

	const int appVersion[] = {2, 0, 1, -1};
	const char* appNamespace = "nettcl2d";

	namespace type {
		const char* rng = "rng";
		const char* populator = "populator";
		const char* network = "network";
		const char* contact = "contact";
		const char* circuit = "circuit";
		const char* tracer = "tracer";
		const char* integrator = "integrator";
		const char* perturbator = "perturbator";
		const char* version = "version";
	}
}

static int InitNettcl2d(Tcl_Interp* const interp) {
	initCommands(interp);

	if (TCL_ERROR == ::Tcl_Init(interp)) {
		return TCL_ERROR;
	}

	return TCL_OK;
}


int main(int argc, char* argv[]) {
	::Tcl_Main(argc, argv, InitNettcl2d);
	return 0;
}
