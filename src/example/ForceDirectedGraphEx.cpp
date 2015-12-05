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
	childNodes.push_back(ForceItemPtr(new ForceItem(center)));
	graph->addForceItem(childNodes.front());
	box2f bounds= getContext()->getViewport();
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
				ForceItemPtr child = ForceItemPtr(new ForceItem(pt));
				graph->addForceItem(child);
				graph->addSpringItem(parent,child);
				tmpList.push_back(child);
			}
		}
		childNodes = tmpList;
	}

}
bool ForceDirectedGraphEx::init(Composite& rootNode) {
	graph = ForceSimulatorPtr(new ForceSimulator("Force Simulator", CoordPX(0.0f, 0.0f),CoordPX(2000,2000)));
	createRadialGraph(graph);
	graph->setDragEnabled(true);
	graph->backgroundColor = MakeColor(32, 32, 32);
	graph->setClampDragToParentBounds(false);
	graph->addForce(SpringForcePtr(new SpringForce()));
	graph->addForce(NBodyForcePtr(new NBodyForce()));
	graph->addForce(GravitationalForcePtr(new GravitationalForce()));
	//graph->addForce(BuoyancyForcePtr(new BuoyancyForce()));
	graph->addForce(WallForcePtr(new WallForce(float2(100.0f, 1800.0f), float2(1900.0f, 1700.0f))));
	graph->addForce(BoxForcePtr(new BoxForce(box2f(float2(0.0f,0.0f), float2(2000.0f,2000.0f)))));
	graph->addForce(CircularWallForcePtr(new CircularWallForce(float2(1000.0f, 1000.0f), 960.0f)));
	graph->addForce(DragForcePtr(new DragForce(0.001f)));
	rootNode.add(graph);
	getContext()->setDebug(true);
	getContext()->addDeferredTask([this]() {
		graph->start();
	});
	
	return true;
}

