/*
 * proc/tagable_wrapper.hpp --
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

#ifndef PROC_TAGABLE_WRAPPER_HPP_
#define PROC_TAGABLE_WRAPPER_HPP_

#include "../calc/tagable.hpp"

namespace proc {

	template <const char** type_name>
	class TagableWrapper : public Wrapper<type_name> {

		typedef TagableWrapper<type_name> Base;

	protected:

		TagableWrapper() {}

		TagableWrapper(const TagableWrapper& src) {}

		virtual Tagable& tagable() = 0;

		Tcl_Obj* getTagsObj(Tcl_Interp * interp) {
			Tcl_Obj* ret = Tcl_NewListObj(0, NULL);
			Tagable& t = tagable();

			for (
					Contact::const_tag_iterator i = t.getTags().begin(), last = t.getTags().end();
					i != last; ++i) {
				Tcl_ListObjAppendElement(interp, ret, Tcl_NewStringObj(i->c_str(), -1));
			}

			return ret;
		}

		static int main(ClientData clientData, Tcl_Interp * interp, int objc, Tcl_Obj * const objv[], const char* usage) {
			if (objc < 2)
				throw WrongNumArgs(interp, 1, objv, "command");

			const std::string cmd = Tcl_GetStringFromObj(objv[1], NULL);

			try {
				if ("has-tag" == cmd) {
					return processInstance(clientData, interp, objc - 2, objv + 2, static_cast<typename Base::InstanceHandler>(&TagableWrapper::hasTag));
				}

				else if ("add-tag" == cmd) {
					return processInstance(clientData, interp, objc - 2, objv + 2, static_cast<typename Base::InstanceHandler>(&TagableWrapper::addTag));
				}

				else if ("remove-tag" == cmd) {
					return processInstance(clientData, interp, objc - 2, objv + 2, static_cast<typename Base::InstanceHandler>(&TagableWrapper::removeTag));
				}

				else if ("get-prop" == cmd) {
					return processInstance(clientData, interp, objc - 2, objv + 2, static_cast<typename Base::InstanceHandler>(&TagableWrapper::getProp));
				}

				else if ("matches" == cmd) {
					return processInstance(clientData, interp, objc - 2, objv + 2, static_cast<typename Base::InstanceHandler>(&TagableWrapper::matches));
				}

				else {
					std::string s;
					s += usage;
					s += " | has-tag | add-tag | remove-tag | get-prop | matches";
					throw WrongArgValue(interp, s);
				}
			} catch (WrongNumArgs& ex) {
				throw WrongNumArgs(interp, 2 + ex.objc, objv, ex.message);
			}

			return TCL_OK;
		}

	private:

		int hasTag(ClientData /* clientData */, Tcl_Interp * interp, int objc, Tcl_Obj * const objv[]) {
			if (objc != 1)
				throw WrongNumArgs(interp, 0, objv, "tag");

			Tcl_SetObjResult(interp, Tcl_NewIntObj(
				tagable().hasTag(Tcl_GetStringFromObj(objv[0], NULL)) ? 1 : 0));

			return TCL_OK;
		}

		int addTag(ClientData /* clientData */, Tcl_Interp * interp, int objc, Tcl_Obj * const objv[]) {
			if (objc != 1)
				throw WrongNumArgs(interp, 0, objv, "tag");

			tagable().addTag(Tcl_GetStringFromObj(objv[0], NULL));

			return TCL_OK;
		}

		int removeTag(ClientData /* clientData */, Tcl_Interp * interp, int objc, Tcl_Obj * const objv[]) {
			if (objc != 1)
				throw WrongNumArgs(interp, 0, objv, "tag");

			tagable().removeTag(Tcl_GetStringFromObj(objv[0], NULL));

			return TCL_OK;
		}

		int getProp(ClientData /* clientData */, Tcl_Interp * interp, int objc, Tcl_Obj * const objv[]) {
			if (objc != 1)
				throw WrongNumArgs(interp, 0, objv, "propName");

			Tcl_SetObjResult(interp, Tcl_NewStringObj(tagable().getProp(Tcl_GetStringFromObj(objv[0], NULL)).c_str(), -1));

			return TCL_OK;
		}

		int matches(ClientData /* clientData */, Tcl_Interp * interp, int objc, Tcl_Obj * const objv[]) {
			if (objc != 1)
				throw WrongNumArgs(interp, 0, objv, "expr");

			Tcl_SetObjResult(interp, Tcl_NewIntObj(
				tagable().matches(Tcl_GetStringFromObj(objv[0], NULL)) ? 1 : 0));
			return TCL_OK;
		}

	};

}

#endif /* PROC_TAGABLE_WRAPPER_HPP_ */
