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
#include "AlloyParameterPane.h"
#include "../../include/example/ParameterPaneEx.h"
using namespace aly;
ParameterPaneEx::ParameterPaneEx() :
		Application(800, 600, "Parameter Pane Example") {
}
bool ParameterPaneEx::init(Composite& rootNode) {
	param1 = aly::Float(5.0f);
	param2 = aly::Integer(3);
	param3 = 2;
	param4 = aly::Color(1.0f, 0.0f, 0.0f, 1.0f);
	param5 = getContext()->getFullPath("images/sfsunset.png");
	param6 = false;
	param7 = false;
	param8.push_back(getContext()->getFullPath("images/sfsunset.png"));
	param8.push_back(getContext()->getFullPath("images/sfmarket.png"));
	BorderCompositePtr borderComposite = BorderCompositePtr(new BorderComposite("Layout",CoordPX(0.0f,0.0f),CoordPercent(1.0f,1.0f),true));
	CompositePtr centerPane = CompositePtr(new Composite("Center", CoordPX(0.0f, 0.0f), CoordPercent(1.0f, 1.0f)));
	ParameterPanePtr paramPane = ParameterPanePtr(new ParameterPane("Parameter Pane",CoordPX(0.0f,0.0f),CoordPercent(1.0f,1.0f),26.0f));

	paramPane->addGroup("Group 1",true);
	paramPane->addFileField("File", param5);
	paramPane->addSelectionField("Selection", param3, std::vector<std::string>{"Cool", "Neat", "Awesome", "Boring"});
	paramPane->addNumberField("Float", param1);
	paramPane->addNumberField("Integer", param2);

	paramPane->addGroup("Group 2",false);
	paramPane->addMultiFileSelector("Multi-File", param8);
	paramPane->addColorField("Color", param4);
	paramPane->addToggleBox("Toggle", param6);
	paramPane->addCheckBox("Check", param7);
	centerPane->backgroundColor = MakeColor(getContext()->theme.LIGHT);
	borderComposite->setCenter(centerPane);
	borderComposite->setEast(paramPane, UnitPX(300.0f));
	rootNode.add(borderComposite);
	return true;
}

