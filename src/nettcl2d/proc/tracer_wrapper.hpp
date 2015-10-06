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
#include "../tracer/avg_flux.hpp"
#include "../tracer/flux.hpp"
#include "../tracer/phase_diff.hpp"
#include "../tracer/phase.hpp"

namespace proc {

	namespace type {
		extern const char* tracer;
	}

	class TracerWrapper : public Wrapper<&type::tracer> {

		typedef Wrapper<&type::tracer> Base;

		explicit TracerWrapper(AbstractTracer* const engine) :
			engine(engine) {}

		virtual Base* clone() const {
			return 0;
		}

		static int doMain(ClientData clientData, Tcl_Interp * interp, int objc, Tcl_Obj * CONST objv[]) {
			return process(clientData, interp, objc, objv, main);
		}

		static int main(ClientData /* clientData */, Tcl_Interp * interp, int objc, Tcl_Obj * CONST objv[]) {
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
				tracer = makeTagableTracer<tracer::AverageVoltage>(interp, objc, objv);
			}

			else if ("voltage" == tracerType) {
				tracer = makeIndexTracer<tracer::Voltage>(interp, objc, objv);
			}

			else if ("avg-flux" == tracerType) {
				tracer = makeTagableTracer<tracer::AverageFlux>(interp, objc, objv);
			}

			else if ("flux" == tracerType) {
				tracer = makeIndexTracer<tracer::Flux>(interp, objc, objv);
			}

			else if ("phase-diff" == tracerType) {
				tracer = makePhaseDiffTracer(interp, objc, objv);
			}

			else if ("phase" == tracerType) {
				tracer = makeIndexTracer<tracer::Phase>(interp, objc, objv);
			}

			else
				throw WrongArgValue(interp, "null | avg-voltage | voltage | avg-flux | flux | phase | phase-diff");

			// instantiate new TCL object
			Tcl_Obj* const w = Tcl_NewObj();
			w->typePtr = TracerWrapper::type();
			w->internalRep.otherValuePtr = new TracerWrapper(tracer);
			::Tcl_SetObjResult(interp, w);

			return TCL_OK;
		}

		template <typename Tracer>
		static Tracer* makeTagableTracer(Tcl_Interp * interp, int objc, Tcl_Obj* const objv[]) {
			if (objc > 6)
				throw WrongNumArgs(interp, 1, objv, "?fileName? ?interval? ?startTime? ?precision? ?tagExpr?");

			typename Tracer::Params params;

			if (objc > 1) {
				const std::string s = Tcl_GetStringFromObj(objv[1], NULL);
				if (!s.empty()) {
					params.fileName = s;
				}
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

			if (objc > 5) {
				params.tagExpr = Tcl_GetStringFromObj(objv[5], NULL);
			}

			return new Tracer(params);
		}

		template <typename Tracer>
		static Tracer* makeIndexTracer(Tcl_Interp * interp, int objc, Tcl_Obj* const objv[]) {
			if (objc > 6)
				throw WrongNumArgs(interp, 1, objv, "?fileNameFormat? ?interval? ?startTime? ?precision? ?tagExpr?");

			typedef typename Tracer::Params Params;
			Params params;

			if (objc > 1) {
				const std::string s = Tcl_GetStringFromObj(objv[1], NULL);
				if (!s.empty()) {
					params.fileNameFormat = s;
				}
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

			if (objc > 5) {
				params.tagExpr = Tcl_GetStringFromObj(objv[5], NULL);
			}

			return new Tracer(params);
		}

		static tracer::PhaseDifference* makePhaseDiffTracer(Tcl_Interp * interp, int objc, Tcl_Obj* const objv[]) {
			if (objc > 7)
				throw WrongNumArgs(interp, 1, objv, "?fileName? ?interval? ?startTime? ?precision? ?tagExpr1? ?tagExpr2?");

			tracer::PhaseDifference::Params params;

			if (objc > 1) {
				const std::string s = Tcl_GetStringFromObj(objv[1], NULL);
				if (!s.empty()) {
					params.fileName = s;
				}
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

			if (objc > 5) {
				params.tagExpr1 = Tcl_GetStringFromObj(objv[5], NULL);
			}

			if (objc > 6) {
				params.tagExpr2 = Tcl_GetStringFromObj(objv[6], NULL);
			}

			return new tracer::PhaseDifference(params);
		}

	public:

		boost::shared_ptr<AbstractTracer> engine;

		static TracerWrapper* validateArg(Tcl_Interp *interp, const Tcl_Obj* arg) {
			return static_cast<TracerWrapper*>(Base::validateArg(interp, arg));
		}

		static void registerCommands(Tcl_Interp * interp) {
			registerCommand(interp, doMain);
		}

	};

}

#endif /* PROC_TRACER_WRAPPER_HPP_ */
