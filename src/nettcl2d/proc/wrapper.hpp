/*
 * wrapper.hpp
 *
 *  Created on: 18.06.2012
 *      Author: andrey
 */

#ifndef WRAPPER_HPP_
#define WRAPPER_HPP_

#include <tcl.h>
#include <string>
#include <phlib/tclutils.h>
#include "var_ref.hpp"

namespace proc {

	extern const char* appNamespace;

	class WrongArgValue : public std::exception {
	public:

		Tcl_Interp* const interp;
		const char * const message;

		WrongArgValue(Tcl_Interp* interp, const char *message) :
			interp(interp), message(message) {}

		const char* what() const throw() {
			return message;
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
		const char * const message;

		WrongNumArgs(Tcl_Interp* const interp, const int objc, Tcl_Obj* const objv[], const char * const message) :
			interp(interp),
			objc(objc),
			objv(objv),
			message(message) {
		}

		const char* what() const throw() {
			return message;
		}

		void notify() const {
			Tcl_WrongNumArgs(interp, objc, objv, message);
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
			const Wrapper* src = reinterpret_cast<Wrapper*>(srcPtr->internalRep.otherValuePtr);
			dupPtr->internalRep.otherValuePtr = src->clone();
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
		typedef int (*StaticHandler)(ClientData clientData, Tcl_Interp * interp, int objc, Tcl_Obj * CONST objv[]);

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

		static int process(ClientData clientData, Tcl_Interp * interp, int objc, Tcl_Obj * CONST objv[], StaticHandler handler) {
			int ret_code = TCL_ERROR;

			try {
				handler(clientData, interp, objc, objv);
				ret_code = TCL_OK;
			} catch (WrongNumArgs& ex) {
				ex.notify();
			} catch (WrongArgValue& ex) {
				ex.notify();
			} catch (std::exception& ex) {
				//phlib::TclUtils::notifyProcError(interp, ex, usage);
			}

			return ret_code;
		}

	private:

		VarRefVector varRefs;

		Wrapper& operator=(const Wrapper&);

		virtual Wrapper* clone() const = 0;
	};

}

#endif /* WRAPPER_HPP_ */