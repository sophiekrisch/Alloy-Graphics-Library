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

//Re-implementation of force directed graph in Prefuse (http://prefuse.org)
//originally written by Jeffrey Heer (http://jheer.org) at UC Berkeley.
#ifndef INCLUDE_CORE_FORCEDIRECTEDGRAPH_H_
#define INCLUDE_CORE_FORCEDIRECTEDGRAPH_H_
#include "ForceDirectedGraph.h"
#include "AlloyMath.h"
#include <vector>
#include <array>
#include <memory>
#include <mutex>
namespace aly {
	class AlloyContext;
namespace dataflow {
class ForceSimulator;

enum class NodeShape {
	Circle = 0, Triangle = 1, Square = 2, Hexagon = 3
};
struct ForceItem {
	float mass;
	float2 force;
	float2 velocity;
	float2 location;
	float2 plocation;
	NodeShape shape;
	std::array<float2, 4> k;
	std::array<float2, 4> l;
	ForceItem() :
			mass(1.0f), force(0.0f), velocity(0.0f), location(0.0f), plocation(
					0.0f),shape(NodeShape::Circle){
	}
	void reset() {
		force = float2(0.0f);
		velocity = float2(0.0f);
		plocation = location;
	}
	void draw(AlloyContext* context);
};
typedef std::shared_ptr<ForceItem> ForceItemPtr;
struct SpringItem {
	ForceItemPtr item1;
	ForceItemPtr item2;
	float coeff;
	float length;
	SpringItem(const ForceItemPtr& fi1, const ForceItemPtr& fi2, float k,
			float len) :
			item1(fi1), item2(fi2), coeff(k), length(len) {
	}
	void draw(AlloyContext* context);
};

typedef std::shared_ptr<SpringItem> SpringItemPtr;

//TODO: use this to wrap parameters.
struct ForceParameter {
	std::string name;
	float value;
	float min;
	float max;
	ForceParameter(const std::string& name, float value, float min, float max) :
			name(name), value(value), min(min), max(max) {
	}
};
template<class C, class R> std::basic_ostream<C, R> & operator <<(
		std::basic_ostream<C, R> & ss, const ForceParameter& param) {
	return ss << param.name << " : " << param.value << " range: [" << param.min
			<< "," << param.max << "]";
}
struct Force {
	std::vector<float> params;
	std::vector<float> minValues;
	std::vector<float> maxValues;
	virtual void init(ForceSimulator& fsim) {
	}
	;
	virtual ~Force() {
	}
	;
	size_t getParameterCount() const {
		return params.size();
	}
	float getParameterValue(size_t i) const {
		return params[i];
	}
	float getMinValue(size_t param) const {
		return minValues[param];
	}
	float getMaxValue(size_t param) const {
		return maxValues[param];
	}
	virtual std::string getParameterName(size_t i) const {
		return std::string();
	}
	ForceParameter getParameter(size_t i) const {
		return ForceParameter(getParameterName(i), getParameterValue(i),
				getMinValue(i), getMaxValue(i));
	}
	virtual void setParameter(size_t i, float val){
		params[i] = val;
	}
	void setMinValue(size_t i, float val) {
		minValues[i] = val;
	}
	void setMaxValue(size_t i, float val) {
		maxValues[i] = val;
	}
	virtual bool isSpringItem() const {
		return false;
	}
	virtual bool isForceItem() const {
		return false;
	}
	virtual void getForce(const ForceItemPtr& item) {
		throw std::runtime_error("Get force item not implemented.");
	}
	;
	virtual void getSpring(const SpringItemPtr& spring) {
		throw std::runtime_error("Get spring item not implemented.");
	}
	;
};

typedef std::shared_ptr<Force> ForcePtr;

struct Integrator {
	virtual void integrate(ForceSimulator& sim, float timestep) const = 0;
	Integrator() {
	}
	;
	virtual ~Integrator() {
	}
};
typedef std::shared_ptr<Integrator> IntegratorPtr;
class ForceSimulator {
	std::mutex lock;
	std::vector<ForceItemPtr> items;
	std::vector<SpringItemPtr> springs;
	std::vector<ForcePtr> iforces;
	std::vector<ForcePtr> sforces;
	std::shared_ptr<Integrator> integrator;
	float speedLimit = 1.0f;
public:
	ForceSimulator(const std::shared_ptr<Integrator>& integr);
	ForceSimulator();
	float getSpeedLimit() const;
	void setSpeedLimit(float limit);
	IntegratorPtr getIntegrator() const;
	void setIntegrator(const IntegratorPtr& intgr);
	void clear();
	void addForce(const ForcePtr& f);
	std::vector<ForcePtr> getForces() const;
	void addItem(const ForceItemPtr& item);
	bool removeItem(ForceItemPtr item);
	std::vector<ForceItemPtr>& getItems();
	std::vector<SpringItemPtr>& getSprings();
	SpringItemPtr addSpring(const ForceItemPtr& item1,
			const ForceItemPtr& item2, float coeff, float length);
	SpringItemPtr addSpring(const ForceItemPtr& item1,
			const ForceItemPtr& item2, float length);
	SpringItemPtr addSpring(const ForceItemPtr& item1,
			const ForceItemPtr& item2);
	void addSpring(const SpringItemPtr& spring);
	void accumulate();
	void runSimulator(float timestep);
};

struct RungeKuttaIntegrator: public Integrator {
	virtual void integrate(ForceSimulator& sim, float timestep) const override;
};
struct EulerIntegrator: public Integrator {
	virtual void integrate(ForceSimulator& sim, float timestep) const override;
};

struct SpringForce: public Force {
	static const std::string pnames[2];
	static const float DEFAULT_SPRING_COEFF;
	static const float DEFAULT_MAX_SPRING_COEFF;
	static const float DEFAULT_MIN_SPRING_COEFF;
	static const float DEFAULT_SPRING_LENGTH;
	static const float DEFAULT_MIN_SPRING_LENGTH;
	static const float DEFAULT_MAX_SPRING_LENGTH;
	static const int SPRING_COEFF;
	static const int SPRING_LENGTH;
	SpringForce(float springCoeff, float defaultLength) {
		params = std::vector<float> { springCoeff, defaultLength };
		minValues = std::vector<float> { DEFAULT_MIN_SPRING_COEFF,
				DEFAULT_MIN_SPRING_LENGTH };
		maxValues = std::vector<float> { DEFAULT_MAX_SPRING_COEFF,
				DEFAULT_MAX_SPRING_LENGTH };
	}
	SpringForce():SpringForce(DEFAULT_SPRING_COEFF, DEFAULT_SPRING_LENGTH){
	}
	virtual bool isSpringItem() const override {
		return true;
	}

