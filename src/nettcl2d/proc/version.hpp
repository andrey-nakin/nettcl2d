/*
 * proc/version.hpp --
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

#ifndef PROC_VERSION_HPP_
#define PROC_VERSION_HPP_

namespace proc {

	extern const int appVersion[];

	namespace type {
		extern const char* version;
	}

	class Version : public Wrapper<&type::version> {

		typedef Wrapper<&type::version> Base;

		explicit Version() {}
		explicit Version(const Version&) {}

		virtual Base* clone() const {
			return new Version(*this);
		}

		static int doMain(ClientData clientData, Tcl_Interp * interp, int objc, Tcl_Obj * CONST objv[]) {
			return process(clientData, interp, objc, objv, main);
		}

		static int main(ClientData clientData, Tcl_Interp * interp, int objc, Tcl_Obj * CONST objv[]) {
			if (objc < 1)
				throw WrongNumArgs(interp, 1, objv, "?command?");

			std::string cmd;
			if (objc > 1) {
				cmd = Tcl_GetStringFromObj(objv[1], NULL);
			}

			try {
				if (cmd.empty()) {
					return asString(interp, objc - 1, objv + 1);
				}

				else if ("as-string" == cmd) {
					return asString(interp, objc - 2, objv + 2);
				}

				else if ("as-list" == cmd) {
					return asList(interp, objc - 2, objv + 2);
				}

				else
					throw WrongArgValue(interp, "as-list | as-string");
			} catch (WrongNumArgs& ex) {
				throw WrongNumArgs(interp, 2 + ex.objc, objv, ex.message);
			}

			return TCL_OK;
		}

		static int asString(Tcl_Interp * interp, int objc, Tcl_Obj * CONST objv[]) {
			if (objc != 0)
				throw WrongNumArgs(interp, 0, objv, NULL);

			std::stringstream s;
			bool first = true;
			for (int i = 0; appVersion[i] >= 0; ++i) {
				if (first) {
					first = false;
				} else {
					s << '.';
				}
				s << appVersion[i];
			}
			const std::string ss = s.str();
			Tcl_SetObjResult(interp, Tcl_NewStringObj(ss.c_str(), -1));

			return TCL_OK;
		}

		static int asList(Tcl_Interp * interp, int objc, Tcl_Obj * CONST objv[]) {
			if (objc != 0)
				throw WrongNumArgs(interp, 0, objv, NULL);

			Tcl_Obj *ret = Tcl_NewListObj(0, NULL);
			for (int i = 0; appVersion[i] >= 0; ++i) {
				Tcl_ListObjAppendElement(interp, ret, Tcl_NewIntObj(appVersion[i]));
			}

			Tcl_SetObjResult(interp, ret);

			return TCL_OK;
		}

	public:

		static void registerCommands(Tcl_Interp * interp) {
			registerCommand(interp, doMain);
		}

	};

}

#endif /* PROC_VERSION_HPP_ */
