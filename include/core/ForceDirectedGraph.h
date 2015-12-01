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
	Spring(const ForceItemPtr& fi1,const ForceItemPtr& fi2, float k, float len) :
			item1(fi1), item2(fi2), coeff(k), length(len) {
	}
};

typedef std::shared_ptr<Spring> SpringPtr;
struct Force {
	virtual void init(const ForceSimulator& fsim)=0;
	virtual int getParameterCount() const=0;
	virtual float getParameter(int i) const=0;
	virtual float getMinValue(int param) const=0;
	virtual float getMaxValue(int param) const=0;
	virtual std::string getParameterName(int i) const=0;
	virtual void setParameter(int i, float val)=0;
	virtual void setMinValue(int i, float val)=0;
	virtual void setMaxValue(int i, float val)=0;
	virtual bool isSpringForce() const=0;
	virtual bool isItemForce() const=0;
	virtual void getForce(ForceItem& item)=0;
	virtual void getForce(Spring& spring)=0;
};

typedef std::shared_ptr<Force> ForcePtr;

struct Integrator {
	virtual void integrate(ForceSimulator& sim, uint64_t timestep)=0;
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
	void runSimulator(uint64_t timestep);
};
}
}
#endif /* INCLUDE_CORE_FORCEDIRECTEDGRAPH_H_ */
