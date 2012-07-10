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
#include <boost/spirit/include/classic.hpp>
#include <boost/spirit/include/phoenix1_binders.hpp>
#include <boost/lambda/bind.hpp>
#include "tagable.hpp"

struct calculator : boost::spirit::classic::grammar<calculator> {

	calculator(bool& result, const Tagable::TagContainer& tags, const Tagable::PropContainer& props) :
		result(result), tags(tags), props(props) {}

	// A production can have an associated closure, to store information
	// for that production.
	struct value_closure : boost::spirit::classic::closure<value_closure, bool> {
		member1 value;
	};

	struct string_closure : boost::spirit::classic::closure<string_closure, std::string> {
		member1 name;
	};

	struct comparison_closure : boost::spirit::classic::closure<comparison_closure, double, std::string> {
	    member1 value;
	    member2 name;
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

			literal	=
				lexeme_d[
					+alnum_p
				][literal.name = construct_<std::string>(arg1, arg2)];

			comparison =
				identifier[comparison.name = arg1] >> '=' >> literal
				[comparison.value = bind(&calculator::lookupProp)(self, comparison.name, arg1) /*construct_<std::string>(arg1, arg2) */];

		    // A variable name must be looked up. This is a straightforward
			// Phoenix binding.
			factor =
				group[factor.value = arg1]
				| comparison[factor.value = arg1]
				| identifier[factor.value = bind(&calculator::lookupTag)(self, arg1)];

			term =
				factor[term.value = arg1] >> *('&' >> factor[term.value = term.value && arg1]);

			expression =
				term[expression.value = arg1] >> *('|' >> term[expression.value = expression.value || arg1]);
		}

		// The start symbol is returned from start().
		boost::spirit::classic::rule<ScannerT> const& start() const {
			return statement;
		}

		// Each rule must be declared, optionally with an associated closure.
		boost::spirit::classic::rule<ScannerT> statement;
		boost::spirit::classic::rule<ScannerT, string_closure::context_t> identifier, literal;
		boost::spirit::classic::rule<ScannerT, comparison_closure::context_t> comparison;
		boost::spirit::classic::rule<ScannerT, value_closure::context_t> expression, factor, group, term;
	};

	bool lookupTag(const std::string& name) const {
		return "*" == name && !tags.empty()
			? true
			: tags.end() != tags.find(name);
	}

	bool lookupProp(const std::string& name, const std::string& value) const {
		const Tagable::PropContainer::const_iterator i = props.find(name);
		return i != props.end() && i->second == value;
	}

	void do_print(bool x) const {
		result = x;
	}

private:

	bool& result;
	const Tagable::TagContainer& tags;
	const Tagable::PropContainer& props;

};

bool Tagable::matches(const std::string& expression) const {
	using namespace boost::spirit::classic;

	bool result = false;
	calculator calc(result, tags, props);

	parse_info<std::string::const_iterator> info = parse(expression.begin(), expression.end(), calc, space_p);
	if (!info.hit) {
		throw ParseException(std::string(expression.begin(), info.stop));
	} else if (!info.full) {
		throw ParseException(std::string("expression is not full"));
	}

	return result;
}
