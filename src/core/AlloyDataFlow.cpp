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
#include "AlloyApplication.h"
namespace aly {
namespace dataflow {
const int MultiPort::FrontIndex = std::numeric_limits<int>::min();
const int MultiPort::BackIndex = std::numeric_limits<int>::max();
const pixel2 Node::DEFAULT_NODE_DIMENSIONS = pixel2(64, 64);

std::shared_ptr<DataFlow> MakeDataFlow(const std::string& name,
		const AUnit2D& pos, const AUnit2D& dims) {
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

std::shared_ptr<Source> MakeSourceNode(const std::string& name,
		const std::string& label, const pixel2& pos) {
	return SourcePtr(
			new Source(name, label, CoordPX(pos),
					CoordPX(Node::DEFAULT_NODE_DIMENSIONS)));
}
std::shared_ptr<Source> MakeSourceNode(const std::string& name,
		const pixel2& pos) {
	return SourcePtr(
			new Source(name, CoordPX(pos),
					CoordPX(Node::DEFAULT_NODE_DIMENSIONS)));
}
std::shared_ptr<Source> MakeSourceNode(const std::string& name,
		const std::string& label) {
	return SourcePtr(
			new Source(name, label, CoordPX(0, 0),
					CoordPX(Node::DEFAULT_NODE_DIMENSIONS)));
}
std::shared_ptr<Source> MakeSourceNode(const std::string& name) {
	return SourcePtr(
			new Source(name, CoordPX(0, 0),
					CoordPX(Node::DEFAULT_NODE_DIMENSIONS)));
}

std::shared_ptr<Destination> MakeDestinationNode(const std::string& name,
		const std::string& label, const pixel2& pos) {
	return DestinationPtr(
			new Destination(name, label, CoordPX(pos),
					CoordPX(Node::DEFAULT_NODE_DIMENSIONS)));
}
std::shared_ptr<Destination> MakeDestinationNode(const std::string& name,
		const pixel2& pos) {
	return DestinationPtr(
			new Destination(name, CoordPX(pos),
					CoordPX(Node::DEFAULT_NODE_DIMENSIONS)));
}
std::shared_ptr<Destination> MakeDestinationNode(const std::string& name,
		const std::string& label) {
	return DestinationPtr(
			new Destination(name, label, CoordPX(0, 0),
					CoordPX(Node::DEFAULT_NODE_DIMENSIONS)));
}
std::shared_ptr<Destination> MakeDestinationNode(const std::string& name) {
	return DestinationPtr(
			new Destination(name, CoordPX(0, 0),
					CoordPX(Node::DEFAULT_NODE_DIMENSIONS)));
}
void Node::setup() {
	TextLabelPtr textLabel = TextLabelPtr(
			new TextLabel(label, CoordPX(0.0f, 0.0f),
					CoordPercent(1.0f, 1.0f)));
	textLabel->setAlignment(HorizontalAlignment::Center,
			VerticalAlignment::Middle);
	textLabel->fontSize = UnitPX(24.0f);
	textLabel->fontType = FontType::Bold;
	add(textLabel);
	setRoundCorners(true);
	setDragEnabled(true);
	Application::addListener(this);
}
bool Node::onEventHandler(AlloyContext* context, const InputEvent& e) {
	bool ret = Composite::onEventHandler(context, e);
	if (e.type == InputType::MouseButton && e.button == GLFW_MOUSE_BUTTON_LEFT&& context->isMouseDown(this)) {
		dynamic_cast<Composite*>(parent)->putLast(this);
	}
	return ret;
}
void View::setup() {
	Node::setup();
	backgroundColor = MakeColor(192, 64, 64);
}
void Data::setup() {
	Node::setup();
	backgroundColor = MakeColor(64, 192, 64);
}
void Compute::setup() {
	Node::setup();
	backgroundColor = MakeColor(192, 64, 192);
}
void Source::setup() {
	Node::setup();
	backgroundColor = MakeColor(192, 128, 64);
}
void Destination::setup() {
	Node::setup();
	backgroundColor = MakeColor(64, 128, 192);
}
void DataFlow::setup() {
	setRoundCorners(true);
	backgroundColor = MakeColor(AlloyApplicationContext()->theme.DARK);
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

