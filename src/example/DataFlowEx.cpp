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
#include "../../include/example/DataFlowEx.h"
using namespace aly;
using namespace aly::dataflow;
DataFlowEx::DataFlowEx() :
		Application(800, 600, "Data Flow Graph Example") {
}
bool DataFlowEx::init(Composite& rootNode) {
	graph = MakeDataFlow("Data Flow", CoordPX(10, 10),
			CoordPerPX(1.0f, 1.0f, -20.0f, -20.0f));
	ComputePtr computeNode = MakeComputeNode("Compute", pixel2(10, 10));
	ViewPtr viewNode = MakeViewNode("View", pixel2(10, 120));
	DataPtr dataNode = MakeDataNode("Data", pixel2(10, 230));
	SourcePtr sourceNode = MakeSourceNode("Source", pixel2(10, 340));
	DestinationPtr destNode = MakeDestinationNode("Destination", pixel2(10, 450));

	for(int i=0;i<4;i++){
		computeNode->add(MakeInputPort(MakeString()<<"Input "<<i));
	}
	for(int i=0;i<2;i++){
		computeNode->add(MakeOutputPort(MakeString()<<"Output "<<i));
	}

	for(int i=0;i<2;i++){
		viewNode->add(MakeInputPort(MakeString()<<"Input "<<i));
	}



	sourceNode->add(MakeInputPort("Input"));
	sourceNode->add(MakeOutputPort("Output"));

	destNode->add(MakeInputPort("Input"));
	destNode->add(MakeOutputPort("Output"));
	graph->add(computeNode);
	graph->add(viewNode);
	graph->add(dataNode);
	graph->add(sourceNode);
	graph->add(destNode);

	rootNode.add(graph);
	return true;
}

