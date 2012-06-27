/*
 * proc/contact.hpp --
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

#ifndef PROC_CONTACT_HPP_
#define PROC_CONTACT_HPP_

#include "../calc/contact.hpp"

namespace proc {

	namespace type {
		extern const char* contact;
	}

	class ContactWrapper : public Wrapper<&type::contact> {

		typedef Wrapper<&type::contact> Base;

		boost::shared_ptr<Network> network;
		std::size_t index;

		ContactWrapper(boost::shared_ptr<Network>& network, std::size_t const index) :
			network(network), index(index) {}

		ContactWrapper(const ContactWrapper& src) :
			network(src.network), index(src.index) {}

		static ContactWrapper* validateArg(Tcl_Interp *interp, const Tcl_Obj* arg) {
			return (ContactWrapper*) Base::validateArg(interp, arg);
		}

		virtual Base* clone() const {
			return new ContactWrapper(*this);
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

			if ("beta" == param) {
				Tcl_SetObjResult(interp, Tcl_NewDoubleObj(contact().beta));
			} else if ("tau" == param) {
					Tcl_SetObjResult(interp, Tcl_NewDoubleObj(contact().tau));
			} else if ("v" == param) {
					Tcl_SetObjResult(interp, Tcl_NewDoubleObj(contact().v));
			} else if ("z" == param) {
					Tcl_SetObjResult(interp, Tcl_NewDoubleObj(contact().z));
			} else if ("phase" == param) {
					Tcl_SetObjResult(interp, Tcl_NewDoubleObj(contact().phase));
			} else if ("voltage" == param) {
					Tcl_SetObjResult(interp, Tcl_NewDoubleObj(contact().voltage));
			} else if ("tag" == param) {
					Tcl_SetObjResult(interp, Tcl_NewIntObj(contact().tag));
			} else {
				throw WrongArgValue(interp, "beta | tau | v | z | phase | voltage | tag");
			}

			return TCL_OK;
		}

		int set(Tcl_Interp * interp, int objc, Tcl_Obj * CONST objv[]) {
			if (objc != 2)
				throw WrongNumArgs(interp, 0, objv, "parameter value");

			const std::string param = Tcl_GetStringFromObj(objv[0], NULL);

			if ("beta" == param) {
				contact().beta = phlib::TclUtils::getDouble(interp, objv[1]);
			} else if ("tau" == param) {
				contact().tau = phlib::TclUtils::getDouble(interp, objv[1]);
			} else if ("v" == param) {
				contact().v = phlib::TclUtils::getDouble(interp, objv[1]);
			} else if ("z" == param) {
				contact().z = phlib::TclUtils::getDouble(interp, objv[1]);
			} else if ("phase" == param) {
				contact().phase = phlib::TclUtils::getDouble(interp, objv[1]);
			} else if ("voltage" == param) {
				contact().voltage = phlib::TclUtils::getDouble(interp, objv[1]);
			} else if ("tag" == param) {
				contact().tag = phlib::TclUtils::getInt(interp, objv[1]);
			} else {
				throw WrongArgValue(interp, "beta | tau | v | z | phase | voltage | tag");
			}

			return TCL_OK;
		}

		Contact& contact() {
			return network->contact(index);
		}

	public:

		static void registerCommands(Tcl_Interp * interp) {
			registerCommand(interp, doMain);
		}

		static Tcl_Obj* create(boost::shared_ptr<Network>& network, std::size_t const index) {
			// instantiate new TCL object
			Tcl_Obj* const w = Tcl_NewObj();
			w->typePtr = ContactWrapper::type();
			w->internalRep.otherValuePtr = new ContactWrapper(network, index);
			return w;
		}

	};

}

#endif /* PROC_CONTACT_HPP_ */
