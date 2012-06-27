/*
 * proc/tracer_wrapper.hpp --
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

#ifndef PROC_TRACER_WRAPPER_HPP_
#define PROC_TRACER_WRAPPER_HPP_

#include <boost/shared_ptr.hpp>
#include "wrapper.hpp"
#include "../calc/abstract_tracer.hpp"
#include "../tracer/null.hpp"
#include "../tracer/avg_voltage.hpp"
#include "../tracer/voltage.hpp"

namespace proc {

	namespace type {
		extern const char* tracer;
	}

	class TracerWrapper : public Wrapper<&type::tracer> {

		typedef Wrapper<&type::tracer> Base;

		explicit TracerWrapper(AbstractTracer* const engine) :
			engine(engine) {}

		virtual Base* clone() const {
			return new TracerWrapper(dynamic_cast<AbstractTracer*>(engine->clone()));
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

				else
					throw WrongArgValue(interp, "create | exists");
			} catch (WrongNumArgs& ex) {
				throw WrongNumArgs(interp, 2 + ex.objc, objv, ex.message);
			}

			return TCL_OK;
		}

		static int create(Tcl_Interp * interp, int objc, Tcl_Obj * CONST objv[]) {
			if (objc < 1)
				throw WrongNumArgs(interp, 0, objv, "tracerType");

			const std::string tracerType = Tcl_GetStringFromObj(objv[0], NULL);
			AbstractTracer* tracer;

			if ("null" == tracerType) {
				if (objc != 1)
					throw WrongNumArgs(interp, 1, objv, "");

				tracer = new tracer::Null();
			}

			else if ("avg-voltage" == tracerType) {
				if (objc > 5)
					throw WrongNumArgs(interp, 1, objv, "?fileName? ?interval? ?startTime? ?precision?");

				tracer::AverageVoltage::Params params;
				if (objc > 1) {
					params.fileName = Tcl_GetStringFromObj(objv[1], NULL);
				}
				if (objc > 2) {
					params.interval = phlib::TclUtils::getDouble(interp, objv[2]);
				}
				if (objc > 3) {
					params.startTime = phlib::TclUtils::getDouble(interp, objv[3]);
				}
				if (objc > 4) {
					params.precision = phlib::TclUtils::getUInt(interp, objv[4]);
				}

				tracer = new tracer::AverageVoltage(params);
			}

			else if ("voltage" == tracerType) {
				if (objc > 5)
					throw WrongNumArgs(interp, 1, objv, "?fileNameFormat? ?interval? ?startTime? ?precision?");

				tracer::Voltage::Params params;
				if (objc > 1) {
					params.fileNameFormat = Tcl_GetStringFromObj(objv[1], NULL);
				}
				if (objc > 2) {
					params.interval = phlib::TclUtils::getDouble(interp, objv[2]);
				}
				if (objc > 3) {
					params.startTime = phlib::TclUtils::getDouble(interp, objv[3]);
				}
				if (objc > 4) {
					params.precision = phlib::TclUtils::getUInt(interp, objv[4]);
				}

				tracer = new tracer::Voltage(params);
			}

			else
				throw WrongArgValue(interp, "null | avg-voltage | voltage");

			// instantiate new TCL object
			Tcl_Obj* const w = Tcl_NewObj();
			w->typePtr = TracerWrapper::type();
			w->internalRep.otherValuePtr = new TracerWrapper(tracer);
			::Tcl_SetObjResult(interp, w);

			return TCL_OK;
		}

		static int exists(Tcl_Interp * interp, int objc, Tcl_Obj * CONST objv[]) {
			if (objc != 1)
				throw WrongNumArgs(interp, 0, objv, "tracerInst");

			::Tcl_SetObjResult(interp, ::Tcl_NewBooleanObj(isInstanceOf(objv[0]) ? 1 : 0));

			return TCL_OK;
		}

	public:

		boost::shared_ptr<AbstractTracer> engine;

		static TracerWrapper* validateArg(Tcl_Interp *interp, const Tcl_Obj* arg) {
			return (TracerWrapper*) Base::validateArg(interp, arg);
		}

		static void registerCommands(Tcl_Interp * interp) {
			registerCommand(interp, doMain);
		}

	};

}

#endif /* PROC_TRACER_WRAPPER_HPP_ */
