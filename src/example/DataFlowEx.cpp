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
	ComputePtr computeNode1 = MakeComputeNode("Compute 1", pixel2(10, 10));
	ComputePtr computeNode2 = MakeComputeNode("Compute 2", pixel2(120, 10));
	ComputePtr computeNode3 = MakeComputeNode("Compute 3", pixel2(230, 10));

	ViewPtr viewNode1 = MakeViewNode("View 1", pixel2(10, 120));
	ViewPtr viewNode2 = MakeViewNode("View 2", pixel2(120, 120));

	DataPtr dataNode1 = MakeDataNode("Data 1", pixel2(10, 230));
	DataPtr dataNode2 = MakeDataNode("Data 2", pixel2(120, 230));
	DataPtr dataNode3 = MakeDataNode("Data 3", pixel2(230, 230));
	DataPtr dataNode4 = MakeDataNode("Data 4", pixel2(340, 230));

	SourcePtr sourceNode1 = MakeSourceNode("Source 1", pixel2(10, 340));
	SourcePtr sourceNode2 = MakeSourceNode("Source 2", pixel2(120, 340));

	DestinationPtr destNode1 = MakeDestinationNode("Destination 1", pixel2(10, 450));
	DestinationPtr destNode2 = MakeDestinationNode("Destination 2", pixel2(120, 450));


	for(int i=0;i<4;i++){
		dataNode1->add(MakeInputPort(MakeString()<<"Input "<<i));
		dataNode2->add(MakeInputPort(MakeString()<<"Input "<<i));
		dataNode3->add(MakeInputPort(MakeString()<<"Input "<<i));
		dataNode4->add(MakeInputPort(MakeString()<<"Input "<<i));

	}
	for(int i=0;i<3;i++){
		dataNode1->add(MakeOutputPort(MakeString()<<"Output "<<i));
		dataNode2->add(MakeOutputPort(MakeString()<<"Output "<<i));
		dataNode3->add(MakeOutputPort(MakeString()<<"Output "<<i));
		dataNode4->add(MakeOutputPort(MakeString()<<"Output "<<i));

	}
	for(int i=0;i<7;i++){
		computeNode1->add(MakeInputPort(MakeString()<<"Input "<<i));
		computeNode2->add(MakeInputPort(MakeString()<<"Input "<<i));
		computeNode3->add(MakeInputPort(MakeString()<<"Input "<<i));
	}
	for(int i=0;i<2;i++){
		computeNode1->add(MakeOutputPort(MakeString()<<"Output "<<i));
		computeNode2->add(MakeOutputPort(MakeString()<<"Output "<<i));
		computeNode3->add(MakeOutputPort(MakeString()<<"Output "<<i));
	}

	for(int i=0;i<3;i++){
		viewNode1->add(MakeInputPort(MakeString()<<"Input "<<i));
		viewNode2->add(MakeInputPort(MakeString()<<"Input "<<i));
	}
	graph->add(computeNode1);
	graph->add(computeNode2);
	graph->add(computeNode3);
	graph->add(viewNode1);
	graph->add(viewNode2);
	graph->add(dataNode1);
	graph->add(dataNode2);
	graph->add(dataNode3);
	graph->add(dataNode4);

	//graph->add(MakeConnection(computeNode3->getOutputPort(0), dataNode4->getInputPort(1)));
	//graph->add(MakeConnection(computeNode3->getOutputPort(1),destNode2->getInputPort()));
	graph->add(MakeRelationship(dataNode1,"uses",dataNode2));
	graph->add(MakeRelationship(dataNode1,"uses",dataNode3));
	graph->add(MakeRelationship(dataNode2,"extends",dataNode3));
	graph->add(MakeRelationship(dataNode4,"extends",dataNode2));
	graph->add(MakeRelationship(dataNode3,"has",dataNode4));

	graph->add(sourceNode1);
	graph->add(sourceNode2);
	graph->add(destNode1);
	graph->add(destNode2);
	rootNode.add(graph);
	return true;
}

