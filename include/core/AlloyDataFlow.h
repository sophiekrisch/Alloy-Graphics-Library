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
#ifndef ALLOYUALGRAPH_H_
#define ALLOYUALGRAPH_H_
#include "AlloyAny.h"
#include "AlloyUI.h"
#include "AvoidanceRouting.h"
#include <string>
#include <vector>
#include <memory>
#include <chrono>
namespace aly {
namespace dataflow {
class Node;
class Data;
class Compute;
class View;
class DataFlow;
class Connection;
struct BoxForce;
class ForceSimulator;
struct ForceItem;
struct SpringItem;

enum class NodeType {
	Unknown = 0,
	Data = 1,
	View = 2,
	Compute = 3,
	Group = 4,
	Source = 5,
	Destination = 6
};

enum class NodeShape {
	Hidden = 0,
	Circle = 1,
	CircleGroup = 2,
	Triangle = 3,
	Square = 4,
	Hexagon = 5
};
enum class PortType {
	Unknown = 0, Input = 1, Output = 2, Child = 3, Parent = 4
};
struct Packet: public AnyInterface {
protected:
	virtual void setValueImpl(Any const & value) = 0;
	virtual Any getValueImpl() const = 0;
};
template<class T> class PacketImpl: public Packet {
protected:
	T value;
	std::string name;
	virtual void setValueImpl(Any const & val) override {
		value = AnyCast<T>(val);
	}
	virtual Any getValueImpl() const override {
		return value;
	}
public:
	PacketImpl(const std::string& name, const T& value) :
			value(value), name(name) {
	}
	PacketImpl(const std::string& name = "") :
			name(name) {
	}
	std::string getName() const {
		return name;
	}
	void setName(const std::string& name) {
		this->name = name;
	}
};
class ConnectionBundle: public std::vector<std::shared_ptr<Connection>> {
public:
	ConnectionBundle() :
			std::vector<std::shared_ptr<Connection>>() {
	}

};
class Port: public Region {
protected:
	Node* parent;
	std::string label;
	Port* proxyIn;
	Port* proxyOut;
	virtual void setup();
public:
	friend class Connection;
	friend class Node;
	ConnectionBundle connections;
	DataFlow* getGraph() const;
	inline Node* getNode() const {
		return parent;
	}
	void setProxy(Port* proxy) {
		proxyIn = proxy;
		if (proxy != nullptr) {
			proxy->proxyOut = this;
		}
	}
	void disconnect(Connection* connection);
	bool isExternalized() const {
		return (proxyIn != nullptr);
	}
	bool isConnected() const {
		return (connections.size() > 0);
	}
	void add(const std::shared_ptr<Connection>& connection) {
		connections.push_back(connection);
	}
	void setParent(Node* parent) {
		this->parent = parent;
	}
	float2 getLocation() const;
	std::shared_ptr<Port> getReference();
	Port(const std::string& name, const std::string& label) :
			Region(name), parent(nullptr), label(label), proxyIn(nullptr), proxyOut(
					nullptr) {
		setup();
	}
	Port(const std::string& name) :
			Region(name), parent(nullptr), label(name), proxyIn(nullptr), proxyOut(
					nullptr) {
		setup();
	}
	virtual PortType getType() const {
		return PortType::Unknown;
	}
	std::string getLabel() const {
		return label;
	}
	void setLabel(const std::string& label) {
		this->label = label;
	}
	virtual void draw(AlloyContext* context) override;
	virtual void setValue(const std::shared_ptr<Packet>& packet) {
	}
	;
	virtual ~Port() {
	}
};
class MultiPort {
protected:
	static const int FrontIndex;
	static const int BackIndex;
public:
	virtual void insertValue(const std::shared_ptr<Packet>& packet,
			int index)=0;
	virtual void setValue(const std::shared_ptr<Packet>& packet, int index)=0;
	inline void insertAtFront(const std::shared_ptr<Packet>& packet) {
		insertValue(packet, FrontIndex);
	}
	inline void insertAtBack(const std::shared_ptr<Packet>& packet) {
		insertValue(packet, BackIndex);
	}
	virtual ~MultiPort() {
	}
};
class InputPort: public Port {
protected:
	std::shared_ptr<Packet> value;
	virtual void setup() override;
public:
	friend class Node;
	static const pixel2 DIMENSIONS;
	InputPort(const std::string& name) :
			Port(name) {
		setup();
	}
	InputPort(const std::string& name, const std::string& label) :
			Port(name, label) {
		setup();
	}
	virtual PortType getType() const override {
		return PortType::Input;
	}
	virtual void setValue(const std::shared_ptr<Packet>& packet) override {
		this->value = packet;
	}

