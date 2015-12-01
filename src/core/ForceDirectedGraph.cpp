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
const std::string SpringForce::pnames[2] = { "SpringCoefficient",
		"DefaultSpringLength" };
const float SpringForce::DEFAULT_SPRING_COEFF = 1E-4f;
const float SpringForce::DEFAULT_MAX_SPRING_COEFF = 1E-3f;
const float SpringForce::DEFAULT_MIN_SPRING_COEFF = 1E-5f;
const float SpringForce::DEFAULT_SPRING_LENGTH = 50;
const float SpringForce::DEFAULT_MIN_SPRING_LENGTH = 0;
const float SpringForce::DEFAULT_MAX_SPRING_LENGTH = 200;
const int SpringForce::SPRING_COEFF = 0;
const int SpringForce::SPRING_LENGTH = 1;

const std::string DragForce::pnames[1] = { "DragCoefficient" };
const float DragForce::DEFAULT_DRAG_COEFF = 0.01f;
const float DragForce::DEFAULT_MIN_DRAG_COEFF = 0.0f;
const float DragForce::DEFAULT_MAX_DRAG_COEFF = 0.1f;
const int DragForce::DRAG_COEFF = 0;

const std::string WallForce::pnames[1] = { "GravitationalConstant" };
const float WallForce::DEFAULT_GRAV_CONSTANT = -0.1f;
const float WallForce::DEFAULT_MIN_GRAV_CONSTANT = -1.0f;
const float WallForce::DEFAULT_MAX_GRAV_CONSTANT = 1.0f;
const int WallForce::GRAVITATIONAL_CONST = 0;

const std::string CircularWallForce::pnames[1] = { "GravitationalConstant" };
const float CircularWallForce::DEFAULT_GRAV_CONSTANT = -0.1f;
const float CircularWallForce::DEFAULT_MIN_GRAV_CONSTANT = -1.0f;
const float CircularWallForce::DEFAULT_MAX_GRAV_CONSTANT = 1.0f;

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
void ForceSimulator::runSimulator(float timestep) {
	accumulate();
	integrator->integrate(*this, timestep);
}
void EulerIntegrator::integrate(ForceSimulator& sim, float timestep) const {
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
void RungeKuttaIntegrator::integrate(ForceSimulator& sim,
		float timestep) const {
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
void SpringForce::getForce(Spring& s) {
	ForceItemPtr item1 = s.item1;
	ForceItemPtr item2 = s.item2;
	float length = (s.length < 0 ? params[SPRING_LENGTH] : s.length);
	float2 p1 = item1->location;
	float2 p2 = item2->location;
	float2 dxy = p2 - p1;
	float r = aly::length(dxy);
	if (r == 0.0) {
		dxy = float2(RandomUniform(-0.5f, 0.5f) / 50.0f,
				RandomUniform(-0.5f, 0.5f) / 50.0f);
		r = aly::length(dxy);
	}
	float d = r - length;
	float coeff = (s.coeff < 0 ? params[SPRING_COEFF] : s.coeff) * d / r;
	item1->force += coeff * dxy;
	item2->force -= coeff * dxy;
}
WallForce::WallForce(float gravConst, float2 p1, float2 p2) :
		p1(p1), p2(p2) {
	params = std::vector<float> { gravConst };
	minValues = std::vector<float> { DEFAULT_MIN_GRAV_CONSTANT };
	maxValues = std::vector<float> { DEFAULT_MAX_GRAV_CONSTANT };
	dxy = p2 - p1;
	float r = length(dxy);
	if (dxy.x != 0.0)
		dxy.x /= r;
	if (dxy.y != 0.0)
		dxy.y /= r;
}
int relativeCCW(float x1, float y1, float x2, float y2, float px, float py) {
	x2 -= x1;
	y2 -= y1;
	px -= x1;
	py -= y1;
	float ccw = px * y2 - py * x2;
	if (ccw == 0.0f) {
		ccw = px * x2 + py * y2;
		if (ccw > 0.0f) {
			px -= x2;
			py -= y2;
			ccw = px * x2 + py * y2;
			if (ccw < 0.0f) {
				ccw = 0.0f;
			}
		}
	}
	return (ccw < 0.0f) ? -1 : ((ccw > 0.0f) ? 1 : 0);
}
float ptSegDistSq(float x1, float y1, float x2, float y2, float px, float py) {
	x2 -= x1;
	y2 -= y1;
	px -= x1;
	py -= y1;
	float dotprod = px * x2 + py * y2;
	float projlenSq;
	if (dotprod <= 0.0f) {

		projlenSq = 0.0f;
	} else {

		px = x2 - px;
		py = y2 - py;
		dotprod = px * x2 + py * y2;
		if (dotprod <= 0.0f) {

			projlenSq = 0.0f;
		} else {

			projlenSq = dotprod * dotprod / (x2 * x2 + y2 * y2);
		}
	}
	float lenSq = px * px + py * py - projlenSq;
	if (lenSq < 0) {
		lenSq = 0;
	}
	return lenSq;
}
void WallForce::getForce(ForceItem& item) {
	float2 n = item.location;
	int ccw = relativeCCW(p1.x, p1.y, p2.x, p2.y, n[0], n[1]);
	float r = (float) std::sqrt(
			ptSegDistSq(p1.x, p1.y, p2.x, p2.y, n[0], n[1]));
	if (r == 0.0)
		r = (float) RandomUniform(0.0f, 0.01f);
	float v = params[GRAVITATIONAL_CONST] * item.mass / (r * r * r);
	if (n[0] >= std::min(p1.x, p2.x) && n[0] <= std::max(p1.x, p2.x))
		item.force[1] += ccw * v * dxy.x;
	if (n[1] >= std::min(p1.y, p2.y) && n[1] <= std::max(p1.y, p2.y))
		item.force[0] += -1.0f * ccw * v * dxy.y;
}
void CircularWallForce::getForce(ForceItem& item) {
	float2 n = item.location;
	float2 dxy = p - n;
	float d = length(dxy);
	float dr = r - d;
	float c = (dr > 0) ? -1.0f : 1.0f;
	float v = c*params[GRAVITATIONAL_CONST] * item.mass / (dr*dr);
	if (d == 0.0) {
		dxy = float2(RandomUniform(-0.5f, 0.5f) / 50.0f, RandomUniform(-0.5f, 0.5f) / 50.0f);
		d = length(dxy);
	}
	item.force += v*dxy / d;
}
}
}

