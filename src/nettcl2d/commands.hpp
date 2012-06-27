/*
 * commands.hpp --
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

#ifndef COMMANDS_HPP_
#define COMMANDS_HPP_

#include "proc/rng_wrapper.hpp"
#include "proc/populator_wrapper.hpp"
#include "proc/network_wrapper.hpp"
#include "proc/contact_wrapper.hpp"
#include "proc/circuit_wrapper.hpp"
#include "proc/tracer_wrapper.hpp"
#include "proc/integrator_wrapper.hpp"
#include "proc/version.hpp"

void initCommands(Tcl_Interp *interp) {
	proc::RngWrapper::registerType();
	proc::RngWrapper::registerCommands(interp);

	proc::PopulatorWrapper::registerType();
	proc::PopulatorWrapper::registerCommands(interp);

	proc::NetworkWrapper::registerType();
	proc::NetworkWrapper::registerCommands(interp);

	proc::ContactWrapper::registerType();
	proc::ContactWrapper::registerCommands(interp);

	proc::CircuitWrapper::registerType();
	proc::CircuitWrapper::registerCommands(interp);

	proc::TracerWrapper::registerType();
	proc::TracerWrapper::registerCommands(interp);

	proc::IntegratorWrapper::registerType();
	proc::IntegratorWrapper::registerCommands(interp);

	proc::PerturbatorWrapper::registerType();
	proc::PerturbatorWrapper::registerCommands(interp);

	proc::Version::registerCommands(interp);
}

#endif /* COMMANDS_HPP_ */