	virtual ~InputPort() {
	}
	virtual void draw(AlloyContext* context) override;
};

class OutputPort: public Port {
protected:
	std::shared_ptr<Packet> value;
	virtual void setup() override;
public:
	friend class Node;
	static const pixel2 DIMENSIONS;
	OutputPort(const std::string& name) :
			Port(name) {
		setup();
	}
	OutputPort(const std::string& name, const std::string& label) :
			Port(name, label) {
		setup();
	}
	virtual PortType getType() const override {
		return PortType::Output;
	}
	virtual void setValue(const std::shared_ptr<Packet>& packet) override {
		this->value = packet;
	}
	virtual ~OutputPort() {
	}
	virtual void draw(AlloyContext* context) override;
};

class ParentPort: public Port {
protected:
	std::shared_ptr<Packet> value;
	virtual void setup() override;
public:
	static const pixel2 DIMENSIONS;
	ParentPort(const std::string& name) :
			Port(name) {
		setup();
	}
	ParentPort(const std::string& name, const std::string& label) :
			Port(name, label) {
		setup();
	}
	virtual PortType getType() const override {
		return PortType::Parent;
	}
	virtual void setValue(const std::shared_ptr<Packet>& packet) override {
		this->value = packet;
	}

	virtual ~ParentPort() {
	}
	virtual void draw(AlloyContext* context) override;
};
class Connection {
protected:
	std::shared_ptr<SpringItem> springItem;
public:
	bool selected = false;
	std::shared_ptr<Port> source;
	std::shared_ptr<Port> destination;
	std::vector<float2> path;
	void setSelected(bool b) {
		selected = b;
	}
	bool isSelected() const {
		return selected;
	}
	std::shared_ptr<SpringItem>& getSpringItem();
	Connection(const std::shared_ptr<Port>& source,
			const std::shared_ptr<Port>& destination) :
			source(source), destination(destination) {

	}
	void update();
	~Connection();
	float distance(const float2& pt);
	void draw(AlloyContext* context, DataFlow* flow);
};


class PacketBundle: public std::vector<std::shared_ptr<Packet>> {
public:
	PacketBundle() :
			std::vector<std::shared_ptr<Packet>>() {
	}

};
class InputMultiPort: public Port, MultiPort {
protected:
	PacketBundle value;
public:
	virtual PortType getType() const override {
		return PortType::Input;
	}
	virtual void insertValue(const std::shared_ptr<Packet>& packet, int index)
			override;
	virtual void setValue(const std::shared_ptr<Packet>& packet, int index)
			override;
	virtual void setValue(const std::shared_ptr<Packet>& packet) override;
	virtual ~InputMultiPort() {
	}

};
class OutputMultiPort: public Port, MultiPort {
protected:
	PacketBundle value;
public:
	virtual PortType getType() const override {
		return PortType::Output;
	}
	virtual void insertValue(const std::shared_ptr<Packet>& packet, int index)
			override;
	virtual void setValue(const std::shared_ptr<Packet>& packet, int index)
			override;
	virtual void setValue(const std::shared_ptr<Packet>& packet) override;
	virtual ~OutputMultiPort() {
	}
};

class ChildPort: public Port {
protected:
	std::shared_ptr<Packet> value;
	virtual void setup() override;
public:

	static const pixel2 DIMENSIONS;
	ChildPort(const std::string& name) :
			Port(name) {
		setup();
	}
	ChildPort(const std::string& name, const std::string& label) :
			Port(name, label) {
		setup();
	}
	virtual PortType getType() const override {
		return PortType::Child;
	}
	virtual void setValue(const std::shared_ptr<Packet>& packet) override {
		this->value = packet;
	}

