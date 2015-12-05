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
#include "AlloyWorker.h"
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
	Circle = 0, Triangle = 1, Square = 2, Hexagon = 3
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
class Port: public Region {
protected:
	Node* parent;
	std::string label;
	Cursor portCursor;
	Connection* connection;
	virtual void setup();

public:
	friend class Connection;
	friend class Node;
	DataFlow* getGraph() const;
	inline Node* getNode() const {
		return parent;
	}
	void setConnection(Connection* connect) {
		connection = connect;
	}
	bool isConnected() const {
		return (connection != nullptr);
	}
	void setParent(Node* parent) {
		this->parent = parent;
	}
	float2 getLocation() const {
		return getBounds(false).center();
	}
	std::shared_ptr<Port> getReference();
	Port(const std::string& name, const std::string& label) :
			Region(name), parent(nullptr), label(label), portCursor(0xf05b,
					16.0f, NVG_ALIGN_MIDDLE | NVG_ALIGN_CENTER, FontType::Icon,
					0.0f, pixel2(-0.25f, -0.25f)), connection(nullptr) {
		setup();
	}
	Port(const std::string& name) :
			Region(name), parent(nullptr), label(name), portCursor(0xf05b,
					16.0f, NVG_ALIGN_MIDDLE | NVG_ALIGN_CENTER, FontType::Icon,
					0.0f, pixel2(-0.25f, -0.25f)), connection(nullptr) {
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
	std::vector<std::shared_ptr<Connection>> connections;
	std::shared_ptr<Packet> value;
	virtual void setup() override;
public:

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
	std::vector<std::shared_ptr<Connection>> connections;
	std::shared_ptr<Packet> value;
	virtual void setup() override;
public:

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
	std::vector<std::shared_ptr<Connection>> connections;
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
public:
	std::shared_ptr<Port> source;
	std::shared_ptr<Port> destination;
	std::vector<float2> path;
	Connection(const std::shared_ptr<Port>& source,
			const std::shared_ptr<Port>& destination) :
			source(source), destination(destination) {
		source->setConnection(this);
		destination->setConnection(this);
	}
	void draw(AlloyContext* context,DataFlow* flow);
};

class ConnectionBundle: public std::vector<std::shared_ptr<Connection>> {
public:
	ConnectionBundle() :
			std::vector<std::shared_ptr<Connection>>() {
	}

};
class PacketBundle: public std::vector<std::shared_ptr<Packet>> {
public:
	PacketBundle() :
			std::vector<std::shared_ptr<Packet>>() {
	}

};
class InputMultiPort: public Port, MultiPort {
protected:
	ConnectionBundle connections;
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
	ConnectionBundle connections;
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
	std::vector<std::shared_ptr<Connection>> connections;
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
	std::shared_ptr<Node> subject;
	std::shared_ptr<Node> object;
	std::shared_ptr<Predicate> predicate;
	std::shared_ptr<SpringItem>& getSpringItem();
	Relationship(const std::shared_ptr<Node>& object,
			const std::shared_ptr<Predicate>& predicate,
			const std::shared_ptr<Node>& subject) :
			subject(subject), object(object), predicate(predicate) {
		getSpringItem();
	}
	void draw(AlloyContext* context);
	void drawText(AlloyContext* context);

};


class NodeIcon: public Region {
protected:
	NodeShape shape;
public:
	NodeIcon(const std::string& name, const AUnit2D& pos, const AUnit2D& dims) :
			Region(name, pos, dims), shape(NodeShape::Circle) {
	}

	void setShape(const NodeShape& s) {
		shape = s;
	}
	virtual void draw(AlloyContext* context) override;
};
class Node: public Composite {
protected:
	std::string label;
	float fontSize;
	DataFlow* parent;
	bool dragging;
	pixel2 cursorDownPosition;
	std::vector<std::shared_ptr<InputPort>> inputPorts;
	std::vector<std::shared_ptr<OutputPort>> outputPorts;
	std::shared_ptr<InputPort> inputPort;
	std::shared_ptr<OutputPort> outputPort;
	std::shared_ptr<ParentPort> parentPort;
	std::shared_ptr<ChildPort> childPort;
	CompositePtr inputPortComposite;
	CompositePtr outputPortComposite;
	TextLabelPtr labelRegion;
	float textWidth;
	std::shared_ptr<ForceItem> forceItem;

