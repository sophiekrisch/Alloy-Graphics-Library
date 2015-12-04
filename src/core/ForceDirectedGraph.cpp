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
#include "AlloyContext.h"
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

const std::string GravitationalForce::pnames[2] = { "GravitationalConstant",
		"Direction" };
const float GravitationalForce::DEFAULT_FORCE_CONSTANT = 1E-4f;
const float GravitationalForce::DEFAULT_MIN_FORCE_CONSTANT = 1E-5f;
const float GravitationalForce::DEFAULT_MAX_FORCE_CONSTANT = 1E-3f;
const float GravitationalForce::DEFAULT_DIRECTION = (float) ALY_PI / 2.0f;
const float GravitationalForce::DEFAULT_MIN_DIRECTION = (float) -ALY_PI;
const float GravitationalForce::DEFAULT_MAX_DIRECTION = (float) ALY_PI;

const std::string NBodyForce::pnames[3] = { "GravitationalConstant", "Distance",
		"BarnesHutTheta" };
const float NBodyForce::DEFAULT_GRAV_CONSTANT = -1.0f;
const float NBodyForce::DEFAULT_MIN_GRAV_CONSTANT = -10.0f;
const float NBodyForce::DEFAULT_MAX_GRAV_CONSTANT = 10.0f;
const float NBodyForce::DEFAULT_DISTANCE = -1.0f;
const float NBodyForce::DEFAULT_MIN_DISTANCE = -1.0f;
const float NBodyForce::DEFAULT_MAX_DISTANCE = 500.0f;
const float NBodyForce::DEFAULT_THETA = 0.9f;
const float NBodyForce::DEFAULT_MIN_THETA = 0.0f;
const float NBodyForce::DEFAULT_MAX_THETA = 1.0f;

const float ForceSimulator::RADIUS = 20.0f;
void ForceItem::draw(AlloyContext* context) {
	const float lineWidth = 4.0f;
	NVGcontext* nvg = context->nvgContext;
	nvgStrokeWidth(nvg, lineWidth);
	nvgStrokeColor(nvg, Color(context->theme.HIGHLIGHT));
	nvgFillColor(nvg, Color(255,64,64));
	nvgStrokeWidth(nvg, lineWidth);
	nvgBeginPath(nvg);
	float r = ForceSimulator::RADIUS;
	if (shape == NodeShape::Circle) {
		nvgCircle(nvg, location.x,
			location.y,r-lineWidth*0.5f);
	}
	else if (shape == NodeShape::Square) {
		nvgRoundedRect(nvg, location.x + lineWidth * 0.5f,
			location.y + lineWidth * 0.5f,
			2*r - lineWidth,
			2 * r - lineWidth, r * 0.5f);
	}
	else if (shape == NodeShape::Triangle) {
		nvgLineJoin(nvg, NVG_ROUND);
		nvgMoveTo(nvg, location.x -r,location.y-r- lineWidth);
		nvgLineTo(nvg, location.x + r + lineWidth * 0.5f,location.y + r + lineWidth * 0.5f);
		nvgLineTo(nvg, location.x-r - lineWidth,location.y -r+ lineWidth * 0.5f);
		nvgClosePath(nvg);
	}
	else if (shape == NodeShape::Hexagon) {
		nvgLineJoin(nvg, NVG_ROUND);
		float cx = location.x;
		float cy = location.y;
		static const float SCALE = 1.0f / std::sqrt(0.75f);
		float rx = (r- lineWidth * 0.5f) * SCALE;
		float ry = (r - lineWidth * 0.5f);
		nvgMoveTo(nvg, cx + rx, cy);
		nvgLineTo(nvg, cx + rx * 0.5f, cy - ry);
		nvgLineTo(nvg, cx - rx * 0.5f, cy - ry);
		nvgLineTo(nvg, cx - rx * 0.5f, cy - ry);
		nvgLineTo(nvg, cx - rx, cy);
		nvgLineTo(nvg, cx - rx * 0.5f, cy + ry);
		nvgLineTo(nvg, cx + rx * 0.5f, cy + ry);
		nvgClosePath(nvg);
	}
	nvgFill(nvg);
	nvgStroke(nvg);
}
void SpringItem::draw(AlloyContext* context) {
	NVGcontext* nvg = context->nvgContext;
	nvgStrokeColor(nvg, context->theme.NEUTRAL);
	nvgStrokeWidth(nvg, 4.0f);
	nvgLineCap(nvg, NVG_ROUND);
	nvgBeginPath(nvg);
	float2 objectPt = item2->location;
	float2 subjectPt = item1->location;
	nvgMoveTo(nvg, objectPt.x, objectPt.y);
	nvgLineTo(nvg, subjectPt.x, subjectPt.y);
	nvgStroke(nvg);
}