	virtual ~ChildPort() {
	}
	virtual void draw(AlloyContext* context) override;
};
class Predicate {
protected:
	const std::string name;
public:
	Predicate(const std::string& name) :
			name(name) {
	}
	std::string getName() const {
		return name;
	}
};
class Relationship {
protected:
	std::shared_ptr<SpringItem> springItem;
public:
	std::shared_ptr<Node> object;
	std::shared_ptr<Node> subject;
	std::shared_ptr<Predicate> predicate;
	std::shared_ptr<SpringItem>& getSpringItem();
	Relationship(const std::shared_ptr<Node>& subject,
			const std::shared_ptr<Predicate>& predicate,
			const std::shared_ptr<Node>& object) :
			object(object), subject(subject), predicate(predicate) {
	}
	void update();
	void draw(AlloyContext* context);
	void drawText(AlloyContext* context);

};

class NodeIcon: public Region {
protected:
	NodeShape shape;
public:

	bool selected = false;
	NodeIcon(const std::string& name, const AUnit2D& pos, const AUnit2D& dims) :
			Region(name, pos, dims), shape(NodeShape::Circle) {
	}

	void setShape(const NodeShape& s) {
		shape = s;
	}
	NodeShape getShape() const {
		return shape;
	}
	virtual void draw(AlloyContext* context) override;
};
class Node: public Composite {
protected:
	std::string label;
	float fontSize;
	float textWidth;
	pixel2 centerOffset;
	DataFlow* parent;
	std::vector<std::shared_ptr<InputPort>> inputPorts;
	std::vector<std::shared_ptr<OutputPort>> outputPorts;
	std::shared_ptr<InputPort> inputPort;
	std::shared_ptr<OutputPort> outputPort;
	std::shared_ptr<ParentPort> parentPort;
	std::shared_ptr<ChildPort> childPort;
	CompositePtr inputPortComposite;
	CompositePtr outputPortComposite;
	ModifiableLabelPtr labelRegion;
	std::shared_ptr<ForceItem> forceItem;
	virtual void setup();
	void prePack();
	void postPack();
public:
	bool isSelected() const {
		return nodeIcon->selected;
	}
	void setSelected(bool b) {
		nodeIcon->selected = b;
	}
	float2 getLocation() const;
	void setLocation(const float2& pt);
	std::shared_ptr<NodeIcon> nodeIcon;
	friend class DataFlow;
	friend class Port;
	static const pixel2 DIMENSIONS;
	std::shared_ptr<ForceSimulator> getForceSimulator();
	void setParent(DataFlow* parent) {
		this->parent = parent;
	}
	const std::shared_ptr<InputPort>& getInputPort(size_t i) const {
		return inputPorts[i];
	}
	const std::shared_ptr<OutputPort>& getOutputPort(size_t i) const {
		return outputPorts[i];
	}
	const std::vector<std::shared_ptr<InputPort>>& getInputPorts() const {
		return inputPorts;
	}
	const std::vector<std::shared_ptr<OutputPort>>& getOutputPorts() const {
		return outputPorts;
	}
	std::vector<std::shared_ptr<InputPort>>& getInputPorts() {
		return inputPorts;
	}
	std::vector<std::shared_ptr<OutputPort>>& getOutputPorts() {
		return outputPorts;
	}
	const std::shared_ptr<InputPort>& getInputPort() const {
		return inputPort;
	}
	const std::shared_ptr<OutputPort>& getOutputPort() const {
		return outputPort;
	}
	const std::shared_ptr<ParentPort>& getParentPort() const {
		return parentPort;
	}
	const std::shared_ptr<ChildPort>& getChildPort() const {
		return childPort;
	}
	box2px getObstacleBounds() const;
	DataFlow* getGraph() const {
		return parent;
	}
	virtual box2px getBounds(bool includeBounds = true) const override;
	virtual box2px getCursorBounds(bool includeOffset = true) const override;
	virtual pixel2 getDrawOffset() const override;
	virtual NodeType getType() const {
		return NodeType::Unknown;
	}
	bool isMouseOver() const;
	pixel2 getCenter() const {
		return getBoundsPosition() + centerOffset;
	}
	pixel2 getForceOffset() const;
	pixel2 getCenterOffset() const {
		return centerOffset;
	}
	std::shared_ptr<ForceItem>& getForceItem();
	float getRadius() const {
		return nodeIcon->getBounds().dimensions.y * 0.5f;
	}
	Node(const std::string& name, const pixel2& pt);
	Node(const std::string& name, const std::string& label, const pixel2& pt);
	void add(const std::shared_ptr<Region>& region) {
		Composite::add(region);
	}
	std::shared_ptr<InputPort>& set(const std::shared_ptr<InputPort>& port);
	std::shared_ptr<OutputPort>& set(const std::shared_ptr<OutputPort>& port);
	void add(const std::shared_ptr<InputPort>& port) {
		inputPortComposite->add(port);
		inputPorts.push_back(port);
		if (parent == nullptr) {
			port->parent = this;
		} else
			throw std::runtime_error(
					"Could not add port because it already has parent.");
	}
	void add(const std::shared_ptr<OutputPort>& port) {
		outputPortComposite->add(port);
		outputPorts.push_back(port);
		if (parent == nullptr) {
			port->parent = this;
		} else
			throw std::runtime_error(
					"Could not add port because it already has parent.");
	}
	std::string getLabel() const {
		return label;
	}
	void setLabel(const std::string& label) {
		this->label = label;
	}

