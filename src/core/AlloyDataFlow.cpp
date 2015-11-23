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
const pixel2 Node::DEFAULT_DIMENSIONS = pixel2(160, 88);
const pixel2 InputPort::DEFAULT_DIMENSIONS = pixel2(12, 12);
const pixel2 OutputPort::DEFAULT_DIMENSIONS = pixel2(12, 11);
std::shared_ptr<InputPort> MakeInputPort(const std::string& name) {
	return InputPortPtr(new InputPort(name));
}
std::shared_ptr<OutputPort> MakeOutputPort(const std::string& name) {
	return OutputPortPtr(new OutputPort(name));
}
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
					CoordPX(Node::DEFAULT_DIMENSIONS)));
}
std::shared_ptr<Data> MakeDataNode(const std::string& name, const pixel2& pos) {
	return DataPtr(
			new Data(name, CoordPX(pos), CoordPX(Node::DEFAULT_DIMENSIONS)));
}
std::shared_ptr<Data> MakeDataNode(const std::string& name,
		const std::string& label) {
	return DataPtr(
			new Data(name, label, CoordPX(0, 0),
					CoordPX(Node::DEFAULT_DIMENSIONS)));
}
std::shared_ptr<Data> MakeDataNode(const std::string& name) {
	return DataPtr(
			new Data(name, CoordPX(0, 0), CoordPX(Node::DEFAULT_DIMENSIONS)));
}

std::shared_ptr<View> MakeViewNode(const std::string& name,
		const std::string& label, const pixel2& pos) {
	return ViewPtr(
			new View(name, label, CoordPX(pos),
					CoordPX(Node::DEFAULT_DIMENSIONS)));
}
std::shared_ptr<View> MakeViewNode(const std::string& name, const pixel2& pos) {
	return ViewPtr(
			new View(name, CoordPX(pos), CoordPX(Node::DEFAULT_DIMENSIONS)));
}
std::shared_ptr<View> MakeViewNode(const std::string& name,
		const std::string& label) {
	return ViewPtr(
			new View(name, label, CoordPX(0, 0),
					CoordPX(Node::DEFAULT_DIMENSIONS)));
}
std::shared_ptr<View> MakeViewNode(const std::string& name) {
	return ViewPtr(
			new View(name, CoordPX(0, 0), CoordPX(Node::DEFAULT_DIMENSIONS)));
}

std::shared_ptr<Compute> MakeComputeNode(const std::string& name,
		const std::string& label, const pixel2& pos) {
	return ComputePtr(
			new Compute(name, label, CoordPX(pos),
					CoordPX(Node::DEFAULT_DIMENSIONS)));
}
std::shared_ptr<Compute> MakeComputeNode(const std::string& name,
		const pixel2& pos) {
	return ComputePtr(
			new Compute(name, CoordPX(pos), CoordPX(Node::DEFAULT_DIMENSIONS)));
}
std::shared_ptr<Compute> MakeComputeNode(const std::string& name,
		const std::string& label) {
	return ComputePtr(
			new Compute(name, label, CoordPX(0, 0),
					CoordPX(Node::DEFAULT_DIMENSIONS)));
}
std::shared_ptr<Compute> MakeComputeNode(const std::string& name) {
	return ComputePtr(
			new Compute(name, CoordPX(0, 0), CoordPX(Node::DEFAULT_DIMENSIONS)));
}

std::shared_ptr<Source> MakeSourceNode(const std::string& name,
		const std::string& label, const pixel2& pos) {
	return SourcePtr(
			new Source(name, label, CoordPX(pos),
					CoordPX(Node::DEFAULT_DIMENSIONS)));
}
std::shared_ptr<Source> MakeSourceNode(const std::string& name,
		const pixel2& pos) {
	return SourcePtr(
			new Source(name, CoordPX(pos), CoordPX(Node::DEFAULT_DIMENSIONS)));
}
std::shared_ptr<Source> MakeSourceNode(const std::string& name,
		const std::string& label) {
	return SourcePtr(
			new Source(name, label, CoordPX(0, 0),
					CoordPX(Node::DEFAULT_DIMENSIONS)));
}
std::shared_ptr<Source> MakeSourceNode(const std::string& name) {
	return SourcePtr(
			new Source(name, CoordPX(0, 0), CoordPX(Node::DEFAULT_DIMENSIONS)));
}

