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
#include <boost/bind.hpp>
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
					return processInstance(clientData, interp, objc - 2, objv + 2, static_cast<InstanceHandler>(&NetworkWrapper::get));
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

		int get(ClientData /* clientData */, Tcl_Interp * interp, int objc, Tcl_Obj * CONST objv[]) {
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
			} else if ("contacts" == param) {
				if (objc > 2)
					throw WrongNumArgs(interp, 0, objv, "contacts ?tagExpr?");

				Tcl_SetObjResult(
					interp,
					makeList(
						interp,
						objc,
						objv,
						boost::bind(&Network::buildContactIndices, engine, _1),
						boost::bind(&ContactWrapper::create, engine, _1)));
			} else if ("circuits" == param) {
				if (objc > 2)
					throw WrongNumArgs(interp, 0, objv, "circuits ?tagExpr?");

				Tcl_SetObjResult(
					interp,
					makeList(
						interp,
						objc,
						objv,
						boost::bind(&Network::buildCircuitIndices, engine, _1),
						boost::bind(&CircuitWrapper::create, engine, _1)));
			} else {
				throw WrongArgValue(interp, "num-of-contacts | num-of-circuits | contact-at | circuit-at | contacts | circuits");
			}

			return TCL_OK;
		}

		template <typename IndexBuilder, typename ElementCreator>
		Tcl_Obj* makeList(Tcl_Interp * interp, int objc, Tcl_Obj* const objv[], IndexBuilder builder, ElementCreator creator) {
			Tcl_Obj *ret = Tcl_NewListObj(0, NULL);

			std::string tagExpr;
			if (objc > 1) {
				tagExpr = Tcl_GetStringFromObj(objv[1], NULL);
			}

			const Network::IndexVector indices = builder(tagExpr);
//			const Network::IndexVector indices = engine->buildContactIndices(tagExpr);
			for (
					Network::IndexVector::const_iterator i = indices.begin(), last = indices.end();
					i != last;
					++i) {

//				Tcl_ListObjAppendElement(interp, ret, ContactWrapper::create(engine, *i));
				Tcl_ListObjAppendElement(interp, ret, creator(*i));
			}

			return ret;
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