	virtual void draw(AlloyContext* context) override;
};
class Data: public Node {
protected:
	virtual void setup() override;
public:

	static const Color COLOR;
	virtual NodeType getType() const override {
		return NodeType::Data;
	}
	Data(const std::string& name, const pixel2& pt) :
			Node(name, pt) {
		setup();
	}
	Data(const std::string& name, const std::string& label, const pixel2& pt) :
			Node(name, label, pt) {
		setup();
	}
	virtual void draw(AlloyContext* context) override;
	virtual void pack(const pixel2& pos, const pixel2& dims,
			const double2& dpmm, double pixelRatio, bool clamp = false)
					override;
};
class Group: public Node {
protected:
	virtual void setup() override;
public:
	std::vector<std::shared_ptr<Node>> nodes;
	std::vector<std::shared_ptr<Connection>> connections;
	std::vector<std::shared_ptr<Relationship>> relationships;

	static const Color COLOR;
	virtual NodeType getType() const override {
		return NodeType::Group;
	}
	Group(const std::string& name, const pixel2& pt) :
			Node(name, pt) {
		setup();
	}
	Group(const std::string& name, const std::string& label, const pixel2& pt) :
			Node(name, label, pt) {
		setup();
	}
	virtual void draw(AlloyContext* context) override;
	virtual void pack(const pixel2& pos, const pixel2& dims,
			const double2& dpmm, double pixelRatio, bool clamp = false)
					override;
};
class View: public Node {
protected:
	virtual void setup() override;
public:
	static const Color COLOR;
	virtual NodeType getType() const override {
		return NodeType::View;
	}
	View(const std::string& name, const pixel2& pt) :
			Node(name, pt) {
		setup();
	}
	View(const std::string& name, const std::string& label, const pixel2& pt) :
			Node(name, label, pt) {
		setup();
	}
	virtual void pack(const pixel2& pos, const pixel2& dims,
			const double2& dpmm, double pixelRatio, bool clamp = false)
					override;
	virtual void draw(AlloyContext* context) override;
};
class Compute: public Node {
protected:
	virtual void setup() override;
public:
	static const Color COLOR;
	virtual NodeType getType() const override {
		return NodeType::Compute;
	}
	Compute(const std::string& name, const pixel2& pt) :
			Node(name, pt) {
		setup();
	}
	Compute(const std::string& name, const std::string& label, const pixel2& pt) :
			Node(name, label, pt) {
		setup();
	}
	virtual void pack(const pixel2& pos, const pixel2& dims,
			const double2& dpmm, double pixelRatio, bool clamp = false)
					override;
	virtual void draw(AlloyContext* context) override;
};
class Source: public Node {
protected:
	virtual void setup() override;

public:

	static const Color COLOR;
	virtual NodeType getType() const override {
		return NodeType::Source;
	}
	Source(const std::string& name, const pixel2& pt) :
			Node(name, pt) {
		setup();
	}
	Source(const std::string& name, const std::string& label, const pixel2& pt) :
			Node(name, label, pt) {
		setup();
	}
	virtual void pack(const pixel2& pos, const pixel2& dims,
			const double2& dpmm, double pixelRatio, bool clamp = false)
					override;
	virtual void draw(AlloyContext* context) override;
};

class Destination: public Node {
protected:
	virtual void setup() override;
public:

