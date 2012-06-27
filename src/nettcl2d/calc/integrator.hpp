/*
 * integrator.hpp
 *
 *  Created on: 21.06.2012
 *      Author: andrey
 */

#ifndef CALC_INTEGRATOR_HPP_
#define CALC_INTEGRATOR_HPP_

#include <stdexcept>
#include <vector>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_odeiv.h>
#include <phlib/cloneable.hpp>
#include "network.hpp"
#include "abstract_tracer.hpp"
#include "abstract_perturbator.hpp"

class Integrator : public phlib::Cloneable {

	typedef std::vector<AbstractTracer*> TracerVector;
	typedef std::vector<AbstractPerturbator*> PerturbatorVector;

	class IntegrationError : std::runtime_error {

		static std::string makeMsg(const int err) {
			std::stringstream s;
			s << "Error integrating ODE system: " << err;
			return s.str();
		}

	public:

		IntegrationError(const int err) : std::runtime_error(makeMsg(err)) {}

	};

	struct CircuitRef {
		int index;
		double gain;

		CircuitRef() : index(-1) {}
	};
	typedef std::pair<CircuitRef, CircuitRef> CircuitRefPair;
	typedef std::vector<CircuitRefPair> CircuitRefPairVector;

	Integrator(const Integrator& src) : params(src.params) {}

	virtual phlib::Cloneable* doClone() const {
		return new Integrator(*this);
	}

public:

	struct Params {
		double step;
		double delta;

		Params() :
			step(1.0e-6),
			delta(1.0e-6)
		{}
	};

	Integrator(const Params& params) : params(params) {}

	void run(Network& network, double const startTime, double const endTime, double const dt) {
		const static int numOfEqs = network.getNumOfContacts() * 2;
		this->network = &network;
		gsl_odeiv_step* s = gsl_odeiv_step_alloc(gsl_odeiv_step_rkf45, numOfEqs);
		gsl_odeiv_control* c = gsl_odeiv_control_y_new(params.delta, 0.0);
		gsl_odeiv_evolve* e = gsl_odeiv_evolve_alloc(numOfEqs);
		gsl_odeiv_system sys = {&solver, NULL, numOfEqs, this};

		beforeRun(network, startTime, endTime, dt);

		buildCircuitRefs();
		y.resize(numOfEqs);
		getYValues(network);

		unsigned long timeSteps = 1;
		for (double time = startTime; time <= endTime; ++timeSteps) {
			double h = params.step;
			double t = time;

			time = startTime + timeSteps * dt;

			// start integration loop
			while (t < time) {
				const int status = ::gsl_odeiv_evolve_apply(e, c, s, &sys, &t, time, &h, &y[0]);
				if (status != GSL_SUCCESS) {
					throw IntegrationError(status);
				}
			}
			// integration completed

			setYValues(network);
			afterIteration(network, time);
		}

		afterRun(network);
	}

	void addTracer(AbstractTracer& tracer) {
		tracers.push_back(&tracer);
	}

	void purgeTracers() {
		tracers.clear();
	}

	void addPerturbator(AbstractPerturbator& perturbator) {
		perturbators.push_back(&perturbator);
	}

	void purgePerturbators() {
		perturbators.clear();
	}

	Params& getParams() {
		return params;
	}

private:

	Params params;
	TracerVector tracers;
	PerturbatorVector perturbators;
	std::vector<double> y;
	Network* network;
	CircuitRefPairVector circuitRefs;
	std::vector<double> circuitPhases;

	void beforeRun(Network& network, double const startTime, double const endTime, double const dt) {
		for (PerturbatorVector::iterator i = perturbators.begin(), last = perturbators.end(); i != last; ++i) {
			(*i)->beforeRun(network, startTime, endTime, dt);
		}

		for (TracerVector::iterator i = tracers.begin(), last = tracers.end(); i != last; ++i) {
			(*i)->beforeRun(network, startTime, endTime, dt);
		}
	}