ForceSimulator::ForceSimulator(const std::string& name, const AUnit2D& pos, const AUnit2D& dims, const std::shared_ptr<Integrator>& integr) 
	:Region(name, pos, dims),integrator(integr) {

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
	std::lock_guard<std::mutex> lockMe(lock);
	if (f->isForceItem()) {
		iforces.push_back(f);
	}
	if (f->isSpringItem()) {
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
	std::lock_guard<std::mutex> lockMe(lock);
	items.push_back(item);
}

bool ForceSimulator::removeItem(ForceItemPtr item) {
	std::lock_guard<std::mutex> lockMe(lock);
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

std::vector<SpringItemPtr>& ForceSimulator::getSprings() {
	return springs;
}
SpringItemPtr ForceSimulator::addSpring(const ForceItemPtr& item1,
		const ForceItemPtr& item2, float coeff, float length) {
	SpringItemPtr s = SpringItemPtr(
			new SpringItem(item1, item2, coeff, length));
	addSpring(s);
	return s;
}
void ForceSimulator::addSpring(const SpringItemPtr& spring) {
	std::lock_guard<std::mutex> lockMe(lock);
	springs.push_back(spring);
}
SpringItemPtr ForceSimulator::addSpring(const ForceItemPtr& item1,
		const ForceItemPtr& item2, float length) {
	return addSpring(item1, item2, -1.f, length);
}

SpringItemPtr ForceSimulator::addSpring(const ForceItemPtr& item1,
		const ForceItemPtr& item2) {
	return addSpring(item1, item2, -1.f, -1.f);
}
void ForceSimulator::draw(AlloyContext* context) {
	for (SpringItemPtr item : springs) {
		item->draw(context);
	}
	for (ForceItemPtr item : items) {
		item->draw(context);
	}
}
void ForceSimulator::accumulate() {
	//std::cout << "Forces " << iforces.size() << " " << sforces.size() << " " << items.size() << std::endl;
	for (ForcePtr f : iforces) {
		f->init(*this);
	}
	for (ForcePtr f : sforces) {
		f->init(*this);
	}
	for (ForceItemPtr item : items) {
		item->force = float2(0.0f);
		for (ForcePtr f : iforces) {
			f->getForce(item);
		}
	}
	for (SpringItemPtr s : springs) {
		for (ForcePtr f : sforces){
			f->getSpring(s);
		}
	}
}
void ForceSimulator::runSimulator(float timestep) {
	std::lock_guard<std::mutex> lockMe(lock);
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
void SpringForce::getSpring(const SpringItemPtr& s) {
	ForceItemPtr item1 = s->item1;
	ForceItemPtr item2 = s->item2;
	float len = (s->length < 0 ? params[SPRING_LENGTH] : s->length);
	float2 p1 = item1->location;
	float2 p2 = item2->location;
	float2 dxy = p2 - p1;
	float r = aly::length(dxy);
	if (r == 0.0f) {
		dxy = float2(RandomUniform(-0.5f, 0.5f) / 50.0f,
				RandomUniform(-0.5f, 0.5f) / 50.0f);
		r = aly::length(dxy);
	}
	float d = r - len;
	float coeff = (s->coeff < 0 ? params[SPRING_COEFF] : s->coeff) * d / r;
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
void WallForce::getForce(const ForceItemPtr& item) {
	float2 n = item->location;
	int ccw = relativeCCW(p1.x, p1.y, p2.x, p2.y, n.x, n.y);
	float r = (float) std::sqrt(ptSegDistSq(p1.x, p1.y, p2.x, p2.y, n.x, n.y));
	if (r == 0.0)
		r = (float) RandomUniform(0.0f, 0.01f);
	float v = params[GRAVITATIONAL_CONST] * item->mass / (r * r * r);
	if (n.x >= std::min(p1.x, p2.x) && n.x <= std::max(p1.x, p2.x))
		item->force.y += ccw * v * dxy.x;
	if (n.y >= std::min(p1.y, p2.y) && n.y <= std::max(p1.y, p2.y))
		item->force.x += -1.0f * ccw * v * dxy.y;
}
void CircularWallForce::getForce(const ForceItemPtr& item) {
	float2 n = item->location;
	float2 dxy = p - n;
	float d = length(dxy);
	float dr = r - d;
	float c = (dr > 0) ? -1.0f : 1.0f;
	float v = c * params[GRAVITATIONAL_CONST] * item->mass / (dr * dr);
	if (d == 0.0) {
		dxy = float2(RandomUniform(-0.5f, 0.5f) / 50.0f,
				RandomUniform(-0.5f, 0.5f) / 50.0f);
		d = length(dxy);
	}
	item->force += v * dxy / d;
}
void GravitationalForce::getForce(const ForceItemPtr& item) {
	float coeff = params[GRAVITATIONAL_CONST] * item->mass;
	item->force += gDirection * coeff;
}

void NBodyForce::insertHelper(const ForceItemPtr& p, const QuadTreeNodePtr& n,
		box2f box) {
	float2 pt = p->location;
	float2 split = box.center();
	int i = (pt.x >= split.x ? 1 : 0) + (pt.y >= split.y ? 2 : 0);
	// create new child node, if necessary
	if (n->children[i].get() == nullptr) {
		n->children[i] = QuadTreeNodePtr(new QuadTreeNode());
		n->hasChildren = true;
	}
	// update bounds
	if (i == 1 || i == 3)
		box.position.x = split.x;
	else
		box.dimensions.x = split.x - box.position.x;
	if (i > 1)
		box.position.y = split.y;
	else
		box.dimensions.y = split.y - box.position.y;
	insert(p, n->children[i], box);
}

void NBodyForce::insert(const ForceItemPtr& p, QuadTreeNodePtr& n, box2f box) {
	// try to insert particle p at node n in the quadtree
	// by construction, each leaf will contain either 1 or 0 particles
	if (n->hasChildren) {
		// n contains more than 1 particle
		insertHelper(p, n, box);
	} else if (n->value.get() != nullptr) {
		// n contains 1 particle
		if (isSameLocation(n->value, p)) {
			insertHelper(p, n, box);
		} else {
			ForceItemPtr v = n->value;
			n->value.reset();
			insertHelper(v, n, box);
			insertHelper(p, n, box);
		}
	} else {
		// n is empty, so is a leaf
		n->value = p;
	}
}
void NBodyForce::clear() {
	root = QuadTreeNodePtr(new QuadTreeNode());
}
void NBodyForce::init(ForceSimulator& fsim) {
	clear();
	float2 p1(1E30f);
	float2 p2(-1E30f);
	for (ForceItemPtr item : fsim.getItems()) {
			float2 p = item->location;
			p1 = aly::min(p, p1);
			p2 = aly::max(p, p2);
	}
	float2 dxy = p2 - p1;
	float2 center = 0.5f*(p1+p2);
	float maxDim = std::max(dxy.x, dxy.y);
	if (dxy.x > dxy.y) {
		p2.y = p1.y + dxy.x;
	} else {
		p2.x = p1.x + dxy.y;
	}
	box2f box = box2f(center - float2(maxDim*0.5f), float2(maxDim));
	setBounds(box);
	for (ForceItemPtr item : fsim.getItems()) {
		insert(item);
	}
	// calculate magnitudes and centers of mass
	calcMass(root);
}
void NBodyForce::insert(ForceItemPtr item) {
	// insert item into the quadtrees
	insert(item, root, bounds);
}

bool NBodyForce::isSameLocation(const ForceItemPtr& f1,
		const ForceItemPtr& f2) const {
	float2 dxy = aly::abs(f1->location - f2->location);
	return (dxy.x < 0.01f && dxy.y < 0.01f);
}
void NBodyForce::calcMass(const QuadTreeNodePtr& n) {
	float2 com(0.0f);
	n->mass = 0;
	if (n->hasChildren) {
		for (int i = 0; i < (int) n->children.size(); i++) {
			if (n->children[i].get() != nullptr) {
				calcMass(n->children[i]);
				n->mass += n->children[i]->mass;
				com += n->children[i]->mass * n->children[i]->com;
			}
		}
	}
	if (n->value.get() != nullptr) {
		n->mass += n->value->mass;
		com += n->value->mass * n->value->location;
	}
	n->com = com / n->mass;
}
void NBodyForce::forceHelper(const ForceItemPtr& item, const QuadTreeNodePtr& n,
		const box2f& box) {
	float2 dxy = n->com - item->location;
	float r = length(dxy);
	bool same = false;
	if (r == 0.0f) {
		// if items are in the exact same place, add some noise
		dxy = float2(RandomUniform(-0.5f, 0.5f) / 50.0f,
				RandomUniform(-0.5f, 0.5f) / 50.0f);
		r = length(dxy);
		same = true;
	}
	bool minDist = params[MIN_DISTANCE] > 0.0f && r > params[MIN_DISTANCE];
	// the Barnes-Hut approximation criteria is if the ratio of the
	// size of the quadtree box to the distance between the point and
	// the box's center of mass is beneath some threshold theta.
	if ((!n->hasChildren && n->value.get() != item.get())
			|| (!same && box.dimensions.x / r < params[BARNES_HUT_THETA])) {
		if (minDist)
			return;
		// either only 1 particle or we meet criteria
		// for Barnes-Hut approximation, so calc force
		float v = params[GRAVITATIONAL_CONST] * item->mass * n->mass
				/ (r * r * r);
		item->force += v * dxy;
	} else if (n->hasChildren) {
		// recurse for more accurate calculation
		float2 split = box.center();
		for (int i = 0; i < (int) n->children.size(); i++) {
			if (n->children[i].get() != nullptr) {
				float2 minPt = float2(
						(i == 1 || i == 3 ? split.x : box.position.x),
						(i > 1 ? split.y : box.position.y));
				float2 maxPt = float2(
						(i == 1 || i == 3 ?
								box.position.x + box.dimensions.x : split.x),
						(i > 1 ? box.position.y + box.dimensions.y : split.y));
				forceHelper(item, n->children[i], box2f(minPt, maxPt - minPt));
			}
		}
		if (minDist)
			return;
		if (n->value.get() != nullptr && n->value.get() != item.get()) {
			float v = params[GRAVITATIONAL_CONST] * item->mass * n->value->mass
					/ (r * r * r);
			item->force += v * dxy;
		}
	}
}
void NBodyForce::getForce(const ForceItemPtr& item) {
	forceHelper(item, root, bounds);
}

}
}