	virtual std::string getParameterName(size_t i) const override {
		return pnames[i];
	}
	virtual void getSpring(const SpringItemPtr& s) override;
};
typedef std::shared_ptr<SpringForce> SpringForcePtr;
struct DragForce: public Force {
	static const std::string pnames[1];
	static const float DEFAULT_DRAG_COEFF;
	static const float DEFAULT_MIN_DRAG_COEFF;
	static const float DEFAULT_MAX_DRAG_COEFF;
	static const int DRAG_COEFF;
	DragForce(float dragCoeff) {
		params = std::vector<float> { dragCoeff };
		minValues = std::vector<float> { DEFAULT_MIN_DRAG_COEFF };
		maxValues = std::vector<float> { DEFAULT_MAX_DRAG_COEFF };
	}
	DragForce() :
			DragForce(DEFAULT_DRAG_COEFF) {
	}
	virtual bool isForceItem() const override {
		return true;
	}
	virtual std::string getParameterName(size_t i) const override {
		return pnames[i];
	}
	virtual void getForce(const ForceItemPtr& item) override {
		item->force -= params[DRAG_COEFF] * item->velocity;
	}
};
typedef std::shared_ptr<DragForce> DragForcePtr;
struct WallForce: public Force {
	static const std::string pnames[1];
	static const float DEFAULT_GRAV_CONSTANT;
	static const float DEFAULT_MIN_GRAV_CONSTANT;
	static const float DEFAULT_MAX_GRAV_CONSTANT;
	static const int GRAVITATIONAL_CONST;
	float2 p1, p2, dxy;
	WallForce(float gravConst, float2 p1, float2 p2);
	WallForce(float2 p1, float2 p2) :
			WallForce(DEFAULT_GRAV_CONSTANT, p1, p2) {
	}
	virtual bool isForceItem() const override {
		return true;
	}
	virtual std::string getParameterName(size_t i) const override {
		return pnames[i];
	}