	static const Color COLOR;
	virtual NodeType getType() const override {
		return NodeType::Destination;
	}
	Destination(const std::string& name, const pixel2& pt) :
			Node(name, pt) {
		setup();
	}
	Destination(const std::string& name, const std::string& label,
			const pixel2& pt) :
			Node(name, label, pt) {
		setup();
	}
	virtual void pack(const pixel2& pos, const pixel2& dims,
			const double2& dpmm, double pixelRatio, bool clamp = false)
					override;
	virtual void draw(AlloyContext* context) override;
};
class DataFlow: public Composite {
protected:
	std::shared_ptr<Group> data;
	std::shared_ptr<BoxForce> boxForce;
	std::list<std::pair<Node*, pixel2>> dragList;
	box2px dragBox;
	box2px graphBounds;
	Node* mouseOverNode;
	Node* mouseDragNode;
	Node* mouseSelectedNode;
	Port* connectingPort;
	Port* currentPort;
	std::mutex routingLock;
	AvoidanceRouting router;
	std::shared_ptr<ForceSimulator> forceSim;
	pixel2 currentDrawOffset;
	pixel2 cursorDownLocation;
	Connection* selectedConnection = nullptr;
	bool draggingGraph = false;
	bool dragAction = false;
	bool draggingNode = false;
	void setup();
	bool updateSimulation(uint64_t iter);
	void addNode(const std::shared_ptr<Node>& node);
	Connection* closestConnection(const float2& pt, float tolernace);

public:
	friend class Node;
	void deleteSelected();
	void groupSelected();
	void ungroupSelected();
	std::shared_ptr<ForceSimulator> getForceSimulator() {
		return forceSim;
	}
	void start();
	void stop();
	virtual void pack(const pixel2& pos, const pixel2& dims,
			const double2& dpmm, double pixelRatio, bool clamp = false)
					override;
	bool isConnecting() const {
		return (connectingPort != nullptr);
	}
	Port* getConnectingPort() const {
		return connectingPort;
	}
	Connection* getSelectedConnection() {
		return selectedConnection;
	}
	void setGroup(const std::shared_ptr<Group>& g);
	void setSelected(Connection* item) {
		selectedConnection = item;
	}
	bool intersects(const lineseg2f& ln);
	void startConnection(Port* port);
	void setCurrentPort(Port* currentPort);
	virtual void draw(AlloyContext* context) override;
	virtual bool onEventHandler(AlloyContext* context, const InputEvent& event)
			override;
	DataFlow(const std::string& name, const AUnit2D& pos, const AUnit2D& dims) :
			Composite(name, pos, dims), mouseOverNode(nullptr), connectingPort(
					nullptr), currentPort(nullptr) {
		setup();
	}
	~DataFlow();
	bool isMouseOverNode(const Node* node) const {
		return (node != nullptr && node == mouseOverNode);
	}
	void add(const std::shared_ptr<Region>& region);
	void add(const std::shared_ptr<Relationship>& relationship);
	void add(const std::shared_ptr<Source>& node);
	void add(const std::shared_ptr<Destination>& node);
	void add(const std::shared_ptr<Data>& node);
	void add(const std::shared_ptr<View>& node);
	void add(const std::shared_ptr<Compute>& node);
	void add(const std::shared_ptr<Group>& node);
	void add(const std::shared_ptr<Connection>& node);

