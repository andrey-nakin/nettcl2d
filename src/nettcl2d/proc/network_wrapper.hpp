/*
 * proc/network.hpp --
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

#ifndef PROC_NETWORK_HPP_
#define PROC_NETWORK_HPP_

#include <boost/shared_ptr.hpp>
#include <phlib/tclutils.h>
#include "../calc/network.hpp"
#include "populator_wrapper.hpp"
#include "contact_wrapper.hpp"
#include "circuit_wrapper.hpp"

namespace proc {

	namespace type {
		extern const char* network;
	}

	class NetworkWrapper : public Wrapper<&type::network> {

		typedef Wrapper<&type::network> Base;

		explicit NetworkWrapper(Network* const engine) :
			engine(engine) {}

		virtual Base* clone() const {
			return new NetworkWrapper(dynamic_cast<Network*>(engine->clone()));
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
					return get(clientData, interp, objc - 2, objv + 2);
				}

				else
					throw WrongArgValue(interp, "create | exists | get");
			} catch (WrongNumArgs& ex) {
				throw WrongNumArgs(interp, 2 + ex.objc, objv, ex.message);
			}

			return TCL_OK;
		}

		static int create(Tcl_Interp * interp, int objc, Tcl_Obj * CONST objv[]) {
			if (objc != 1)
				throw WrongNumArgs(interp, 0, objv, "populatorInst");

			AbstractPopulator& p = *PopulatorWrapper::validateArg(interp, objv[0])->engine;
			Network* net = new Network();
			p.populate(*net);

			// instantiate new TCL object
			Tcl_Obj* const w = Tcl_NewObj();
			w->typePtr = NetworkWrapper::type();
			w->internalRep.otherValuePtr = new NetworkWrapper(net);
			::Tcl_SetObjResult(interp, w);

			return TCL_OK;
		}

		static int exists(Tcl_Interp * interp, int objc, Tcl_Obj * CONST objv[]) {
			if (objc != 1)
				throw WrongNumArgs(interp, 0, objv, "networkInst");

			::Tcl_SetObjResult(interp, ::Tcl_NewBooleanObj(isInstanceOf(objv[0]) ? 1 : 0));

			return TCL_OK;
		}

		static int get(ClientData clientData, Tcl_Interp * interp, int objc, Tcl_Obj * CONST objv[]) {
			if (objc < 1)
				throw WrongNumArgs(interp, 0, objv, "networkInst");

			int result = TCL_ERROR;
			try {
				result = validateArg(interp, objv[0])->get(interp, objc - 1, objv + 1);
			} catch (WrongNumArgs& ex) {
				throw WrongNumArgs(interp, 1 + ex.objc, objv, ex.message);
			}

			return result;
		}

		int get(Tcl_Interp * interp, int objc, Tcl_Obj * CONST objv[]) {
			if (objc < 1)
				throw WrongNumArgs(interp, 0, objv, "parameter");

			const std::string param = Tcl_GetStringFromObj(objv[0], NULL);

			if ("num-of-contacts" == param) {
				Tcl_SetObjResult(interp, Tcl_NewIntObj(engine->getNumOfContacts()));
			} else if ("num-of-circuits" == param) {
					Tcl_SetObjResult(interp, Tcl_NewIntObj(engine->getNumOfCircuits()));
			} else if ("contact-at" == param) {
				if (objc < 2)
					throw WrongNumArgs(interp, 0, objv, "parameter contactIndex");
				Tcl_SetObjResult(interp, ContactWrapper::create(engine, phlib::TclUtils::getUInt(interp, objv[1])));
			} else if ("circuit-at" == param) {
				if (objc < 2)
					throw WrongNumArgs(interp, 0, objv, "parameter circuitIndex");
				Tcl_SetObjResult(interp, CircuitWrapper::create(engine, phlib::TclUtils::getUInt(interp, objv[1])));
			} else {
				throw WrongArgValue(interp, "num-of-contacts | num-of-circuits | contact-at | circuit-at");
			}

			return TCL_OK;
		}

	public:

		boost::shared_ptr<Network> engine;

		static NetworkWrapper* validateArg(Tcl_Interp *interp, const Tcl_Obj* arg) {
			return (NetworkWrapper*) Base::validateArg(interp, arg);
		}

		static void registerCommands(Tcl_Interp * interp) {
			registerCommand(interp, doMain);
		}

	};

}

#endif /* PROC_NETWORK_HPP_ */