std::shared_ptr<Destination> MakeDestinationNode(const std::string& name,
		const std::string& label, const pixel2& pos) {
	return DestinationPtr(
			new Destination(name, label, CoordPX(pos),
					CoordPX(Node::DEFAULT_DIMENSIONS)));
}
std::shared_ptr<Destination> MakeDestinationNode(const std::string& name,
		const pixel2& pos) {
	return DestinationPtr(
			new Destination(name, CoordPX(pos),
					CoordPX(Node::DEFAULT_DIMENSIONS)));
}
std::shared_ptr<Destination> MakeDestinationNode(const std::string& name,
		const std::string& label) {
	return DestinationPtr(
			new Destination(name, label, CoordPX(0, 0),
					CoordPX(Node::DEFAULT_DIMENSIONS)));
}
std::shared_ptr<Destination> MakeDestinationNode(const std::string& name) {
	return DestinationPtr(
			new Destination(name, CoordPX(0, 0),
					CoordPX(Node::DEFAULT_DIMENSIONS)));
}
void Node::setup() {
	radius = 0.5f * (Node::DEFAULT_DIMENSIONS.y- OutputPort::DEFAULT_DIMENSIONS.y-InputPort::DEFAULT_DIMENSIONS.y);
	labelRegion = TextLabelPtr(
			new TextLabel(label,
					CoordPX(radius*2.0f,
							2*InputPort::DEFAULT_DIMENSIONS.y + 1.0f),
					CoordPerPX(1.0f, 1.0f, -2.0f*radius,
							-2*OutputPort::DEFAULT_DIMENSIONS.y
									- 2*InputPort::DEFAULT_DIMENSIONS.y - 2.0f)));
	labelRegion->setAlignment(HorizontalAlignment::Left,
			VerticalAlignment::Middle);
	labelRegion->setAspectRule(AspectRule::Unspecified);
	labelRegion->fontSize = UnitPX(20.0f);
	labelRegion->fontType = FontType::Bold;
	borderWidth = UnitPX(3.0f);
	Composite::add(labelRegion);
	inputPortComposite = CompositePtr(
			new Composite("Input Ports",
					CoordPX(2*radius, InputPort::DEFAULT_DIMENSIONS.y),
					CoordPX(0.0f,InputPort::DEFAULT_DIMENSIONS.y)));
	outputPortComposite = CompositePtr(
			new Composite("Output Ports",
					CoordPX(2*radius,Node::DEFAULT_DIMENSIONS.y - 2*OutputPort::DEFAULT_DIMENSIONS.y),
					CoordPX(0.0f, OutputPort::DEFAULT_DIMENSIONS.y)));

	inputPort = MakeInputPort("Input");
	outputPort = MakeOutputPort("Output");

	inputPort->position = CoordPX(
			radius - InputPort::DEFAULT_DIMENSIONS.x * 0.5f, 0.0f);
	outputPort->position = CoordPerPX(0.0f, 1.0f,
			radius - InputPort::DEFAULT_DIMENSIONS.x * 0.5f,
			-OutputPort::DEFAULT_DIMENSIONS.y);

	inputPortComposite->setOrientation(Orientation::Horizontal, pixel2(2, 0),
			pixel2(2, 0));
	outputPortComposite->setOrientation(Orientation::Horizontal, pixel2(2, 0),
			pixel2(2, 0));

	Composite::add(inputPortComposite);
	Composite::add(outputPortComposite);
	Composite::add(inputPort);
	Composite::add(outputPort);
	setRoundCorners(true);
	setDragEnabled(true);
	Application::addListener(this);
}
bool Node::onEventHandler(AlloyContext* context, const InputEvent& e) {
	bool ret = Composite::onEventHandler(context, e);
	if (e.type == InputType::MouseButton && e.button == GLFW_MOUSE_BUTTON_LEFT
			&& context->isMouseDown(this)) {
		dynamic_cast<Composite*>(parent)->putLast(this);
	}
	return ret;
}
void View::setup() {
	Node::setup();
	color = Color(192, 64, 64);
}
void Data::setup() {
	Node::setup();
	color = Color(64, 192, 64);
}
void Compute::setup() {
	Node::setup();
	color = Color(192, 64, 192);
}
void Source::setup() {
	Node::setup();
	color = Color(192, 128, 64);
}
void Destination::setup() {
	Node::setup();
	color = Color(64, 128, 192);
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
void Port::setup() {
	position = CoordPX(0.0f, 0.0f);
	borderWidth = UnitPX(1.0f);
}
void InputPort::setup() {
	position = CoordPX(0.0f, 0.0f);
	dimensions = CoordPX(InputPort::DEFAULT_DIMENSIONS);
}
void OutputPort::setup() {
	position = CoordPX(0.0f, 0.0f);
	dimensions = CoordPX(OutputPort::DEFAULT_DIMENSIONS);
}
void Port::draw(AlloyContext* context) {
	NVGcontext* nvg = context->nvgContext;
	box2px bounds = getBounds();
	nvgFillColor(nvg, Color(context->theme.LIGHT));
	nvgBeginPath(nvg);
	nvgEllipse(nvg, bounds.position.x + bounds.dimensions.x * 0.5f,
			bounds.position.y + bounds.dimensions.y * 0.5f,
			bounds.dimensions.x * 0.5f, bounds.dimensions.y * 0.5f);
	nvgFill(nvg);
}
void InputPort::draw(AlloyContext* context) {
	NVGcontext* nvg = context->nvgContext;
	box2px bounds = getBounds();
	pixel lineWidth = borderWidth.toPixels(bounds.dimensions.y, context->dpmm.y,
			context->pixelRatio);
	if (context->isMouseOver(this)) {
		nvgFillColor(nvg, Color(context->theme.HIGHLIGHT));
		nvgStrokeColor(nvg, Color(context->theme.HIGHLIGHT));
	} else {
		nvgFillColor(nvg, Color(context->theme.LIGHT));
		nvgStrokeColor(nvg, Color(context->theme.LIGHT));
	}
	nvgStrokeWidth(nvg, lineWidth);
	nvgBeginPath(nvg);
	nvgEllipse(nvg, bounds.position.x + bounds.dimensions.x * 0.5f,
			bounds.position.y + bounds.dimensions.y * 0.5f,
			bounds.dimensions.x * 0.5f - 0.5f * lineWidth,
			bounds.dimensions.y * 0.5f - 0.5f * lineWidth);
	nvgFill(nvg);

	nvgStroke(nvg);
}
void OutputPort::draw(AlloyContext* context) {
	NVGcontext* nvg = context->nvgContext;
	box2px bounds = getBounds();
	pixel lineWidth = borderWidth.toPixels(bounds.dimensions.y, context->dpmm.y,
			context->pixelRatio);
	if (context->isMouseOver(this)) {
		nvgFillColor(nvg, Color(context->theme.HIGHLIGHT));
		nvgStrokeColor(nvg, Color(context->theme.HIGHLIGHT));
	} else {
		nvgFillColor(nvg, Color(context->theme.LIGHT));
		nvgStrokeColor(nvg, Color(context->theme.LIGHT));
	}
	nvgLineCap(nvg, NVG_ROUND);
	nvgLineJoin(nvg, NVG_BEVEL);
	nvgStrokeWidth(nvg, lineWidth);
	nvgBeginPath(nvg);

	nvgMoveTo(nvg, bounds.position.x + bounds.dimensions.x * 0.5f,bounds.position.y + bounds.dimensions.y - lineWidth);
	nvgLineTo(nvg, bounds.position.x+0.5f, bounds.position.y + lineWidth * 0.5f);
	nvgLineTo(nvg, bounds.position.x + bounds.dimensions.x-1.0f,bounds.position.y + lineWidth * 0.5f);
	nvgClosePath(nvg);
	nvgFill(nvg);
	nvgStroke(nvg);

}
void Node::draw(AlloyContext* context) {
	NVGcontext* nvg = context->nvgContext;
	box2px bounds = getBounds();
	pixel lineWidth = borderWidth.toPixels(bounds.dimensions.y, context->dpmm.y,
			context->pixelRatio);
	nvgStrokeColor(nvg, context->theme.LIGHT_TEXT);
	nvgStrokeWidth(nvg, lineWidth);
	nvgLineCap(nvg, NVG_ROUND);
	box2px lbounds = labelRegion->getBounds();
	box2px ibounds = inputPortComposite->getBounds();
	box2px obounds = outputPortComposite->getExtents();
	pixel2 inStart = pixel2(
			bounds.position.x + radius,
			bounds.position.y+2*InputPort::DEFAULT_DIMENSIONS.y+2);
	pixel2 inEnd = pixel2(
			bounds.position.x  + 2*radius
					+ ibounds.dimensions.x,
					bounds.position.y+2*InputPort::DEFAULT_DIMENSIONS.y+ 2);
	pixel2 outStart = pixel2(
			bounds.position.x + radius,
			bounds.position.y+bounds.dimensions.y-2*OutputPort::DEFAULT_DIMENSIONS.y - 1);
	pixel2 outEnd = pixel2(bounds.position.x + 2*radius+ obounds.dimensions.x,
					bounds.position.y+bounds.dimensions.y-2*OutputPort::DEFAULT_DIMENSIONS.y- 1);

	pixel2 labelStart = aly::min(aly::min(lbounds.min(), inStart), outStart);
	pixel2 labelEnd = aly::max(aly::max(lbounds.max(), inEnd), outEnd);
	nvgStrokeWidth(nvg, 2.0f);
	if(inputPorts.size()>0||outputPorts.size()>0){
		nvgStrokeColor(nvg, Color(context->theme.LIGHT_TEXT.toSemiTransparent(0.5f)));
		nvgFillColor(nvg, color.toSemiTransparent(0.5f));
	} else {
		nvgStrokeColor(nvg,Color( COLOR_NONE));//Color(context->theme.LIGHT_TEXT.toSemiTransparent(0.5f))
		nvgFillColor(nvg, Color(context->theme.DARK.toSemiTransparent(0.5f)));
	}
	nvgBeginPath(nvg);
	nvgRoundedRect(nvg, labelStart.x, labelStart.y, labelEnd.x - labelStart.x,labelEnd.y - labelStart.y, context->theme.CORNER_RADIUS);
	nvgFill(nvg);
	nvgBeginPath(nvg);
	nvgRoundedRect(nvg, labelStart.x+1.0f, labelStart.y+1.0f, labelEnd.x - labelStart.x-2.0f,labelEnd.y - labelStart.y-2.0f, context->theme.CORNER_RADIUS);
	nvgStroke(nvg);

	nvgStrokeWidth(nvg, lineWidth);
	if (context->isMouseOver(this)||context->isMouseOver(labelRegion.get())) {
		nvgStrokeColor(nvg, Color(context->theme.HIGHLIGHT));
		nvgFillColor(nvg, color.toLighter(0.25f));
	} else {
		nvgStrokeColor(nvg, Color(context->theme.LIGHT_TEXT));
		nvgFillColor(nvg, color);
	}

	nvgStrokeWidth(nvg, lineWidth);
	nvgBeginPath(nvg);
	nvgCircle(nvg, bounds.position.x + radius,
			bounds.position.y  + radius + InputPort::DEFAULT_DIMENSIONS.y,radius-lineWidth*0.5f-1);
	nvgFill(nvg);
	nvgStroke(nvg);
	Composite::draw(context);
}
void View::draw(AlloyContext* context) {
	Node::draw(context);
}
void Compute::draw(AlloyContext* context) {
	Node::draw(context);
}
void Data::draw(AlloyContext* context) {
	Node::draw(context);
}
void Source::draw(AlloyContext* context) {
	Node::draw(context);
}
void Destination::draw(AlloyContext* context) {
	Node::draw(context);
}
}
}

