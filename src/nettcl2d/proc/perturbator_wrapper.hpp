/*
 * proc/perturbator_wrapper.hpp --
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

#ifndef PROC_PERTURBATOR_WRAPPER_HPP_
#define PROC_PERTURBATOR_WRAPPER_HPP_

#include <boost/shared_ptr.hpp>
#include "../calc/abstract_perturbator.hpp"
#include "../perturbator/null.hpp"
#include "../perturbator/static.hpp"
#include "wrapper.hpp"
#include "rng_wrapper.hpp"

namespace proc {

	namespace type {
		extern const char* perturbator;
	}

	class PerturbatorWrapper : public Wrapper<&type::perturbator> {

		typedef Wrapper<&type::perturbator> Base;

		explicit PerturbatorWrapper(AbstractPerturbator* const engine) :
			engine(engine) {}

		virtual Base* clone() const {
			return new PerturbatorWrapper(dynamic_cast<AbstractPerturbator*>(engine->clone()));
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
				throw WrongNumArgs(interp, 0, objv, "perturbatorType");

			const std::string perturbatorType = Tcl_GetStringFromObj(objv[0], NULL);
			AbstractPerturbator* perturbator;
			std::vector<Tcl_Obj*> varRefs;

			if ("null" == perturbatorType) {
				if (objc != 1)
					throw WrongNumArgs(interp, 1, objv, "");

				perturbator = new perturbator::Null();
			}

			else if ("static" == perturbatorType) {
				if (objc < 3 || objc > 4)
					throw WrongNumArgs(interp, 1, objv, "average rngInst ?tag?");

				int tag = 0;
				if (objc > 3) {
					tag = phlib::TclUtils::getInt(interp, objv[3]);
				}
				perturbator::Static::Params params(
						phlib::TclUtils::getDouble(interp, objv[1]),
						*RngWrapper::validateArg(interp, objv[2])->engine,
						tag);
				varRefs.push_back(objv[2]);

				perturbator = new perturbator::Static(params);
			}

			else
				throw WrongArgValue(interp, "null | static");

			PerturbatorWrapper* const pw = new PerturbatorWrapper(perturbator);
			pw->addVarRefs(varRefs);

			// instantiate new TCL object
			Tcl_Obj* const w = Tcl_NewObj();
			w->typePtr = PerturbatorWrapper::type();
			w->internalRep.otherValuePtr = pw;
			::Tcl_SetObjResult(interp, w);

			return TCL_OK;
		}

		static int exists(Tcl_Interp * interp, int objc, Tcl_Obj * CONST objv[]) {
			if (objc != 1)
				throw WrongNumArgs(interp, 0, objv, "perturbatorInst");

			::Tcl_SetObjResult(interp, ::Tcl_NewBooleanObj(isInstanceOf(objv[0]) ? 1 : 0));

			return TCL_OK;
		}

	public:

		boost::shared_ptr<AbstractPerturbator> engine;

		static PerturbatorWrapper* validateArg(Tcl_Interp *interp, const Tcl_Obj* arg) {
			return (PerturbatorWrapper*) Base::validateArg(interp, arg);
		}

		static void registerCommands(Tcl_Interp * interp) {
			registerCommand(interp, doMain);
		}

	};

}

#endif /* PROC_PERTURBATOR_WRAPPER_HPP_ */
