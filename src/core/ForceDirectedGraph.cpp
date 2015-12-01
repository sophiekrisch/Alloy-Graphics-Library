/*
 * Copyright(C) 2015, Blake C. Lucas, Ph.D. (img.science@gmail.com)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "ForceDirectedGraph.h"
namespace aly {
namespace dataflow {
ForceSimulator::ForceSimulator(const std::shared_ptr<Integrator>& integr) :
		iforces(5), sforces(5), iflen(0), sflen(0), integrator(integr) {
}
float ForceSimulator::getSpeedLimit() const {
	return speedLimit;
}
void ForceSimulator::setSpeedLimit(float limit) {
	speedLimit = limit;
}
IntegratorPtr ForceSimulator::getIntegrator() const {
	return integrator;
}
void ForceSimulator::setIntegrator(const IntegratorPtr& intgr) {
	integrator = intgr;
}
void ForceSimulator::clear() {
	items.clear();
	springs.clear();
}
void ForceSimulator::addForce(const ForcePtr& f) {
	if (f->isItemForce()) {
		iforces.push_back(f);
	}
	if (f->isSpringForce()) {
		sforces.push_back(f);
	}
}
std::vector<ForcePtr> ForceSimulator::getForces() const {
	std::vector<ForcePtr> forcesOut;
	forcesOut.insert(forcesOut.end(), iforces.begin(), iforces.end());
	forcesOut.insert(forcesOut.end(), sforces.begin(), sforces.end());
	return forcesOut;
}
void ForceSimulator::addItem(const ForceItemPtr& item) {
	items.push_back(item);
}

bool ForceSimulator::removeItem(ForceItemPtr item) {
	for (size_t i = 0; i < items.size(); i++) {
		if (items[i].get() == item.get()) {
			items.erase(items.begin() + i);
			return true;
		}
	}
	return false;
}

std::vector<ForceItemPtr>& ForceSimulator::getItems() {
	return items;
}

std::vector<SpringPtr>& ForceSimulator::getSprings() {
	return springs;
}
SpringPtr ForceSimulator::addSpring(const ForceItemPtr& item1,
		const ForceItemPtr& item2, float coeff, float length) {
	SpringPtr s = SpringPtr(new Spring(item1, item2, coeff, length));
	springs.push_back(s);
	return s;
}
SpringPtr ForceSimulator::addSpring(const ForceItemPtr& item1,
		const ForceItemPtr& item2, float length) {
	return addSpring(item1, item2, -1.f, length);
}

SpringPtr ForceSimulator::addSpring(const ForceItemPtr& item1,
		const ForceItemPtr& item2) {
	return addSpring(item1, item2, -1.f, -1.f);
}
void ForceSimulator::accumulate() {
	for (int i = 0; i < iflen; i++)
		iforces[i]->init(*this);
	for (int i = 0; i < sflen; i++)
		sforces[i]->init(*this);
	for (ForceItemPtr item : items) {
		item->force = float2(0.0f);
		for (int i = 0; i < iflen; i++)
			iforces[i]->getForce(*item);
	}
	for (SpringPtr s : springs) {
		for (int i = 0; i < sflen; i++) {
			sforces[i]->getForce(*s);
		}
	}
}
void ForceSimulator::runSimulator(uint64_t timestep) {
	accumulate();
	integrator->integrate(*this, timestep);
}
void EulerIntegrator::integrate(ForceSimulator& sim, float timestep) {
	float speedLimit = sim.getSpeedLimit();
	float coeff, len;
	for (ForceItemPtr item : sim.getItems()) {
		item->location += timestep * item->velocity;
		coeff = timestep / item->mass;
		item->velocity += coeff * item->force;
		float2 vel = item->velocity;
		len = length(vel);
		if (len > speedLimit) {
			vel = speedLimit * vel / len;
		}
	}
}
void RungeKuttaIntegrator::integrate(ForceSimulator& sim, float timestep) {
	float speedLimit = sim.getSpeedLimit();
	float coeff;
	float len;
	std::array<float2, 4> k, l;
	for (ForceItemPtr item : sim.getItems()) {
		coeff = timestep / item->mass;
		k = item->k;
		l = item->l;
		item->plocation = item->location;
		k[0] = timestep * item->velocity;
		l[0] = coeff * item->force;
		item->location += 0.5f * k[0];
	}
	sim.accumulate();
	for (ForceItemPtr item : sim.getItems()) {
		coeff = timestep / item->mass;
		k = item->k;
		l = item->l;
		float2 vel = item->velocity + 0.5f * l[0];
		len = length(vel);
		if (len > speedLimit) {
			vel = speedLimit * vel / len;
		}
		k[1] = timestep * vel;
		l[1] = coeff * item->force;

		// Set the position to the new predicted position
		item->location = item->plocation + 0.5f * k[1];
	}

	// recalculate forces
	sim.accumulate();

	for (ForceItemPtr item : sim.getItems()) {
		coeff = timestep / item->mass;
		k = item->k;
		l = item->l;
		float2 vel = item->velocity + 0.5f * l[1];
		len = length(vel);
		if (len > speedLimit) {
			vel = speedLimit * vel / len;
		}
		k[2] = timestep * vel;
		l[2] = coeff * item->force;
		item->location = item->plocation + 0.5f * k[2];
	}
	// recalculate forces
	sim.accumulate();

	for (ForceItemPtr item : sim.getItems()) {
		coeff = timestep / item->mass;
		k = item->k;
		l = item->l;
		float2 p = item->plocation;
		float2 vel = item->velocity + 0.5f * l[1];
		len = length(vel);
		if (len > speedLimit) {
			vel = speedLimit * vel / len;
		}
		k[3] = timestep * vel;
		l[3] = coeff * item->force;
		item->location = p + (k[0] + k[3]) / 6.0f + (k[1] + k[2]) / 3.0f;
		vel = (l[0] + l[3]) / 6.0f + (l[1] + l[2]) / 3.0f;
		len = length(vel);
		if (len > speedLimit) {
			vel = speedLimit * vel / len;
		}
		item->velocity += vel;
	}
}
}
}

