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
#include "AlloyDataFlow.h"
namespace aly {
namespace dataflow {
const int MultiPort::FrontIndex = std::numeric_limits<int>::min();
const int MultiPort::BackIndex = std::numeric_limits<int>::max();
const pixel2 Node::DEFAULT_NODE_DIMENSIONS = pixel2(256, 256);

std::shared_ptr<DataFlow> MakeDataFlow(const std::string& name, const AUnit2D& pos,
		const AUnit2D& dims) {
	return DataFlowPtr(new DataFlow(name, pos, dims));
}
std::shared_ptr<Connection> MakeConnection(
		const std::shared_ptr<OutputPort>& source,
		const std::shared_ptr<InputPort>& destination) {
	return ConnectionPtr(new Connection(source, destination));
}
std::shared_ptr<Relationship> MakeRelationship(
		const std::shared_ptr<Node>& object,
		const std::shared_ptr<Predicate>& predicate,
		const std::shared_ptr<Node>& subject) {
	return RelationshipPtr(new Relationship(object, predicate, subject));
}
std::shared_ptr<Data> MakeDataNode(const std::string& name,
		const std::string& label, const pixel2& pos) {
	return DataPtr(
			new Data(name, label, CoordPX(pos),
					CoordPX(Node::DEFAULT_NODE_DIMENSIONS)));
}
std::shared_ptr<Data> MakeDataNode(const std::string& name, const pixel2& pos) {
	return DataPtr(
			new Data(name, CoordPX(pos),
					CoordPX(Node::DEFAULT_NODE_DIMENSIONS)));
}
std::shared_ptr<Data> MakeDataNode(const std::string& name,
		const std::string& label) {
	return DataPtr(
			new Data(name, label, CoordPX(0, 0),
					CoordPX(Node::DEFAULT_NODE_DIMENSIONS)));
}
std::shared_ptr<Data> MakeDataNode(const std::string& name) {
	return DataPtr(
			new Data(name, CoordPX(0, 0),
					CoordPX(Node::DEFAULT_NODE_DIMENSIONS)));
}

std::shared_ptr<View> MakeViewNode(const std::string& name,
		const std::string& label, const pixel2& pos) {
	return ViewPtr(
			new View(name, label, CoordPX(pos),
					CoordPX(Node::DEFAULT_NODE_DIMENSIONS)));
}
std::shared_ptr<View> MakeViewNode(const std::string& name, const pixel2& pos) {
	return ViewPtr(
			new View(name, CoordPX(pos),
					CoordPX(Node::DEFAULT_NODE_DIMENSIONS)));
}
std::shared_ptr<View> MakeViewNode(const std::string& name,
		const std::string& label) {
	return ViewPtr(
			new View(name, label, CoordPX(0, 0),
					CoordPX(Node::DEFAULT_NODE_DIMENSIONS)));
}
std::shared_ptr<View> MakeViewNode(const std::string& name) {
	return ViewPtr(
			new View(name, CoordPX(0, 0),
					CoordPX(Node::DEFAULT_NODE_DIMENSIONS)));
}

std::shared_ptr<Compute> MakeComputeNode(const std::string& name,
		const std::string& label, const pixel2& pos) {
	return ComputePtr(
			new Compute(name, label, CoordPX(pos),
					CoordPX(Node::DEFAULT_NODE_DIMENSIONS)));
}
std::shared_ptr<Compute> MakeComputeNode(const std::string& name,
		const pixel2& pos) {
	return ComputePtr(
			new Compute(name, CoordPX(pos),
					CoordPX(Node::DEFAULT_NODE_DIMENSIONS)));
}
std::shared_ptr<Compute> MakeComputeNode(const std::string& name,
		const std::string& label) {
	return ComputePtr(
			new Compute(name, label, CoordPX(0, 0),
					CoordPX(Node::DEFAULT_NODE_DIMENSIONS)));
}
std::shared_ptr<Compute> MakeComputeNode(const std::string& name) {
	return ComputePtr(
			new Compute(name, CoordPX(0, 0),
					CoordPX(Node::DEFAULT_NODE_DIMENSIONS)));
}
void OutputMultiPort::insertValue(const std::shared_ptr<Packet>& packet,
		int index) {
	if (index == FrontIndex) {
		value.insert(value.begin(), packet);
	} else if (index == BackIndex) {
		value.push_back(packet);
	} else {
		if (index >= (int) value.size()) {
			value.push_back(packet);
		} else {
			value.insert(value.begin() + (size_t) index, packet);
		}
	}
}
void OutputMultiPort::setValue(const std::shared_ptr<Packet>& packet,
		int index) {
	if (index < 0) {
		throw std::runtime_error("Cannot set port value. Index out of range.");
	} else {
		if (index >= (int) value.size()) {
			value.resize(index + 1);
		}
		value[index] = packet;
	}
}
void InputMultiPort::insertValue(const std::shared_ptr<Packet>& packet,
		int index) {
	if (index == FrontIndex) {
		value.insert(value.begin(), packet);
	} else if (index == BackIndex) {
		value.push_back(packet);
	} else {
		if (index >= (int) value.size()) {
			value.push_back(packet);
		} else {
			value.insert(value.begin() + (size_t) index, packet);
		}
	}
}
void InputMultiPort::setValue(const std::shared_ptr<Packet>& packet,
		int index) {
	if (index < 0) {
		throw std::runtime_error("Cannot set port value. Index out of range.");
	} else {
		if (index >= (int) value.size()) {
			value.resize(index + 1);
		}
		value[index] = packet;
	}
}
void InputMultiPort::setValue(const std::shared_ptr<Packet>& packet) {
	insertAtBack(packet);
}
void OutputMultiPort::setValue(const std::shared_ptr<Packet>& packet) {
	insertAtBack(packet);
}
}
}

