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
#include <string>
#include <vector>
#include <memory>
namespace aly {
namespace dataflow {
class Node;
class Data;
class Compute;
class View;
class Connection;
enum class NodeType {
	Unknown = 0,
	Data = 1,
	View = 2,
	Compute = 3,
	Group = 4,
	Source = 5,
	Destination = 6
};
enum class PortType {
	Unknown = 0, Input = 1, Output = 2
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
	std::string label;
	virtual void setup();
public:
	friend class Connection;
	Port(const std::string& name, const std::string& label) :
			Region(name), label(label) {
		setup();
	}
	Port(const std::string& name) :
			Region(name), label(name) {
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
	virtual void setValue(const std::shared_ptr<Packet>& packet)=0;
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
		return PortType::Input;
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
	std::shared_ptr<OutputPort> source;
	std::shared_ptr<InputPort> destination;
	Connection(const std::shared_ptr<OutputPort>& source,
			const std::shared_ptr<InputPort>& destination) :
			source(source), destination(destination) {
	}
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
		return PortType::Input;
	}
	virtual void setValue(const std::shared_ptr<Packet>& packet) override {
		this->value = packet;
	}

	virtual ~ChildPort() {
	}
	virtual void draw(AlloyContext* context) override;
};
class Predicate {
public:
	std::string name;

};
class Relationship {
public:
	std::shared_ptr<Node> subject;
	std::shared_ptr<Node> object;
	std::shared_ptr<Predicate> predicate;
	Relationship(const std::shared_ptr<Node>& object,
			const std::shared_ptr<Predicate>& predicate,
			const std::shared_ptr<Node>& subject) :
			subject(subject), object(object), predicate(predicate) {

	}
};
enum class NodeShape {
	Circle = 0, Triangle = 1, Square = 2
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
	std::vector<std::shared_ptr<InputPort>> inputPorts;
	std::vector<std::shared_ptr<OutputPort>> outputPorts;
	std::shared_ptr<InputPort> inputPort;
	std::shared_ptr<OutputPort> outputPort;
	CompositePtr inputPortComposite;
	CompositePtr outputPortComposite;
	TextLabelPtr labelRegion;
	std::shared_ptr<NodeIcon> nodeIcon;
	virtual void setup();
public:
	static const pixel2 DIMENSIONS;
	virtual NodeType getType() const {
		return NodeType::Unknown;
	}
	Node(const std::string& name) :
			Composite(name), label(name) {
		setup();
	}
	Node(const std::string& name, const std::string& label) :
			Composite(name), label(label) {
		setup();
	}
	Node(const std::string& name, const std::string& label, const AUnit2D& pos,
			const AUnit2D& dims) :
			Composite(name, pos, dims), label(label) {
		setup();
	}
	Node(const std::string& name, const AUnit2D& pos, const AUnit2D& dims) :
			Composite(name, pos, dims), label(name) {
		setup();
	}
	void add(const std::shared_ptr<Region>& region) {
		Composite::add(region);
	}
	void add(const std::shared_ptr<InputPort>& port) {
		inputPortComposite->add(port);
		inputPorts.push_back(port);
	}
	void add(const std::shared_ptr<OutputPort>& port) {
		outputPortComposite->add(port);
		outputPorts.push_back(port);
	}
	std::string getLabel() const {
		return label;
	}
	void setLabel(const std::string& label) {
		this->label = label;
	}
	virtual void draw(AlloyContext* context) override;
	virtual bool onEventHandler(AlloyContext* context, const InputEvent& event)
			override;
};
class Data: public Node {
protected:
	virtual void setup() override;
public:
	virtual NodeType getType() const override {
		return NodeType::Data;
	}
	Data(const std::string& name) :
			Node(name) {
		setup();
	}
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
	virtual void draw(AlloyContext* context) override;
};
class View: public Node {
protected:
	virtual void setup() override;
public:
	virtual NodeType getType() const override {
		return NodeType::View;
	}
	View(const std::string& name) :
			Node(name) {
		setup();
	}
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
	virtual void draw(AlloyContext* context) override;
};
class Compute: public Node {
protected:
	virtual void setup() override;
public:
	virtual NodeType getType() const override {
		return NodeType::Compute;
	}
	Compute(const std::string& name) :
			Node(name) {
		setup();
	}
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
	virtual void draw(AlloyContext* context) override;
};
class Source: public Node {
protected:
	virtual void setup() override;
	std::shared_ptr<ParentPort> parentPort;
	std::shared_ptr<ChildPort> childPort;
public:
	virtual NodeType getType() const override {
		return NodeType::Source;
	}
	Source(const std::string& name) :
			Node(name) {
		setup();
	}
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
	virtual void draw(AlloyContext* context) override;
};

class Destination: public Node {
protected:
	virtual void setup() override;
public:
	virtual NodeType getType() const override {
		return NodeType::Destination;
	}
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
	void setup();
public:
	DataFlow(const std::string& name, const AUnit2D& pos, const AUnit2D& dims) :
			Composite(name, pos, dims) {
		setup();
	}
	void add(const std::shared_ptr<Region>& region) {
		Composite::add(region);
	}
	void add(const std::shared_ptr<Source>& node) {
		Composite::add(node);
		sourceNodes.push_back(node);
	}
	void add(const std::shared_ptr<Destination>& node) {
		Composite::add(node);
		destinationNodes.push_back(node);
	}
	void add(const std::shared_ptr<Data>& node) {
		Composite::add(node);
		dataNodes.push_back(node);
	}
	void add(const std::shared_ptr<View>& node) {
		Composite::add(node);
		viewNodes.push_back(node);
	}
	void add(const std::shared_ptr<Compute>& node) {
		Composite::add(node);
		computeNodes.push_back(node);
	}
	void add(const std::shared_ptr<Connection>& node) {
		connections.push_back(node);
	}

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

	Group(const std::string& name) :
			Node(name) {
	}
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
	void setGraph(std::shared_ptr<DataFlow>& graph) {
		this->graph = graph;
	}
	std::shared_ptr<DataFlow> getGraph() {
		return graph;
	}
};
std::shared_ptr<Connection> MakeConnection(
		const std::shared_ptr<OutputPort>& source,
		const std::shared_ptr<InputPort>& destination);
std::shared_ptr<Relationship> MakeRelationship(
		const std::shared_ptr<Node>& object,
		const std::shared_ptr<Predicate>& predicate,
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