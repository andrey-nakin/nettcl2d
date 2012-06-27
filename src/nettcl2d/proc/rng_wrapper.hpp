/*
 * proc/rng.cpp --
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

#ifndef PROC_RNG_HPP_
#define PROC_RNG_HPP_

#include <tcl.h>
#include <string>

#include <boost/shared_ptr.hpp>
#include <phlib/tclutils.h>
#include "wrapper.hpp"
#include "../calc/abstract_rng.hpp"
#include "../rng/uniform_rng.hpp"
#include "../rng/const_rng.hpp"

namespace proc {

	namespace type {
		extern const char* rng;
	}

	class RngWrapper : public Wrapper<&type::rng> {

		typedef Wrapper<&type::rng> Base;

		explicit RngWrapper(AbstractRng* const engine) :
			engine(engine) {}

		virtual Base* clone() const {
			return new RngWrapper(dynamic_cast<AbstractRng*>(engine->clone()));
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

				else if ("seed" == cmd) {
					return seed(clientData, interp, objc - 2, objv + 2);
				}

				else if ("next" == cmd) {
					return next(clientData, interp, objc - 2, objv + 2);
				}

				else
					throw WrongArgValue(interp, "create | exists | seed | next");
			} catch (WrongNumArgs& ex) {
				throw WrongNumArgs(interp, 2 + ex.objc, objv, ex.message);
			}

			return TCL_OK;
		}

		static int create(Tcl_Interp * interp, int objc, Tcl_Obj * CONST objv[]) {
			if (objc < 1)
				throw WrongNumArgs(interp, 0, objv, "rngType");

			const std::string methodName = Tcl_GetStringFromObj(objv[0], NULL);
			AbstractRng* rng;

			if ("uniform" == methodName) {
				if (objc != 3)
					throw WrongNumArgs(interp, 1, objv, "mean range");

				rng = new rng::Uniform(
						phlib::TclUtils::getDouble(interp, objv[1]),
						phlib::TclUtils::getDouble(interp, objv[2]));
			}

			else if ("const" == methodName) {
				if (objc != 2)
					throw WrongNumArgs(interp, 1, objv, "value");

				rng = new rng::Const(phlib::TclUtils::getDouble(interp, objv[1]));
			}

			else
				throw WrongArgValue(interp, "const | uniform");

			// instantiate new TCL object
			Tcl_Obj* const w = Tcl_NewObj();
			w->typePtr = RngWrapper::type();
			w->internalRep.otherValuePtr = new RngWrapper(rng);
			::Tcl_SetObjResult(interp, w);

			return TCL_OK;
		}

		static int exists(Tcl_Interp * interp, int objc, Tcl_Obj * CONST objv[]) {
			if (objc != 1)
				throw phlib::TclUtils::wrong_num_args_exception(interp, objc, objv);

			::Tcl_SetObjResult(interp, ::Tcl_NewBooleanObj(isInstanceOf(objv[0]) ? 1 : 0));

			return TCL_OK;
		}

		static int seed(ClientData clientData, Tcl_Interp * interp, int objc, Tcl_Obj * CONST objv[]) {
			if (objc < 1)
				throw phlib::TclUtils::wrong_num_args_exception(interp, objc, objv);

			RngWrapper* const wrapper = validateArg(interp, objv[0]);
			return wrapper->seed(interp, objc - 1, objv + 1);
		}

		int seed(Tcl_Interp * interp, int objc, Tcl_Obj * CONST objv[]) {
			if (objc != 1)
				throw phlib::TclUtils::wrong_num_args_exception(interp, objc, objv);

			engine->seed(phlib::TclUtils::getLong(interp, objv[0]));
			return TCL_OK;
		}

		static int next(ClientData clientData, Tcl_Interp * interp, int objc, Tcl_Obj * CONST objv[]) {
			if (objc != 1)
				throw phlib::TclUtils::wrong_num_args_exception(interp, objc, objv);

			RngWrapper* const wrapper = validateArg(interp, objv[0]);
			return wrapper->next(interp, objc - 1, objv + 1);
		}

		int next(Tcl_Interp * interp, int objc, Tcl_Obj * CONST objv[]) {
			if (objc != 0)
				throw phlib::TclUtils::wrong_num_args_exception(interp, objc, objv);

			::Tcl_SetObjResult(interp, ::Tcl_NewDoubleObj(engine->generate()));
			return TCL_OK;
		}

	public:

		boost::shared_ptr<AbstractRng> engine;

		static RngWrapper* validateArg(Tcl_Interp *interp, const Tcl_Obj* arg) {
			return (RngWrapper*) Base::validateArg(interp, arg);
		}

		static void registerCommands(Tcl_Interp * interp) {
			registerCommand(interp, doMain);
		}

	};

}

#endif // PROC_RNG_HPP_
