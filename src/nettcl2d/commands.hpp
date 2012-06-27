/*
 * commands.hpp
 *
 *  Created on: 18.06.2012
 *      Author: andrey
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
