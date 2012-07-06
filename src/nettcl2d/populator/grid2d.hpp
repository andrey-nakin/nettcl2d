/*
 * populator/grid2d.hpp --
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

#ifndef GRID2D_HPP_
#define GRID2D_HPP_

#include <math.h>
#include "../calc/abstract_populator.hpp"
#include "../calc/abstract_rng.hpp"
#include "../util/statistics.hpp"

namespace populator {

	class Grid2d : public AbstractPopulator {

		virtual void doPopulate(Network& network) {
			std::vector<std::vector<Point> > points;
			std::vector<std::vector<std::size_t> > xIndices, yIndices;
			Statistics squares;
			std::size_t startCircuit = network.getNumOfCircuits();

			for (unsigned row = 0; row < params.rows; ++row) {
				points.push_back(std::vector<Point>());
				xIndices.push_back(std::vector<std::size_t>());
				yIndices.push_back(std::vector<std::size_t>());

				for (unsigned col = 0; col < params.columns; ++col) {
					const Point p(1.0 * col + params.xRng(), 1.0 * row + params.yRng());
					points[row].push_back(p);

					std::size_t xIndex = 0, yIndex = 0;

					if (col > 0) {
						xIndex = network.addContact(Contact(params.betaRng(), params.tauRng(), params.vRng()));
						network.contact(xIndex).addTag("horizontal");
						if (row == 0) {
							network.contact(xIndex).addTag("bottom");
							network.contact(xIndex).addTag("boundary");
						}
						else if (row == params.rows - 1) {
							network.contact(xIndex).addTag("top");
							network.contact(xIndex).addTag("boundary");
						}
						else {
							network.contact(xIndex).addTag("inner");
						}
						xIndices[row].push_back(xIndex);
					}

					if (row > 0) {
						yIndex = network.addContact(Contact(params.betaRng(), params.tauRng(), params.vRng()));
						network.contact(yIndex).addTag("vertical");
						if (col == 0) {
							network.contact(yIndex).addTag("left");
							network.contact(yIndex).addTag("boundary");
						}
						else if (col == params.columns - 1) {
							network.contact(yIndex).addTag("right");
							network.contact(yIndex).addTag("boundary");
						}
						else {
							network.contact(yIndex).addTag("inner");
						}
						yIndices[row].push_back(yIndex);
					}

					if (col > 0 && row > 0) {
						// add a contour
						Circuit c(rectSquare(
								points[row-1][col-1],
								points[row][col-1],
								points[row][col],
								points[row-1][col]));
						squares.accum(c.square);

						// bottom
						c.addContactRef(ContactRef(xIndices[row-1][col-1], 1.0, -1.0));
						// left
						c.addContactRef(ContactRef(yIndices[row][col-1], -1.0, 1.0));
						// upper
						c.addContactRef(ContactRef(xIndex, -1.0, 1.0));
						// right
						c.addContactRef(ContactRef(yIndex, 1.0, -1.0));

						if (col == 1) {
							c.addTag("left");
							c.addTag("boundary");
						}

						if (col == params.columns - 1) {
							c.addTag("right");
							c.addTag("boundary");
						}

						if (row == 1) {
							c.addTag("bottom");
							c.addTag("boundary");
						}

						if (row == params.rows - 1) {
							c.addTag("top");
							c.addTag("boundary");
						}

						if (col > 1 && col < params.columns - 1 && row > 1 && row < params.rows - 1) {
							c.addTag("inner");
						}

						network.addCircuit(c);
					}
				}
			}

			// normalize circuit squares
			const double meanSquare = squares.getMean();
			double sss = 0;
			for (Network::circuit_iterator i = network.circuitBegin() + startCircuit, last = network.circuitEnd();
					i < last;
					++i) {
				i->square = meanSquare / i->square;
				sss += i->square;
			}
		}

		virtual phlib::Cloneable* doClone() const {
			return new Grid2d(params);
		}

	public:

		struct Params {
			unsigned columns, rows;
			AbstractRng& xRng;
			AbstractRng& yRng;
			AbstractRng& betaRng;
			AbstractRng& tauRng;
			AbstractRng& vRng;

			Params(
					unsigned columns,
					unsigned rows,
					AbstractRng& xRng,
					AbstractRng& yRng,
					AbstractRng& betaRng,
					AbstractRng& tauRng,
					AbstractRng& vRng) :
				columns(columns), rows(rows),
				xRng(xRng), yRng(yRng),
				betaRng(betaRng), tauRng(tauRng), vRng(vRng) {}
		};

		Grid2d(const Params& params) : params(params){
		}

	private:

		const Params params;

		static double rectSquare(const Point& a, const Point& b, const Point& c, const Point& d) {
			return triSquare(a, b, c) + triSquare(a, c, d);
		}

		static double triSquare(const Point& a, const Point& b, const Point& c) {
			return 0.5 * fabs( (b.x - a.x) * (c.y - a.y) - (c.x - a.x) * (b.y - a.y) );
		}

	};
}

#endif /* GRID2D_HPP_ */
