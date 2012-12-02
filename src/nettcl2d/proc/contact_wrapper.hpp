/*
 * proc/contact_wrapper.hpp --
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
#include "../calc/network.hpp"
#include "tagable_wrapper.hpp"

namespace proc {

	namespace type {
		extern const char* contact;
	}

	class ContactWrapper : public TagableWrapper<&type::contact> {

		typedef TagableWrapper<&type::contact> Base;

		boost::shared_ptr<Network> network;
		Network::index_type index;

		ContactWrapper(boost::shared_ptr<Network>& network, Network::index_type const index) :
			network(network), index(index) {}

		ContactWrapper(const ContactWrapper& src) :
			network(src.network), index(src.index) {}

		static ContactWrapper* validateArg(Tcl_Interp *interp, const Tcl_Obj* arg) {
			return static_cast<ContactWrapper*>(Base::validateArg(interp, arg));
		}

		virtual Base* clone() const {
			return new ContactWrapper(*this);
		}

		virtual Tagable& tagable() {
			return contact();
		}

		static int doMain(ClientData clientData, Tcl_Interp * interp, int objc, Tcl_Obj * const objv[]) {
			return process(clientData, interp, objc, objv, main);
		}

		static int main(ClientData clientData, Tcl_Interp * interp, int objc, Tcl_Obj * const objv[]) {
			if (objc < 2)
				throw WrongNumArgs(interp, 1, objv, "command");

			const std::string cmd = Tcl_GetStringFromObj(objv[1], NULL);

			try {

				if ("exists" == cmd) {
					return exists(interp, objc - 2, objv + 2);
				}

				else if ("get" == cmd) {
					return processInstance(clientData, interp, objc - 2, objv + 2, static_cast<InstanceHandler>(&ContactWrapper::get));
				}

				else if ("set" == cmd) {
					return processInstance(clientData, interp, objc - 2, objv + 2, static_cast<InstanceHandler>(&ContactWrapper::set));
				}

			} catch (WrongNumArgs& ex) {
				throw WrongNumArgs(interp, 2 + ex.objc, objv, ex.message);
			}

			return Base::main(clientData, interp, objc, objv, "exists | get | set");
		}

		int get(ClientData /* clientData */, Tcl_Interp * interp, int objc, Tcl_Obj * const objv[]) {
			if (objc != 1)
				throw WrongNumArgs(interp, 0, objv, "parameter");

			const std::string param = Tcl_GetStringFromObj(objv[0], NULL);
			const Contact& c = contact();

			if ("beta" == param) {
				Tcl_SetObjResult(interp, Tcl_NewDoubleObj(c.beta));
			} else if ("tau" == param) {
				Tcl_SetObjResult(interp, Tcl_NewDoubleObj(c.tau));
			} else if ("v" == param) {
				Tcl_SetObjResult(interp, Tcl_NewDoubleObj(c.v));
			} else if ("z" == param) {
				Tcl_SetObjResult(interp, Tcl_NewDoubleObj(c.z));
			} else if ("phase" == param) {
				Tcl_SetObjResult(interp, Tcl_NewDoubleObj(c.phase));
			} else if ("voltage" == param) {
				Tcl_SetObjResult(interp, Tcl_NewDoubleObj(c.voltage));
			} else if ("tags" == param) {
				Tcl_SetObjResult(interp, getTagsObj(interp));
			} else {
				throw WrongArgValue(interp, "beta | tau | v | z | phase | voltage | tags");
			}

			return TCL_OK;
		}

		int set(ClientData /* clientData */, Tcl_Interp * interp, int objc, Tcl_Obj * const objv[]) {
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
			} else {
				throw WrongArgValue(interp, "beta | tau | v | z | phase | voltage");
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

		static Tcl_Obj* create(boost::shared_ptr<Network>& network, Network::index_type const index) {
			// instantiate new TCL object
			Tcl_Obj* const w = Tcl_NewObj();
			w->typePtr = ContactWrapper::type();
			w->internalRep.otherValuePtr = new ContactWrapper(network, index);
			return w;
		}

	};

}

#endif /* PROC_CONTACT_HPP_ */
