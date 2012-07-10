/*
 * calc/tagable.hpp --
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

#ifndef CALC_TAGGABLE_HPP_
#define CALC_TAGGABLE_HPP_

#include <string>
#include <set>
#include <map>
#include <exception>
#include <sstream>
#include "point.hpp"

struct Tagable {

	typedef std::set<std::string> TagContainer;
	typedef TagContainer::const_iterator const_tag_iterator;

	typedef std::map<std::string, std::string> PropContainer;
	typedef PropContainer::const_iterator const_prop_iterator;

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

	TagContainer tags;
	PropContainer props;

	Tagable() {}

	Tagable(const Tagable& src) : tags(src.tags), props(src.props) {}

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

	template <typename Type>
	void setProp(const std::string& name, const Type value) {
		std::stringstream s;
		s << value;
		props[name] = s.str();
	}

	const std::string& getProp(const std::string& name) const {
		static const std::string empty;
		const Tagable::PropContainer::const_iterator i = props.find(name);
		return i == props.end() ? empty : i->second;
	}

	/**
	 tag         ::= '*' | (<latin letter> | <digit> | '-' | '_')+
	 group       ::= '(' expression ')'
     factor      ::= group | prop = <value> | tag
     term        ::= factor ('&' factor)*
     expression  ::= term ('|' term)*
    */
	bool matches(const std::string& expression) const;

	const TagContainer& getTags() const {
		return tags;
	}

};

#endif /* CALC_TAGGABLE_HPP_ */