	virtual void setup();
public:
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
	box2px getObstacleBounds() const ;
	DataFlow* getGraph() const {
		return parent;
	}
	virtual NodeType getType() const {
		return NodeType::Unknown;
	}
	virtual bool onEventHandler(AlloyContext* context, const InputEvent& event)
			override;
	bool isMouseOver() const;
	pixel2 getCenter() const {
		return nodeIcon->getBounds().center();
	}
	std::shared_ptr<ForceItem>& getForceItem();
	float getRadius() const {
		return nodeIcon->getBounds().dimensions.y * 0.5f;
	}
	Node(const std::string& name, const pixel2& pt);
	Node(const std::string& name, const std::string& label, const pixel2& pt);
	/*
	Node(const std::string& name, const std::string& label) :
			Composite(name), label(label), parent(nullptr) {
		setup();
	}
	Node(const std::string& name, const std::string& label, const AUnit2D& pos,
			const AUnit2D& dims) :
			Composite(name, pos, dims), label(label), parent(nullptr) {
		setup();
	}
	Node(const std::string& name, const AUnit2D& pos, const AUnit2D& dims) :
			Composite(name, pos, dims), label(name), parent(nullptr) {
		setup();
	}
	*/
	void add(const std::shared_ptr<Region>& region) {
		Composite::add(region);
	}
	void add(const std::shared_ptr<InputPort>& port) {
		inputPortComposite->add(port);
		inputPorts.push_back(port);
		port->parent = this;
	}
	void add(const std::shared_ptr<OutputPort>& port) {
		outputPortComposite->add(port);
		outputPorts.push_back(port);
		port->parent = this;
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
	static const Color COLOR;
	virtual void setup() override;
public:
	virtual NodeType getType() const override {
		return NodeType::Data;
	}
	Data(const std::string& name,const pixel2& pt) :
			Node(name,pt) {
		setup();
	}
	Data(const std::string& name, const std::string& label, const pixel2& pt) :
		Node(name, label, pt) {
		setup();
	}
	/*
	Data(const std::string& name, const std::string& label) :
			Node(name, label) {
		setup();
	}
	Data(const std::string& name, const std::string& label, const AUnit2D& pos,
			const AUnit2D& dims) :
			Node(name, label, pos, dims) {
		setup();
	}
	Data(const std::string& name, const AUnit2D& pos, const AUnit2D& dims) :
			Node(name, pos, dims) {
		setup();
	}
	*/
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
		Node(name,label, pt) {
		setup();
	}
	/*
	View(const std::string& name, const std::string& label) :
			Node(name, label) {
		setup();
	}
	View(const std::string& name, const std::string& label, const AUnit2D& pos,
			const AUnit2D& dims) :
			Node(name, label, pos, dims) {
		setup();
	}
	View(const std::string& name, const AUnit2D& pos, const AUnit2D& dims) :
			Node(name, pos, dims) {
		setup();
	}
	*/
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
	/*
	Compute(const std::string& name, const std::string& label) :
			Node(name, label) {
		setup();
	}
	Compute(const std::string& name, const std::string& label,
			const AUnit2D& pos, const AUnit2D& dims) :
			Node(name, label, pos, dims) {
		setup();
	}
	Compute(const std::string& name, const AUnit2D& pos, const AUnit2D& dims) :
			Node(name, pos, dims) {
		setup();
	}
	*/
	virtual void pack(const pixel2& pos, const pixel2& dims,
			const double2& dpmm, double pixelRatio, bool clamp = false)
					override;
	virtual void draw(AlloyContext* context) override;
};
class Source: public Node {
protected:
	static const Color COLOR;
	virtual void setup() override;

public:
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
	/*
	Source(const std::string& name, const std::string& label) :
			Node(name, label) {
		setup();
	}
	Source(const std::string& name, const std::string& label,
			const AUnit2D& pos, const AUnit2D& dims) :
			Node(name, label, pos, dims) {
		setup();
	}
	Source(const std::string& name, const AUnit2D& pos, const AUnit2D& dims) :
			Node(name, pos, dims) {
		setup();
	}
	*/
	virtual void pack(const pixel2& pos, const pixel2& dims,
		const double2& dpmm, double pixelRatio, bool clamp = false)
		override;
	virtual void draw(AlloyContext* context) override;
};

class Destination: public Node {
protected:
	static const Color COLOR;
	virtual void setup() override;
public:
	virtual NodeType getType() const override {
		return NodeType::Destination;
	}
	Destination(const std::string& name, const pixel2& pt) :
		Node(name, pt) {
		setup();
	}
	Destination(const std::string& name, const std::string& label, const pixel2& pt) :
		Node(name, label, pt) {
		setup();
	}
	/*
	Destination(const std::string& name) :
			Node(name) {
		setup();
	}
	Destination(const std::string& name, const std::string& label) :
			Node(name, label) {
		setup();
	}
	Destination(const std::string& name, const std::string& label,
			const AUnit2D& pos, const AUnit2D& dims) :
			Node(name, label, pos, dims) {
		setup();
	}
	Destination(const std::string& name, const AUnit2D& pos,
			const AUnit2D& dims) :
			Node(name, pos, dims) {
		setup();
	}
	*/
	virtual void pack(const pixel2& pos, const pixel2& dims,
		const double2& dpmm, double pixelRatio, bool clamp = false)
		override;
	virtual void draw(AlloyContext* context) override;
};
class DataFlow: public Composite {
protected:
	std::vector<std::shared_ptr<View>> viewNodes;
	std::vector<std::shared_ptr<Data>> dataNodes;
	std::vector<std::shared_ptr<Compute>> computeNodes;
	std::vector<std::shared_ptr<Source>> sourceNodes;
	std::vector<std::shared_ptr<Destination>> destinationNodes;
	std::vector<std::shared_ptr<Connection>> connections;
	std::vector<std::shared_ptr<Relationship>> relationships;
	std::chrono::steady_clock::time_point lastTime;
	int renderCount=0;
	float frameRate=0.0f;
	Node* mouseOverNode;
	Port* connectingPort;
	Port* currentPort;
	AvoidanceRouting router;
	std::shared_ptr<ForceSimulator> forceSim;
	pixel2 currentDrawOffset;
	pixel2 cursorDownLocation;
	RecurrentTaskPtr simWorker;
	bool dragging = false;
	void setup();
	bool updateSimulation(uint64_t iter);
	void addNode(const std::shared_ptr<Node>& node);
public:
	std::shared_ptr<ForceSimulator> getForceSimulator() {
		return forceSim;
	}
	float getFrameRate() const {
		return frameRate;
	}
	void start();
	virtual void pack(const pixel2& pos, const pixel2& dims, const double2& dpmm,
		double pixelRatio, bool clamp = false) override;
	bool isConnecting() const {
		return (connectingPort != nullptr);
	}
	Port* getConnectingPort() const {
		return connectingPort;
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
	void add(const std::shared_ptr<Connection>& node);

	const std::vector<std::shared_ptr<Data>>& getDataNodes() const {
		return dataNodes;
	}
	const std::vector<std::shared_ptr<View>>& getViewNodes() const {
		return viewNodes;
	}
	const std::vector<std::shared_ptr<Compute>>& getComputeNodes() const {
		return computeNodes;
	}
	const std::vector<std::shared_ptr<Source>>& getSourceNodes() const {
		return sourceNodes;
	}
	const std::vector<std::shared_ptr<Destination>>& getDestinationNodes() const {
		return destinationNodes;
	}

	std::vector<std::shared_ptr<Data>> getDataNodes() {
		return dataNodes;
	}
	std::vector<std::shared_ptr<View>> getViewNodes() {
		return viewNodes;
	}
	std::vector<std::shared_ptr<Compute>> getComputeNodes() {
		return computeNodes;
	}
	std::vector<std::shared_ptr<Source>> getSourceNodes() {
		return sourceNodes;
	}
	std::vector<std::shared_ptr<Destination>>& getDestinationNodes() {
		return destinationNodes;
	}
	std::vector<std::shared_ptr<Connection>>& getConnections() {
		return connections;
	}
	const std::vector<std::shared_ptr<Connection>>& getConnections() const {
		return connections;
	}
};
class Group: public Node {
protected:
	std::shared_ptr<DataFlow> graph;
public:
	NodeType getType() const override {
		return NodeType::Group;
	}
	Group(const std::string& name, const pixel2& pt) :
		Node(name, pt) {
		setup();
	}
	/*
	Group(const std::string& name, const std::string& label) :
			Node(name, label) {
	}
	Group(const std::string& name, const std::string& label, const AUnit2D& pos,
			const AUnit2D& dims) :
			Node(name, label, pos, dims) {
	}
	Group(const std::string& name, const AUnit2D& pos, const AUnit2D& dims) :
			Node(name, pos, dims) {
	}
	*/
	void setGraph(std::shared_ptr<DataFlow>& graph) {
		this->graph = graph;
	}
	std::shared_ptr<DataFlow> getGraph() {
		return graph;
	}
};
std::shared_ptr<Connection> MakeConnection(
		const std::shared_ptr<Port>& source,
		const std::shared_ptr<Port>& destination);
std::shared_ptr<Relationship> MakeRelationship(
		const std::shared_ptr<Node>& object,
		const std::shared_ptr<Predicate>& predicate,
		const std::shared_ptr<Node>& subject);
std::shared_ptr<Relationship> MakeRelationship(
		const std::shared_ptr<Node>& object, const std::string& predicate,
		const std::shared_ptr<Node>& subject);
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
