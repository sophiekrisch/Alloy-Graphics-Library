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
const pixel2 Node::DIMENSIONS = pixel2(80, 80);
const pixel2 InputPort::DIMENSIONS = pixel2(12, 12);
const pixel2 OutputPort::DIMENSIONS = pixel2(12, 11);
const pixel2 ParentPort::DIMENSIONS = pixel2(12, 12);
const pixel2 ChildPort::DIMENSIONS = pixel2(11, 12);
const float NODE_SATURATION=0.6f;
const float NODE_LUMINANCE=0.65f;
const Color View::COLOR = HSVAtoColor(HSVA(300 / 360.0f, NODE_SATURATION, NODE_LUMINANCE, 1.0f));
const Color Compute::COLOR = HSVAtoColor(HSVA(0.0f, NODE_SATURATION, NODE_LUMINANCE, 1.0f));
const Color Data::COLOR = HSVAtoColor(HSVA(60.0f / 360.0f, NODE_SATURATION, NODE_LUMINANCE, 1.0f));
const Color Destination::COLOR = HSVAtoColor(HSVA(120.0f / 360.0f, NODE_SATURATION, NODE_LUMINANCE, 1.0f));
const Color Source::COLOR = HSVAtoColor(HSVA(225.0f / 360.0f,NODE_SATURATION, NODE_LUMINANCE, 1.0f));

std::shared_ptr<InputPort> MakeInputPort(const std::string& name) {
	return InputPortPtr(new InputPort(name));
}
std::shared_ptr<OutputPort> MakeOutputPort(const std::string& name) {
	return OutputPortPtr(new OutputPort(name));
}
std::shared_ptr<ParentPort> MakeParentPort(const std::string& name) {
	return ParentPortPtr(new ParentPort(name));
}
std::shared_ptr<ChildPort> MakeChildPort(const std::string& name) {
	return ChildPortPtr(new ChildPort(name));
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
			new Data(name, label, CoordPX(pos), CoordPX(Node::DIMENSIONS)));
}
std::shared_ptr<Data> MakeDataNode(const std::string& name, const pixel2& pos) {
	return DataPtr(new Data(name, CoordPX(pos), CoordPX(Node::DIMENSIONS)));
}
std::shared_ptr<Data> MakeDataNode(const std::string& name,
		const std::string& label) {
	return DataPtr(
			new Data(name, label, CoordPX(0, 0), CoordPX(Node::DIMENSIONS)));
}
std::shared_ptr<Data> MakeDataNode(const std::string& name) {
	return DataPtr(new Data(name, CoordPX(0, 0), CoordPX(Node::DIMENSIONS)));
}

std::shared_ptr<View> MakeViewNode(const std::string& name,
		const std::string& label, const pixel2& pos) {
	return ViewPtr(
			new View(name, label, CoordPX(pos), CoordPX(Node::DIMENSIONS)));
}
std::shared_ptr<View> MakeViewNode(const std::string& name, const pixel2& pos) {
	return ViewPtr(new View(name, CoordPX(pos), CoordPX(Node::DIMENSIONS)));
}
std::shared_ptr<View> MakeViewNode(const std::string& name,
		const std::string& label) {
	return ViewPtr(
			new View(name, label, CoordPX(0, 0), CoordPX(Node::DIMENSIONS)));
}
std::shared_ptr<View> MakeViewNode(const std::string& name) {
	return ViewPtr(new View(name, CoordPX(0, 0), CoordPX(Node::DIMENSIONS)));
}

std::shared_ptr<Compute> MakeComputeNode(const std::string& name,
		const std::string& label, const pixel2& pos) {
	return ComputePtr(
			new Compute(name, label, CoordPX(pos), CoordPX(Node::DIMENSIONS)));
}
std::shared_ptr<Compute> MakeComputeNode(const std::string& name,
		const pixel2& pos) {
	return ComputePtr(
			new Compute(name, CoordPX(pos), CoordPX(Node::DIMENSIONS)));
}
std::shared_ptr<Compute> MakeComputeNode(const std::string& name,
		const std::string& label) {
	return ComputePtr(
			new Compute(name, label, CoordPX(0, 0), CoordPX(Node::DIMENSIONS)));
}
std::shared_ptr<Compute> MakeComputeNode(const std::string& name) {
	return ComputePtr(
			new Compute(name, CoordPX(0, 0), CoordPX(Node::DIMENSIONS)));
}

