/*
 * calc/tabable.cpp --
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

/*
 * I've took a calculator example from the link below and adopted it
 * http://www.oreillynet.com/network/2003/05/06/examples/calculatorexample.html
 */

#include <string>
#include <map>
#include <cstdlib>
#include <iomanip>
#include <boost/spirit/include/classic.hpp>
#include <boost/spirit/include/phoenix1_binders.hpp>
#include <boost/lambda/bind.hpp>
#include "tagable.hpp"

struct calculator : boost::spirit::classic::grammar<calculator> {

	calculator(bool& result, const Tagable::TagContainer& tags) :
		result(result), tags(tags) {}

	// A production can have an associated closure, to store information
	// for that production.
	struct value_closure : boost::spirit::classic::closure<value_closure, bool> {
		member1 value;
	};

	struct string_closure : boost::spirit::classic::closure<string_closure, std::string> {
		member1 name;
	};

	// Following is the grammar definition.
	template <typename ScannerT>
	struct definition {
		definition(calculator const& self) {
			using namespace boost::spirit::classic;
			using namespace phoenix;

			// The lexeme_d directive tells the scanner to treat white space as
			// significant. Thus, an identifier cannot have internal white space.
			// The alpha_p and alnum_p parsers are built-in.
			// Notice how the semantic action uses a Phoenix lambda function
			// that constructs a std::string. The arg1 and arg2 placeholders are
			// are bound at runtime to the iterator range that matches this rule.
			identifier =
				lexeme_d[
					+( alnum_p | '_' | '-')
				][identifier.name = construct_<std::string>(arg1, arg2)];

			group =
				'('	>> expression[group.value = arg1] >> ')';

			// A statement can end at the end of the line, or with a semicolon.
			statement =
				expression[bind(&calculator::do_print)(self, arg1)] >> end_p;

			// A variable name must be looked up. This is a straightforward
			// Phoenix binding.
			factor =
				group[factor.value = arg1]
				| identifier[factor.value = bind(&calculator::lookup)(self, arg1)];

			term =
				factor[term.value = arg1] >> *('&' >> factor[term.value *= arg1]);

			expression =
				term[expression.value = arg1] >> *('|' >> term[expression.value += arg1]);
		}

		// The start symbol is returned from start().
		boost::spirit::classic::rule<ScannerT> const& start() const {
			return statement;
		}

		// Each rule must be declared, optionally with an associated closure.
		boost::spirit::classic::rule<ScannerT> statement;
		boost::spirit::classic::rule<ScannerT, string_closure::context_t> identifier;
		boost::spirit::classic::rule<ScannerT, value_closure::context_t> expression, factor, group, term;
	};

	bool lookup(const std::string& name) const {
		return "*" == name && !tags.empty()
			? true
			: tags.end() != tags.find(name);
	}

	void do_print(bool x) const {
		result = x;
	}

private:

	bool& result;
	const Tagable::TagContainer& tags;

};

bool Tagable::matches(const std::string& expression) const {
	using namespace boost::spirit::classic;

	bool result = false;
	calculator calc(result, tags);

	parse_info<std::string::const_iterator> info = parse(expression.begin(), expression.end(), calc, space_p);
	if (!info.hit) {
		throw ParseException(std::string(expression.begin(), info.stop));
	} else if (!info.full) {
		throw ParseException(std::string("expression is not full"));
	}

	return result;
}
