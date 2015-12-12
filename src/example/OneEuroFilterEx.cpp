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
#include "../../include/example/OneEuroFilterEx.h"
using namespace aly;
OneEuroFilterEx::OneEuroFilterEx() :
		Application(600, 600, "Units Example"){
	dataBuffer.assign(pixel2(-1, -1));
	filterBuffer.assign(pixel2(-1, -1));
}
bool OneEuroFilterEx::init(Composite& rootNode) {
	drawRegion = DrawPtr(new Draw("Cursor Draw", CoordPX(0.0f, 0.0f), CoordPerPX(1.0f, 1.0f, 0.0f, 0.0f), [this](AlloyContext* context, const box2px& bounds){
		drawCursor(context,bounds);
	}));
	rootNode.add(drawRegion);
	//getContext()->setDebug(true);
	return true;
}
void OneEuroFilterEx::drawCursor(AlloyContext* context, const box2px& bounds) {
	NVGcontext* nvg = context->nvgContext;

	const float w = 120;
	const float h = 120;
	pixel2 offset(w, h);

	pixel2 pt;
	pixel2 cursor = context->getCursorPosition();
	if (context->isMouseOver(drawRegion.get())&&cursor.x>=offset.x&&cursor.y>=offset.y) {
		noiseCursor = aly::max(float2(RandomGaussian(cursor.x, noise), RandomGaussian(cursor.y, noise)),pixel2(0.0f,0.0f));
		pt = (noiseCursor - offset)*(bounds.dimensions) / (bounds.dimensions - offset);
		dataBuffer[index] = pt;
		filterBuffer[index] = filter.evaluate(pt);
		index=(index+1)%BUFFER_SIZE;
		nvgStrokeColor(nvg, Color(1.0f, 0.6f, 0.6f));
		nvgStrokeWidth(nvg, 3.0f);
		nvgBeginPath(nvg);
		nvgCircle(nvg, noiseCursor.x, noiseCursor.y, 12.0f);
		nvgStroke(nvg);
		context->setCursor(&Cursor::CrossHairs);
	}

	nvgBeginPath(nvg);
	nvgFillColor(nvg, Color(0.3f, 0.3f, 0.3f));
	nvgRect(nvg, 0, 0,w,h);
	nvgFill(nvg);

	pt = (cursor-offset)*(bounds.dimensions) / (bounds.dimensions - offset);
	nvgBeginPath(nvg);
	nvgStrokeWidth(nvg, 2.0f);
	nvgStrokeColor(nvg, Color(1.0f, 0.6f, 0.6f));
	nvgMoveTo(nvg, 0, h*pt.y / bounds.dimensions.y);
	nvgLineTo(nvg, w, h*pt.y / bounds.dimensions.y);
	nvgMoveTo(nvg, w*pt.x / bounds.dimensions.x, 0);
	nvgLineTo(nvg, w*pt.x / bounds.dimensions.x, h);
	nvgStroke(nvg);

	nvgBeginPath(nvg);
	nvgFillColor(nvg, Color(0.8f, 0.8f, 0.8f));
	nvgCircle(nvg, w*pt.x / bounds.dimensions.x, h*pt.y / bounds.dimensions.y, 4.0f);
	nvgFill(nvg);



	nvgBeginPath(nvg);
	nvgFillColor(nvg, Color(0.3f, 0.3f, 0.3f));
	nvgRect(nvg, 0, h, w, bounds.dimensions.y - h);
	nvgFill(nvg);

	nvgBeginPath(nvg);
	nvgRect(nvg, w, 0, bounds.dimensions.x - w,h);
	nvgFill(nvg);

	nvgStrokeColor(nvg, Color(1.0f, 0.6f, 0.6f));
	nvgStrokeWidth(nvg, 2.0f);

	int count = 0;
	nvgBeginPath(nvg);
	for (int i = BUFFER_SIZE - 1; i >= 0;i--) {
		pt = dataBuffer[(i + index) % BUFFER_SIZE];
		if (pt.x > 0) {
			if (count == 0) {
				nvgMoveTo(nvg, w*pt.x / bounds.dimensions.x, (bounds.dimensions.y - h) *count / BUFFER_SIZE + h);
			} else {
				nvgLineTo(nvg, w*pt.x / bounds.dimensions.x, (bounds.dimensions.y - h) *count / BUFFER_SIZE + h);
			}
			count++;
		}
	}
	nvgStroke(nvg);

	count = 0;
	nvgBeginPath(nvg);
	for (int i = BUFFER_SIZE - 1; i >= 0; i--) {
		pt = dataBuffer[(i+index)%BUFFER_SIZE];
		if (pt.x > 0) {
			if (count == 0) {
				nvgMoveTo(nvg, (bounds.dimensions.x-w) *count/ BUFFER_SIZE+w,h*pt.y/bounds.dimensions.y);
			}
			else {
				nvgLineTo(nvg, (bounds.dimensions.x - w) *count / BUFFER_SIZE + w, h*pt.y / bounds.dimensions.y);
			}
			count++;
		}
	}
	nvgStroke(nvg);

	nvgStrokeColor(nvg, Color(1.0f, 1.0f,1.0f));
	count = 0;
	/*
	nvgBeginPath(nvg);
	for (int i = BUFFER_SIZE - 1; i >= 0; i--) {
		pt = filterBuffer[(i + index) % BUFFER_SIZE];
		if (pt.x > 0) {
			if (count == 0) {
				nvgMoveTo(nvg, w*pt.x / bounds.dimensions.x, (bounds.dimensions.y - h) *count / BUFFER_SIZE + h);
			}
			else {
				nvgLineTo(nvg, w*pt.x / bounds.dimensions.x, (bounds.dimensions.y - h) *count / BUFFER_SIZE + h);
			}
			count++;
		}
	}
	nvgStroke(nvg);

	count = 0;
	nvgBeginPath(nvg);
	for (int i = BUFFER_SIZE - 1; i >= 0; i--) {
		pt = filterBuffer[(i + index) % BUFFER_SIZE];
		if (pt.x > 0) {
			if (count == 0) {
				nvgMoveTo(nvg, (bounds.dimensions.x - w) *count / BUFFER_SIZE + w, h*pt.y / bounds.dimensions.y);
			}
			else {
				nvgLineTo(nvg, (bounds.dimensions.x - w) *count / BUFFER_SIZE + w, h*pt.y / bounds.dimensions.y);
			}
			count++;
		}
	}
	nvgStroke(nvg);
	*/
	nvgBeginPath(nvg);
	nvgStrokeWidth(nvg, 2.0f);
	nvgStrokeColor(nvg, Color(0.5f, 0.5f, 0.5f));
	nvgRect(nvg, 1, 1, w-2, h-2);
	nvgStroke(nvg);
}