std::shared_ptr<Source> MakeSourceNode(const std::string& name,
		const std::string& label, const pixel2& pos) {
	return SourcePtr(
			new Source(name, label, CoordPX(pos), CoordPX(Node::DIMENSIONS)));
}
std::shared_ptr<Source> MakeSourceNode(const std::string& name,
		const pixel2& pos) {
	return SourcePtr(new Source(name, CoordPX(pos), CoordPX(Node::DIMENSIONS)));
}
std::shared_ptr<Source> MakeSourceNode(const std::string& name,
		const std::string& label) {
	return SourcePtr(
			new Source(name, label, CoordPX(0, 0), CoordPX(Node::DIMENSIONS)));
}
std::shared_ptr<Source> MakeSourceNode(const std::string& name) {
	return SourcePtr(new Source(name, CoordPX(0, 0), CoordPX(Node::DIMENSIONS)));
}

std::shared_ptr<Destination> MakeDestinationNode(const std::string& name,
		const std::string& label, const pixel2& pos) {
	return DestinationPtr(
			new Destination(name, label, CoordPX(pos),
					CoordPX(Node::DIMENSIONS)));
}
std::shared_ptr<Destination> MakeDestinationNode(const std::string& name,
		const pixel2& pos) {
	return DestinationPtr(
			new Destination(name, CoordPX(pos), CoordPX(Node::DIMENSIONS)));
}
std::shared_ptr<Destination> MakeDestinationNode(const std::string& name,
		const std::string& label) {
	return DestinationPtr(
			new Destination(name, label, CoordPX(0, 0),
					CoordPX(Node::DIMENSIONS)));
}
std::shared_ptr<Destination> MakeDestinationNode(const std::string& name) {
	return DestinationPtr(
			new Destination(name, CoordPX(0, 0), CoordPX(Node::DIMENSIONS)));
}
void NodeIcon::draw(AlloyContext* context) {
	NVGcontext* nvg = context->nvgContext;
	box2px bounds = getBounds();
	pixel lineWidth = borderWidth.toPixels(bounds.dimensions.y, context->dpmm.y,
			context->pixelRatio);
	nvgStrokeWidth(nvg, lineWidth);
	if (context->isMouseOver(this)) {
		nvgStrokeColor(nvg, Color(context->theme.HIGHLIGHT));
		nvgFillColor(nvg, backgroundColor->toLighter(0.25f));
	} else {
		nvgStrokeColor(nvg, Color(context->theme.LIGHT_TEXT));
		nvgFillColor(nvg, *backgroundColor);
	}

	nvgStrokeWidth(nvg, lineWidth);

	nvgBeginPath(nvg);

	if (shape == NodeShape::Circle) {
		nvgEllipse(nvg, bounds.position.x + bounds.dimensions.x * 0.5f,
				bounds.position.y + bounds.dimensions.y * 0.5f,
				0.5f * bounds.dimensions.y - lineWidth * 0.5f,
				0.5f * bounds.dimensions.y - lineWidth * 0.5f);
	} else if (shape == NodeShape::Square) {
		nvgRoundedRect(nvg, bounds.position.x + lineWidth * 0.5f,
				bounds.position.y + lineWidth * 0.5f,
				bounds.dimensions.x - lineWidth,
				bounds.dimensions.y - lineWidth, bounds.dimensions.x * 0.25f);
	} else if (shape == NodeShape::Triangle) {
		nvgLineJoin(nvg, NVG_ROUND);
		nvgMoveTo(nvg, bounds.position.x + bounds.dimensions.x * 0.5f,
				bounds.position.y + bounds.dimensions.y - lineWidth);
		nvgLineTo(nvg, bounds.position.x + lineWidth * 0.5f,
				bounds.position.y + lineWidth * 0.5f);
		nvgLineTo(nvg, bounds.position.x + bounds.dimensions.x - lineWidth,
				bounds.position.y + lineWidth * 0.5f);
		nvgClosePath(nvg);
	} else if (shape == NodeShape::Hexagon) {
		nvgLineJoin(nvg, NVG_ROUND);
		float cx=bounds.position.x + bounds.dimensions.x * 0.5f;
		float cy=bounds.position.y + bounds.dimensions.y * 0.5f;
		static const float SCALE=1.0f/std::sqrt(0.75f);
		float rx=(0.5f*bounds.dimensions.x - lineWidth*0.5f)*SCALE;
		float ry=(0.5f*bounds.dimensions.y - lineWidth*0.5f);
		nvgMoveTo(nvg, cx+rx,cy);
		nvgLineTo(nvg, cx+rx*0.5f,cy-ry);
		nvgLineTo(nvg, cx-rx*0.5f,cy-ry);
		nvgLineTo(nvg, cx-rx*0.5f,cy-ry);
		nvgLineTo(nvg, cx-rx,cy);
		nvgLineTo(nvg, cx-rx*0.5f,cy+ry);
		nvgLineTo(nvg, cx+rx*0.5f,cy+ry);
		nvgClosePath(nvg);
	}
	nvgFill(nvg);
	nvgStroke(nvg);
}
void Node::setup() {
	borderWidth = UnitPX(4.0f);
	fontSize = 20.0f;
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
	setOrientation(Orientation::Horizontal, pixel2(0, 0));
	CompositePtr iconContainer = MakeComposite("Icon Container",
			CoordPX(0.0f, 0.0f),
			CoordPX(
					Node::DIMENSIONS.x - InputPort::DIMENSIONS.x
							- OutputPort::DIMENSIONS.x, Node::DIMENSIONS.y));

	CompositePtr labelContainer = MakeComposite("label Container",
			CoordPX(0.0f, 0.0f),
			CoordPerPX(1.0, 0.0f, 0.0f, Node::DIMENSIONS.y));
	labelContainer->setAspectRule(AspectRule::FixedHeight);

	nodeIcon = NodeIconPtr(
			new NodeIcon("Icon", CoordPX(0.0f, InputPort::DIMENSIONS.y + 1.0f),
					CoordPerPX(1.0f, 1.0f, 0.0f,
							-OutputPort::DIMENSIONS.y - InputPort::DIMENSIONS.y
									- 2.0f)));
	nodeIcon->setAspectRule(AspectRule::FixedHeight);
	nodeIcon->setAspectRatio(1.0f);
	iconContainer->add(nodeIcon);
	NVGcontext* nvg = AlloyApplicationContext()->nvgContext;
	nvgFontSize(nvg, fontSize);
	nvgFontFaceId(nvg,
			AlloyApplicationContext()->getFont(FontType::Bold)->handle);
	float tw = nvgTextBounds(nvg, 0, 0, label.c_str(), nullptr, nullptr);
	labelRegion = TextLabelPtr(
			new TextLabel(label, CoordPX(0.0f, 2 * InputPort::DIMENSIONS.y),
					CoordPerPX(0.0f, 1.0f, tw + 10.0f,
							-2 * OutputPort::DIMENSIONS.y
									- 2 * InputPort::DIMENSIONS.y - 2.0f)));
	labelRegion->setAlignment(HorizontalAlignment::Left,
			VerticalAlignment::Middle);
	labelRegion->setAspectRule(AspectRule::FixedHeight);
	labelRegion->fontSize = UnitPX(fontSize);
	labelRegion->fontType = FontType::Bold;
	labelContainer->add(labelRegion);

	inputPortComposite = CompositePtr(
			new Composite("Input Ports", CoordPX(0.0f, InputPort::DIMENSIONS.y),
					CoordPX(0.0f, InputPort::DIMENSIONS.y)));
	outputPortComposite = CompositePtr(
			new Composite("Output Ports",
					CoordPerPX(0.0f, 1.0f, 0.0f, -2 * OutputPort::DIMENSIONS.y),
					CoordPX(0.0f, OutputPort::DIMENSIONS.y)));

	inputPortComposite->setOrientation(Orientation::Horizontal, pixel2(2, 0),
			pixel2(2, 0));
	outputPortComposite->setOrientation(Orientation::Horizontal, pixel2(2, 0),
			pixel2(2, 0));

	labelContainer->add(inputPortComposite);
	labelContainer->add(outputPortComposite);

	Composite::add(iconContainer);
	Composite::add(labelContainer);
	setRoundCorners(true);
	setDragEnabled(true);
	Application::addListener(this);
	nodeIcon->backgroundColor = MakeColor(COLOR);
	nodeIcon->setShape(NodeShape::Square);
	nodeIcon->borderWidth = borderWidth;
}
void Data::setup() {
	setOrientation(Orientation::Horizontal, pixel2(0, 0));
	CompositePtr iconContainer = MakeComposite("Icon Container",
			CoordPX(0.0f, 0.0f),
			CoordPX(
					Node::DIMENSIONS.x - InputPort::DIMENSIONS.x
							- OutputPort::DIMENSIONS.x, Node::DIMENSIONS.y));

	CompositePtr labelContainer = MakeComposite("label Container",
			CoordPX(0.0f, 0.0f),
			CoordPerPX(1.0, 0.0f, 0.0f, Node::DIMENSIONS.y));
	labelContainer->setAspectRule(AspectRule::FixedHeight);

	nodeIcon = NodeIconPtr(
			new NodeIcon("Icon", CoordPX(0.0f, InputPort::DIMENSIONS.y + 1.0f),
					CoordPerPX(1.0f, 1.0f, 0.0f,
							-OutputPort::DIMENSIONS.y - InputPort::DIMENSIONS.y
									- 2.0f)));
	nodeIcon->setAspectRule(AspectRule::FixedHeight);
	nodeIcon->setAspectRatio(1.0f);
	inputPort = MakeInputPort("Input");
	outputPort = MakeOutputPort("Output");
	inputPort->position = CoordPerPX(0.5f, 0.0f,
			-InputPort::DIMENSIONS.x * 0.5f, 0.0f);
	outputPort->position = CoordPerPX(0.5f, 1.0f,
			-OutputPort::DIMENSIONS.x * 0.5f, -OutputPort::DIMENSIONS.y);
	iconContainer->add(nodeIcon);
	iconContainer->add(inputPort);
	iconContainer->add(outputPort);
	NVGcontext* nvg = AlloyApplicationContext()->nvgContext;
	nvgFontSize(nvg, fontSize);
	nvgFontFaceId(nvg,
			AlloyApplicationContext()->getFont(FontType::Bold)->handle);
	float tw = nvgTextBounds(nvg, 0, 0, label.c_str(), nullptr, nullptr);
	labelRegion = TextLabelPtr(
			new TextLabel(label,
					CoordPX(0.0f, 2 * InputPort::DIMENSIONS.y + 1.0f),
					CoordPerPX(0.0f, 1.0f, tw + 10.0f,
							-2 * OutputPort::DIMENSIONS.y
									- 2 * InputPort::DIMENSIONS.y - 2.0f)));
	labelRegion->setAlignment(HorizontalAlignment::Left,
			VerticalAlignment::Middle);
	labelRegion->setAspectRule(AspectRule::FixedHeight);
	labelRegion->fontSize = UnitPX(fontSize);
	labelRegion->fontType = FontType::Bold;
	labelContainer->add(labelRegion);
	Composite::add(iconContainer);
	Composite::add(labelContainer);
	setRoundCorners(true);
	setDragEnabled(true);
	Application::addListener(this);
	nodeIcon->backgroundColor = MakeColor(COLOR);
	nodeIcon->borderWidth = borderWidth;
}
void Compute::setup() {
	setOrientation(Orientation::Horizontal, pixel2(0, 0));
	CompositePtr iconContainer = MakeComposite("Icon Container",
			CoordPX(0.0f, 0.0f),
			CoordPX(
					Node::DIMENSIONS.x - InputPort::DIMENSIONS.x
							- OutputPort::DIMENSIONS.x, Node::DIMENSIONS.y));

	CompositePtr labelContainer = MakeComposite("label Container",
			CoordPX(0.0f, 0.0f),
			CoordPerPX(1.0, 0.0f, 0.0f, Node::DIMENSIONS.y));
	labelContainer->setAspectRule(AspectRule::FixedHeight);

	nodeIcon = NodeIconPtr(
			new NodeIcon("Icon", CoordPX(0.0f, InputPort::DIMENSIONS.y + 1.0f),
					CoordPerPX(1.0f, 1.0f, 0.0f,
							-OutputPort::DIMENSIONS.y - InputPort::DIMENSIONS.y
									- 2.0f)));
	nodeIcon->setAspectRule(AspectRule::FixedHeight);
	nodeIcon->setAspectRatio(1.0f);
	inputPort = MakeInputPort("Input");
	outputPort = MakeOutputPort("Output");
	inputPort->position = CoordPerPX(0.5f, 0.0f,
			-InputPort::DIMENSIONS.x * 0.5f, 0.0f);
	outputPort->position = CoordPerPX(0.5f, 1.0f,
			-OutputPort::DIMENSIONS.x * 0.5f, -OutputPort::DIMENSIONS.y);
	iconContainer->add(nodeIcon);
	iconContainer->add(inputPort);
	iconContainer->add(outputPort);
	NVGcontext* nvg = AlloyApplicationContext()->nvgContext;
	nvgFontSize(nvg, fontSize);
	nvgFontFaceId(nvg,
			AlloyApplicationContext()->getFont(FontType::Bold)->handle);
	float tw = nvgTextBounds(nvg, 0, 0, label.c_str(), nullptr, nullptr);
	labelRegion = TextLabelPtr(
			new TextLabel(label,
					CoordPX(4.0f, 2 * InputPort::DIMENSIONS.y + 1.0f),
					CoordPerPX(0.0f, 1.0f, tw + 6.0f,
							-2 * OutputPort::DIMENSIONS.y
									- 2 * InputPort::DIMENSIONS.y - 2.0f)));
	labelRegion->setAlignment(HorizontalAlignment::Left,
			VerticalAlignment::Middle);
	labelRegion->setAspectRule(AspectRule::FixedHeight);
	labelRegion->fontSize = UnitPX(fontSize);
	labelRegion->fontType = FontType::Bold;
	labelContainer->add(labelRegion);
	inputPortComposite = CompositePtr(
			new Composite("Input Ports", CoordPX(0.0f, InputPort::DIMENSIONS.y),
					CoordPX(0.0f, InputPort::DIMENSIONS.y)));
	outputPortComposite = CompositePtr(
			new Composite("Output Ports",
					CoordPerPX(0.0f, 1.0f, 0.0f, -2 * OutputPort::DIMENSIONS.y),
					CoordPX(0.0f, OutputPort::DIMENSIONS.y)));

	inputPortComposite->setOrientation(Orientation::Horizontal, pixel2(2, 0),
			pixel2(2, 0));
	outputPortComposite->setOrientation(Orientation::Horizontal, pixel2(2, 0),
			pixel2(2, 0));

	labelContainer->add(inputPortComposite);
	labelContainer->add(outputPortComposite);

	Composite::add(iconContainer);
	Composite::add(labelContainer);
	setRoundCorners(true);
	setDragEnabled(true);
	Application::addListener(this);
	nodeIcon->backgroundColor = MakeColor(COLOR);
	nodeIcon->borderWidth = borderWidth;
	nodeIcon->setShape(NodeShape::Hexagon);
}
void Source::setup() {
	setOrientation(Orientation::Vertical, pixel2(0, 0));
	CompositePtr iconContainer = MakeComposite("Icon Container",
			CoordPX(0.0f, 0.0f),
			CoordPX(Node::DIMENSIONS.x,
					Node::DIMENSIONS.y - InputPort::DIMENSIONS.y));

	//labelContainer->setAspectRatio(1.0f);
	nodeIcon = NodeIconPtr(
			new NodeIcon("Icon", CoordPX(ParentPort::DIMENSIONS.x + 1.0f, 1.0f),
					CoordPerPX(1.0f, 1.0f,
							-ParentPort::DIMENSIONS.x - ChildPort::DIMENSIONS.x
									- 2.0f, -OutputPort::DIMENSIONS.y - 2.0f)));
	nodeIcon->setAspectRatio(1.0f);
	nodeIcon->setAspectRule(AspectRule::FixedHeight);
	outputPort = MakeOutputPort("Output");
	parentPort = MakeParentPort("Parent");
	childPort = MakeChildPort("Child");
	outputPort->position = CoordPerPX(0.5f, 1.0f,
			-OutputPort::DIMENSIONS.x * 0.5f, -OutputPort::DIMENSIONS.y);
	parentPort->position = CoordPerPX(0.0f, 0.5f, 0.0f,
			-OutputPort::DIMENSIONS.y);
	childPort->position = CoordPerPX(1.0f, 0.5f, -ChildPort::DIMENSIONS.x,
			-OutputPort::DIMENSIONS.y);
	iconContainer->add(nodeIcon);
	iconContainer->add(outputPort);
	iconContainer->add(parentPort);
	iconContainer->add(childPort);
	NVGcontext* nvg = AlloyApplicationContext()->nvgContext;
	nvgFontSize(nvg, fontSize);
	nvgFontFaceId(nvg,
			AlloyApplicationContext()->getFont(FontType::Bold)->handle);
	float tw = nvgTextBounds(nvg, 0, 0, label.c_str(), nullptr, nullptr);
	labelRegion = TextLabelPtr(
			new TextLabel(label, CoordPercent(0.5f, 0.0f),
					CoordPX(tw + 10.0f, fontSize)));
	labelRegion->setOrigin(Origin::TopCenter);
	labelRegion->setAlignment(HorizontalAlignment::Center,
			VerticalAlignment::Middle);
	labelRegion->setAspectRule(AspectRule::FixedHeight);
	labelRegion->fontSize = UnitPX(fontSize);
	labelRegion->fontType = FontType::Bold;

	Composite::add(labelRegion);
	Composite::add(iconContainer);
	setRoundCorners(true);
	setDragEnabled(true);
	Application::addListener(this);
	nodeIcon->backgroundColor = MakeColor(COLOR);
	nodeIcon->borderWidth = borderWidth;
}
void Destination::setup() {
	setOrientation(Orientation::Vertical, pixel2(0, 0));
	CompositePtr iconContainer = MakeComposite("Icon Container",
			CoordPX(0.0f, 0.0f),
			CoordPX(Node::DIMENSIONS.x,
					Node::DIMENSIONS.y - InputPort::DIMENSIONS.y));

	//labelContainer->setAspectRatio(1.0f);
	nodeIcon = NodeIconPtr(
			new NodeIcon("Icon",
					CoordPX(0.5f * ParentPort::DIMENSIONS.x,
							InputPort::DIMENSIONS.y + 1.0f),
					CoordPerPX(1.0f, 1.0f, -ParentPort::DIMENSIONS.x,
							-OutputPort::DIMENSIONS.y - 2.0f)));
	//nodeIcon->setAspectRatio(1.0f);
	//nodeIcon->setAspectRule(AspectRule::FixedWidth);
	inputPort = MakeInputPort("Input");
	inputPort->position = CoordPerPX(0.5f, 0.0f,
			-InputPort::DIMENSIONS.x * 0.5f, 0.0f);
	iconContainer->add(nodeIcon);
	iconContainer->add(inputPort);
	NVGcontext* nvg = AlloyApplicationContext()->nvgContext;
	nvgFontSize(nvg, fontSize);
	nvgFontFaceId(nvg,
			AlloyApplicationContext()->getFont(FontType::Bold)->handle);
	float tw = nvgTextBounds(nvg, 0, 0, label.c_str(), nullptr, nullptr);
	labelRegion = TextLabelPtr(
			new TextLabel(label, CoordPercent(0.5f, 0.0f),
					CoordPX(tw + 10.0f, fontSize)));
	labelRegion->setOrigin(Origin::TopCenter);
	labelRegion->setAlignment(HorizontalAlignment::Center,
			VerticalAlignment::Middle);
	labelRegion->setAspectRule(AspectRule::FixedHeight);
	labelRegion->fontSize = UnitPX(fontSize);
	labelRegion->fontType = FontType::Bold;

	Composite::add(iconContainer);
	Composite::add(labelRegion);
	setRoundCorners(true);
	setDragEnabled(true);
	Application::addListener(this);
	nodeIcon->backgroundColor = MakeColor(COLOR);
	nodeIcon->setShape(NodeShape::Triangle);
	nodeIcon->borderWidth = borderWidth;
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
	dimensions = CoordPX(InputPort::DIMENSIONS);
}
void OutputPort::setup() {
	position = CoordPX(0.0f, 0.0f);
	dimensions = CoordPX(OutputPort::DIMENSIONS);
}
void ParentPort::setup() {
	position = CoordPX(0.0f, 0.0f);
	dimensions = CoordPX(OutputPort::DIMENSIONS);
}
void ChildPort::setup() {
	position = CoordPX(0.0f, 0.0f);
	dimensions = CoordPX(OutputPort::DIMENSIONS);
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
void ParentPort::draw(AlloyContext* context) {
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
	nvgRect(nvg, bounds.position.x + 0.5f * lineWidth,
			bounds.position.y + 0.5f * lineWidth,
			bounds.dimensions.x - lineWidth, bounds.dimensions.y - lineWidth);
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

	nvgMoveTo(nvg, bounds.position.x + bounds.dimensions.x * 0.5f,
			bounds.position.y + bounds.dimensions.y - lineWidth);
	nvgLineTo(nvg, bounds.position.x + 0.5f,
			bounds.position.y + lineWidth * 0.5f);
	nvgLineTo(nvg, bounds.position.x + bounds.dimensions.x - 1.0f,
			bounds.position.y + lineWidth * 0.5f);
	nvgClosePath(nvg);
	nvgFill(nvg);
	nvgStroke(nvg);

}

void ChildPort::draw(AlloyContext* context) {
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

	nvgMoveTo(nvg, bounds.position.x + bounds.dimensions.x - lineWidth,
			bounds.position.y + bounds.dimensions.y * 0.5f);
	nvgLineTo(nvg, bounds.position.x + lineWidth * 0.5f,
			bounds.position.y + 0.5f);
	nvgLineTo(nvg, bounds.position.x + lineWidth * 0.5f,
			bounds.position.y + bounds.dimensions.y - 1.0f);
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
	box2px obounds = outputPortComposite->getBounds();
	box2px cbounds = nodeIcon->getBounds();

	pixel2 labelStart = pixel2(cbounds.position.x + cbounds.dimensions.x * 0.5f,
			lbounds.position.y);
	pixel2 labelEnd = pixel2(
			lbounds.position.x
					+ std::max(
							std::max(lbounds.dimensions.x,
									ibounds.dimensions.x),
							obounds.dimensions.x),
			lbounds.position.y + lbounds.dimensions.y);
	nvgStrokeWidth(nvg, 2.0f);
	if (inputPorts.size() > 0 || outputPorts.size() > 0) {
		nvgStrokeColor(nvg, Color(context->theme.DARK.toLighter(0.5f)));
		nvgFillColor(nvg, context->theme.DARK.toSemiTransparent(0.5f));
	} else {
		nvgStrokeColor(nvg, Color(COLOR_NONE)); //Color(context->theme.LIGHT_TEXT.toSemiTransparent(0.5f))
		nvgFillColor(nvg, Color(context->theme.DARK.toSemiTransparent(0.5f)));
	}
	nvgBeginPath(nvg);
	nvgRoundedRect(nvg, labelStart.x, labelStart.y, labelEnd.x - labelStart.x,
			labelEnd.y - labelStart.y, context->theme.CORNER_RADIUS);
	nvgFill(nvg);
	nvgBeginPath(nvg);
	nvgRoundedRect(nvg, labelStart.x + 1.0f, labelStart.y + 1.0f,
			labelEnd.x - labelStart.x - 2.0f, labelEnd.y - labelStart.y - 2.0f,
			context->theme.CORNER_RADIUS);
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
	NVGcontext* nvg = context->nvgContext;
	box2px bounds = getBounds();
	pixel lineWidth = borderWidth.toPixels(bounds.dimensions.y, context->dpmm.y,
			context->pixelRatio);
	nvgStrokeColor(nvg, context->theme.LIGHT_TEXT);
	nvgStrokeWidth(nvg, lineWidth);
	nvgLineCap(nvg, NVG_ROUND);
	box2px lbounds = labelRegion->getBounds();
	box2px cbounds = nodeIcon->getBounds();
	pixel2 labelStart = pixel2(cbounds.position.x + cbounds.dimensions.x * 0.5f,
			lbounds.position.y);
	pixel2 labelEnd = pixel2(lbounds.position.x + lbounds.dimensions.x,
			lbounds.position.y + lbounds.dimensions.y);
	nvgStrokeWidth(nvg, 2.0f);
	if (inputPorts.size() > 0 || outputPorts.size() > 0) {
		nvgStrokeColor(nvg, Color(context->theme.DARK.toLighter(0.5f)));
		nvgFillColor(nvg, context->theme.DARK.toSemiTransparent(0.5f));
	} else {
		nvgStrokeColor(nvg, Color(COLOR_NONE));
		nvgFillColor(nvg, Color(context->theme.DARK.toSemiTransparent(0.5f)));
	}
	nvgBeginPath(nvg);
	nvgRoundedRect(nvg, labelStart.x, labelStart.y, labelEnd.x - labelStart.x,
			labelEnd.y - labelStart.y, context->theme.CORNER_RADIUS);
	nvgFill(nvg);
	nvgBeginPath(nvg);
	nvgRoundedRect(nvg, labelStart.x + 1.0f, labelStart.y + 1.0f,
			labelEnd.x - labelStart.x - 2.0f, labelEnd.y - labelStart.y - 2.0f,
			context->theme.CORNER_RADIUS);
	nvgStroke(nvg);

	Composite::draw(context);
}
void Source::draw(AlloyContext* context) {
	NVGcontext* nvg = context->nvgContext;
	box2px bounds = getBounds();
	pixel lineWidth = borderWidth.toPixels(bounds.dimensions.y, context->dpmm.y,
			context->pixelRatio);
	nvgStrokeColor(nvg, context->theme.LIGHT_TEXT);
	nvgStrokeWidth(nvg, lineWidth);
	nvgLineCap(nvg, NVG_ROUND);
	box2px lbounds = labelRegion->getBounds();
	nvgStrokeWidth(nvg, 2.0f);
	if (inputPorts.size() > 0 || outputPorts.size() > 0) {
		nvgStrokeColor(nvg,
				Color(context->theme.LIGHT_TEXT.toSemiTransparent(0.5f)));
		nvgFillColor(nvg, nodeIcon->backgroundColor->toSemiTransparent(0.5f));
	} else {
		nvgStrokeColor(nvg, Color(COLOR_NONE)); //Color(context->theme.LIGHT_TEXT.toSemiTransparent(0.5f))
		nvgFillColor(nvg, Color(context->theme.DARK.toSemiTransparent(0.5f)));
	}
	nvgBeginPath(nvg);
	nvgRoundedRect(nvg, lbounds.position.x, lbounds.position.y,
			lbounds.dimensions.x, lbounds.dimensions.y,
			context->theme.CORNER_RADIUS);
	nvgFill(nvg);
	nvgBeginPath(nvg);
	nvgRoundedRect(nvg, lbounds.position.x + 1.0f, lbounds.position.y + 1.0f,
			lbounds.dimensions.x - 2.0f, lbounds.dimensions.y - 2.0f,
			context->theme.CORNER_RADIUS);
	nvgStroke(nvg);

	Composite::draw(context);
}
void Destination::draw(AlloyContext* context) {
	NVGcontext* nvg = context->nvgContext;
	box2px bounds = getBounds();
	pixel lineWidth = borderWidth.toPixels(bounds.dimensions.y, context->dpmm.y,
			context->pixelRatio);
	nvgStrokeColor(nvg, context->theme.LIGHT_TEXT);
	nvgStrokeWidth(nvg, lineWidth);
	nvgLineCap(nvg, NVG_ROUND);
	box2px lbounds = labelRegion->getBounds();
	nvgStrokeWidth(nvg, 2.0f);
	if (inputPorts.size() > 0 || outputPorts.size() > 0) {
		nvgStrokeColor(nvg,
				Color(context->theme.LIGHT_TEXT.toSemiTransparent(0.5f)));
		nvgFillColor(nvg, nodeIcon->backgroundColor->toSemiTransparent(0.5f));
	} else {
		nvgStrokeColor(nvg, Color(COLOR_NONE)); //Color(context->theme.LIGHT_TEXT.toSemiTransparent(0.5f))
		nvgFillColor(nvg, Color(context->theme.DARK.toSemiTransparent(0.5f)));
	}
	nvgBeginPath(nvg);
	nvgRoundedRect(nvg, lbounds.position.x, lbounds.position.y,
			lbounds.dimensions.x, lbounds.dimensions.y,
			context->theme.CORNER_RADIUS);
	nvgFill(nvg);
	nvgBeginPath(nvg);
	nvgRoundedRect(nvg, lbounds.position.x + 1.0f, lbounds.position.y + 1.0f,
			lbounds.dimensions.x - 2.0f, lbounds.dimensions.y - 2.0f,
			context->theme.CORNER_RADIUS);
	nvgStroke(nvg);

	Composite::draw(context);
}
}
}

