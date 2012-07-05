/*
 * calc/contact.hpp --
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

#ifndef CONTACT_HPP_
#define CONTACT_HPP_

#include <string>
#include <set>
#include <exception>
#include "point.hpp"

struct Contact {

	typedef std::set<std::string> TagContainer;
	typedef TagContainer::const_iterator const_tag_iterator;

	class ParseException : public std::exception {

		const std::string message;

	public:

		ParseException(const std::string& msg) : message(msg) {}
		virtual ~ParseException() throw() {}

		virtual const char* what() const throw() {
			return message.c_str();
		}

		const std::string& getMessage() const {
			return message;
		}
	};

	double beta, tau, v;

	double z;
	double phase;
	double voltage;
	TagContainer tags;

	Contact() :
		beta(1.0), tau(1.0), v(40.0), z(0.0), phase(0.0), voltage(0.0) {}

	Contact(double const beta, double const tau, double const v) :
		beta(beta), tau(tau), v(v), z(0.0), phase(0.0), voltage(0.0) {}

	void addTag(const char* const tag) {
		tags.insert(std::string(tag));
	}

	void removeTag(const char* const tag) {
		TagContainer::iterator i = tags.find(std::string(tag));
		if (i != tags.end()) {
			tags.erase(i);
		}
	}

	bool hasTag(const char* const tag) const {
		const std::string t(tag);
		return "*" == t || tags.end() != tags.find(t);
	}

	bool hasTag(const std::string& tag) const {
		return "*" == tag || tags.end() != tags.find(tag);
	}

	bool hasTags(const TagContainer& c) const {
		for (TagContainer::const_iterator i = c.begin(), last = c.end(); i != last; ++i) {
			if (hasTag(*i)) {
				return true;
			}
		}
		return false;
	}

	/**
	 tag         ::= '*' | (<latin letter> | <digit> | '-' | '_')+
	 group       ::= '(' expression ')'
     factor      ::= tag | group
     term        ::= factor ('&' factor)*
     expression  ::= term ('|' term)*
    */
	bool matches(const std::string& expression) const;

	const TagContainer& getTags() const {
		return tags;
	}

};

#endif /* CONTACT_HPP_ */
