/*
 * proc/integrator_wrapper.hpp --
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

#ifndef PROC_INTEGRATOR_WRAPPER_HPP_
#define PROC_INTEGRATOR_WRAPPER_HPP_

#include <boost/shared_ptr.hpp>
#include "wrapper.hpp"
#include "../calc/integrator.hpp"
#include "network_wrapper.hpp"
#include "tracer_wrapper.hpp"
#include "perturbator_wrapper.hpp"

namespace proc {

	namespace type {
		extern const char* integrator;
	}

	class IntegratorWrapper : public Wrapper<&type::integrator> {

		typedef Wrapper<&type::integrator> Base;

		boost::shared_ptr<Integrator> engine;
		VarRefVector perturbatorRefs;

		explicit IntegratorWrapper(const IntegratorWrapper& src) :
			engine(dynamic_cast<Integrator*>(src.engine->clone())),
			perturbatorRefs(src.perturbatorRefs) {}

		explicit IntegratorWrapper(Integrator* const engine) :
			engine(engine) {}

		static IntegratorWrapper* validateArg(Tcl_Interp *interp, const Tcl_Obj* arg) {
			return static_cast<IntegratorWrapper*>(Base::validateArg(interp, arg));
		}

		virtual Base* clone() const {
			return new IntegratorWrapper(*this);
		}

		static int doMain(ClientData clientData, Tcl_Interp * interp, int objc, Tcl_Obj * CONST objv[]) {
			return process(clientData, interp, objc, objv, main);
		}

		static int main(ClientData clientData, Tcl_Interp * interp, int objc, Tcl_Obj * CONST objv[]) {
			if (objc < 2)
				throw WrongNumArgs(interp, 1, objv, "command");

			const std::string cmd = Tcl_GetStringFromObj(objv[1], NULL);

			try {
				if ("create" == cmd) {
					return create(interp, objc - 2, objv + 2);
				}

				else if ("exists" == cmd) {
					return exists(interp, objc - 2, objv + 2);
				}

				else if ("get" == cmd) {
					return processInstance(clientData, interp, objc - 2, objv + 2, static_cast<InstanceHandler>(&IntegratorWrapper::get));
				}

				else if ("set" == cmd) {
					return processInstance(clientData, interp, objc - 2, objv + 2, static_cast<InstanceHandler>(&IntegratorWrapper::set));
				}

				else if ("run" == cmd) {
					return processInstance(clientData, interp, objc - 2, objv + 2, static_cast<InstanceHandler>(&IntegratorWrapper::run));
				}

				else if ("add-tracer" == cmd) {
					return processInstance(clientData, interp, objc - 2, objv + 2, static_cast<InstanceHandler>(&IntegratorWrapper::addTracer));
				}

				else if ("purge-tracers" == cmd) {
					return processInstance(clientData, interp, objc - 2, objv + 2, static_cast<InstanceHandler>(&IntegratorWrapper::purgeTracers));
				}

				else if ("add-perturbator" == cmd) {
					return processInstance(clientData, interp, objc - 2, objv + 2, static_cast<InstanceHandler>(&IntegratorWrapper::addPerturbator));
				}

				else if ("purge-perturbators" == cmd) {
					return processInstance(clientData, interp, objc - 2, objv + 2, static_cast<InstanceHandler>(&IntegratorWrapper::purgePerturbators));
				}

				else
					throw WrongArgValue(interp, "create | exists | get | set | run | add-tracer | purge-tracers | add-perturbator | purge-perturbators");
			} catch (WrongNumArgs& ex) {
				throw WrongNumArgs(interp, 2 + ex.objc, objv, ex.message);
			}

			return TCL_OK;
		}

		static int create(Tcl_Interp * interp, int objc, Tcl_Obj * CONST objv[]) {
			if (objc > 2)
				throw WrongNumArgs(interp, 0, objv, "?step? ?delta?");

			Integrator::Params params;
			if (objc > 0) {
				params.step = phlib::TclUtils::getDouble(interp, objv[0]);
			}
			if (objc > 1) {
				params.delta = phlib::TclUtils::getDouble(interp, objv[1]);
			}

			// instantiate new TCL object
			Tcl_Obj* const w = Tcl_NewObj();
			w->typePtr = IntegratorWrapper::type();
			w->internalRep.otherValuePtr = new IntegratorWrapper(new Integrator(params));
			::Tcl_SetObjResult(interp, w);

			return TCL_OK;
		}

		int get(ClientData /* clientData */, Tcl_Interp * interp, int objc, Tcl_Obj * CONST objv[]) {
			if (objc < 1)
				throw WrongNumArgs(interp, 0, objv, "parameter");

			const std::string param = Tcl_GetStringFromObj(objv[0], NULL);

			if ("step" == param) {
				Tcl_SetObjResult(interp, Tcl_NewDoubleObj(engine->getParams().step));
			} else if ("delta" == param) {
				Tcl_SetObjResult(interp, Tcl_NewDoubleObj(engine->getParams().delta));
			} else {
				throw WrongArgValue(interp, "step | delta");
			}

			return TCL_OK;
		}

		int set(ClientData /* clientData */, Tcl_Interp * interp, int objc, Tcl_Obj * CONST objv[]) {
			if (objc != 2)
				throw WrongNumArgs(interp, 0, objv, "parameter value");

			const std::string param = Tcl_GetStringFromObj(objv[0], NULL);

			if ("step" == param) {
				engine->getParams().step = phlib::TclUtils::getDouble(interp, objv[1]);
			} else if ("delta" == param) {
				engine->getParams().delta = phlib::TclUtils::getDouble(interp, objv[1]);
			} else {
				throw WrongArgValue(interp, "step | delta");
			}

			return TCL_OK;
		}

		int run(ClientData /* clientData */, Tcl_Interp * interp, int objc, Tcl_Obj * CONST objv[]) {
			if (objc != 4)
				throw WrongNumArgs(interp, 0, objv, "networkInst startTime endTime dt");

			engine->run(
				*NetworkWrapper::validateArg(interp, objv[0])->engine,
				phlib::TclUtils::getDouble(interp, objv[1]),
				phlib::TclUtils::getDouble(interp, objv[2]),
				phlib::TclUtils::getDouble(interp, objv[3]));

			return TCL_OK;
		}

		int addTracer(ClientData /* clientData */, Tcl_Interp * interp, int objc, Tcl_Obj * CONST objv[]) {
			if (objc != 1)
				throw WrongNumArgs(interp, 0, objv, "tracerInst");

			engine->addTracer(*TracerWrapper::validateArg(interp, objv[0])->engine);
			addVarRef(objv[0]);

			return TCL_OK;
		}

		int purgeTracers(ClientData /* clientData */, Tcl_Interp * interp, int objc, Tcl_Obj * CONST objv[]) {
			if (objc != 0)
				throw WrongNumArgs(interp, 0, objv, "");

			engine->purgeTracers();
			purgeVarRefs();

			return TCL_OK;
		}

		int addPerturbator(ClientData /* clientData */, Tcl_Interp * interp, int objc, Tcl_Obj * CONST objv[]) {
			if (objc != 1)
				throw WrongNumArgs(interp, 0, objv, "perturbatorInst");

			engine->addPerturbator(*PerturbatorWrapper::validateArg(interp, objv[0])->engine);
			perturbatorRefs.push_back(VarRef(objv[0]));

			return TCL_OK;
		}

		int purgePerturbators(ClientData /* clientData */, Tcl_Interp * interp, int objc, Tcl_Obj * CONST objv[]) {
			if (objc != 0)
				throw WrongNumArgs(interp, 0, objv, "");

			engine->purgePerturbators();
			perturbatorRefs.clear();

			return TCL_OK;
		}

	public:

		static void registerCommands(Tcl_Interp * interp) {
			registerCommand(interp, doMain);
		}

	};

}

#endif /* PROC_INTEGRATOR_WRAPPER_HPP_ */
