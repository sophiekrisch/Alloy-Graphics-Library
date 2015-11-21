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

enum class NodeType {
	Unknown = 0, Node = 1, Data = 2, View = 3, Compute = 4, Group = 5
};
enum class PortType {
	Unknown = 0, Input = 1, Output = 2
};
class Packet {
public:
	Packet() {
	}
};
class Port: public Region {
public:
	std::string name;
	std::string label;
	virtual PortType getType() const {
		return PortType::Unknown;
	}
};
struct VectorPort {
};

class InputPort: public Port {
public:
	virtual PortType getType() const override {
		return PortType::Input;
	}
};

class OutputPort: public Port {
public:
	virtual PortType getType() const override {
		return PortType::Output;
	}
};
class InputMultiPort: public InputPort, VectorPort {

};
class OutputMultiPort: public OutputPort, VectorPort {

};
class Path {
	std::shared_ptr<OutputPort> source;
	std::shared_ptr<InputPort> destination;
};
class Node: public Composite {
protected:
	std::string name;
public:
	virtual NodeType getType() const {
		return NodeType::Node;
	}
	Node(const std::string& name) :
			name(name) {

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
