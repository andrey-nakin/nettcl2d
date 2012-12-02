/*
 * proc/populator.hpp --
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

#ifndef POPULATOR_HPP_
#define POPULATOR_HPP_

#include <boost/shared_ptr.hpp>
#include "rng_wrapper.hpp"
#include "var_ref.hpp"
#include "../calc/abstract_populator.hpp"
#include "../populator/grid2d.hpp"

namespace proc {

	namespace type {
		extern const char* populator;
	}

	class PopulatorWrapper : public Wrapper<&type::populator> {

		typedef Wrapper<&type::populator> Base;

		explicit PopulatorWrapper(AbstractPopulator* const engine) :
			engine(engine) {}

		virtual Base* clone() const {
			return new PopulatorWrapper(dynamic_cast<AbstractPopulator*>(engine->clone()));
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
				throw WrongNumArgs(interp, 0, objv, "populatorType");

			const std::string methodName = Tcl_GetStringFromObj(objv[0], NULL);
			AbstractPopulator* p;
			std::vector<Tcl_Obj*> varRefs;

			if ("grid2d" == methodName) {
				if (objc != 8)
					throw WrongNumArgs(interp, 1, objv, "cols rows xRng yRng betaRng tauRng vRng");

				populator::Grid2d::Params params(
					phlib::TclUtils::getUInt(interp, objv[1]),
					phlib::TclUtils::getUInt(interp, objv[2]),
					*RngWrapper::validateArg(interp, objv[3])->engine,
					*RngWrapper::validateArg(interp, objv[4])->engine,
					*RngWrapper::validateArg(interp, objv[5])->engine,
					*RngWrapper::validateArg(interp, objv[6])->engine,
					*RngWrapper::validateArg(interp, objv[7])->engine);

				p = new populator::Grid2d(params);
				for (size_t i = 3; i <= 7; ++i) {
					varRefs.push_back(objv[i]);
				}
			}

			else
				throw WrongArgValue(interp, "grid2d");

			PopulatorWrapper* wrp = new PopulatorWrapper(p);
			wrp->addVarRefs(varRefs);

			// instantiate new TCL object
			Tcl_Obj* const w = Tcl_NewObj();
			w->typePtr = PopulatorWrapper::type();
			w->internalRep.otherValuePtr = wrp;
			::Tcl_SetObjResult(interp, w);

			return TCL_OK;
		}

	public:

		boost::shared_ptr<AbstractPopulator> engine;

		static PopulatorWrapper* validateArg(Tcl_Interp *interp, const Tcl_Obj* arg) {
			return static_cast<PopulatorWrapper*>(Base::validateArg(interp, arg));
		}

		static void registerCommands(Tcl_Interp * interp) {
			registerCommand(interp, doMain);
		}

	};

}

#endif /* POPULATOR_HPP_ */
