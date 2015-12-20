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
#include "AlloyDrawUtil.h"
#include "ForceDirectedGraph.h"
namespace aly {
namespace dataflow {
const int MultiPort::FrontIndex = std::numeric_limits<int>::min();
const int MultiPort::BackIndex = std::numeric_limits<int>::max();
const pixel2 Node::DIMENSIONS = pixel2(80, 80);
const pixel2 InputPort::DIMENSIONS = pixel2(12, 12);
const pixel2 OutputPort::DIMENSIONS = pixel2(12, 11);
const pixel2 ParentPort::DIMENSIONS = pixel2(12, 12);
const pixel2 ChildPort::DIMENSIONS = pixel2(11, 12);
const float NODE_SATURATION = 0.6f;
const float NODE_LUMINANCE = 0.65f;
const float NODE_ALPHA = 0.75f;
const Color View::COLOR = HSVAtoColor(
		HSVA(30 / 360.0f, NODE_SATURATION, 0.5f * NODE_LUMINANCE, 1.0f));
const Color Compute::COLOR = HSVAtoColor(
		HSVA(0.0f, NODE_SATURATION, NODE_LUMINANCE, 1.0f));
const Color Data::COLOR = HSVAtoColor(
		HSVA(60.0f / 360.0f, NODE_SATURATION, NODE_LUMINANCE, 1.0f));
const Color Group::COLOR = HSVAtoColor(
		HSVA(300.0f / 360.0f, NODE_SATURATION, 0.5f * NODE_LUMINANCE, 1.0f));
const Color Destination::COLOR = HSVAtoColor(
		HSVA(120.0f / 360.0f, NODE_SATURATION, NODE_LUMINANCE, 1.0f));
const Color Source::COLOR = HSVAtoColor(
		HSVA(225.0f / 360.0f, NODE_SATURATION, NODE_LUMINANCE, 1.0f));

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
std::shared_ptr<Connection> MakeConnection(const std::shared_ptr<Port>& source,
		const std::shared_ptr<Port>& destination) {
	ConnectionPtr connection = ConnectionPtr(
			new Connection(source, destination));
	source->connect(connection);
	destination->connect(connection);
	return connection;
}
std::shared_ptr<Relationship> MakeRelationship(
		const std::shared_ptr<Node>& subject,
		const std::shared_ptr<Predicate>& predicate,
		const std::shared_ptr<Node>& object) {
	return RelationshipPtr(new Relationship(subject, predicate, object));
}
std::shared_ptr<Relationship> MakeRelationship(
		const std::shared_ptr<Node>& subject, const std::string& name,
		const std::shared_ptr<Node>& object) {
	return RelationshipPtr(
			new Relationship(subject, PredicatePtr(new Predicate(name)),
					object));
}
std::shared_ptr<Data> MakeDataNode(const std::string& name,
		const std::string& label, const pixel2& pos) {
	return DataPtr(new Data(name, label, pos));
}
std::shared_ptr<Data> MakeDataNode(const std::string& name, const pixel2& pos) {
	return DataPtr(new Data(name, pos));
}
std::shared_ptr<Data> MakeDataNode(const std::string& name,
		const std::string& label) {
	return DataPtr(new Data(name, label, pixel2(0.0f)));
}
std::shared_ptr<Data> MakeDataNode(const std::string& name) {
	return DataPtr(new Data(name, pixel2(0.0f)));
}

std::shared_ptr<View> MakeViewNode(const std::string& name,
		const std::string& label, const pixel2& pos) {
	return ViewPtr(new View(name, label, pos));
}
std::shared_ptr<View> MakeViewNode(const std::string& name, const pixel2& pos) {
	return ViewPtr(new View(name, pos));
}
std::shared_ptr<View> MakeViewNode(const std::string& name,
		const std::string& label) {
	return ViewPtr(new View(name, label, pixel2(0.0f)));
}
std::shared_ptr<View> MakeViewNode(const std::string& name) {
	return ViewPtr(new View(name, pixel2(0.0f)));
}

std::shared_ptr<Compute> MakeComputeNode(const std::string& name,
		const std::string& label, const pixel2& pos) {
	return ComputePtr(new Compute(name, label, pos));
}
std::shared_ptr<Compute> MakeComputeNode(const std::string& name,
		const pixel2& pos) {
	return ComputePtr(new Compute(name, pos));
}
std::shared_ptr<Compute> MakeComputeNode(const std::string& name,
		const std::string& label) {
	return ComputePtr(new Compute(name, label, pixel2(0.0f)));
}
std::shared_ptr<Compute> MakeComputeNode(const std::string& name) {
	return ComputePtr(new Compute(name, pixel2(0.0f)));
}

std::shared_ptr<Group> MakeGroupNode(const std::string& name,
		const std::string& label, const pixel2& pos) {
	return GroupPtr(new Group(name, label, pos));
}
std::shared_ptr<Group> MakeGroupNode(const std::string& name,
		const pixel2& pos) {
	return GroupPtr(new Group(name, pos));
}
std::shared_ptr<Group> MakeGroupNode(const std::string& name,
		const std::string& label) {
	return GroupPtr(new Group(name, label, pixel2(0.0f)));
}
std::shared_ptr<Group> MakeGroupNode(const std::string& name) {
	return GroupPtr(new Group(name, pixel2(0.0f)));
}
std::shared_ptr<Source> MakeSourceNode(const std::string& name,
		const std::string& label, const pixel2& pos) {
	return SourcePtr(new Source(name, label, pos));
}
std::shared_ptr<Source> MakeSourceNode(const std::string& name,
		const pixel2& pos) {
	return SourcePtr(new Source(name, pos));
}
std::shared_ptr<Source> MakeSourceNode(const std::string& name,
		const std::string& label) {
	return SourcePtr(new Source(name, label, pixel2(0.0f)));
}
std::shared_ptr<Source> MakeSourceNode(const std::string& name) {
	return SourcePtr(new Source(name, pixel2(0.0f)));
}

std::shared_ptr<Destination> MakeDestinationNode(const std::string& name,
		const std::string& label, const pixel2& pos) {
	return DestinationPtr(new Destination(name, label, pos));
}
std::shared_ptr<Destination> MakeDestinationNode(const std::string& name,
		const pixel2& pos) {
	return DestinationPtr(new Destination(name, pos));
}
std::shared_ptr<Destination> MakeDestinationNode(const std::string& name,
		const std::string& label) {
	return DestinationPtr(new Destination(name, label, pixel2(0.0f)));
}
std::shared_ptr<Destination> MakeDestinationNode(const std::string& name) {
	return DestinationPtr(new Destination(name, pixel2(0.0f)));
}

DataFlow::~DataFlow() {
	if (forceSim.get() != nullptr)
		forceSim->stop();
}
void NodeIcon::draw(AlloyContext* context) {
	NVGcontext* nvg = context->nvgContext;
	box2px bounds = getBounds();
	pixel lineWidth = 0;
	nvgStrokeWidth(nvg, lineWidth);
	static std::vector<float2> pentagon;
	if (pentagon.size() == 0) {
		pentagon.resize(6);
		for (int k = 0; k < 5; k++) {
			pentagon[k] = float2(std::sin(ALY_PI * 2 * k / 5.0f),
					-std::cos(ALY_PI * 2 * k / 5.0f));
		}
		pentagon[5] = float2(0.0f);
	}
	if (context->isMouseOver(this)) {
		nvgFillColor(nvg, backgroundColor->toLighter(0.25f));
		//context->setCursor(&Cursor::Position);
	} else {
		nvgFillColor(nvg, *backgroundColor);
	}
	Color strokeColor;
	if (selected) {
		nvgStrokeWidth(nvg, 6.0f);
		lineWidth = 6;
		nvgStrokeColor(nvg, strokeColor = context->theme.HIGHLIGHT);
	} else {
		nvgStrokeWidth(nvg, 4.0f);
		lineWidth = 4;
		nvgStrokeColor(nvg,
				strokeColor = context->theme.HIGHLIGHT.toDarker(0.8f));
	}
	if (shape == NodeShape::Circle) {
		nvgBeginPath(nvg);
		nvgEllipse(nvg, bounds.position.x + bounds.dimensions.x * 0.5f,
				bounds.position.y + bounds.dimensions.y * 0.5f,
				0.5f * bounds.dimensions.y - lineWidth * 0.5f,
				0.5f * bounds.dimensions.y - lineWidth * 0.5f);

		nvgFill(nvg);
		nvgStroke(nvg);
	} else if (shape == NodeShape::CircleGroup) {
		nvgBeginPath(nvg);
		nvgEllipse(nvg, bounds.position.x + bounds.dimensions.x * 0.5f,
				bounds.position.y + bounds.dimensions.y * 0.5f,
				0.5f * bounds.dimensions.y - lineWidth * 0.5f,
				0.5f * bounds.dimensions.y - lineWidth * 0.5f);

		nvgFill(nvg);
		nvgStroke(nvg);
		nvgFillColor(nvg, strokeColor);
		float cx = bounds.position.x + bounds.dimensions.x * 0.5f;
		float cy = bounds.position.y + bounds.dimensions.y * 0.5f;
		float r = 4.0f;
		float rx = 0.5f * (0.5f * bounds.dimensions.x);
		float ry = 0.5f * (0.5f * bounds.dimensions.y);
		for (int k = 0; k < (int) pentagon.size(); k++) {
			nvgBeginPath(nvg);
			float2 pt = pentagon[k];
			float x = cx + rx * pt.x;
			float y = cy + ry * pt.y;
			nvgCircle(nvg, x, y, r);
			nvgFill(nvg);
		}
	} else if (shape == NodeShape::Square) {
		nvgBeginPath(nvg);
		nvgRoundedRect(nvg, bounds.position.x + lineWidth * 0.5f,
				bounds.position.y + lineWidth * 0.5f,
				bounds.dimensions.x - lineWidth,
				bounds.dimensions.y - lineWidth, bounds.dimensions.x * 0.25f);
		nvgFill(nvg);
		nvgStroke(nvg);
	} else if (shape == NodeShape::Triangle) {
		nvgBeginPath(nvg);
		nvgLineJoin(nvg, NVG_ROUND);
		nvgMoveTo(nvg, bounds.position.x + bounds.dimensions.x * 0.5f,
				bounds.position.y + bounds.dimensions.y - lineWidth);
		nvgLineTo(nvg, bounds.position.x + lineWidth * 0.5f,
				bounds.position.y + lineWidth * 0.5f);
		nvgLineTo(nvg, bounds.position.x + bounds.dimensions.x - lineWidth,
				bounds.position.y + lineWidth * 0.5f);
		nvgClosePath(nvg);
		nvgFill(nvg);
		nvgStroke(nvg);
	} else if (shape == NodeShape::Hexagon) {
		nvgBeginPath(nvg);
		nvgLineJoin(nvg, NVG_ROUND);
		float cx = bounds.position.x + bounds.dimensions.x * 0.5f;
		float cy = bounds.position.y + bounds.dimensions.y * 0.5f;
		static const float SCALE = 1.0f / std::sqrt(0.75f);
		float rx = (0.5f * bounds.dimensions.x - lineWidth * 0.5f) * SCALE;
		float ry = (0.5f * bounds.dimensions.y - lineWidth * 0.5f);
		nvgMoveTo(nvg, cx + rx, cy);
		nvgLineTo(nvg, cx + rx * 0.5f, cy - ry);
		nvgLineTo(nvg, cx - rx * 0.5f, cy - ry);
		nvgLineTo(nvg, cx - rx * 0.5f, cy - ry);
		nvgLineTo(nvg, cx - rx, cy);
		nvgLineTo(nvg, cx - rx * 0.5f, cy + ry);
		nvgLineTo(nvg, cx + rx * 0.5f, cy + ry);
		nvgClosePath(nvg);
		nvgFill(nvg);
		nvgStroke(nvg);
	}
}
void Node::setup() {
	borderWidth = UnitPX(4.0f);
	fontSize = 20.0f;
}
ForceSimulatorPtr Node::getForceSimulator() {
	if (parent == nullptr)
		throw std::runtime_error("Node has no parent dataflow.");
	return parentFlow->getForceSimulator();
}
box2px Node::getObstacleBounds() const {
	box2px box = nodeIcon->getBounds(false);
	if (labelRegion.get() != nullptr) {
		box2px cbox = labelRegion->getBounds(false);
		box.merge(cbox);
	}
	box.position += getForceOffset();
	return box;
}
pixel2 Node::getForceOffset() const {
	return forceItem->location - centerOffset;
}
float2 Port::getLocation() const {
	return getBounds(false).center() + parent->getForceOffset();
}
void View::setup() {
	setOrientation(Orientation::Horizontal, pixel2(0, 0));
	NVGcontext* nvg = AlloyApplicationContext()->nvgContext;
	nvgFontSize(nvg, fontSize);
	nvgFontFaceId(nvg,
			AlloyApplicationContext()->getFont(FontType::Bold)->handle);
	textWidth = nvgTextBounds(nvg, 0, 0, label.c_str(), nullptr, nullptr);
	CompositePtr iconContainer = MakeComposite("Icon Container",
			CoordPX(0.0f, 0.0f),
			CoordPX(
					Node::DIMENSIONS.x - InputPort::DIMENSIONS.x
							- OutputPort::DIMENSIONS.x, Node::DIMENSIONS.y));

	CompositePtr labelContainer = MakeComposite("label Container",
			CoordPX(0.0f, 0.0f),
			CoordPerPX(1.0f, 0.0f, 0.0f, Node::DIMENSIONS.y));
	nodeIcon = NodeIconPtr(
			new NodeIcon("Icon", CoordPX(0.0f, InputPort::DIMENSIONS.y + 1.0f),
					CoordPerPX(1.0f, 1.0f, 0.0f,
							-OutputPort::DIMENSIONS.y - InputPort::DIMENSIONS.y
									- 2.0f)));
	nodeIcon->setAspectRule(AspectRule::FixedHeight);
	nodeIcon->setAspectRatio(1.0f);
	inputPort = MakeInputPort("Input");
	inputPort->position = CoordPerPX(0.5f, 0.0f,
			-InputPort::DIMENSIONS.x * 0.5f, 0.0f);
	inputPort->setParent(this);
	iconContainer->add(inputPort);
	iconContainer->add(nodeIcon);
	labelRegion = ModifiableLabelPtr(
			new ModifiableLabel(name,
					CoordPX(0.0f, 2 * InputPort::DIMENSIONS.y + 1.0f),
					CoordPerPX(0.0f, 1.0f,
							std::max(10 + textWidth, Node::DIMENSIONS.x),
							-2 * OutputPort::DIMENSIONS.y
									- 2 * InputPort::DIMENSIONS.y - 2.0f)));
	labelRegion->setValue(label);
	labelRegion->setAlignment(HorizontalAlignment::Left,
			VerticalAlignment::Middle);
	labelRegion->fontSize = UnitPX(fontSize);
	labelRegion->fontType = FontType::Bold;
	labelRegion->onTextEntered =
			[this](TextField* field) {
				textWidth = labelRegion->getTextDimensions(AlloyApplicationContext().get()).x;
				labelRegion->dimensions = CoordPerPX(0.0f, 1.0f, std::max(10 + textWidth, Node::DIMENSIONS.x), -2 * OutputPort::DIMENSIONS.y - 2 * InputPort::DIMENSIONS.y - 2.0f);
			};
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
	nodeIcon->backgroundColor = MakeColor(COLOR);
	nodeIcon->setShape(NodeShape::Square);
	nodeIcon->borderWidth = borderWidth;
	forceItem->buoyancy = 0;

}
void Group::setup() {
	setOrientation(Orientation::Horizontal, pixel2(0, 0));
	NVGcontext* nvg = AlloyApplicationContext()->nvgContext;
	nvgFontSize(nvg, fontSize);
	nvgFontFaceId(nvg,
			AlloyApplicationContext()->getFont(FontType::Bold)->handle);
	textWidth = nvgTextBounds(nvg, 0, 0, label.c_str(), nullptr, nullptr);
	CompositePtr iconContainer = MakeComposite("Icon Container",
			CoordPX(0.0f, 0.0f),
			CoordPX(
					Node::DIMENSIONS.x - InputPort::DIMENSIONS.x
							- OutputPort::DIMENSIONS.x, Node::DIMENSIONS.y));

	CompositePtr labelContainer = MakeComposite("label Container",
			CoordPX(Node::DIMENSIONS.x, 0.0f),
			CoordPerPX(1.0f, 0.0f, 0.0f, Node::DIMENSIONS.y));

	nodeIcon = NodeIconPtr(
			new NodeIcon("Icon", CoordPX(0.0f, InputPort::DIMENSIONS.y + 1.0f),
					CoordPerPX(1.0f, 1.0f, 0.0f,
							-OutputPort::DIMENSIONS.y - InputPort::DIMENSIONS.y
									- 2.0f)));
	nodeIcon->setAspectRule(AspectRule::FixedHeight);
	nodeIcon->setAspectRatio(1.0f);
	iconContainer->add(nodeIcon);
	labelRegion = ModifiableLabelPtr(
			new ModifiableLabel(name,
					CoordPX(0.0f, 2 * InputPort::DIMENSIONS.y + 1.0f),
					CoordPerPX(0.0f, 1.0f,
							std::max(10 + textWidth, Node::DIMENSIONS.x),
							-2 * OutputPort::DIMENSIONS.y
									- 2 * InputPort::DIMENSIONS.y - 2.0f)));
	labelRegion->setValue(label);
	labelRegion->setAlignment(HorizontalAlignment::Left,
			VerticalAlignment::Middle);
	labelRegion->fontSize = UnitPX(fontSize);
	labelRegion->fontType = FontType::Bold;
	labelRegion->onTextEntered =
			[this](TextField* field) {
				textWidth = labelRegion->getTextDimensions(AlloyApplicationContext().get()).x;
				labelRegion->dimensions = CoordPerPX(0.0f, 1.0f, std::max(10 + textWidth, Node::DIMENSIONS.x), -2 * OutputPort::DIMENSIONS.y - 2 * InputPort::DIMENSIONS.y - 2.0f);
			};
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
	nodeIcon->setShape(NodeShape::CircleGroup);
	nodeIcon->backgroundColor = MakeColor(COLOR);
	nodeIcon->borderWidth = borderWidth;
	forceItem->buoyancy = 0;

}
void Data::setup() {

	setOrientation(Orientation::Horizontal, pixel2(0, 0));
	NVGcontext* nvg = AlloyApplicationContext()->nvgContext;
	nvgFontSize(nvg, fontSize);
	nvgFontFaceId(nvg,
			AlloyApplicationContext()->getFont(FontType::Bold)->handle);
	textWidth = nvgTextBounds(nvg, 0, 0, label.c_str(), nullptr, nullptr);
	CompositePtr iconContainer = MakeComposite("Icon Container",
			CoordPX(0.0f, 0.0f),
			CoordPX(
					Node::DIMENSIONS.x - InputPort::DIMENSIONS.x
							- OutputPort::DIMENSIONS.x, Node::DIMENSIONS.y));

	CompositePtr labelContainer = MakeComposite("label Container",
			CoordPX(Node::DIMENSIONS.x, 0.0f),
			CoordPerPX(1.0f, 0.0f, 0.0f, Node::DIMENSIONS.y));

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
	inputPort->setParent(this);
	outputPort->setParent(this);
	iconContainer->add(inputPort);
	iconContainer->add(outputPort);
	iconContainer->add(nodeIcon);
	labelRegion = ModifiableLabelPtr(
			new ModifiableLabel(name,
					CoordPX(0.0f, 2 * InputPort::DIMENSIONS.y + 1.0f),
					CoordPerPX(0.0f, 1.0f,
							std::max(10 + textWidth, Node::DIMENSIONS.x),
							-2 * OutputPort::DIMENSIONS.y
									- 2 * InputPort::DIMENSIONS.y - 2.0f)));
	labelRegion->setValue(label);
	labelRegion->setAlignment(HorizontalAlignment::Left,
			VerticalAlignment::Middle);
	labelRegion->fontSize = UnitPX(fontSize);
	labelRegion->fontType = FontType::Bold;
	labelRegion->onTextEntered =
			[this](TextField* field) {
				textWidth = labelRegion->getTextDimensions(AlloyApplicationContext().get()).x;
				labelRegion->dimensions = CoordPerPX(0.0f, 1.0f, std::max(10 + textWidth, Node::DIMENSIONS.x), -2 * OutputPort::DIMENSIONS.y - 2 * InputPort::DIMENSIONS.y - 2.0f);
			};
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
	nodeIcon->setShape(NodeShape::Circle);
	nodeIcon->backgroundColor = MakeColor(COLOR);
	nodeIcon->borderWidth = borderWidth;
	forceItem->buoyancy = 0;

}
void Compute::setup() {
	setOrientation(Orientation::Horizontal, pixel2(0, 0));
	NVGcontext* nvg = AlloyApplicationContext()->nvgContext;
	nvgFontSize(nvg, fontSize);
	nvgFontFaceId(nvg,
			AlloyApplicationContext()->getFont(FontType::Bold)->handle);
	textWidth = nvgTextBounds(nvg, 0, 0, label.c_str(), nullptr, nullptr);
	CompositePtr iconContainer = MakeComposite("Icon Container",
			CoordPX(0.0f, 0.0f),
			CoordPX(
					Node::DIMENSIONS.x - InputPort::DIMENSIONS.x
							- OutputPort::DIMENSIONS.x, Node::DIMENSIONS.y));

	CompositePtr labelContainer = MakeComposite("label Container",
			CoordPX(Node::DIMENSIONS.x, 0.0f),
			CoordPerPX(1.0f, 0.0f, 0.0f, Node::DIMENSIONS.y));

	nodeIcon = NodeIconPtr(
			new NodeIcon("Icon", CoordPX(0.0f, InputPort::DIMENSIONS.y + 1.0f),
					CoordPerPX(1.0f, 1.0f, 0.0f,
							-OutputPort::DIMENSIONS.y - InputPort::DIMENSIONS.y
									- 2.0f)));
	nodeIcon->setAspectRule(AspectRule::FixedHeight);
	nodeIcon->setAspectRatio(1.0f);
	inputPort = MakeInputPort("Input");
	inputPort->position = CoordPerPX(0.5f, 0.0f,
			-InputPort::DIMENSIONS.x * 0.5f, 0.0f);
	inputPort->setParent(this);
	iconContainer->add(inputPort);
	iconContainer->add(nodeIcon);
	labelRegion = ModifiableLabelPtr(
			new ModifiableLabel(name,
					CoordPX(2.0f, 2 * InputPort::DIMENSIONS.y + 1.0f),
					CoordPerPX(0.0f, 1.0f,
							std::max(10 + textWidth, Node::DIMENSIONS.x),
							-2 * OutputPort::DIMENSIONS.y
									- 2 * InputPort::DIMENSIONS.y - 2.0f)));
	labelRegion->setValue(label);
	labelRegion->setAlignment(HorizontalAlignment::Left,
			VerticalAlignment::Middle);
	labelRegion->fontSize = UnitPX(fontSize);
	labelRegion->fontType = FontType::Bold;
	labelRegion->onTextEntered =
			[this](TextField* field) {
				textWidth = labelRegion->getTextDimensions(AlloyApplicationContext().get()).x;
				labelRegion->dimensions = CoordPerPX(0.0f, 1.0f, std::max(10 + textWidth, Node::DIMENSIONS.x), -2 * OutputPort::DIMENSIONS.y - 2 * InputPort::DIMENSIONS.y - 2.0f);
			};
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
	nodeIcon->backgroundColor = MakeColor(COLOR);
	nodeIcon->borderWidth = borderWidth;
	nodeIcon->setShape(NodeShape::Hexagon);
	forceItem->buoyancy = 0;

}
void Source::setup() {
	CompositePtr iconContainer = MakeComposite("Icon Container",
			CoordPerPX(0.5f, 0.0f, -Node::DIMENSIONS.x * 0.5f,
					fontSize + 2 * TextField::PADDING),
			CoordPX(Node::DIMENSIONS.x,
					Node::DIMENSIONS.y - InputPort::DIMENSIONS.y));
	nodeIcon = NodeIconPtr(
			new NodeIcon("Icon", CoordPX(ParentPort::DIMENSIONS.x + 1.0f, 1.0f),
					CoordPerPX(1.0f, 1.0f,
							-ParentPort::DIMENSIONS.x - ChildPort::DIMENSIONS.x
									- 2.0f, -OutputPort::DIMENSIONS.y - 2.0f)));
	nodeIcon->setAspectRatio(1.0f);
	nodeIcon->setAspectRule(AspectRule::FixedHeight);
	outputPort = MakeOutputPort("Output");
	outputPort->setParent(this);
	parentPort = MakeParentPort("Parent");
	childPort = MakeChildPort("Child");
	outputPort->position = CoordPerPX(0.5f, 1.0f,
			-OutputPort::DIMENSIONS.x * 0.5f, -OutputPort::DIMENSIONS.y);
	parentPort->position = CoordPerPX(0.0f, 0.5f, 0.0f,
			-OutputPort::DIMENSIONS.y);
	childPort->position = CoordPerPX(1.0f, 0.5f, -ChildPort::DIMENSIONS.x,
			-OutputPort::DIMENSIONS.y);
	childPort->setParent(this);
	parentPort->setParent(this);
	iconContainer->add(nodeIcon);
	iconContainer->add(outputPort);
	iconContainer->add(parentPort);
	iconContainer->add(childPort);
	NVGcontext* nvg = AlloyApplicationContext()->nvgContext;
	nvgFontSize(nvg, fontSize);
	nvgFontFaceId(nvg,
			AlloyApplicationContext()->getFont(FontType::Bold)->handle);
	textWidth = nvgTextBounds(nvg, 0, 0, label.c_str(), nullptr, nullptr);
	labelRegion = ModifiableLabelPtr(
			new ModifiableLabel(name, CoordPerPX(0.5f, 0.0f, 0.0f, 0.0f),
					CoordPX(std::max(textWidth + 10.0f, Node::DIMENSIONS.x),
							fontSize + 2 * TextField::PADDING)));
	labelRegion->setValue(label);
	labelRegion->setOrigin(Origin::TopCenter);
	labelRegion->setAlignment(HorizontalAlignment::Center,
			VerticalAlignment::Middle);
	labelRegion->fontSize = UnitPX(fontSize);
	labelRegion->fontType = FontType::Bold;
	labelRegion->onTextEntered =
			[this](TextField* field) {
				textWidth = labelRegion->getTextDimensions(AlloyApplicationContext().get()).x;
				labelRegion->dimensions = CoordPX(std::max(textWidth + 10.0f, Node::DIMENSIONS.x), fontSize + 2 * TextField::PADDING);
			};
	Composite::add(labelRegion);
	Composite::add(iconContainer);
	setRoundCorners(true);
	nodeIcon->backgroundColor = MakeColor(COLOR);
	nodeIcon->borderWidth = borderWidth;
	forceItem->buoyancy = -1;

}
void Destination::setup() {
	CompositePtr iconContainer = MakeComposite("Icon Container",
			CoordPerPX(0.5f, 0.0f, -Node::DIMENSIONS.x * 0.5f, 0.0f),
			CoordPX(Node::DIMENSIONS.x,
					Node::DIMENSIONS.y - InputPort::DIMENSIONS.y));
	nodeIcon = NodeIconPtr(
			new NodeIcon("Icon",
					CoordPX(0.5f * ParentPort::DIMENSIONS.x,
							InputPort::DIMENSIONS.y + 1.0f),
					CoordPerPX(1.0f, 1.0f, -ParentPort::DIMENSIONS.x,
							-OutputPort::DIMENSIONS.y - 2.0f)));
	inputPort = MakeInputPort("Input");
	inputPort->position = CoordPerPX(0.5f, 0.0f,
			-InputPort::DIMENSIONS.x * 0.5f, 0.0f);
	inputPort->setParent(this);
	iconContainer->add(nodeIcon);
	iconContainer->add(inputPort);
	NVGcontext* nvg = AlloyApplicationContext()->nvgContext;
	nvgFontSize(nvg, fontSize);
	nvgFontFaceId(nvg,
			AlloyApplicationContext()->getFont(FontType::Bold)->handle);
	textWidth = nvgTextBounds(nvg, 0, 0, label.c_str(), nullptr, nullptr);
	labelRegion = ModifiableLabelPtr(
			new ModifiableLabel(name,
					CoordPerPX(0.5f, 0.0f, 0.0f,
							Node::DIMENSIONS.y - InputPort::DIMENSIONS.y),
					CoordPX(std::max(textWidth + 10.0f, Node::DIMENSIONS.x),
							fontSize + 2 * TextField::PADDING)));
	labelRegion->setValue(label);
	labelRegion->setOrigin(Origin::TopCenter);
	labelRegion->setAlignment(HorizontalAlignment::Center,
			VerticalAlignment::Middle);
	labelRegion->fontSize = UnitPX(fontSize);
	labelRegion->fontType = FontType::Bold;
	labelRegion->onTextEntered =
			[this](TextField* field) {
				textWidth = labelRegion->getTextDimensions(AlloyApplicationContext().get()).x;
				labelRegion->dimensions = CoordPX(std::max(textWidth + 10.0f, Node::DIMENSIONS.x), fontSize + 2 * TextField::PADDING);
			};
	Composite::add(iconContainer);
	Composite::add(labelRegion);
	setRoundCorners(true);
	nodeIcon->backgroundColor = MakeColor(COLOR);
	nodeIcon->setShape(NodeShape::Triangle);
	nodeIcon->borderWidth = borderWidth;
	forceItem->buoyancy = 1;

}
bool Node::isMouseOver() const {
	if (parent != nullptr)
		return parentFlow->isMouseOverNode(this);
	return false;
}
ForceItemPtr& Node::getForceItem() {
	return forceItem;
}
void DataFlow::setCurrentPort(Port* currentPort) {
	this->currentPort = currentPort;
}
Connection::~Connection() {

	if (source.get() != nullptr)
		source->disconnect(this);
	if (destination.get() != nullptr)
		destination->disconnect(this);
}
void Port::disconnect(Connection* connection) {
	for (auto iter = connections.begin(); iter != connections.end(); iter++) {
		if (iter->get() == connection) {
			connections.erase(iter);
			iter--;
		}
	}
}
void DataFlow::groupSelected() {
	GroupPtr group = MakeGroupNode("Group");
	float2 center(0.0f);
	std::list<ConnectionPtr> connectionList;
	std::list<RelationshipPtr> relationshipList;
	for (ConnectionPtr connection : data->connections) {
		connection->setSelected(false);//Do not delete connections on group
	}
	for (NodePtr node : data->nodes) {
		if (node->isSelected()) {
			center += node->getLocation();
			group->nodes.push_back(node);
			for (InputPortPtr port : node->getInputPorts()) {
				if (port->isConnected()) {
					InputPortPtr newPort = MakeInputPort(port->name);
					bool outside = false;
					for (ConnectionPtr connection : port->getConnections()) {
						if (!connection->source->getNode()->isSelected()) {
							outside = true;
							connectionList.push_back(
									MakeConnection(connection->source,
											newPort));
							connection->source->disconnect(connection.get());
						} else {
							group->connections.push_back(connection);
						}
					}
					if (outside) {
						port->setProxyIn(newPort);
						newPort->setProxyOut(port);
						group->add(newPort);
					}
				}
			}
			for (OutputPortPtr port : node->getOutputPorts()) {
				if (port->isConnected()) {
					OutputPortPtr newPort = MakeOutputPort(port->name);
					bool outside = false;
					for (ConnectionPtr connection : port->getConnections()) {
						if (!connection->destination->getNode()->isSelected()) {
							outside = true;
							connectionList.push_back(
									MakeConnection(newPort,
											connection->destination));
							connection->destination->disconnect(
									connection.get());
						} else {
							group->connections.push_back(connection);
						}
					}
					if (outside) {
						port->setProxyIn(newPort);
						newPort->setProxyOut(port);
						group->add(newPort);
					}
				}
			}
			{
				ParentPortPtr port = node->getParentPort();
				if (port.get() != nullptr && port->isConnected()) {
					for (ConnectionPtr connection : port->getConnections()) {
						if (connection->source->getNode()->isSelected()) {
							group->connections.push_back(connection);
						}
					}
				}
			}
			{
				ChildPortPtr port = node->getChildPort();
				if (port.get() != nullptr && port->isConnected()) {
					for (ConnectionPtr connection : port->getConnections()) {
						if (connection->destination->getNode()->isSelected()) {
							group->connections.push_back(connection);
						}
					}
				}
			}
			{
				InputPortPtr port = node->getInputPort();
				if (port.get() != nullptr && port->isConnected()) {
					InputPortPtr newPort = MakeInputPort(port->name);;
					bool outside = false;
					for (ConnectionPtr connection : port->getConnections()) {
						if (!connection->source->getNode()->isSelected()) {
							outside = true;
							connectionList.push_back(
									MakeConnection(connection->source,
											newPort));
							connection->source->disconnect(connection.get());
						} else {
							group->connections.push_back(connection);
						}
					}
					if (outside) {
						port->setProxyIn(newPort);
						newPort->setProxyOut(port);
						group->add(newPort);
					}
				}
			}
			{
				OutputPortPtr port = node->getOutputPort();
				if (port.get() != nullptr && port->isConnected()) {
					OutputPortPtr newPort = MakeOutputPort(port->name);;
					bool outside = false;
					for (ConnectionPtr connection : port->getConnections()) {
						if (!connection->destination->getNode()->isSelected()) {
							outside = true;
							connectionList.push_back(
									MakeConnection(newPort,
											connection->destination));
							connection->destination->disconnect(
									connection.get());
						} else {
							group->connections.push_back(connection);
						}
					}
					if (outside) {
						port->setProxyIn(newPort);
						newPort->setProxyOut(port);
						group->add(newPort);
					}
				}
			}
		}
	}
	for (RelationshipPtr relationship : data->relationships) {
		if (relationship->object->isSelected()
				&& !relationship->subject->isSelected()) {
			relationshipList.push_back(
					MakeRelationship(relationship->subject,
							relationship->predicate, group));
			group->relationships.push_back(
					MakeRelationship(relationship->subject,
							relationship->predicate, relationship->object));
		} else if (!relationship->object->isSelected()
				&& relationship->subject->isSelected()) {
			relationshipList.push_back(
					MakeRelationship(group, relationship->predicate,
							relationship->object));
			group->relationships.push_back(
					MakeRelationship(relationship->subject,
							relationship->predicate, relationship->object));
		} else if (relationship->object->isSelected()
				&& relationship->subject->isSelected()) {
			group->relationships.push_back(relationship);
		}
	}
	if (group->nodes.size() == 0)
		return;
	center /= (float) group->nodes.size();
	group->setLocation(center);
	add(group);
	for (ConnectionPtr connection : connectionList) {
		add(connection);
		connection->update();
	}
	for (RelationshipPtr relationship : relationshipList) {
		add(relationship);
		relationship->update();
	}
	deleteSelected();
	for (NodePtr node : group->nodes) {
		node->setSelected(false);
	}
	for (ConnectionPtr connection : group->connections) {
		connection->setSelected(false);
	}

}
void DataFlow::ungroupSelected() {
	std::vector<GroupPtr> deleteList;
	for (NodePtr node : data->nodes) {
		if (node->isSelected() && node->getType() == NodeType::Group) {
			GroupPtr group = std::dynamic_pointer_cast<Group>(node);
			deleteList.push_back(group);
		}
		node->setSelected(false);//Do not delete things that are not groups.
	}
	for (ConnectionPtr connection : data->connections) {
		connection->setSelected(false);//Do not delete connections on ungroup
	}
	for (GroupPtr group : deleteList) {
		group->setSelected(true);
		std::list<NodePtr> nodeList;
		std::list<ConnectionPtr> connectionList;
		std::list<RelationshipPtr> relationshipList;
			float2 center(0.0f);
			if (group->nodes.size() > 0) {
				for (NodePtr node : group->nodes) {
					center += node->getLocation();
				}
				center /= (float)group->nodes.size();
				center -= group->getLocation();
				for (NodePtr node : group->nodes) {
					node->setLocation(node->getLocation() - center);
				}
				nodeList.insert(nodeList.end(), group->nodes.begin(), group->nodes.end());
			}
			relationshipList.insert(relationshipList.end(), group->relationships.begin(), group->relationships.end());
			connectionList.insert(connectionList.end(),group->connections.begin(), group->connections.end());
			for (InputPortPtr port : group->getInputPorts()) {
				PortPtr proxy = port->getProxyOut();
				if (proxy.get() != nullptr) {
					proxy->setProxyIn();
					for (ConnectionPtr connection : port->getConnections()) {
						connectionList.push_back(MakeConnection(connection->source, proxy));
					}
				}
				port->getConnections().clear();
			}
			for (OutputPortPtr port : group->getOutputPorts()) {
				PortPtr proxy = port->getProxyOut();
				if (proxy.get() != nullptr) {
					proxy->setProxyIn();
					for (ConnectionPtr connection : port->getConnections()) {
						connectionList.push_back(MakeConnection(proxy, connection->destination));
					}
				}
				port->getConnections().clear();
			}
		deleteSelected();
		for (NodePtr node : nodeList) {
			addNode(node);
		}
		for (ConnectionPtr connection : connectionList) {
			add(connection);
		}
		for (RelationshipPtr connection : relationshipList) {
			add(connection);
		}
	}

}
void DataFlow::deleteSelected() {
	std::list<ForceItemPtr> deleteForceList;
	std::list<Node*> deleteNodeList;
	{
		std::vector<RegionPtr> tmpList;
		router.nodes.clear();
		data->nodes.clear();
		for (RegionPtr child : children) {
			Node* node = dynamic_cast<Node*>(child.get());
			if (node) {
				if (node->isSelected()) {
					deleteForceList.push_back(node->forceItem);
					deleteNodeList.push_back(node);
					node->parent=nullptr;
				} else {
					NodePtr nodePtr = std::dynamic_pointer_cast<Node>(child);
					router.nodes.push_back(nodePtr);
					data->nodes.push_back(nodePtr);
					tmpList.push_back(child);
				}
			} else {
				tmpList.push_back(child);
			}
		}
		router.update();
		children = tmpList;
	}
	std::list<SpringItemPtr> deleteSpringList;
	{
		std::vector<ConnectionPtr> tmpList;
		for (ConnectionPtr connection : data->connections) {
			if (connection->source->getNode()->isSelected()|| connection->destination->getNode()->isSelected()|| connection->selected) {
				connection->source->disconnect(connection);
				connection->destination->disconnect(connection);
				if (connection->selected) {
					deleteSpringList.push_back(connection->getSpringItem());
				}
			} else {
				tmpList.push_back(connection);
			}
		}

		routingLock.lock();
		data->connections = tmpList;
		routingLock.unlock();
	}
	{
		std::vector<RelationshipPtr> tmpList;
		for (RelationshipPtr relationship : data->relationships) {
			if (!relationship->object->isSelected()
					&& !relationship->subject->isSelected()) {
				tmpList.push_back(relationship);
			}
		}
		data->relationships = tmpList;
	}

	forceSim->erase(deleteForceList);
	forceSim->erase(deleteSpringList);
}
bool DataFlow::onEventHandler(AlloyContext* context, const InputEvent& e) {
	if (connectingPort != nullptr && e.type == InputType::MouseButton
			&& e.isUp()) {
		if (currentPort != nullptr && currentPort != connectingPort
				&& context->isMouseOver(currentPort)) {
			PortPtr source = connectingPort->getReference();
			PortPtr target = currentPort->getReference();
			ConnectionPtr last;
			if (source->getType() == PortType::Output
					&& target->getType() == PortType::Input) {
				add(last = MakeConnection(source, target));
			} else if (source->getType() == PortType::Input
					&& target->getType() == PortType::Output) {
				add(last = MakeConnection(target, source));
			} else if (source->getType() == PortType::Parent
					&& target->getType() == PortType::Child) {
				add(last = MakeConnection(target, source));
			} else if (source->getType() == PortType::Child
					&& target->getType() == PortType::Parent) {
				add(last = MakeConnection(source, target));
			}
		}
		connectingPort = nullptr;
	}

	if (Composite::onEventHandler(context, e))
		return true;

	if (mouseOverNode != nullptr) {
		forceSim->setSelected(mouseOverNode->forceItem.get());
		mouseOverNode->forceItem->velocity = float2(0.0f);
		mouseOverNode->forceItem->plocation =
				mouseOverNode->forceItem->location;
	}
	if (mouseDragNode != nullptr) {
		if (draggingNode && e.type == InputType::Cursor) {
			for (std::pair<Node*, pixel2> pr : dragList) {
				pr.first->setDragOffset(e.cursor, pr.second);
			}
			dragAction = true;
			context->requestPack();
		} else if (e.type == InputType::MouseButton && e.isUp()) {
			context->requestPack();
			dragList.clear();
			mouseDragNode = nullptr;
			dragAction = false;
			draggingNode = false;
		}
	}
	if (!draggingNode && mouseSelectedNode != nullptr) {
		if (e.type == InputType::MouseButton
				&& e.button == GLFW_MOUSE_BUTTON_LEFT && e.isDown()) {
			mouseDragNode = mouseSelectedNode;
			if (context->isShiftDown()) {
				mouseDragNode->setSelected(!mouseDragNode->isSelected());
			}
			putLast(mouseDragNode);
			dragList.clear();
			if (mouseDragNode->isSelected()) {
				for (RegionPtr child : children) {
					Node* node = dynamic_cast<Node*>(child.get());
					if (node && node->isSelected()) {
						dragList.push_back(
								std::pair<Node*, pixel2>(node,
										e.cursor - node->getBoundsPosition()));
					}
				}
			} else {
				dragList.push_back(
						std::pair<Node*, pixel2>(mouseDragNode,
								e.cursor - mouseDragNode->getBoundsPosition()));
			}
			draggingNode = true;
			dragAction = false;
		}
	}
	if (e.type == InputType::Cursor || e.type == InputType::MouseButton) {
		if (context->isMouseDrag() && e.button == GLFW_MOUSE_BUTTON_LEFT) {
			if (dragBox.dimensions.x * dragBox.dimensions.y > 0
					|| (connectingPort == nullptr && mouseOverNode == nullptr
							&& mouseDragNode == nullptr)) {
				float2 cursorDown = context->getCursorDownPosition();
				float2 stPt = aly::min(cursorDown, e.cursor);
				float2 endPt = aly::max(cursorDown, e.cursor);
				dragBox.position = stPt;
				dragBox.dimensions = endPt - stPt;
				dragBox.intersect(getBounds());
			}
		}
	}
	if (e.type == InputType::Cursor) {
		if (context->isMouseOver(forceSim.get())
				|| !context->isMouseContainedIn(this)) {
			forceSim->setSelected(nullptr);
		}
	} else if (e.type == InputType::MouseButton) {
		if (e.isDown()) {
			if (e.button == GLFW_MOUSE_BUTTON_LEFT) {
				if (selectedConnection) {
					if (!context->isShiftDown()) {
						for (ConnectionPtr connection : data->connections) {
							connection->selected = false;
						}
					}
					selectedConnection->selected = true;
				}
			}
			if (!context->isControlDown()) {
				forceSim->stop();
			}

		} else if (e.isUp()) {
			if (e.button == GLFW_MOUSE_BUTTON_RIGHT && !dragAction
					&& draggingGraph) {
				for (ConnectionPtr connection : data->connections) {
					connection->selected = false;
				}
				for (RegionPtr child : children) {
					Node* node = dynamic_cast<Node*>(child.get());
					if (node) {
						node->setSelected(false);
					}
				}

			}
			forceSim->start();
		}
	}
	if (e.type == InputType::MouseButton && e.isUp()) {
		if (draggingGraph) {
			draggingGraph = false;
			dragAction = false;
		} else if (draggingNode) {
			dragAction = false;
			draggingNode = false;
		} else if (dragBox.dimensions.x * dragBox.dimensions.y > 0) {
			pixel2 offset = getDrawOffset();
			for (RegionPtr child : children) {
				Node* node = dynamic_cast<Node*>(child.get());
				if (node) {
					if (dragBox.contains(
							node->getForceItem()->location + offset)) {
						node->setSelected(true);
					}
				}
			}
			dragBox = box2px(pixel2(0.0f), pixel2(0.0f));
		}
	}

	if (e.type
			== InputType::MouseButton&&e.isDown() && e.button == GLFW_MOUSE_BUTTON_RIGHT) {
		currentDrawOffset = this->extents.position;
		cursorDownLocation = e.cursor;
		draggingGraph = true;
		dragAction = false;
	}
	if (e.type == InputType::Cursor && draggingGraph) {
		this->extents.position = currentDrawOffset + e.cursor
				- cursorDownLocation;
		if (lengthL1(e.cursor - cursorDownLocation) > 5) {
			dragAction = true;
		}
	}
	if (e.isDown() && e.type == InputType::Key) {
		switch (e.key) {
		case GLFW_KEY_DELETE:
			deleteSelected();
			return true;
			break;
		case GLFW_KEY_A:
			if (e.isControlDown()) {
				for (RegionPtr child : children) {
					Node* node = dynamic_cast<Node*>(child.get());
					if (node) {
						node->setSelected(true);
					}
				}
				return true;
			}
			break;
		case GLFW_KEY_D:
			if (e.isControlDown()) {
				for (RegionPtr child : children) {
					Node* node = dynamic_cast<Node*>(child.get());
					if (node) {
						node->setSelected(false);
					}
				}
				return true;
			}
			break;
		case GLFW_KEY_G:
			if (e.isControlDown()) {
				groupSelected();
			}
			break;
		case GLFW_KEY_U:
			if (e.isControlDown()) {
				ungroupSelected();
			}
			break;
		case GLFW_KEY_I:
			if (e.isControlDown()) {
				for (RegionPtr child : children) {
					Node* node = dynamic_cast<Node*>(child.get());
					if (node) {
						node->setSelected(!node->isSelected());
					}
				}
				return true;
			}
			break;
		}
	}
	return Composite::onEventHandler(context, e);
}
float Connection::distance(const float2& pt) {
	float minD = 1E30f;
	for (int i = 0; i < (int) path.size() - 1; i++) {
		lineseg2f ls(path[i], path[i + 1]);
		minD = std::min(ls.distance(pt), minD);
	}
	return minD;
}
void Connection::draw(AlloyContext* context, DataFlow* flow) {
	if (path.size() == 0)
		return;
	NVGcontext* nvg = context->nvgContext;
	if (selected) {
		nvgStrokeWidth(nvg, 6.0f);

		nvgStrokeColor(nvg, context->theme.HIGHLIGHT);
	} else {
		nvgStrokeWidth(nvg, 4.0f);

		nvgStrokeColor(nvg, context->theme.HIGHLIGHT.toDarker(0.8f));
	}
	pixel2 start;
	pixel2 end;
	if (source.get() != nullptr) {
		box2px bounds = source->getBounds();
		const float nudge = context->theme.CORNER_RADIUS;
		switch (source->getType()) {
		case PortType::Input:
			start = pixel2(bounds.position.x + bounds.dimensions.x * 0.5f,
					bounds.position.y + nudge);
			break;
		case PortType::Output:
			start = pixel2(bounds.position.x + bounds.dimensions.x * 0.5f,
					bounds.position.y + bounds.dimensions.y - nudge);
			break;
		case PortType::Child:
			start = pixel2(bounds.position.x + bounds.dimensions.x - nudge,
					bounds.position.y + bounds.dimensions.y * 0.5f);
			break;
		case PortType::Parent:
			start = pixel2(bounds.position.x + nudge,
					bounds.position.y + bounds.dimensions.y * 0.5f);
			break;
		case PortType::Unknown:
			start = pixel2(bounds.position.x + bounds.dimensions.x * 0.5f,
					bounds.position.y + bounds.dimensions.y * 0.5f);
		}
	}
	if (destination.get() != nullptr) {
		box2px bounds = destination->getBounds();
		switch (destination->getType()) {
		case PortType::Input:
			end = pixel2(bounds.position.x + bounds.dimensions.x * 0.5f,
					bounds.position.y);
			break;
		case PortType::Output:
			end = pixel2(bounds.position.x + bounds.dimensions.x * 0.5f,
					bounds.position.y + bounds.dimensions.y);
			break;
		case PortType::Child:
			end = pixel2(bounds.position.x + bounds.dimensions.x,
					bounds.position.y + bounds.dimensions.y * 0.5f);
			break;
		case PortType::Parent:
			end = pixel2(bounds.position.x,
					bounds.position.y + bounds.dimensions.y * 0.5f);
			break;
		case PortType::Unknown:
			end = pixel2(bounds.position.x + bounds.dimensions.x * 0.5f,
					bounds.position.y + bounds.dimensions.y * 0.5f);
		}
	}
	float2 offset = flow->getDrawOffset();
	nvgLineCap(nvg, NVG_ROUND);
	nvgLineJoin(nvg, NVG_BEVEL);
	nvgBeginPath(nvg);
	float2 pt0 = offset + path.front();
	nvgMoveTo(nvg, pt0.x, pt0.y);
	for (int i = 1; i < (int) path.size() - 1; i++) {
		float2 pt1 = offset + path[i];
		float2 pt2 = offset + path[i + 1];
		float diff = 0.5f
				* std::min(
						std::max(std::abs(pt1.x - pt2.x),
								std::abs(pt1.y - pt2.y)),
						std::max(std::abs(pt1.x - pt0.x),
								std::abs(pt1.y - pt0.y)));
		if (diff < context->theme.CORNER_RADIUS) {
			nvgLineTo(nvg, pt1.x, pt1.y);
		} else {
			nvgArcTo(nvg, pt1.x, pt1.y, pt2.x, pt2.y,
					context->theme.CORNER_RADIUS);
		}
		pt0 = pt1;
	}
	pt0 = offset + path.back();
	nvgLineTo(nvg, pt0.x, pt0.y);
	nvgStroke(nvg);
}
bool DataFlow::intersects(const lineseg2f& ln) {
	for (box2px box : router.getObstacles()) {
		if (ln.intersects(box))
			return true;
	}
	return false;
}
void DataFlow::startConnection(Port* port) {
	connectingPort = port;
}

void DataFlow::add(const std::shared_ptr<Region>& region) {
	Composite::add(region);
}
std::vector<std::shared_ptr<Connection>>& DataFlow::getConnections() {
	return data->connections;
}
std::vector<std::shared_ptr<Relationship>>& DataFlow::getRelationships() {
	return data->relationships;
}
const std::vector<std::shared_ptr<Connection>>& DataFlow::getConnections() const {
	return data->connections;
}
const std::vector<std::shared_ptr<Relationship>>& DataFlow::getRelationships() const {
	return data->relationships;
}

void DataFlow::add(const std::shared_ptr<Relationship>& relationship) {
	relationship->getSpringItem().reset(
			new SpringItem(relationship->object->getForceItem(),
					relationship->subject->getForceItem(), -1.0f,
					std::max(
							distance(
									relationship->object->getForceItem()->location,
									relationship->subject->getForceItem()->location),
							2 * Node::DIMENSIONS.x)));
	relationship->getSpringItem()->visible = false;
	routingLock.lock();
	forceSim->addSpringItem(relationship->getSpringItem());
	data->relationships.push_back(relationship);
	routingLock.unlock();
}
void DataFlow::addNode(const std::shared_ptr<Node>& node) {
	Composite::add(node);
	data->nodes.push_back(node);
	routingLock.lock();
	router.add(node);
	routingLock.unlock();
	forceSim->addForceItem(node->getForceItem());
	node->parentFlow = this;
}

void DataFlow::setGroup(const std::shared_ptr<Group>& g) {
	routingLock.lock();
	router.nodes = g->nodes;
	router.update();
	routingLock.unlock();
	forceSim->clear();
	std::vector<RegionPtr> tmpList;
	for (RegionPtr child : children) {
		Node* node = dynamic_cast<Node*>(child.get());
		if (node == nullptr) {
			tmpList.push_back(child);
		}
	}
	children = tmpList;
	for (NodePtr node : g->nodes) {
		node->parent = this;
		forceSim->addForceItem(node->getForceItem());
		children.push_back(node);
	}
	for (ConnectionPtr connection : g->connections) {
		forceSim->addSpringItem(connection->getSpringItem());
	}
	for (RelationshipPtr relationship : g->relationships) {
		forceSim->addSpringItem(relationship->getSpringItem());
	}
	graphBounds = box2px(pixel2(0.0f), pixel2(0.0f));
	AlloyApplicationContext()->requestPack();
}
void DataFlow::add(const std::shared_ptr<Source>& node) {
	addNode(node);
}
void DataFlow::add(const std::shared_ptr<Destination>& node) {
	addNode(node);
}
void DataFlow::add(const std::shared_ptr<Data>& node) {
	addNode(node);
}
void DataFlow::add(const std::shared_ptr<Group>& node) {
	addNode(node);
}
void DataFlow::add(const std::shared_ptr<View>& node) {
	addNode(node);
}
void DataFlow::add(const std::shared_ptr<Compute>& node) {
	addNode(node);
}
void DataFlow::add(const std::shared_ptr<Connection>& connection) {
	SpringItem* spring = new SpringItem(
			connection->source->getNode()->getForceItem(),
			connection->destination->getNode()->getForceItem(), -1.0f,
			2 * ForceSimulator::RADIUS);
	spring->gamma = 0.1f;
	spring->visible = false;
	spring->length = distance(spring->item1->location, spring->item2->location);
	connection->getSpringItem().reset(spring);
	routingLock.lock();
	forceSim->addSpringItem(connection->getSpringItem());
	data->connections.push_back(connection);
	routingLock.unlock();
}
Connection* DataFlow::closestConnection(const float2& pt, float tolernace) {
	for (int i = (int) data->connections.size() - 1; i >= 0; i--) {
		Connection* c = data->connections[i].get();
		float d = c->distance(pt);

		if (d < tolernace) {
			return c;
		}
	}
	return nullptr;
}
void DataFlow::setup() {
	data = MakeGroupNode(this->name);
	mouseDragNode = nullptr;
	mouseSelectedNode = nullptr;
	mouseOverNode = nullptr;
	dragBox = box2px(float2(0, 0), float2(0, 0));
	graphBounds = box2px(pixel2(0.0f), pixel2(0.0f));
	setRoundCorners(true);
	backgroundColor = MakeColor(AlloyApplicationContext()->theme.DARK);
	forceSim = ForceSimulatorPtr(
			new ForceSimulator("Force Simulator", CoordPX(0.0f, 0.0f),
					CoordPercent(1.0f, 1.0f)));
	DrawPtr pathsRegion =
			DrawPtr(
					new Draw("Paths", CoordPX(0.0f, 0.0f),
							CoordPercent(1.0f, 1.0f),
							[this](AlloyContext* context, const box2px& bounds) {
								for (RelationshipPtr& relationship : data->relationships) {
									relationship->draw(context);
								}
								routingLock.lock();
								for (ConnectionPtr& connection : data->connections) {
									connection->draw(context, this);
								}
								routingLock.unlock();
								for (RelationshipPtr& relationship : data->relationships) {
									relationship->drawText(context);
								}
							}));

	Composite::add(pathsRegion);
	Composite::add(forceSim);
	Application::addListener(this);
	forceSim->addForce(SpringForcePtr(new SpringForce()));
	forceSim->addForce(NBodyForcePtr(new NBodyForce()));
	forceSim->addForce(MakeShared<BuoyancyForce>());
	forceSim->addForce(
			boxForce = BoxForcePtr(
					new BoxForce(
							box2px(pixel2(0.0f, 0.0f), pixel2(1920, 1080)))));
	boxForce->setVisible(false);
	forceSim->addForce(DragForcePtr(new DragForce(0.001f)));
	forceSim->setZoom(1.0f);
	forceSim->setOffset(pixel2(0.0f, 0.0f));
	forceSim->onStep = [this](float stepSize) {
		AlloyContext* context = AlloyApplicationContext().get();
		if (context) {
			context->requestPack();
		}
	};

	start();
}
void DataFlow::start() {
	forceSim->start();
}
void DataFlow::stop() {
	forceSim->stop();
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
std::shared_ptr<Port> Port::getReference() {
	if (parent->inputPort.get() == this) {
		return parent->inputPort;
	}
	if (parent->outputPort.get() == this) {
		return parent->outputPort;
	}
	if (parent->childPort.get() == this) {
		return parent->childPort;
	}
	if (parent->parentPort.get() == this) {
		return parent->parentPort;
	}
	for (std::shared_ptr<Port> port : parent->inputPorts) {
		if (port.get() == this)
			return port;
	}
	for (std::shared_ptr<Port> port : parent->outputPorts) {
		if (port.get() == this)
			return port;
	}
	return std::shared_ptr<Port>();

}
void Port::setup() {
	position = CoordPX(0.0f, 0.0f);
	borderWidth = UnitPX(1.0f);
	onMouseDown = [this](AlloyContext* context, const InputEvent& e) {
		if (e.button == GLFW_MOUSE_BUTTON_LEFT&&e.isDown()) {
			getGraph()->startConnection(this);
		}
		return false;
	};
}
DataFlow* Port::getGraph() const {
	return (parent != nullptr) ? parent->parentFlow : nullptr;
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
	if (!isVisible())
		return;

	NVGcontext* nvg = context->nvgContext;
	box2px bounds = getBounds();
	pixel lineWidth = borderWidth.toPixels(bounds.dimensions.y, context->dpmm.y,
			context->pixelRatio);
	bool over = false;
	if (isExternalized()) {
		if (context->isMouseOver(this)) {
			getGraph()->setCurrentPort(this);
			over = true;
			nvgFillColor(nvg, Color(context->theme.DARK));
			nvgStrokeColor(nvg, Color(context->theme.HIGHLIGHT));
		} else {
			nvgFillColor(nvg, Color(context->theme.DARK));
			nvgStrokeColor(nvg, Color(context->theme.HIGHLIGHT));
		}
	} else {
		if (context->isMouseOver(this)) {
			nvgFillColor(nvg, Color(context->theme.HIGHLIGHT));
			nvgStrokeColor(nvg, Color(context->theme.HIGHLIGHT));
			context->setCursor(&Cursor::CrossHairs);
			getGraph()->setCurrentPort(this);
			over = true;
		} else {
			if (isConnected()) {
				nvgFillColor(nvg, Color(context->theme.HIGHLIGHT));
				nvgStrokeColor(nvg, Color(context->theme.HIGHLIGHT));
			} else {
				nvgFillColor(nvg, Color(context->theme.LIGHT));
				nvgStrokeColor(nvg, Color(context->theme.LIGHT));
			}
		}
	}
	nvgStrokeWidth(nvg, lineWidth);
	nvgBeginPath(nvg);
	nvgEllipse(nvg, bounds.position.x + bounds.dimensions.x * 0.5f,
			bounds.position.y + bounds.dimensions.y * 0.5f,
			bounds.dimensions.x * 0.5f - 0.5f * lineWidth,
			bounds.dimensions.y * 0.5f - 0.5f * lineWidth);
	nvgFill(nvg);
	nvgStroke(nvg);
	if (over) {
		nvgFontFaceId(nvg, context->getFontHandle(FontType::Normal));
		nvgFontSize(nvg, 18.0f);
		nvgTextAlign(nvg, NVG_ALIGN_BOTTOM | NVG_ALIGN_LEFT);
		nvgSave(nvg);
		nvgTranslate(nvg, bounds.position.x + bounds.dimensions.x,
				bounds.position.y);
		nvgRotate(nvg, -ALY_PI * 0.25f);
		aly::drawText(nvg, 0.0f, 0.0f, name.c_str(), FontStyle::Outline,
				context->theme.HIGHLIGHT, context->theme.DARK, nullptr);
		nvgRestore(nvg);
	}
}
void ParentPort::draw(AlloyContext* context) {
	NVGcontext* nvg = context->nvgContext;
	box2px bounds = getBounds();
	pixel lineWidth = borderWidth.toPixels(bounds.dimensions.y, context->dpmm.y,
			context->pixelRatio);
	bool over = false;
	if (context->isMouseOver(this)) {
		nvgFillColor(nvg, Color(context->theme.HIGHLIGHT));
		nvgStrokeColor(nvg, Color(context->theme.HIGHLIGHT));
		context->setCursor(&Cursor::CrossHairs);
		getGraph()->setCurrentPort(this);
		over = true;
	} else {
		if (isConnected()) {
			nvgFillColor(nvg, Color(context->theme.HIGHLIGHT));
			nvgStrokeColor(nvg, Color(context->theme.HIGHLIGHT));
		} else {
			nvgFillColor(nvg, Color(context->theme.LIGHT));
			nvgStrokeColor(nvg, Color(context->theme.LIGHT));
		}
	}
	nvgStrokeWidth(nvg, lineWidth);
	nvgBeginPath(nvg);
	nvgRect(nvg, bounds.position.x + 0.5f * lineWidth,
			bounds.position.y + 0.5f * lineWidth,
			bounds.dimensions.x - lineWidth, bounds.dimensions.y - lineWidth);
	nvgFill(nvg);
	nvgStroke(nvg);
	if (over) {
		nvgFontFaceId(nvg, context->getFontHandle(FontType::Normal));
		nvgFontSize(nvg, 18.0f);
		nvgTextAlign(nvg, NVG_ALIGN_BOTTOM | NVG_ALIGN_RIGHT);
		nvgSave(nvg);
		nvgTranslate(nvg, bounds.position.x + bounds.dimensions.x,
				bounds.position.y);
		aly::drawText(nvg, 0.0f, 0.0f, name.c_str(), FontStyle::Outline,
				context->theme.HIGHLIGHT, context->theme.DARK, nullptr);
		nvgRestore(nvg);
	}
}
void OutputPort::draw(AlloyContext* context) {
	if (!isVisible())
		return;
	NVGcontext* nvg = context->nvgContext;
	box2px bounds = getBounds();
	pixel lineWidth = borderWidth.toPixels(bounds.dimensions.y, context->dpmm.y,
			context->pixelRatio);
	bool over = false;
	if (isExternalized()) {
		if (context->isMouseOver(this)) {
			getGraph()->setCurrentPort(this);
			over = true;
			nvgFillColor(nvg, Color(context->theme.DARK));
			nvgStrokeColor(nvg, Color(context->theme.HIGHLIGHT));
		}
		else {
			nvgFillColor(nvg, Color(context->theme.DARK));
			nvgStrokeColor(nvg, Color(context->theme.HIGHLIGHT));
		}
	} else {
		if (context->isMouseOver(this)) {
			nvgFillColor(nvg, Color(context->theme.HIGHLIGHT));
			nvgStrokeColor(nvg, Color(context->theme.HIGHLIGHT));
			context->setCursor(&Cursor::CrossHairs);
			getGraph()->setCurrentPort(this);
			over = true;
		} else {
			if (isConnected()) {
				nvgFillColor(nvg, Color(context->theme.HIGHLIGHT));
				nvgStrokeColor(nvg, Color(context->theme.HIGHLIGHT));
			} else {
				nvgFillColor(nvg, Color(context->theme.LIGHT));
				nvgStrokeColor(nvg, Color(context->theme.LIGHT));
			}
		}
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
	if (over) {
		nvgFontFaceId(nvg, context->getFontHandle(FontType::Normal));
		nvgFontSize(nvg, 18.0f);
		nvgTextAlign(nvg, NVG_ALIGN_TOP | NVG_ALIGN_LEFT);
		nvgSave(nvg);
		nvgTranslate(nvg, bounds.position.x + bounds.dimensions.x,
				bounds.position.y + bounds.dimensions.y);
		nvgRotate(nvg, ALY_PI * 0.25f);
		aly::drawText(nvg, 0.0f, 0.0f, name.c_str(), FontStyle::Outline,
				context->theme.HIGHLIGHT, context->theme.DARK, nullptr);
		nvgRestore(nvg);
	}
}

void ChildPort::draw(AlloyContext* context) {
	NVGcontext* nvg = context->nvgContext;
	box2px bounds = getBounds();
	pixel lineWidth = borderWidth.toPixels(bounds.dimensions.y, context->dpmm.y,
			context->pixelRatio);
	bool over = false;
	if (context->isMouseOver(this)) {
		nvgFillColor(nvg, Color(context->theme.HIGHLIGHT));
		nvgStrokeColor(nvg, Color(context->theme.HIGHLIGHT));
		context->setCursor(&Cursor::CrossHairs);
		getGraph()->setCurrentPort(this);
		over = true;
	} else {
		if (isConnected()) {
			nvgFillColor(nvg, Color(context->theme.HIGHLIGHT));
			nvgStrokeColor(nvg, Color(context->theme.HIGHLIGHT));
		} else {
			nvgFillColor(nvg, Color(context->theme.LIGHT));
			nvgStrokeColor(nvg, Color(context->theme.LIGHT));
		}
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
	if (over) {
		nvgFontFaceId(nvg, context->getFontHandle(FontType::Normal));
		nvgFontSize(nvg, 18.0f);
		nvgTextAlign(nvg, NVG_ALIGN_BOTTOM | NVG_ALIGN_LEFT);
		nvgSave(nvg);
		nvgTranslate(nvg, bounds.position.x, bounds.position.y);
		aly::drawText(nvg, 0.0f, 0.0f, name.c_str(), FontStyle::Outline,
				context->theme.HIGHLIGHT, context->theme.DARK, nullptr);
		nvgRestore(nvg);
	}
}
float2 Node::getLocation() const {
	return forceItem->location;
}

void Node::setLocation(const float2& pt) {
	forceItem->location = pt;
	forceItem->plocation = pt;
	forceItem->velocity = float2(0.0f);
}
void Node::prePack() {
	pixel2 dragOffset = getDragOffset();
	if (lengthL1(dragOffset) > 0) {
		std::lock_guard<std::mutex> lockMe(
				parentFlow->getForceSimulator()->getLock());
		forceItem->location += dragOffset;
		setDragOffset(pixel2(0.0f));
		pixel2 minPt = parentFlow->graphBounds.min();
		pixel2 maxPt = parentFlow->graphBounds.max();
		float minX = 1E30f;
		float maxX = -1E30f;
		float minY = 1E30f;
		float maxY = -1E30f;
		Node* minXnode = nullptr;
		Node* maxXnode = nullptr;
		Node* minYnode = nullptr;
		Node* maxYnode = nullptr;
		for (RegionPtr region : parentFlow->getChildren()) {
			Node* node = dynamic_cast<Node*>(region.get());
			if (node) {
				box2px box = box2px(
						node->getForceItem()->location
								- Node::DIMENSIONS * 0.5f, Node::DIMENSIONS);
				if (box.position.x < minX) {
					minX = box.position.x;
					minXnode = node;
				} else if (box.position.x + box.dimensions.x > maxX) {
					maxX = box.position.x + box.dimensions.x;
					maxXnode = node;
				}
				if (box.position.y < minY) {
					minY = box.position.y;
					minYnode = node;
				} else if (box.position.y + box.dimensions.y > maxY) {
					maxY = box.position.y + box.dimensions.y;
					maxYnode = node;
				}
			}
		}
		if (maxXnode == this) {
			maxPt.x = maxX;
		}
		if (minXnode == this) {
			minPt.x = minX;
		}
		if (maxYnode == this) {
			maxPt.y = maxY;
		}
		if (minYnode == this) {
			minPt.y = minY;
		}
		parentFlow->graphBounds = box2px(minPt, maxPt - minPt);
		parentFlow->forceSim->setBounds(CoordPX(parentFlow->graphBounds.position),
				CoordPX(parentFlow->graphBounds.dimensions));
		parentFlow->boxForce->setBounds(parentFlow->graphBounds);
		if (!AlloyApplicationContext()->isControlDown()) {
			for (ConnectionPtr connector : parentFlow->getConnections()) {
				connector->getSpringItem()->update();
			}
			for (RelationshipPtr relationship : parentFlow->getRelationships()) {
				relationship->getSpringItem()->update();
			}
		}
	}
}
void Node::postPack() {
	centerOffset = nodeIcon->getBounds(false).center()
			- getBounds(false).position;

}
void Data::pack(const pixel2& pos, const pixel2& dims, const double2& dpmm,
		double pixelRatio, bool clamp) {
	prePack();
	this->dimensions = CoordPX(
			std::max(
					std::max(textWidth + 10.0f,
							2.0f
									+ inputPorts.size()
											* (InputPort::DIMENSIONS.x + 2.0f)),
					2.0f
							+ outputPorts.size()
									* (OutputPort::DIMENSIONS.x + 2.0f)),
			Node::DIMENSIONS.y);
	Composite::pack(pos, dims, dpmm, pixelRatio, clamp);
	postPack();
}
void Group::pack(const pixel2& pos, const pixel2& dims, const double2& dpmm,
		double pixelRatio, bool clamp) {
	prePack();
	this->dimensions = CoordPX(
			std::max(
					std::max(textWidth + 10.0f,
							2.0f
									+ inputPorts.size()
											* (InputPort::DIMENSIONS.x + 2.0f)),
					2.0f
							+ outputPorts.size()
									* (OutputPort::DIMENSIONS.x + 2.0f)),
			Node::DIMENSIONS.y);
	Composite::pack(pos, dims, dpmm, pixelRatio, clamp);
	postPack();
}
void View::pack(const pixel2& pos, const pixel2& dims, const double2& dpmm,
		double pixelRatio, bool clamp) {
	prePack();
	this->dimensions = CoordPX(
			std::max(
					std::max(textWidth + 10.0f,
							2.0f
									+ inputPorts.size()
											* (InputPort::DIMENSIONS.x + 2.0f)),
					2.0f
							+ outputPorts.size()
									* (OutputPort::DIMENSIONS.x + 2.0f)),
			Node::DIMENSIONS.y);
	Composite::pack(pos, dims, dpmm, pixelRatio, clamp);
	postPack();
}
void Compute::pack(const pixel2& pos, const pixel2& dims, const double2& dpmm,
		double pixelRatio, bool clamp) {
	prePack();
	this->dimensions = CoordPX(
			std::max(
					std::max(textWidth + 14.0f,
							2.0f
									+ inputPorts.size()
											* (InputPort::DIMENSIONS.x + 2.0f)),
					2.0f
							+ outputPorts.size()
									* (OutputPort::DIMENSIONS.x + 2.0f)),
			Node::DIMENSIONS.y);
	Composite::pack(pos, dims, dpmm, pixelRatio, clamp);
	postPack();
}
void Source::pack(const pixel2& pos, const pixel2& dims, const double2& dpmm,
		double pixelRatio, bool clamp) {
	prePack();
	this->dimensions = CoordPX(std::max(textWidth + 10.0f, Node::DIMENSIONS.x),
			Node::DIMENSIONS.y + OutputPort::DIMENSIONS.y + 2);
	Composite::pack(pos, dims, dpmm, pixelRatio, clamp);
	postPack();
}
void Destination::pack(const pixel2& pos, const pixel2& dims,
		const double2& dpmm, double pixelRatio, bool clamp) {
	prePack();
	this->dimensions = CoordPX(std::max(textWidth + 10.0f, Node::DIMENSIONS.x),
			Node::DIMENSIONS.y - InputPort::DIMENSIONS.y + fontSize
					+ TextField::PADDING * 2);
	Composite::pack(pos, dims, dpmm, pixelRatio, clamp);
	postPack();
}
Node::Node(const std::string& name, const pixel2& pt) :
		Composite(name, CoordPX(0.0f, 0.0f), CoordPX(Node::DIMENSIONS)), label(
				name), parentFlow(nullptr) {
	forceItem = ForceItemPtr(new ForceItem(pt + Node::DIMENSIONS * 0.5f));
	setup();
}
Node::Node(const std::string& name, const std::string& label, const pixel2& pt) :
		Composite(name, CoordPX(0.0f, 0.0f), CoordPX(Node::DIMENSIONS)), label(
				label), parentFlow(nullptr) {
	forceItem = ForceItemPtr(new ForceItem(pt + Node::DIMENSIONS * 0.5f));
	setup();
}
box2px Node::getBounds(bool includeBounds) const {
	box2px box = Composite::getBounds(includeBounds);
	if (includeBounds) {
		box.position += forceItem->location - centerOffset;
	}
	return box;
}
box2px Node::getCursorBounds(bool includeOffset) const {
	box2px box = (
			isDetached() ?
					getBounds(includeOffset) :
					box2px(bounds.position + forceItem->location - centerOffset,
							bounds.dimensions));
	box.position += getDragOffset();
	if (parent != nullptr && (!isDetached() && includeOffset)) {
		box.position += parent->getDrawOffset();
		if (AlloyApplicationContext()->getOnTopRegion() != this) {
			box.intersect(parent->getCursorBounds());
		}
	}

	return box;

}
pixel2 Node::getDrawOffset() const {
	return Composite::getDrawOffset() + forceItem->location - centerOffset;
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
	box2px cbounds = nodeIcon->getBounds();

	pixel2 labelStart = pixel2(cbounds.position.x + cbounds.dimensions.x * 0.5f,
			lbounds.position.y);
	pixel2 labelEnd = pixel2(bounds.position.x + bounds.dimensions.x,
			lbounds.position.y + lbounds.dimensions.y);
	nvgStrokeWidth(nvg, 1.0f);
	if (inputPorts.size() > 0 || outputPorts.size() > 0) {
		nvgStrokeColor(nvg, Color(context->theme.DARK.toLighter(0.25f)));
		nvgFillColor(nvg,
				context->theme.DARK.toLighter(0.25f).toSemiTransparent(
						NODE_ALPHA));
	} else {
		nvgStrokeColor(nvg, Color(COLOR_NONE));
		nvgFillColor(nvg,
				Color(context->theme.DARK.toSemiTransparent(NODE_ALPHA)));
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
	Port* p = getGraph()->getConnectingPort();
	if (p == inputPort.get()) {
		inputPort->setVisible(true);
	} else {
		if (isMouseOver()) {
			if (!inputPort->isVisible()) {
				inputPort->setVisible(true);
			}
		} else {
			if (!inputPort->isConnected())
				inputPort->setVisible(false);
		}
	}
}
void Compute::draw(AlloyContext* context) {
	Node::draw(context);
	Port* p = getGraph()->getConnectingPort();
	if (p == inputPort.get()) {
		inputPort->setVisible(true);
	} else {
		if (isMouseOver()) {
			if (!inputPort->isVisible()) {
				inputPort->setVisible(true);
			}
		} else {
			if (!inputPort->isConnected())
				inputPort->setVisible(false);
		}
	}
}

void Data::draw(AlloyContext* context) {
	Node::draw(context);
	Port* p = getGraph()->getConnectingPort();
	if (p == inputPort.get()) {
		inputPort->setVisible(true);
		if (!outputPort->isConnected())
			outputPort->setVisible(false);
	} else if (p == outputPort.get()) {
		outputPort->setVisible(true);
		if (!inputPort->isConnected())
			inputPort->setVisible(false);
	} else {
		if (isMouseOver()) {
			if (!inputPort->isVisible()) {
				inputPort->setVisible(true);
			}
			if (!outputPort->isVisible()) {
				outputPort->setVisible(true);
			}
		} else {
			if (!inputPort->isConnected())
				inputPort->setVisible(false);

			if (!outputPort->isConnected())
				outputPort->setVisible(false);
		}
	}
}
void Group::draw(AlloyContext* context) {
	Node::draw(context);
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
				Color(context->theme.LIGHT_TEXT.toSemiTransparent(NODE_ALPHA)));
		nvgFillColor(nvg,
				nodeIcon->backgroundColor->toSemiTransparent(NODE_ALPHA));
	} else {
		nvgStrokeColor(nvg, Color(COLOR_NONE)); //Color(context->theme.LIGHT_TEXT.toSemiTransparent(NODE_ALPHA))
		nvgFillColor(nvg,
				Color(context->theme.DARK.toSemiTransparent(NODE_ALPHA)));
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
				Color(context->theme.LIGHT_TEXT.toSemiTransparent(NODE_ALPHA)));
		nvgFillColor(nvg,
				nodeIcon->backgroundColor->toSemiTransparent(NODE_ALPHA));
	} else {
		nvgStrokeColor(nvg, Color(COLOR_NONE)); //Color(context->theme.LIGHT_TEXT.toSemiTransparent(NODE_ALPHA))
		nvgFillColor(nvg,
				Color(context->theme.DARK.toSemiTransparent(NODE_ALPHA)));
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
SpringItemPtr& Relationship::getSpringItem() {
	return springItem;
}
SpringItemPtr& Connection::getSpringItem() {
	return springItem;
}
void Connection::update() {
	if (springItem.get() == nullptr)
		springItem->update();
}
void Relationship::update() {
	if (springItem.get() == nullptr)
		springItem->update();
}
void Relationship::draw(AlloyContext* context) {
	pixel2 subjectPt = object->getCenter();
	pixel2 objectPt = subject->getCenter();
	NVGcontext* nvg = context->nvgContext;

	pixel2 vec = subjectPt - objectPt;
	float len = length(vec);
	const float arrowLength = 10;
	const float arrowWidth = 10;

	float r = subject->getRadius();
	if (len > 2 * r) {
		pixel2 mid = 0.5f * (objectPt + subjectPt);
		vec /= len;
		pixel2 ortho(-vec.y, vec.x);
		pixel2 pt1 = subjectPt - vec * r;

		nvgStrokeColor(nvg, context->theme.NEUTRAL);
		nvgStrokeWidth(nvg, 4.0f);
		nvgLineCap(nvg, NVG_ROUND);
		nvgBeginPath(nvg);
		nvgMoveTo(nvg, objectPt.x, objectPt.y);
		subjectPt = pt1 - arrowLength * vec;
		nvgLineTo(nvg, subjectPt.x, subjectPt.y);
		nvgStroke(nvg);

		pixel2 pt2 = subjectPt + ortho * arrowWidth * 0.5f;
		pixel2 pt3 = subjectPt - ortho * arrowWidth * 0.5f;
		nvgFillColor(nvg, context->theme.NEUTRAL);
		nvgBeginPath(nvg);
		nvgMoveTo(nvg, pt1.x, pt1.y);
		nvgLineTo(nvg, pt2.x, pt2.y);
		nvgLineTo(nvg, pt3.x, pt3.y);
		nvgClosePath(nvg);
		nvgFill(nvg);
		if (object->isMouseOver() || subject->isMouseOver()) {
			nvgFontFaceId(nvg, context->getFont(FontType::Bold)->handle);
			const float th = 20;
			nvgFontSize(nvg, th - 2);
			nvgTextAlign(nvg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
			float tw = nvgTextBounds(nvg, 0, 0, predicate->getName().c_str(),
					nullptr, nullptr) + 2;
			nvgFillColor(nvg,
					context->theme.DARK.toSemiTransparent(NODE_ALPHA));
			nvgBeginPath(nvg);
			nvgRoundedRect(nvg, mid.x - tw * 0.5f, mid.y - th * 0.5f, tw, th,
					context->theme.CORNER_RADIUS);
			nvgFill(nvg);
			aly::drawText(nvg, mid, predicate->getName(), FontStyle::Normal,
					context->theme.HIGHLIGHT, context->theme.DARK);
		}
	}
}
void Relationship::drawText(AlloyContext* context) {
	pixel2 scenter = object->getCenter();
	pixel2 ocenter = subject->getCenter();
	NVGcontext* nvg = context->nvgContext;
	float r = subject->getRadius();
	float len = distance(scenter, ocenter);
	if (len > 2 * r) {
		pixel2 mid = 0.5f * (scenter + ocenter);
		if (object->isMouseOver() || subject->isMouseOver()) {
			nvgFontFaceId(nvg, context->getFont(FontType::Bold)->handle);
			const float th = 20;
			nvgFontSize(nvg, th - 2);
			nvgTextAlign(nvg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
			float tw = nvgTextBounds(nvg, 0, 0, predicate->getName().c_str(),
					nullptr, nullptr) + 2;
			nvgFillColor(nvg,
					context->theme.DARK.toSemiTransparent(NODE_ALPHA));
			nvgBeginPath(nvg);
			nvgRoundedRect(nvg, mid.x - tw * 0.5f, mid.y - th * 0.5f, tw, th,
					context->theme.CORNER_RADIUS);
			nvgFill(nvg);
			aly::drawText(nvg, mid, predicate->getName(), FontStyle::Normal,
					context->theme.HIGHLIGHT, context->theme.DARK);
		}
	}
}

void DataFlow::draw(AlloyContext* context) {
	mouseOverNode = nullptr;
	mouseSelectedNode = nullptr;
	const float nudge = context->theme.CORNER_RADIUS;
	if (selectedConnection != nullptr && context->getCursor() == nullptr) {
		context->setCursor(&Cursor::CrossHairs);
	}
	for (std::shared_ptr<Region> child : children) {
		Node* node = dynamic_cast<Node*>(child.get());
		if (node) {
			if (context->isMouseOver(node, true)) {
				mouseOverNode = node;
				if (context->isMouseOver(node->nodeIcon.get())) {
					mouseSelectedNode = node;
				}
				break;
			}
		}
	}
	NVGcontext* nvg = context->nvgContext;

	box2px bounds = getBounds();
	float w = bounds.dimensions.x;
	float h = bounds.dimensions.y;
	pixel lineWidth = borderWidth.toPixels(bounds.dimensions.y, context->dpmm.y,
			context->pixelRatio);
	pushScissor(nvg, getCursorBounds());
	if (backgroundColor->a > 0) {
		nvgBeginPath(nvg);
		if (roundCorners) {
			nvgRoundedRect(nvg, bounds.position.x, bounds.position.y,
					bounds.dimensions.x, bounds.dimensions.y,
					context->theme.CORNER_RADIUS);
		} else {
			nvgRect(nvg, bounds.position.x, bounds.position.y,
					bounds.dimensions.x, bounds.dimensions.y);
		}
		nvgFillColor(nvg, *backgroundColor);
		nvgFill(nvg);
	}
	{
		nvgBeginPath(nvg);
		nvgFillColor(nvg, context->theme.DARK.toDarker(0.75f));
		nvgStrokeWidth(nvg, 1.0f);
		float2 offset = getDrawOffset() + getBoundsPosition();
		nvgRoundedRect(nvg,
				graphBounds.position.x + offset.x - Node::DIMENSIONS.x * 0.5f,
				graphBounds.position.y + offset.y - Node::DIMENSIONS.y * 0.5f,
				graphBounds.dimensions.x + Node::DIMENSIONS.x,
				graphBounds.dimensions.y + Node::DIMENSIONS.y,
				context->theme.CORNER_RADIUS);
		nvgFill(nvg);
	}
	for (std::shared_ptr<Region>& region : children) {
		if (region->isVisible()) {
			region->draw(context);
		}
	}

	if (verticalScrollTrack.get() != nullptr) {
		if (isScrollEnabled()) {
			if (extents.dimensions.y > h) {
				verticalScrollTrack->draw(context);
				verticalScrollHandle->draw(context);
			} else {
				verticalScrollTrack->draw(context);
			}
			if (extents.dimensions.x > w) {
				horizontalScrollTrack->draw(context);
				horizontalScrollHandle->draw(context);
			} else {
				horizontalScrollTrack->draw(context);
			}
		}
	}
	popScissor(nvg);
	if (borderColor->a > 0) {

		nvgLineJoin(nvg, NVG_ROUND);
		nvgBeginPath(nvg);
		if (roundCorners) {
			nvgRoundedRect(nvg, bounds.position.x + lineWidth * 0.5f,
					bounds.position.y + lineWidth * 0.5f,
					bounds.dimensions.x - lineWidth,
					bounds.dimensions.y - lineWidth,
					context->theme.CORNER_RADIUS);
		} else {
			nvgRect(nvg, bounds.position.x + lineWidth * 0.5f,
					bounds.position.y + lineWidth * 0.5f,
					bounds.dimensions.x - lineWidth,
					bounds.dimensions.y - lineWidth);
		}
		nvgStrokeColor(nvg, *borderColor);
		nvgStrokeWidth(nvg, lineWidth);
		nvgStroke(nvg);
		nvgLineJoin(nvg, NVG_MITER);
	}

	if (connectingPort) {

		pixel2 cursor = context->cursorPosition;

		if (getBounds().contains(cursor)) {
			float2 offset = getDrawOffset();
			NVGcontext* nvg = context->nvgContext;
			nvgStrokeWidth(nvg, 2.0f);
			nvgStrokeColor(nvg, context->theme.HIGHLIGHT);

			box2px bounds = connectingPort->getBounds();
			pixel2 start;
			Direction dir = Direction::Unkown;
			bool flipDir = false;
			switch (connectingPort->getType()) {
			case PortType::Input:
				start = pixel2(bounds.position.x + bounds.dimensions.x * 0.5f,
						bounds.position.y + nudge);
				dir = Direction::South;
				flipDir = true;
				break;
			case PortType::Output:
				start = pixel2(bounds.position.x + bounds.dimensions.x * 0.5f,
						bounds.position.y + bounds.dimensions.y - nudge);
				dir = Direction::South;
				break;
			case PortType::Child:
				start = pixel2(bounds.position.x + bounds.dimensions.x - nudge,
						bounds.position.y + bounds.dimensions.y * 0.5f);
				dir = Direction::East;
				break;
			case PortType::Parent:
				start = pixel2(bounds.position.x + nudge,
						bounds.position.y + bounds.dimensions.y * 0.5f);
				dir = Direction::East;
				flipDir = true;
				break;
			case PortType::Unknown:
				start = pixel2(bounds.position.x + bounds.dimensions.x * 0.5f,
						bounds.position.y + bounds.dimensions.y * 0.5f);
			}
			start -= offset;
			float2 end = cursor - offset;
			std::vector<float2> path;
			if (flipDir) {
				router.evaluate(path, end, start, dir);
			} else {
				router.evaluate(path, start, end, dir);
			}
			nvgLineCap(nvg, NVG_ROUND);
			nvgLineJoin(nvg, NVG_BEVEL);
			nvgStrokeColor(nvg, context->theme.HIGHLIGHT);
			nvgBeginPath(nvg);
			float2 pt0 = offset + path.front();
			nvgMoveTo(nvg, pt0.x, pt0.y);
			for (int i = 1; i < (int) path.size() - 1; i++) {
				float2 pt1 = offset + path[i];
				float2 pt2 = offset + path[i + 1];
				float diff = 0.5f
						* std::min(
								std::max(std::abs(pt1.x - pt2.x),
										std::abs(pt1.y - pt2.y)),
								std::max(std::abs(pt1.x - pt0.x),
										std::abs(pt1.y - pt0.y)));
				if (diff < context->theme.CORNER_RADIUS) {
					nvgLineTo(nvg, pt1.x, pt1.y);
				} else {
					nvgArcTo(nvg, pt1.x, pt1.y, pt2.x, pt2.y,
							context->theme.CORNER_RADIUS);
				}
				pt0 = pt1;
			}
			pt0 = offset + path.back();
			nvgLineTo(nvg, pt0.x, pt0.y);
			nvgStroke(nvg);
		}

	}
	if (dragBox.dimensions.x > 0 && dragBox.dimensions.y > 0) {
		nvgBeginPath(nvg);
		nvgRect(nvg, dragBox.position.x, dragBox.position.y,
				dragBox.dimensions.x, dragBox.dimensions.y);
		nvgFillColor(nvg, Color(64, 64, 64, 128));
		nvgFill(nvg);

		nvgBeginPath(nvg);
		nvgRect(nvg, dragBox.position.x, dragBox.position.y,
				dragBox.dimensions.x, dragBox.dimensions.y);
		nvgStrokeWidth(nvg, 2.0f);
		nvgStrokeColor(nvg, Color(220, 220, 220));
		nvgStroke(nvg);
	}
}
void DataFlow::pack(const pixel2& pos, const pixel2& dims, const double2& dpmm,
		double pixelRatio, bool clamp) {
	currentDrawOffset = extents.position;
	Region::pack(pos, dims, dpmm, pixelRatio);
	box2px bounds = getBounds(false);
	pixel2 offset = cellPadding;
	pixel2 scrollExtent = pixel2(0.0f);
	for (std::shared_ptr<Region>& region : children) {
		if (!region->isVisible()) {
			continue;
		}
		if (orientation == Orientation::Vertical) {
			pixel2 pix = region->position.toPixels(bounds.dimensions, dpmm,
					pixelRatio);
			region->position = CoordPX(pix.x, offset.y);
		}
		if (orientation == Orientation::Horizontal) {
			pixel2 pix = region->position.toPixels(bounds.dimensions, dpmm,
					pixelRatio);
			region->position = CoordPX(offset.x, pix.y);
		}
		region->pack(bounds.position, bounds.dimensions, dpmm, pixelRatio);
		box2px cbounds = region->getBounds();
		if (orientation == Orientation::Horizontal) {
			offset.x += cellSpacing.x + cbounds.dimensions.x;

		}
		if (orientation == Orientation::Vertical) {
			offset.y += cellSpacing.y + cbounds.dimensions.y;
		}
		scrollExtent = aly::max(
				cbounds.dimensions + cbounds.position - this->bounds.position,
				scrollExtent);
	}
	extents.dimensions = scrollExtent;
	extents.position = currentDrawOffset;
	if (graphBounds.dimensions.x * graphBounds.dimensions.y == 0) {
		graphBounds = box2px(Node::DIMENSIONS * 0.5f,
				forceSim->getBoundsDimensions() - Node::DIMENSIONS);
		forceSim->setBounds(CoordPX(graphBounds.position),
				CoordPX(graphBounds.dimensions));
		boxForce->setBounds(graphBounds);
	}
	for (std::shared_ptr<Region>& region : children) {
		if (region->onPack)
			region->onPack();
	}
	routingLock.lock();
	router.update();
	for (ConnectionPtr& connect : data->connections) {
		router.evaluate(connect);
	}
	Connection* c = closestConnection(
			AlloyApplicationContext()->getCursorPosition() - getDrawOffset(),
			4.0f);
	selectedConnection = c;
	routingLock.unlock();
}
}
}