	void afterRun(Network& network) {
		for (TracerVector::iterator i = tracers.begin(), last = tracers.end(); i != last; ++i) {
			(*i)->afterRun(network);
		}

		for (PerturbatorVector::iterator i = perturbators.begin(), last = perturbators.end(); i != last; ++i) {
			(*i)->afterRun(network);
		}
	}

	void afterIteration(const Network& network, double const time) {
		for (TracerVector::iterator i = tracers.begin(), last = tracers.end(); i != last; ++i) {
			(*i)->afterIteration(network, time);
		}
	}

	void getYValues(const Network& network) {
		std::vector<double>::iterator i = y.begin();
		for (Network::contact_const_iterator c = network.contactBegin(), last = network.contactEnd(); c != last; ++c) {
			*i++ = c->phase;
			*i++ = c->voltage;
		}
	}

	void setYValues(Network& network) {
		std::vector<double>::const_iterator i = y.begin();
		for (Network::contact_iterator c = network.contactBegin(), last = network.contactEnd(); c != last; ++c) {
			c->phase = *i++;
			c->voltage = *i++;
		}
	}

	static int solver(const double t, const double y[], double f[], void* params) {
		return reinterpret_cast<Integrator*>(params)->solverImpl(t, y, f);
	}

	int solverImpl(const double t, const double y[], double f[]) {
		const static double twoPi = 2.0 * 3.1415926535897932384626433832795;

		buildCircuitPhases();

		int i = 0;
		CircuitRefPairVector::const_iterator ref = circuitRefs.begin();
		for (
				Network::contact_const_iterator c = network->contactBegin(), end = network->contactEnd();
				c != end;
				++c, ++ref, i += 2) {
			/*
			 * y[i]    : phi(t)
			 * y[i + 1]: u(t)
			 * f[i]    : d(phi)/dt
			 * f[i + 1]: d(u)/dt
			 */
			f[i] = y[i + 1];
			f[i + 1] = 1.0 / c->beta * (
				twoPi * c->z
				+ calcCircuits(*ref)
				- c->tau * y[i + 1]
				- c->v * sin(y[i])
			);
		}

		return GSL_SUCCESS;
	}

	double calcCircuits(const CircuitRefPair& refs) const {
		return calcCircuit(refs.first) + calcCircuit(refs.second);
	}

	double calcCircuit(const CircuitRef& ref) const {
		if (ref.index < 0) {
			return 0.0;
		}

		return circuitPhases[ref.index] * ref.gain;
	}

	void buildCircuitRefs() {
		circuitRefs.clear();
		circuitRefs.resize(network->getNumOfContacts());

		for (
				Network::circuit_const_iterator first = network->circuitBegin(), circuit = first, end = network->circuitEnd();
				circuit != end;
				++circuit) {

			for (Circuit::const_iterator ci = circuit->begin(), last = circuit->end(); ci != last; ++ci) {
				CircuitRefPair& pair = circuitRefs[ci->index];
				CircuitRef& ref = pair.first.index < 0 ? pair.first : pair.second;
				ref.index = std::distance(first, circuit);
				ref.gain = ci->gain;
			}
		}
	}

	void buildCircuitPhases() {
		circuitPhases.resize(network->getNumOfCircuits());
		std::vector<double>::iterator dest = circuitPhases.begin();
		for (
				Network::circuit_const_iterator first = network->circuitBegin(), circuit = first, end = network->circuitEnd();
				circuit != end;
				++circuit, ++dest) {

			double sum = 0.0;
			for (Circuit::const_iterator ci = circuit->begin(), last = circuit->end(); ci != last; ++ci) {
				const Contact& contact = network->contact(ci->index);
				sum += contact.phase * ci->weight;
			}
			*dest = sum * circuit->square;
		}
	}
};


#endif /* CALC_INTEGRATOR_HPP_ */
