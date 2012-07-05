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
#include "../calc/network.hpp"

namespace proc {

	namespace type {
		extern const char* contact;
	}

	class ContactWrapper : public Wrapper<&type::contact> {

		typedef Wrapper<&type::contact> Base;

		boost::shared_ptr<Network> network;
		Network::index_type index;

		ContactWrapper(boost::shared_ptr<Network>& network, Network::index_type const index) :
			network(network), index(index) {}

		ContactWrapper(const ContactWrapper& src) :
			network(src.network), index(src.index) {}

		static ContactWrapper* validateArg(Tcl_Interp *interp, const Tcl_Obj* arg) {
			return (ContactWrapper*) Base::validateArg(interp, arg);
		}

		virtual Base* clone() const {
			return new ContactWrapper(*this);
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

				else if ("has-tag" == cmd) {
					return processInstance(clientData, interp, objc - 2, objv + 2, static_cast<InstanceHandler>(&ContactWrapper::hasTag));
				}

				else if ("add-tag" == cmd) {
					return processInstance(clientData, interp, objc - 2, objv + 2, static_cast<InstanceHandler>(&ContactWrapper::addTag));
				}

				else if ("remove-tag" == cmd) {
					return processInstance(clientData, interp, objc - 2, objv + 2, static_cast<InstanceHandler>(&ContactWrapper::removeTag));
				}

				else if ("matches" == cmd) {
					return processInstance(clientData, interp, objc - 2, objv + 2, static_cast<InstanceHandler>(&ContactWrapper::matches));
				}

				else
					throw WrongArgValue(interp, "exists | get | set | has-tag | add-tag | remove-tag | matches");
			} catch (WrongNumArgs& ex) {
				throw WrongNumArgs(interp, 2 + ex.objc, objv, ex.message);
			}

			return TCL_OK;
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
				Tcl_Obj *ret = Tcl_NewListObj(0, NULL);

				for (
						Contact::const_tag_iterator i = c.getTags().begin(), last = c.getTags().end();
						i != last; ++i) {
					Tcl_ListObjAppendElement(interp, ret, Tcl_NewStringObj(i->c_str(), -1));
				}

				Tcl_SetObjResult(interp, ret);
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

		int hasTag(ClientData /* clientData */, Tcl_Interp * interp, int objc, Tcl_Obj * const objv[]) {
			if (objc != 1)
				throw WrongNumArgs(interp, 0, objv, "tag");

			Tcl_SetObjResult(interp, Tcl_NewIntObj(
				contact().hasTag(Tcl_GetStringFromObj(objv[0], NULL)) ? 1 : 0));

			return TCL_OK;
		}

		int addTag(ClientData /* clientData */, Tcl_Interp * interp, int objc, Tcl_Obj * const objv[]) {
			if (objc != 1)
				throw WrongNumArgs(interp, 0, objv, "tag");

			contact().addTag(Tcl_GetStringFromObj(objv[0], NULL));

			return TCL_OK;
		}

		int removeTag(ClientData /* clientData */, Tcl_Interp * interp, int objc, Tcl_Obj * const objv[]) {
			if (objc != 1)
				throw WrongNumArgs(interp, 0, objv, "tag");

			contact().removeTag(Tcl_GetStringFromObj(objv[0], NULL));

			return TCL_OK;
		}

		int matches(ClientData /* clientData */, Tcl_Interp * interp, int objc, Tcl_Obj * const objv[]) {
			if (objc != 1)
				throw WrongNumArgs(interp, 0, objv, "expr");

			Tcl_SetObjResult(interp, Tcl_NewIntObj(
				contact().matches(Tcl_GetStringFromObj(objv[0], NULL)) ? 1 : 0));
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
