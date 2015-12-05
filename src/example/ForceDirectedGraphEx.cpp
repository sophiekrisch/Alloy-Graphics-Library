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

#include "Alloy.h"
#include "../../include/example/ForceDirectedGraphEx.h"
using namespace aly;
using namespace aly::dataflow;
ForceDirectedGraphEx::ForceDirectedGraphEx() :
		Application(1920, 1080, "Force Directed Graph Example") {
}
void ForceDirectedGraphEx::createRadialGraph(const ForceSimulatorPtr& graph) {
	int D =3;
	int N = 6;
	float armLength = 500.0f;
	float2 center(1000.0f,1000.0f);
	std::vector<ForceItemPtr> childNodes;
	ForceItemPtr child = ForceItemPtr(new ForceItem(center));
	child->buoyancy = -1;
	childNodes.push_back(child);
	graph->addForceItem(childNodes.front());
	for (int d = 0; d < D; d++) {
		std::vector<ForceItemPtr> tmpList;
		for (ForceItemPtr parent : childNodes) {
			for (int n = 0; n < N; n++) {
				float2 pt = parent->location
						+ (armLength*std::pow(0.3f,(float)d))
								* float2(
										std::cos(n * ALY_PI * 2.0f / (float) N),
										std::sin(
												n * ALY_PI * 2.0f / (float) N));
				child = ForceItemPtr(new ForceItem(pt));
				child->buoyancy = -1;
				graph->addForceItem(child);
				graph->addSpringItem(parent,child);
				tmpList.push_back(child);
			}
		}
		childNodes = tmpList;
	}

}
bool ForceDirectedGraphEx::init(Composite& rootNode) {

	ExpandBarPtr controlRegion = ExpandBarPtr(new ExpandBar("Controls", CoordPX(0.0f, 0.0f), CoordPercent(1.0f, 1.0f)));
	CompositePtr displayRegion = MakeComposite("Display", CoordPX(0.0f, 0.0f), CoordPercent(1.0f, 1.0f));
	displayRegion->setScrollEnabled(true);
	controlRegion->setScrollEnabled(true);
	controlRegion->setOrientation(Orientation::Vertical);
	controlRegion->backgroundColor = MakeColor(32,32,32);
	controlRegion->borderColor = MakeColor(200, 200, 200);
	controlRegion->borderWidth = UnitPX(1.0f);
	BorderCompositePtr borderRegion = BorderCompositePtr(new BorderComposite("Layout", CoordPX(0.0f, 0.0f), CoordPercent(1.0f, 1.0f),true));
	borderRegion->setWest(controlRegion, UnitPX(350.0f));
	borderRegion->setCenter(displayRegion);
	graph = ForceSimulatorPtr(new ForceSimulator("Force Simulator", CoordPX(0.0f, 0.0f),CoordPX(2000,2000)));
	displayRegion->add(graph);
	createRadialGraph(graph);
	graph->backgroundColor = MakeColor(64,64,64);
	graph->setClampDragToParentBounds(false);
	graph->addForce(SpringForcePtr(new SpringForce()));
	graph->addForce(NBodyForcePtr(new NBodyForce()));
	graph->addForce(GravitationalForcePtr(new GravitationalForce()));
	graph->addForce(BuoyancyForcePtr(new BuoyancyForce()));
	graph->addForce(WallForcePtr(new WallForce(float2(100.0f, 1800.0f), float2(1900.0f, 1700.0f))));
	graph->addForce(BoxForcePtr(new BoxForce(box2f(float2(0.0f,0.0f), float2(2000.0f,2000.0f)))));
	graph->addForce(CircularWallForcePtr(new CircularWallForce(float2(1000.0f, 1000.0f), 960.0f)));
	graph->addForce(DragForcePtr(new DragForce(0.001f)));
	for (ForcePtr f : graph->getForces()) {
		int N = (int)f->getParameterCount();
		CompositePtr paramRegion = MakeComposite(f->getName(), CoordPX(0.0f, 0.0f), CoordPercent(1.0f, 1.0f));
		paramRegion->setOrientation(Orientation::Vertical,pixel2(5.0f),pixel2(5.0f));
		ToggleBoxPtr enabledToggle = ToggleBoxPtr(new ToggleBox("Force Enabled", CoordPX(5.0f,5.0f),CoordPerPX(1.0f,0.0f,-10.0f,25.0f),true));
		enabledToggle->onChange = [=](bool b) {
			f->setEnabled(b);
		};
		paramRegion->add(enabledToggle);
		for (int n = 0; n < N; n++) {
			HorizontalSliderPtr hslider = HorizontalSliderPtr(new HorizontalSlider(f->getParameterName(n), CoordPX(5.0f, 5.0f), CoordPerPX(1.0f, 0.0f, -10.0f, 40.0f),Float(f->getMinValue(n)),Float(f->getMaxValue(n)),Float(f->getParameterValue(n))));
			paramRegion->add(hslider);
			hslider->setOnChangeEvent(
				[=](const Number& val) {
				f->setParameter(n,val.toFloat());
			});
		}
		controlRegion->add(paramRegion, 30.0f+N * (40.0f +5.0f)+5.0f, true);
	}
	rootNode.add(borderRegion);
	getContext()->addDeferredTask([this]() {
		graph->start();
	});
	
	return true;
}