	std::vector<std::shared_ptr<Connection>>& getConnections();
	std::vector<std::shared_ptr<Relationship>>& getRelationships();
	const std::vector<std::shared_ptr<Connection>>& getConnections() const;
	const std::vector<std::shared_ptr<Relationship>>& getRelationships() const;

};
template<class C, class R> std::basic_ostream<C, R> & operator <<(
		std::basic_ostream<C, R> & ss, const Connection& line) {
	return ss << "[" << line.source->getName() << "->"
			<< line.destination->getName() << "]";
}
template<class C, class R> std::basic_ostream<C, R> & operator <<(
		std::basic_ostream<C, R> & ss, const Relationship& line) {
	return ss << "[" << line.subject->getName() << "] "
			<< line.predicate->getName() << " [" << line.object->getName()
			<< "]";
}
std::shared_ptr<Connection> MakeConnection(const std::shared_ptr<Port>& source,
		const std::shared_ptr<Port>& destination);
std::shared_ptr<Relationship> MakeRelationship(
		const std::shared_ptr<Node>& subject,
		const std::shared_ptr<Predicate>& predicate,
		const std::shared_ptr<Node>& object);
std::shared_ptr<Relationship> MakeRelationship(
		const std::shared_ptr<Node>& subject, const std::string& predicate,
		const std::shared_ptr<Node>& object);
std::shared_ptr<Data> MakeDataNode(const std::string& name,
		const std::string& label, const pixel2& pos);
std::shared_ptr<Data> MakeDataNode(const std::string& name, const pixel2& pos);
std::shared_ptr<Data> MakeDataNode(const std::string& name,
		const std::string& label);
std::shared_ptr<Data> MakeDataNode(const std::string& name);

std::shared_ptr<Compute> MakeComputeNode(const std::string& name,
		const std::string& label, const pixel2& pos);
std::shared_ptr<Compute> MakeComputeNode(const std::string& name,
		const pixel2& pos);
std::shared_ptr<Compute> MakeComputeNode(const std::string& name,
		const std::string& label);
std::shared_ptr<Compute> MakeComputeNode(const std::string& name);

std::shared_ptr<Group> MakeGroupNode(const std::string& name,
		const std::string& label, const pixel2& pos);
std::shared_ptr<Group> MakeGroupNode(const std::string& name,
		const pixel2& pos);
std::shared_ptr<Group> MakeGroupNode(const std::string& name,
		const std::string& label);
std::shared_ptr<Group> MakeGroupNode(const std::string& name);

std::shared_ptr<View> MakeViewNode(const std::string& name,
		const std::string& label, const pixel2& pos);
std::shared_ptr<View> MakeViewNode(const std::string& name, const pixel2& pos);
std::shared_ptr<View> MakeViewNode(const std::string& name,
		const std::string& label);
std::shared_ptr<View> MakeViewNode(const std::string& name);

std::shared_ptr<Destination> MakeDestinationNode(const std::string& name,
		const std::string& label, const pixel2& pos);
std::shared_ptr<Destination> MakeDestinationNode(const std::string& name,
		const pixel2& pos);
std::shared_ptr<Destination> MakeDestinationNode(const std::string& name,
		const std::string& label);
std::shared_ptr<Destination> MakeDestinationNode(const std::string& name);

std::shared_ptr<Source> MakeSourceNode(const std::string& name,
		const std::string& label, const pixel2& pos);
std::shared_ptr<Source> MakeSourceNode(const std::string& name,
		const pixel2& pos);
std::shared_ptr<Source> MakeSourceNode(const std::string& name,
		const std::string& label);
std::shared_ptr<Source> MakeSourceNode(const std::string& name);

std::shared_ptr<DataFlow> MakeDataFlow(const std::string& name,
		const AUnit2D& pos, const AUnit2D& dims);

std::shared_ptr<InputPort> MakeInputPort(const std::string& name);
std::shared_ptr<OutputPort> MakeOutputPort(const std::string& name);
std::shared_ptr<ParentPort> MakeParentPort(const std::string& name);
std::shared_ptr<ChildPort> MakeChildPort(const std::string& name);
typedef std::shared_ptr<Node> NodePtr;
typedef std::shared_ptr<View> ViewPtr;
typedef std::shared_ptr<Compute> ComputePtr;
typedef std::shared_ptr<Data> DataPtr;
typedef std::shared_ptr<Destination> DestinationPtr;
typedef std::shared_ptr<Source> SourcePtr;
typedef std::shared_ptr<Group> GroupPtr;
typedef std::shared_ptr<DataFlow> DataFlowPtr;
typedef std::shared_ptr<Port> PortPtr;
typedef std::shared_ptr<InputPort> InputPortPtr;
typedef std::shared_ptr<OutputPort> OutputPortPtr;
typedef std::shared_ptr<MultiPort> MultiPortPtr;
typedef std::shared_ptr<Connection> ConnectionPtr;
typedef std::shared_ptr<ConnectionBundle> ConnectionBundlePtr;
typedef std::shared_ptr<Relationship> RelationshipPtr;
typedef std::shared_ptr<Predicate> PredicatePtr;
typedef std::shared_ptr<Packet> PacketPtr;
typedef std::shared_ptr<NodeIcon> NodeIconPtr;
typedef std::shared_ptr<ParentPort> ParentPortPtr;
typedef std::shared_ptr<ChildPort> ChildPortPtr;
}
}
#endif /* INCLUDE_CORE_ALLOYUALGRAPH_H_ */
