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
	virtual bool isItemForce() const override {
		return true;
	}
	virtual std::string getParameterName(size_t i) const override {
		return pnames[i];
	}
	virtual void getForce(ForceItem& item) override;
};

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
	GravitationalForce(float forceConstant, float direction) {
		params = std::vector<float> { forceConstant, direction };
		minValues = std::vector<float> { DEFAULT_MIN_FORCE_CONSTANT,
				DEFAULT_MIN_DIRECTION };
		maxValues = std::vector<float> { DEFAULT_MAX_FORCE_CONSTANT,
				DEFAULT_MAX_DIRECTION };
	}
	GravitationalForce() :
			GravitationalForce(DEFAULT_FORCE_CONSTANT, DEFAULT_DIRECTION) {
	}
	virtual bool isItemForce() const override {
		return true;
	}
	virtual std::string getParameterName(size_t i) const override {
		return pnames[i];
	}
	virtual void getForce(ForceItem& item) override;
};

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
/*
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
 //private float xMin, xMax, yMin, yMax;
 box2f bounds;
 //private QuadTreeNodeFactory factory = new QuadTreeNodeFactory();private QuadTreeNode root;
 //private Random rand = new Random(12345678L); // deterministic randomness

 NBodyForce(float gravConstant, float minDistance, float theta) {
 params = {gravConstant, minDistance, theta};
 minValues = {DEFAULT_MIN_GRAV_CONSTANT,
 DEFAULT_MIN_DISTANCE, DEFAULT_MIN_THETA};
 maxValues = {DEFAULT_MAX_GRAV_CONSTANT,
 DEFAULT_MAX_DISTANCE, DEFAULT_MAX_THETA};
 //root = factory.getQuadTreeNode();
 }
 NBodyForce():NBodyForce(DEFAULT_GRAV_CONSTANT, DEFAULT_DISTANCE, DEFAULT_THETA) {
 }
 virtual bool isItemForce() const override {
 return true;
 }
 virtual std::string getParameterName(size_t i) const override {
 return pnames[i];
 }
 void setBounds(const box2f& b) {
 bounds=b;
 }
 void clear() {
 root=QuadTreeNodePtr(new QuadTreeNode());
 }
 void insert(const ForceItemPtr& p, QuadTreeNode& n,
 float x1, float y1, float x2, float y2)
 {
 // try to insert particle p at node n in the quadtree
 // by construction, each leaf will contain either 1 or 0 particles
 if ( n.hasChildren ) {
 // n contains more than 1 particle
 insertHelper(p,n,x1,y1,x2,y2);
 } else if ( n.value != null ) {
 // n contains 1 particle
 if ( isSameLocation(n.value, p) ) {
 insertHelper(p,n,x1,y1,x2,y2);
 } else {
 ForceItem v = n.value; n.value = null;
 insertHelper(v,n,x1,y1,x2,y2);
 insertHelper(p,n,x1,y1,x2,y2);
 }
 } else {
 // n is empty, so is a leaf
 n.value = p;
 }
 }
 virtual void init(ForceSimulator& fsim) override {
 clear(); // clear internal state
 // compute and squarify bounds of quadtree
 float x1 = Float.MAX_VALUE, y1 = Float.MAX_VALUE;
 float x2 = Float.MIN_VALUE, y2 = Float.MIN_VALUE;
 Iterator itemIter = fsim.getItems();
 while ( itemIter.hasNext() ) {
 ForceItem item = (ForceItem)itemIter.next();
 float x = item.location[0];
 float y = item.location[1];
 if ( x < x1 ) x1 = x;
 if ( y < y1 ) y1 = y;
 if ( x > x2 ) x2 = x;
 if ( y > y2 ) y2 = y;
 }
 float dx = x2-x1, dy = y2-y1;
 if ( dx > dy ) {y2 = y1 + dx;} else {x2 = x1 + dy;}
 setBounds(x1,y1,x2,y2);

 // insert items into quadtree
 itemIter = fsim.getItems();
 while ( itemIter.hasNext() ) {
 ForceItem item = (ForceItem)itemIter.next();
 insert(item);
 }

 // calculate magnitudes and centers of mass
 calcMass(root);
 }

 virtual void insert(ForceItemPtr item) override {
 // insert item into the quadtrees
 try {
 insert(item, root, xMin, yMin, xMax, yMax);
 } catch ( StackOverflowError e ) {
 // TODO: safe to remove?
 e.printStackTrace();
 }
 }

 private static boolean isSameLocation(ForceItem f1, ForceItem f2) {
 float dx = Math.abs(f1.location[0]-f2.location[0]);
 float dy = Math.abs(f1.location[1]-f2.location[1]);
 return ( dx < 0.01 && dy < 0.01 );
 }

 private void insertHelper(ForceItem p, QuadTreeNode n,
 float x1, float y1, float x2, float y2)
 {
 float x = p.location[0], y = p.location[1];
 float splitx = (x1+x2)/2;
 float splity = (y1+y2)/2;
 int i = (x>=splitx ? 1 : 0) + (y>=splity ? 2 : 0);
 // create new child node, if necessary
 if ( n.children[i] == null ) {
 n.children[i] = factory.getQuadTreeNode();
 n.hasChildren = true;
 }
 // update bounds
 if ( i==1 || i==3 ) x1 = splitx; else x2 = splitx;
 if ( i > 1 ) y1 = splity; else y2 = splity;
 // recurse
 insert(p,n.children[i],x1,y1,x2,y2);
 }

 private void calcMass(QuadTreeNode n) {
 float xcom = 0, ycom = 0;
 n.mass = 0;
 if ( n.hasChildren ) {
 for ( int i=0; i < n.children.length; i++ ) {
 if ( n.children[i] != null ) {
 calcMass(n.children[i]);
 n.mass += n.children[i].mass;
 xcom += n.children[i].mass * n.children[i].com[0];
 ycom += n.children[i].mass * n.children[i].com[1];
 }
 }
 }
 if ( n.value != null ) {
 n.mass += n.value.mass;
 xcom += n.value.mass * n.value.location[0];
 ycom += n.value.mass * n.value.location[1];
 }
 n.com[0] = xcom / n.mass;
 n.com[1] = ycom / n.mass;
 }

 public void getForce(ForceItem item) {
 try {
 forceHelper(item,root,xMin,yMin,xMax,yMax);
 } catch ( StackOverflowError e ) {
 // TODO: safe to remove?
 e.printStackTrace();
 }
 }

 private void forceHelper(ForceItem item, QuadTreeNode n,
 float x1, float y1, float x2, float y2)
 {
 float dx = n.com[0] - item.location[0];
 float dy = n.com[1] - item.location[1];
 float r = (float)Math.sqrt(dx*dx+dy*dy);
 boolean same = false;
 if ( r == 0.0f ) {
 // if items are in the exact same place, add some noise
 dx = (rand.nextFloat()-0.5f) / 50.0f;
 dy = (rand.nextFloat()-0.5f) / 50.0f;
 r = (float)Math.sqrt(dx*dx+dy*dy);
 same = true;
 }
 boolean minDist = params[MIN_DISTANCE]>0f && r>params[MIN_DISTANCE];

 // the Barnes-Hut approximation criteria is if the ratio of the
 // size of the quadtree box to the distance between the point and
 // the box's center of mass is beneath some threshold theta.
 if ( (!n.hasChildren && n.value != item) ||
 (!same && (x2-x1)/r < params[BARNES_HUT_THETA]) )
 {
 if ( minDist ) return;
 // either only 1 particle or we meet criteria
 // for Barnes-Hut approximation, so calc force
 float v = params[GRAVITATIONAL_CONST]*item.mass*n.mass
 / (r*r*r);
 item.force[0] += v*dx;
 item.force[1] += v*dy;
 } else if ( n.hasChildren ) {
 // recurse for more accurate calculation
 float splitx = (x1+x2)/2;
 float splity = (y1+y2)/2;
 for ( int i=0; i<n.children.length; i++ ) {
 if ( n.children[i] != null ) {
 forceHelper(item, n.children[i],
 (i==1||i==3?splitx:x1), (i>1?splity:y1),
 (i==1||i==3?x2:splitx), (i>1?y2:splity));
 }
 }
 if ( minDist ) return;
 if ( n.value != null && n.value != item ) {
 float v = params[GRAVITATIONAL_CONST]*item.mass*n.value.mass
 / (r*r*r);
 item.force[0] += v*dx;
 item.force[1] += v*dy;
 }
 }
 }
 };
 */
}
}
#endif /* INCLUDE_CORE_FORCEDIRECTEDGRAPH_H_ */
