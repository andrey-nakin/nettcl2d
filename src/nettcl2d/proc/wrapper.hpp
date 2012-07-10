/*
 * proc/wrapper.hpp --
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

#ifndef WRAPPER_HPP_
#define WRAPPER_HPP_

#include <tcl.h>
#include <string>
#include <phlib/tclutils.h>
#include "../calc/tagable.hpp"
#include "var_ref.hpp"

namespace proc {

	extern const char* appNamespace;

	class WrongArgValue : public std::exception {
	public:

		Tcl_Interp* const interp;
		const std::string message;

		WrongArgValue(Tcl_Interp* interp, const char* const message) :
			interp(interp), message(message) {}

		WrongArgValue(Tcl_Interp* interp, const std::string& message) :
			interp(interp), message(message) {}

		virtual ~WrongArgValue() throw() {}

		const char* what() const throw() {
			return message.c_str();
		}

		void notify() const {
			std::string msg("Wrong argument value, should be: ");
			msg += message;
			Tcl_AppendResult(interp, msg.c_str(), NULL);
		}
	};

	class WrongNumArgs : public std::exception {
	public:

		Tcl_Interp* const interp;
		const int objc;
		Tcl_Obj* const * const objv;
		const std::string message;

		WrongNumArgs(Tcl_Interp* const interp, const int objc, Tcl_Obj* const objv[], const char * const message) :
			interp(interp),
			objc(objc),
			objv(objv),
			message(message) {
		}

		WrongNumArgs(Tcl_Interp* const interp, const int objc, Tcl_Obj* const objv[], const std::string& message) :
			interp(interp),
			objc(objc),
			objv(objv),
			message(message) {
		}

		virtual ~WrongNumArgs() throw() {}

		const char* what() const throw() {
			return message.c_str();
		}

		void notify() const {
			Tcl_WrongNumArgs(interp, objc, objv, message.c_str());
		}
	};

	template<const char** type_name>
	class Wrapper {
	public:

		virtual ~Wrapper() {}

		static const char* typeName() {
			return *type_name;
		}

		static void free(Tcl_Obj * const objPtr) {
			delete reinterpret_cast<Wrapper*>(objPtr->internalRep.otherValuePtr);
		}

		static void dup(Tcl_Obj * const srcPtr, Tcl_Obj * const dupPtr) {
			Wrapper* src = reinterpret_cast<Wrapper*>(srcPtr->internalRep.otherValuePtr);
			Wrapper* dup = src->clone();
			dupPtr->internalRep.otherValuePtr = dup ? dup : src;
		}

		static Tcl_ObjType* type(void) {
			Tcl_ObjType* type = Tcl_GetObjType(*type_name);
			if (!type) {
				std::string msg("type is not registered: ");
				msg += *type_name;
				throw phlib::TclUtils::tcl_error(msg.c_str());
			}
			return type;
		}

		static bool isInstanceOf(const Tcl_Obj* arg) {
			return arg->typePtr == type();
		}

		static Wrapper* validateArg(Tcl_Interp *interp, const Tcl_Obj* arg) {
			if (!isInstanceOf(arg)) {
				std::string msg("argument passed is not a valid instance of ");
				msg += *type_name;
				throw WrongArgValue(interp, msg.c_str());
			}

			return reinterpret_cast<Wrapper*>(arg->internalRep.otherValuePtr);
		}

		static void registerType(void) {
			static Tcl_ObjType typedesc;

			typedesc.name = const_cast<char*>(*type_name);
			typedesc.freeIntRepProc = free;
			typedesc.dupIntRepProc = dup;
			typedesc.updateStringProc = NULL;
			typedesc.setFromAnyProc = NULL;

			::Tcl_RegisterObjType(&typedesc);
		}

		void purgeVarRefs() {
			varRefs.clear();
		}

		void addVarRef(Tcl_Obj* var) {
			varRefs.push_back(VarRef(var));
		}

		void addVarRefs(const std::vector<Tcl_Obj*>& vars) {
			for (std::vector<Tcl_Obj*>::const_iterator i = vars.begin(), last = vars.end(); i != last; ++i) {
				addVarRef(*i);
			}
		}

	protected:

		typedef std::vector<VarRef> VarRefVector;
		typedef int (*StaticHandler)(ClientData clientData, Tcl_Interp * interp, int objc, Tcl_Obj * const objv[]);
		typedef int (Wrapper::*InstanceHandler)(ClientData clientData, Tcl_Interp * interp, int objc, Tcl_Obj * const objv[]);

		static void registerCommand(Tcl_Interp * interp, StaticHandler handler) {
			std::string cmdName;
			cmdName += appNamespace;
			cmdName += "::";
			cmdName += *type_name;

			(void) ::Tcl_CreateObjCommand(
				interp,
				cmdName.c_str(),
				reinterpret_cast<Tcl_ObjCmdProc*>(handler),
				NULL,
				NULL);
		}

		static int process(ClientData clientData, Tcl_Interp * interp, int objc, Tcl_Obj * const objv[], StaticHandler handler) {
			int ret_code = TCL_ERROR;

			try {
				handler(clientData, interp, objc, objv);
				ret_code = TCL_OK;
			} catch (Tagable::ParseException& ex) {
				std::string msg("Wrong tag expression:\n");
				msg += ex.getMessage();
				msg += " ^";
				Tcl_AppendResult(interp, msg.c_str(), NULL);
			} catch (WrongNumArgs& ex) {
				ex.notify();
			} catch (WrongArgValue& ex) {
				ex.notify();
			} catch (std::exception& ex) {
				std::string msg("exception ");
				msg += typeid(ex).name();
				msg += ": ";
				msg += ex.what();
				Tcl_AppendResult(interp, msg.c_str(), NULL);
			} catch (...) {
				Tcl_AppendResult(interp, "unexpected error", NULL);
			}

			return ret_code;
		}

		static int processInstance(ClientData clientData, Tcl_Interp * interp, int objc, Tcl_Obj * const objv[], InstanceHandler handler) {
			if (objc < 1) {
				std::stringstream sb;
				sb << *type_name << "Inst";
				throw WrongNumArgs(interp, 0, objv, sb.str());
			}

			int result = TCL_ERROR;
			try {
				Wrapper* const wrapper = validateArg(interp, objv[0]);
				result = (wrapper->*handler)(clientData, interp, objc - 1, objv + 1);
			} catch (WrongNumArgs& ex) {
				throw WrongNumArgs(interp, 1 + ex.objc, objv, ex.message);
			}

			return result;
		}

		static int exists(Tcl_Interp * interp, int objc, Tcl_Obj * const objv[]) {
			if (objc != 1) {
				std::stringstream sb;
				sb << *type_name << "Inst";
				throw WrongNumArgs(interp, 0, objv, sb.str());
			}

			::Tcl_SetObjResult(interp, ::Tcl_NewBooleanObj(isInstanceOf(objv[0]) ? 1 : 0));

			return TCL_OK;
		}

	private:

		VarRefVector varRefs;

		Wrapper& operator=(const Wrapper&);

		virtual Wrapper* clone() const = 0;
	};

}

#endif /* WRAPPER_HPP_ */
