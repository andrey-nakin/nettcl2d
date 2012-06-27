/*
 * circuit_wrapper.hpp
 *
 *  Created on: 21.06.2012
 *      Author: andrey
 */

#ifndef PROC_CIRCUIT_WRAPPER_HPP_
#define PROC_CIRCUIT_WRAPPER_HPP_

#include "../calc/circuit.hpp"

namespace proc {

	namespace type {
		extern const char* circuit;
	}

	class CircuitWrapper : public Wrapper<&type::circuit> {

		typedef Wrapper<&type::circuit> Base;

		boost::shared_ptr<Network> network;
		std::size_t index;

		CircuitWrapper(boost::shared_ptr<Network>& network, std::size_t const index) :
			network(network), index(index) {}

		CircuitWrapper(const CircuitWrapper& src) :
			network(src.network), index(src.index) {}

		static CircuitWrapper* validateArg(Tcl_Interp *interp, const Tcl_Obj* arg) {
			return (CircuitWrapper*) Base::validateArg(interp, arg);
		}

		virtual Base* clone() const {
			return new CircuitWrapper(*this);
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
					return get(clientData, interp, objc - 2, objv + 2);
				}

				else if ("set" == cmd) {
					return set(clientData, interp, objc - 2, objv + 2);
				}

				else
					throw WrongArgValue(interp, "exists | get | set");
			} catch (WrongNumArgs& ex) {
				throw WrongNumArgs(interp, 2 + ex.objc, objv, ex.message);
			}

			return TCL_OK;
		}

		static int exists(Tcl_Interp * interp, int objc, Tcl_Obj * CONST objv[]) {
			if (objc != 1)
				throw WrongNumArgs(interp, 0, objv, "contactInst");

			::Tcl_SetObjResult(interp, ::Tcl_NewBooleanObj(isInstanceOf(objv[0]) ? 1 : 0));

			return TCL_OK;
		}

		static int get(ClientData clientData, Tcl_Interp * interp, int objc, Tcl_Obj * CONST objv[]) {
			if (objc < 1)
				throw WrongNumArgs(interp, 0, objv, "contactInst");

			int result = TCL_ERROR;
			try {
				result = validateArg(interp, objv[0])->get(interp, objc - 1, objv + 1);
			} catch (WrongNumArgs& ex) {
				throw WrongNumArgs(interp, 1 + ex.objc, objv, ex.message);
			}

			return result;
		}

		static int set(ClientData clientData, Tcl_Interp * interp, int objc, Tcl_Obj * CONST objv[]) {
			if (objc < 1)
				throw WrongNumArgs(interp, 0, objv, "contactInst");

			int result = TCL_ERROR;
			try {
				result = validateArg(interp, objv[0])->set(interp, objc - 1, objv + 1);
			} catch (WrongNumArgs& ex) {
				throw WrongNumArgs(interp, 1 + ex.objc, objv, ex.message);
			}

			return result;
		}

		int get(Tcl_Interp * interp, int objc, Tcl_Obj * CONST objv[]) {
			if (objc != 1)
				throw WrongNumArgs(interp, 0, objv, "parameter");

			const std::string param = Tcl_GetStringFromObj(objv[0], NULL);

			if ("square" == param) {
				Tcl_SetObjResult(interp, Tcl_NewDoubleObj(circuit().square));
			} else {
				throw WrongArgValue(interp, "square");
			}

			return TCL_OK;
		}

		int set(Tcl_Interp * interp, int objc, Tcl_Obj * CONST objv[]) {
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