	virtual void getForce(const ForceItemPtr& item) override;
};
typedef std::shared_ptr<WallForce> WallForcePtr;
struct CircularWallForce: public Force {
	static const std::string pnames[1];
	static const float DEFAULT_GRAV_CONSTANT;
	static const float DEFAULT_MIN_GRAV_CONSTANT;
	static const float DEFAULT_MAX_GRAV_CONSTANT;
	static const int GRAVITATIONAL_CONST = 0;
	float2 p;
	float r;
	CircularWallForce(float gravConst, float2 p, float r) :
			p(p), r(r) {
		params = std::vector<float> { gravConst };
		minValues = std::vector<float> { DEFAULT_MIN_GRAV_CONSTANT };
		maxValues = std::vector<float> { DEFAULT_MAX_GRAV_CONSTANT };
	}
	CircularWallForce(float2 p, float r) :
			CircularWallForce(DEFAULT_GRAV_CONSTANT, p, r) {
	}
	virtual bool isForceItem() const override {
		return true;
	}
	virtual std::string getParameterName(size_t i) const override {
		return pnames[i];
	}
	virtual void getForce(const ForceItemPtr& item) override;
};
typedef std::shared_ptr<CircularWallForce> CircularWallForcePtr;
struct GravitationalForce: public Force {
	static const std::string pnames[2];
	static const int GRAVITATIONAL_CONST = 0;
	static const int DIRECTION = 1;
	static const float DEFAULT_FORCE_CONSTANT;
	static const float DEFAULT_MIN_FORCE_CONSTANT;
	static const float DEFAULT_MAX_FORCE_CONSTANT;
	static const float DEFAULT_DIRECTION;
	static const float DEFAULT_MIN_DIRECTION;
	static const float DEFAULT_MAX_DIRECTION;
	float2 gDirection;
	GravitationalForce(float forceConstant, float direction) {
		params = std::vector<float> { forceConstant, direction };
		minValues = std::vector<float> { DEFAULT_MIN_FORCE_CONSTANT,
				DEFAULT_MIN_DIRECTION };
		maxValues = std::vector<float> { DEFAULT_MAX_FORCE_CONSTANT,
				DEFAULT_MAX_DIRECTION };
		float theta = params[DIRECTION];
		gDirection = float2(std::cos(theta), std::sin(theta));
	}
	GravitationalForce() :
			GravitationalForce(DEFAULT_FORCE_CONSTANT, DEFAULT_DIRECTION) {
	}
	virtual void setParameter(size_t i, float val) override {
		params[i] = val;
		if (i == DIRECTION) {
			gDirection = float2(std::cos(val), std::sin(val));
		}
	}
	virtual bool isForceItem() const override {
		return true;
	}
	virtual std::string getParameterName(size_t i) const override {
		return pnames[i];
	}
	virtual void getForce(const ForceItemPtr& item) override;
};
typedef std::shared_ptr<GravitationalForce> GravitationalForcePtr;
struct QuadTreeNode {
	float mass;
	float2 com;
	bool hasChildren;
	ForceItemPtr value;
	std::array<std::shared_ptr<QuadTreeNode>, 4> children;
	QuadTreeNode(float mass = 0.0f, float2 com = float2(0.0f)) :
			mass(mass), com(com), hasChildren(false) {
	}
};
typedef std::shared_ptr<QuadTreeNode> QuadTreeNodePtr;

struct NBodyForce: public Force {
	static const std::string pnames[3];
	static const float DEFAULT_GRAV_CONSTANT;
	static const float DEFAULT_MIN_GRAV_CONSTANT;
	static const float DEFAULT_MAX_GRAV_CONSTANT;
	static const float DEFAULT_DISTANCE;
	static const float DEFAULT_MIN_DISTANCE;
	static const float DEFAULT_MAX_DISTANCE;
	static const float DEFAULT_THETA;
	static const float DEFAULT_MIN_THETA;
	static const float DEFAULT_MAX_THETA;
	static const int GRAVITATIONAL_CONST = 0;
	static const int MIN_DISTANCE = 1;
	static const int BARNES_HUT_THETA = 2;
	QuadTreeNodePtr root;
	box2f bounds;
public:
	NBodyForce(float gravConstant, float minDistance, float theta) {
		params = {gravConstant, minDistance, theta};
		minValues = {DEFAULT_MIN_GRAV_CONSTANT,
			DEFAULT_MIN_DISTANCE, DEFAULT_MIN_THETA};
		maxValues = {DEFAULT_MAX_GRAV_CONSTANT,
			DEFAULT_MAX_DISTANCE, DEFAULT_MAX_THETA};
		root = QuadTreeNodePtr(new QuadTreeNode());
	}
	NBodyForce() :NBodyForce(DEFAULT_GRAV_CONSTANT, DEFAULT_DISTANCE, DEFAULT_THETA) {
	}
	virtual bool isForceItem() const override {
		return true;
	}
	virtual std::string getParameterName(size_t i) const override {
		return pnames[i];
	}
	void setBounds(const box2f& b) {
		bounds = b;
	}
	void clear();
	virtual void init(ForceSimulator& fsim) override;
	void getForce(const ForceItemPtr& item) override;
private:
	void insertHelper(const ForceItemPtr& p, const QuadTreeNodePtr& n, box2f box);
	void insert(const ForceItemPtr& p, QuadTreeNodePtr& n, box2f box);
	void insert(ForceItemPtr item);
	bool isSameLocation(const ForceItemPtr& f1, const ForceItemPtr& f2) const;
	void calcMass(const QuadTreeNodePtr& n);
	void forceHelper(const ForceItemPtr& item, const QuadTreeNodePtr& n, const box2f& box);
};
typedef std::shared_ptr<NBodyForce> NBodyForcePtr;
}
}
#endif /* INCLUDE_CORE_FORCEDIRECTEDGRAPH_H_ */
