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
#include "../../include/example/TabPaneEx.h"
using namespace aly;
TabPaneEx::TabPaneEx() :
		Application(800, 600, "Tab Pane Example") {
}
bool TabPaneEx::init(Composite& rootNode) {
	TabBarPtr tabBar=TabBarPtr(new TabBar("Tab Bar",CoordPX(10,10),CoordPerPX(1.0,1.0,-20.0f,-20.0f)));
	tabBar->backgroundColor=MakeColor(64,64,64);
	CompositePtr tabPane1=CompositePtr(new Composite("Tab 1",CoordPX(0,0),CoordPercent(1.0f,1.0f)));
	tabPane1->backgroundColor=MakeColor(HSVAtoColor(HSVA(0.0f,0.7f,1.0f,1.0f)));
	tabBar->add(MakeShared<TabPane>(tabPane1));

	CompositePtr tabPane2=CompositePtr(new Composite("Tab 2",CoordPX(0,0),CoordPercent(1.0f,1.0f)));
	tabPane2->backgroundColor=MakeColor(HSVAtoColor(HSVA(0.2f,0.7f,1.0f,1.0f)));
	tabBar->add(MakeShared<TabPane>(tabPane2));

	CompositePtr tabPane3=CompositePtr(new Composite("Tab 3",CoordPX(0,0),CoordPercent(1.0f,1.0f)));
	tabPane3->backgroundColor=MakeColor(HSVAtoColor(HSVA(0.4f,0.7f,1.0f,1.0f)));
	tabBar->add(MakeShared<TabPane>(tabPane3));

	CompositePtr tabPane4=CompositePtr(new Composite("Tab 4",CoordPX(0,0),CoordPercent(1.0f,1.0f)));
	tabPane4->backgroundColor=MakeColor(HSVAtoColor(HSVA(0.6f,0.7f,1.0f,1.0f)));
	tabBar->add(MakeShared<TabPane>(tabPane4));

	CompositePtr tabPane5=CompositePtr(new Composite("Tab 5",CoordPX(0,0),CoordPercent(1.0f,1.0f)));
	tabPane5->backgroundColor=MakeColor(HSVAtoColor(HSVA(0.8f,0.7f,1.0f,1.0f)));
	tabBar->add(MakeShared<TabPane>(tabPane5));

	rootNode.backgroundColor=MakeColor(0,0,0);
	rootNode.add(tabBar);
	return true;
}
