/*
 * proc/circuit_wrapper.hpp --
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

#ifndef PROC_CIRCUIT_WRAPPER_HPP_
#define PROC_CIRCUIT_WRAPPER_HPP_

#include "../calc/circuit.hpp"
#include "tagable_wrapper.hpp"

namespace proc {

	namespace type {
		extern const char* circuit;
	}

	class CircuitWrapper : public TagableWrapper<&type::circuit> {

		typedef TagableWrapper<&type::circuit> Base;

		boost::shared_ptr<Network> network;
		std::size_t index;

		CircuitWrapper(boost::shared_ptr<Network>& network, std::size_t const index) :
			network(network), index(index) {}

		CircuitWrapper(const CircuitWrapper& src) :
			network(src.network), index(src.index) {}

		static CircuitWrapper* validateArg(Tcl_Interp *interp, const Tcl_Obj* arg) {
			return static_cast<CircuitWrapper*>(Base::validateArg(interp, arg));
		}

		virtual Base* clone() const {
			return new CircuitWrapper(*this);
		}

		virtual Tagable& tagable() {
			return circuit();
		}

		static int doMain(ClientData clientData, Tcl_Interp * interp, int objc, Tcl_Obj * CONST objv[]) {
			return process(clientData, interp, objc, objv, main);
		}

		static int main(ClientData clientData, Tcl_Interp * interp, int objc, Tcl_Obj * CONST objv[]) {
			if (objc < 2)
				throw WrongNumArgs(interp, 1, objv, "command");

			const std::string cmd = Tcl_GetStringFromObj(objv[1], NULL);

			try {

				if ("exists" == cmd) {
					return exists(interp, objc - 2, objv + 2);
				}

				else if ("get" == cmd) {
					return processInstance(clientData, interp, objc - 2, objv + 2, static_cast<InstanceHandler>(&CircuitWrapper::get));
				}

				else if ("set" == cmd) {
					return processInstance(clientData, interp, objc - 2, objv + 2, static_cast<InstanceHandler>(&CircuitWrapper::set));
				}

			} catch (WrongNumArgs& ex) {
				throw WrongNumArgs(interp, 2 + ex.objc, objv, ex.message);
			}

			return Base::main(clientData, interp, objc, objv, "exists | get | set");
		}

		int get(ClientData /* clientData */, Tcl_Interp * interp, int objc, Tcl_Obj * CONST objv[]) {
			if (objc != 1)
				throw WrongNumArgs(interp, 0, objv, "parameter");

			const std::string param = Tcl_GetStringFromObj(objv[0], NULL);

			if ("square" == param) {
				Tcl_SetObjResult(interp, Tcl_NewDoubleObj(circuit().square));
			} else if ("tags" == param) {
				Tcl_SetObjResult(interp, getTagsObj(interp));
			} else {
				throw WrongArgValue(interp, "square | tags");
			}

			return TCL_OK;
		}

		int set(ClientData /* clientData */, Tcl_Interp * interp, int objc, Tcl_Obj * CONST objv[]) {
			if (objc != 2)
				throw WrongNumArgs(interp, 0, objv, "parameter value");

			const std::string param = Tcl_GetStringFromObj(objv[0], NULL);

			if ("square" == param) {
				circuit().square = phlib::TclUtils::getDouble(interp, objv[1]);
			} else {
				throw WrongArgValue(interp, "square");
			}

			return TCL_OK;
		}

		Circuit& circuit() {
			return network->circuit(index);
		}

	public:

		static void registerCommands(Tcl_Interp * interp) {
			registerCommand(interp, doMain);
		}

		static Tcl_Obj* create(boost::shared_ptr<Network>& network, std::size_t const index) {
			// instantiate new TCL object
			Tcl_Obj* const w = Tcl_NewObj();
			w->typePtr = CircuitWrapper::type();
			w->internalRep.otherValuePtr = new CircuitWrapper(network, index);
			return w;
		}

	};

}

#endif /* PROC_CIRCUIT_WRAPPER_HPP_ */
