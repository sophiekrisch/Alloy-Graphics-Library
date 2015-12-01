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
namespace aly {
namespace dataflow {
class ForceSimulator;
struct ForceItem {
	float mass;
	float2 force;
	float2 velocity;
	float2 location;
	float2 plocation;
	std::array<float2, 4> k;
	std::array<float2, 4> l;
	ForceItem() :
			mass(1.0f), force(0.0f), velocity(0.0f), location(0.0f), plocation(
					0.0f) {
	}
};
typedef std::shared_ptr<ForceItem> ForceItemPtr;
struct Spring {
	ForceItemPtr item1;
	ForceItemPtr item2;
	float coeff;
	float length;
	Spring(const ForceItemPtr& fi1, const ForceItemPtr& fi2, float k, float len) :
			item1(fi1), item2(fi2), coeff(k), length(len) {
	}
};

typedef std::shared_ptr<Spring> SpringPtr;
struct Force {
	std::vector<float> params;
	std::vector<float> minValues;
	std::vector<float> maxValues;
	virtual void init(const ForceSimulator& fsim) {
	}
	;
	virtual ~Force() {
	}
	;
	size_t getParameterCount() const {
		return params.size();
	}
	float getParameter(size_t i) const {
		return params[i];
	}
	float getMinValue(int param) const {
		return minValues[param];
	}
	float getMaxValue(int param) const {
		return maxValues[param];
	}
	virtual std::string getParameterName(size_t i) const {
		return std::string();
	}
	void setParameter(size_t i, float val) {
		params[i] = val;
	}
	void setMinValue(size_t i, float val) {
		minValues[i] = val;
	}
	void setMaxValue(size_t i, float val) {
		maxValues[i] = val;
	}
	virtual bool isSpringForce() const {
		return false;
	}
	virtual bool isItemForce() const {
		return false;
	}
	virtual void getForce(ForceItem& item) {
		throw std::runtime_error("Get force item not implemented.");
	}
	;
	virtual void getForce(Spring& spring) {
		throw std::runtime_error("Get spring not implemented.");
	}
	;
};

typedef std::shared_ptr<Force> ForcePtr;

struct Integrator {
	virtual void integrate(ForceSimulator& sim, float timestep) const =0;
	Integrator() {
	}
	;
	virtual ~Integrator() {
	}
};
typedef std::shared_ptr<Integrator> IntegratorPtr;
class ForceSimulator {
	std::vector<ForceItemPtr> items;
	std::vector<SpringPtr> springs;
	std::vector<ForcePtr> iforces;
	std::vector<ForcePtr> sforces;
	int iflen, sflen;
	std::shared_ptr<Integrator> integrator;
	float speedLimit = 1.0f;
public:
	ForceSimulator(const std::shared_ptr<Integrator>& integr);
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
	std::vector<SpringPtr>& getSprings();
	SpringPtr addSpring(const ForceItemPtr& item1, const ForceItemPtr& item2,
			float coeff, float length);
	SpringPtr addSpring(const ForceItemPtr& item1, const ForceItemPtr& item2,
			float length);
	SpringPtr addSpring(const ForceItemPtr& item1, const ForceItemPtr& item2);
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
	SpringForce() {
		SpringForce(DEFAULT_SPRING_COEFF, DEFAULT_SPRING_LENGTH);
	}
	virtual bool isSpringForce() const override {
		return true;
	}

	virtual std::string getParameterName(size_t i) const override {
		return pnames[i];
	}
	virtual void getForce(Spring& s) override;
};
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
	virtual bool isItemForce() const override {
		return true;
	}
	virtual std::string getParameterName(size_t i) const override {
		return pnames[i];
	}
	virtual void getForce(ForceItem& item) override {
		item.force -= params[DRAG_COEFF] * item.velocity;
	}
};
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
	virtual bool isItemForce() const override {
		return true;
	}
	virtual std::string getParameterName(size_t i) const override {
		return pnames[i];
	}

	virtual void getForce(ForceItem& item) override;
};
struct CircularWallForce: public Force {
	static const std::string pnames[1];
	static const float DEFAULT_GRAV_CONSTANT;
	static const float DEFAULT_MIN_GRAV_CONSTANT ;
	static const float DEFAULT_MAX_GRAV_CONSTANT ;
	static const int GRAVITATIONAL_CONST = 0;
	float2 p;
	float r;
	CircularWallForce(float gravConst,float2 p, float r):p(p),r(r)
	{
		params = std::vector<float>{ gravConst };
		minValues = std::vector<float>{ DEFAULT_MIN_GRAV_CONSTANT };
		maxValues = std::vector<float>{ DEFAULT_MAX_GRAV_CONSTANT };
	}
	CircularWallForce(float2 p, float r):CircularWallForce(DEFAULT_GRAV_CONSTANT,p,r) {
	}
	virtual bool isItemForce() const override {
		return true;
	}
	virtual std::string getParameterName(size_t i) const override {
		return pnames[i];
	}
	virtual void getForce(ForceItem& item) override;
};
}
}
#endif /* INCLUDE_CORE_FORCEDIRECTEDGRAPH_H_ */
