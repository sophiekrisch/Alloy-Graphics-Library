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
	Unknown = 0, Data = 1, View = 2, Compute = 3, Group = 4
};
enum class PortType {
	Unknown = 0, Input = 1, Output = 2
};
struct PacketInterface: public AnyInterface {
protected:
	virtual void setValueImpl(Any const & value) = 0;
	virtual Any getValueImpl() const = 0;
};
template<class T> class Packet: public PacketInterface {
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
	Packet(const std::string& name, const T& value) :
			value(value), name(name) {
	}
	Packet(const std::string& name = "") :
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
	std::string name;
	std::string label;
public:
	friend class Connection;
	Port(const std::string& name, const std::string& label) :
			name(name), label(label) {

	}
	Port(const std::string& name = "") :
			name(name), label(name) {

	}
	Port() :
			name(""), label("") {

	}
	virtual PortType getType() const {
		return PortType::Unknown;
	}
	std::string getName() const {
		return name;
	}
	std::string getLabel() const {
		return label;
	}
	void setName(const std::string& name) {
		this->name = name;
	}
	void setLabel(const std::string& label) {
		this->label = label;
	}
};
struct MultiPort {
};
class InputPort: public Port {
protected:
	std::vector<std::shared_ptr<Connection>> connections;
public:
	virtual PortType getType() const override {
		return PortType::Input;
	}
};

class OutputPort: public Port {
protected:
	std::vector<std::shared_ptr<Connection>> connections;
public:
	virtual PortType getType() const override {
		return PortType::Output;
	}
};
class Connection {
	std::shared_ptr<OutputPort> source;
	std::shared_ptr<InputPort> destination;
};

class ConnectionBundle: public std::map<int, std::shared_ptr<Connection>> {
public:
	ConnectionBundle() :
			std::map<int, std::shared_ptr<Connection>>() {

	}
};
class InputMultiPort: public Port, MultiPort {
protected:
	ConnectionBundle connections;
public:
	virtual PortType getType() const override {
		return PortType::Input;
	}
};
class OutputMultiPort: public Port, MultiPort {
protected:
	ConnectionBundle connections;
public:
	virtual PortType getType() const override {
		return PortType::Output;
	}
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
};
class Node: public Composite {
protected:
	std::string name;
public:
	virtual NodeType getType() const {
		return NodeType::Unknown;
	}
	Node(const std::string& name) :
			name(name) {
	}
	std::string getName() const {
		return name;
	}
	void setName(const std::string& name) {
		this->name = name;
	}
};
class Data: public Node {
public:
	virtual NodeType getType() const override {
		return NodeType::Data;
	}
	Data(const std::string& name) :
			Node(name) {
	}
};
class View: public Node {
public:
	virtual NodeType getType() const override {
		return NodeType::View;
	}
	View(const std::string& name) :
			Node(name) {
	}
};
class Compute: public Node {
public:
	virtual NodeType getType() const override {
		return NodeType::Compute;
	}
	Compute(const std::string& name) :
			Node(name) {
	}
};
class Group: public Node {
public:
	NodeType getType() const override {
		return NodeType::Group;
	}
	Group(const std::string& name) :
			Node(name) {
	}
};
}
}
#endif /* INCLUDE_CORE_ALLOYUALGRAPH_H_ */
