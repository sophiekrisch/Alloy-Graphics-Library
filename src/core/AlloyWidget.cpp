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

#include "AlloyApplication.h"
#include "AlloyWidget.h"
using namespace std;
namespace aly {
CheckBox::CheckBox(const std::string& label, const AUnit2D& position,const AUnit2D& dimensions,bool checked):Widget(label),checked(checked){
	this->position = position;
	this->dimensions = dimensions;
	this->aspect = 4.0f;
	CompositePtr valueContainer=MakeComposite("Check Bounds",CoordPerPX(0.0f, 0.0f, 5.0f, 5.0f),CoordPerPX(1.0f, 1.0f, -10.0f, -10.0f));

	checkLabel = MakeTextLabel(label,
					CoordPercent(0.0f, 0.0f),
					CoordPercent(1.0f, 1.0f),
					FontType::Bold, UnitPercent(1.0f),
					RGBA(255, 255, 255, 255), HorizontalAlignment::Left,
					VerticalAlignment::Middle);

	valueLabel=MakeGlyphRegion(Application::getContext()->createAwesomeGlyph(0xf00c),
					CoordPercent(1.0f, 0.0f), CoordPercent(0.0f, 1.0f), COLOR_NONE,
					RGBA(255, 255, 255, 255));
	valueLabel->origin=Origin::TopRight;
	valueLabel->aspectRatio=AspectRatio::FixedHeight;
	valueContainer->add(checkLabel);
	valueContainer->add(valueLabel);
	add(valueContainer);
	this->valueLabel->foregroundColor=(this->checked)?MakeColor(255,255,255):MakeColor(64,64,64);

	valueLabel->onMouseDown=[this](AlloyContext* context,const InputEvent& event){
		if(event.button==GLFW_MOUSE_BUTTON_LEFT){
			this->checked=!this->checked;
			this->valueLabel->foregroundColor=(this->checked)?MakeColor(255,255,255):MakeColor(64,64,64);
		}
	};
	checkLabel->onMouseDown=[this](AlloyContext* context,const InputEvent& event){
		if(event.button==GLFW_MOUSE_BUTTON_LEFT){
			this->checked=!this->checked;
			this->valueLabel->foregroundColor=(this->checked)?MakeColor(255,255,255):MakeColor(64,64,64);
		}
	};
}
void CheckBox::draw(AlloyContext* context){
	float cornerRadius = 5.0f;
	NVGcontext* nvg = context->nvgContext;
	bool hover=context->isMouseContainedIn(this);
	if(hover){
		nvgBeginPath(nvg);
		NVGpaint shadowPaint = nvgBoxGradient(nvg, bounds.position.x + 1,
				bounds.position.y, bounds.dimensions.x - 2, bounds.dimensions.y,
				cornerRadius, 8, Color(0, 0, 0, 255), Color(255, 255, 255, 0));
		nvgFillPaint(nvg, shadowPaint);
		nvgRoundedRect(nvg, bounds.position.x + 1, bounds.position.y + 4,
				bounds.dimensions.x, bounds.dimensions.y, 4.0f);
		nvgFill(nvg);
	}

	nvgBeginPath(nvg);
	nvgRoundedRect(nvg, bounds.position.x, bounds.position.y,
			bounds.dimensions.x, bounds.dimensions.y, cornerRadius);
	nvgFillColor(nvg, Color(64, 64, 64, 255));
	nvgFill(nvg);

		nvgBeginPath(nvg);
		NVGpaint hightlightPaint = nvgBoxGradient(nvg, bounds.position.x,
				bounds.position.y, bounds.dimensions.x,
				bounds.dimensions.y, cornerRadius, 2, Color(64, 64, 64, 0),
				Color(255,255,255,255));
		nvgFillPaint(nvg, hightlightPaint);
		nvgRoundedRect(nvg, bounds.position.x, bounds.position.y,
				bounds.dimensions.x, bounds.dimensions.y, cornerRadius);
		nvgFill(nvg);


	nvgBeginPath(nvg);
	nvgFillColor(nvg, Color(128,128,128));
	box2px clickbox=valueLabel->getBounds();
	nvgRoundedRect(nvg,clickbox.position.x, clickbox.position.y,clickbox.dimensions.x, clickbox.dimensions.y, cornerRadius);
	nvgFill(nvg);

	if(context->isMouseOver(valueLabel.get())||context->isMouseOver(checkLabel.get())){
		nvgBeginPath(nvg);
		nvgStrokeColor(nvg, Color(200,200,200));
		nvgStrokeWidth(nvg,2.0f);
		nvgRoundedRect(nvg,clickbox.position.x, clickbox.position.y,clickbox.dimensions.x, clickbox.dimensions.y, cornerRadius);
		nvgStroke(nvg);
	}

	Composite::draw(context);
}
void Widget::add(const std::shared_ptr<Region>& region) {
	Composite::add(region);
}

Button::Button(const std::string& label, const AUnit2D& position,
		const AUnit2D& dimensions) :
		Widget(label) {
	this->position = position;
	this->dimensions = dimensions;

	backgroundColor = MakeColor(Color(255, 255, 255, 255));
	textColor = MakeColor(Color(0, 0, 0, 255));
	borderColor = MakeColor(Color(255, 255, 255, 255));
	fontSize = UnitPerPX(1.0f, -10);
	this->aspectRatio = AspectRatio::FixedHeight;
}
void ScrollTrack::draw(AlloyContext* context) {
	NVGcontext* nvg = context->nvgContext;

	nvgBeginPath(nvg);
	nvgMoveTo(nvg, bounds.position.x + 14,
			bounds.position.y + bounds.dimensions.y * 0.5f);
	nvgLineTo(nvg, bounds.position.x - 14 + bounds.dimensions.x,
			bounds.position.y + bounds.dimensions.y * 0.5f);
	nvgStrokeColor(nvg, Color(255, 255, 255, 255));
	nvgStrokeWidth(nvg, 10.0f);
	nvgLineCap(nvg, NVG_ROUND);
	nvgStroke(nvg);
	for (std::shared_ptr<Region> ptr : children) {
		ptr->draw(context);
	}
}
void ScrollHandle::draw(AlloyContext* context) {
	NVGcontext* nvg = context->nvgContext;
	if (context->isMouseOver(this) || context->isMouseDown(this)) {
		nvgBeginPath(nvg);
		nvgCircle(nvg, bounds.position.x + bounds.dimensions.x * 0.5f,
				bounds.position.y + bounds.dimensions.y * 0.5f,
				bounds.dimensions.y * 0.5f);
		nvgFillColor(nvg, Color(220, 220, 220, 128));
		nvgFill(nvg);
	}
	nvgBeginPath(nvg);
	nvgCircle(nvg, bounds.position.x + bounds.dimensions.x * 0.5f,
			bounds.position.y + bounds.dimensions.y * 0.5f,
			bounds.dimensions.y * 0.25f);
	nvgFillColor(nvg, Color(255, 255, 255, 255));
	nvgFill(nvg);

	nvgBeginPath(nvg);
	nvgStrokeWidth(nvg, 2.0f);
	nvgStrokeColor(nvg, Color(128, 128, 128, 128));
	nvgCircle(nvg, bounds.position.x + bounds.dimensions.x * 0.5f,
			bounds.position.y + bounds.dimensions.y * 0.5f,
			bounds.dimensions.y * 0.25f);
	nvgStroke(nvg);

}
void SelectionBox::draw(AlloyContext* context){

}
void SelectionBox::drawOnTop(AlloyContext* context){
	context->setDragObject(this);
	NVGcontext* nvg = context->nvgContext;
	float boxHeight=(options.size()+1)*bounds.dimensions.y;
	pixel lineWidth = borderWidth.toPixels(bounds.dimensions.y, context->dpmm.y,
			context->pixelRatio);
	if (backgroundColor->a > 0) {
		nvgBeginPath(nvg);
		nvgRoundedRect(nvg, bounds.position.x,
				bounds.position.y,
				bounds.dimensions.x,
				boxHeight,5.0f);
		nvgFillColor(nvg, *backgroundColor);
		nvgFill(nvg);
	}

	float th = fontSize.toPixels(bounds.dimensions.y, context->dpmm.y,
			context->pixelRatio);
	nvgFontSize(nvg, th);
	nvgFillColor(nvg, *textColor);

	float tw=0.0f;
	for(std::string& label:options){
		tw = std::max(tw,nvgTextBounds(nvg, 0, 0, label.c_str(), nullptr, nullptr));
	}
	nvgTextAlign(nvg,NVG_ALIGN_LEFT|NVG_ALIGN_TOP);
	pixel2 offset(0, 0);

	nvgFillColor(nvg,  Color(64,64,64));

	nvgFontFaceId(nvg, context->getFontHandle(FontType::Bold));

	nvgText(nvg, bounds.position.x + offset.x+lineWidth, bounds.position.y + offset.y,name.c_str(), nullptr);
	int index=0;

	nvgFontFaceId(nvg, context->getFontHandle(FontType::Normal));

	selectedIndex=-1;
	for(std::string& label:options){
		offset.y+=bounds.dimensions.y;
		if(context->isMouseContainedIn(bounds.position+offset,bounds.dimensions)){
			nvgScissor(nvg,bounds.position.x + offset.x, bounds.position.y + offset.y,bounds.dimensions.x,bounds.dimensions.y);
			nvgBeginPath(nvg);
			nvgRoundedRect(nvg,
					bounds.position.x,
					bounds.position.y,
					bounds.dimensions.x,
					boxHeight,5.0f);
			nvgFillColor(nvg, Color(64,64,64));
			nvgFill(nvg);
			nvgScissor(nvg,0,0,context->viewport.dimensions.x,context->viewport.dimensions.y);
			selectedIndex=index;
		}
		index++;
		nvgFillColor(nvg, *textColor);
		nvgText(nvg, bounds.position.x + offset.x+lineWidth, bounds.position.y + offset.y,label.c_str(), nullptr);
	}
	if (borderColor->a > 0) {
		nvgBeginPath(nvg);
		nvgRoundedRect(nvg, bounds.position.x + lineWidth * 0.5f,
				bounds.position.y + lineWidth * 0.5f,
				bounds.dimensions.x - lineWidth,
				boxHeight- lineWidth,5.0f);
		nvgStrokeColor(nvg, *borderColor);
		nvgStrokeWidth(nvg, lineWidth);
		nvgStroke(nvg);
	}
}
SelectionBox::SelectionBox(const std::string& name,const std::vector<std::string>& labels):Region(name),options(labels){
}
Selection::Selection(const std::string& label,const AUnit2D& position,const AUnit2D& dimensions,const std::vector<std::string>& options):Widget(label){
	this->position=position;
	this->dimensions=dimensions;
	CompositePtr valueContainer=MakeComposite(label,CoordPerPX(0.0f, 0.0f, 5.0f, 5.0f),CoordPerPX(1.0f, 1.0f, -10.0f, -10.0f));
	selectionLabel = MakeTextLabel(label,
					CoordPercent(0.0f, 0.0f),
					CoordPercent(1.0f, 1.0f),
					FontType::Bold, UnitPercent(1.0f),
					RGBA(255, 255, 255, 255), HorizontalAlignment::Left,
					VerticalAlignment::Middle);

	arrowLabel=MakeGlyphRegion(Application::getContext()->createAwesomeGlyph(0xf13a),
					CoordPercent(1.0f, 0.0f), CoordPercent(0.0f, 1.0f), COLOR_NONE,
					RGBA(255, 255, 255, 255));

	selectionBox=SelectionBoxPtr(new SelectionBox("Selection",options));
	selectionBox->setPosition(CoordPercent(0.0f,0.0f));
	selectionBox->setDimensions(CoordPercent(1.0f,1.0f));
	selectionBox->fontSize=selectionLabel->fontSize;
	selectionBox->backgroundColor=MakeColor(200,200,200);
	selectionBox->borderColor=MakeColor(255,255,255);
	selectionBox->setVisible(false);
	arrowLabel->origin=Origin::TopRight;
	arrowLabel->aspectRatio=AspectRatio::FixedHeight;

	valueContainer->add(selectionBox);
	valueContainer->add(selectionLabel);
	valueContainer->add(arrowLabel);
	add(valueContainer);

	selectionLabel->onMouseDown=[this](AlloyContext* context,const InputEvent& event){
		if(event.button==GLFW_MOUSE_BUTTON_LEFT){
			selectionBox->setVisible(true);
		}
	};
	arrowLabel->onMouseDown=[this](AlloyContext* context,const InputEvent& event){
		if(event.button==GLFW_MOUSE_BUTTON_LEFT){
			selectionBox->setVisible(true);
		}
	};
	selectionBox->onMouseUp=[this](AlloyContext* context,const InputEvent& event){
		if(event.button==GLFW_MOUSE_BUTTON_LEFT){
			selectionBox->setVisible(false);
			int newSelection=selectionBox->getSelectedIndex();
			if(newSelection<0){
				selectionBox->setSelectedIndex(selectedIndex);
			} else {
				selectedIndex=selectionBox->getSelectedIndex();
			}
			selectionLabel->label=this->getSelection();

		}
	};
}
void Selection::draw(AlloyContext* context){
	NVGcontext* nvg = context->nvgContext;
if (parent != nullptr) {
	box2px pbounds = parent->getBounds();
	nvgScissor(nvg, pbounds.position.x, pbounds.position.y,
			pbounds.dimensions.x, pbounds.dimensions.y);
}

float cornerRadius = 5.0f;
bool hover=context->isMouseContainedIn(this);
if(hover){
	nvgBeginPath(nvg);
	NVGpaint shadowPaint = nvgBoxGradient(nvg, bounds.position.x + 1,
			bounds.position.y, bounds.dimensions.x - 2, bounds.dimensions.y,
			cornerRadius, 8, Color(0, 0, 0, 255), Color(255, 255, 255, 0));
	nvgFillPaint(nvg, shadowPaint);
	nvgRoundedRect(nvg, bounds.position.x + 1, bounds.position.y + 4,
			bounds.dimensions.x, bounds.dimensions.y, 4.0f);
	nvgFill(nvg);
}

nvgBeginPath(nvg);
nvgRoundedRect(nvg, bounds.position.x, bounds.position.y,
		bounds.dimensions.x, bounds.dimensions.y, cornerRadius);
nvgFillColor(nvg, Color(64, 64, 64, 255));
nvgFill(nvg);

	nvgBeginPath(nvg);
	NVGpaint hightlightPaint = nvgBoxGradient(nvg, bounds.position.x,
			bounds.position.y, bounds.dimensions.x,
			bounds.dimensions.y, cornerRadius, 2, Color(64, 64, 64, 0),
			Color(255,255,255,255));
	nvgFillPaint(nvg, hightlightPaint);
	nvgRoundedRect(nvg, bounds.position.x, bounds.position.y,
			bounds.dimensions.x, bounds.dimensions.y, cornerRadius);
	nvgFill(nvg);
Composite::draw(context);
}
HorizontalSlider::HorizontalSlider(const std::string& label,
		const AUnit2D& position, const AUnit2D& dimensions, const Number& min,
		const Number& max, const Number& value) :
		Widget(label), minValue(min), maxValue(max), value(value) {
	this->position = position;
	this->dimensions = dimensions;
	float handleSize = 30.0f;
	float trackPadding = 10.0f;
	this->aspect = 4.0f;
	textColor = MakeColor(Color(0, 0, 0, 255));
	borderColor = MakeColor(Color(255, 255, 255, 255));
	scrollHandle = std::shared_ptr<ScrollHandle>(
			new ScrollHandle("Scroll Handle"));

	scrollHandle->setPosition(CoordPercent(0.0, 0.0));
	scrollHandle->setDimensions(CoordPX(handleSize, handleSize));
	scrollHandle->backgroundColor = MakeColor(255, 128, 64, 255);
	scrollHandle->setEnableDrag(true);

	scrollTrack = std::shared_ptr<ScrollTrack>(new ScrollTrack("Scroll Track"));

	scrollTrack->setPosition(CoordPerPX(0.0f, 1.0f, 0.0f, -handleSize));
	scrollTrack->setDimensions(CoordPerPX(1.0f, 0.0f, 0.0f, handleSize));

	scrollTrack->backgroundColor = MakeColor(128, 128, 128, 255);
	scrollTrack->add(scrollHandle);
	scrollTrack->backgroundColor = MakeColor(255, 0, 0);

	scrollTrack->onMouseDown =
			[this](AlloyContext* context,const InputEvent& e) {this->onMouseDown(context,scrollTrack.get(),e);};
	scrollHandle->onMouseDown =
			[this](AlloyContext* context,const InputEvent& e) {this->onMouseDown(context,scrollHandle.get(),e);};
	scrollHandle->onMouseUp =
			[this](AlloyContext* context,const InputEvent& e) {this->onMouseUp(context,scrollHandle.get(),e);};
	scrollHandle->onMouseDrag =
			[this](AlloyContext* context,const InputEvent& e,const pixel2& lastDragPosition) {this->onMouseDrag(context,scrollHandle.get(),e,lastDragPosition);};

	add(
			sliderLabel = MakeTextLabel(label,
					CoordPerPX(0.0f, 0.0f, trackPadding, 2.0f),
					CoordPerPX(0.5f, 1.0f, 0,
							-(handleSize - trackPadding * 0.75f)),
					FontType::Bold, UnitPerPX(1.0f, 0),
					RGBA(255, 255, 255, 255), HorizontalAlignment::Left,
					VerticalAlignment::Bottom));
	add(
			valueLabel = MakeTextLabel("Value",
					CoordPerPX(0.0f, 0.0f, 0.0f, 2.0f),
					CoordPerPX(1.0f, 1.0f, -trackPadding,
							-(handleSize - trackPadding * 0.75f)),
					FontType::Normal, UnitPerPX(1.0f, -2),
					RGBA(255, 255, 255, 255), HorizontalAlignment::Right,
					VerticalAlignment::Bottom));
	add(scrollTrack);
	this->onPack = [this]() {
		this->setValue(this->value.toDouble());
	};
}
void HorizontalSlider::setValue(double value) {
	double interp = clamp(
			(value - minValue.toDouble())
					/ (maxValue.toDouble() - minValue.toDouble()), 0.0, 1.0);
	double xoff = scrollTrack->getBoundsPositionX()
			+ interp
					* (scrollTrack->getBoundsDimensionsX()
							- scrollHandle->getBoundsDimensionsX());
	scrollHandle->setDragOffset(
			pixel2(xoff, scrollHandle->getBoundsDimensionsY()),
			pixel2(0.0f, 0.0f));
}
void HorizontalSlider::update() {
	double interp = (scrollHandle->getBoundsPositionX()
			- scrollTrack->getBoundsPositionX())
			/ (double) (scrollTrack->getBoundsDimensionsX()
					- scrollHandle->getBoundsDimensionsX());
	double val = (1.0 - interp) * minValue.toDouble()
			+ interp * maxValue.toDouble();
	value.setValue(val);
}
void HorizontalSlider::onMouseDown(AlloyContext* context, Region* region,
		const InputEvent& event) {
	if(event.button==GLFW_MOUSE_BUTTON_LEFT){
		if (region == scrollTrack.get()) {
			scrollHandle->setDragOffset(event.cursor,
					scrollHandle->getBoundsDimensions() * 0.5f);
			context->setDragObject(scrollHandle.get());
			update();
		} else if (region == scrollHandle.get()) {
			update();
		}
	}
}
void HorizontalSlider::onMouseUp(AlloyContext* context, Region* region,
		const InputEvent& event) {
}
void HorizontalSlider::onMouseDrag(AlloyContext* context, Region* region,
		const InputEvent& event, const pixel2& lastCursorLocation) {
	if (region == scrollHandle.get()) {
		region->setDragOffset(event.cursor, lastCursorLocation);
		update();
	}
}
void HorizontalSlider::draw(AlloyContext* context) {

	valueLabel->label = labelFormatter(value);
	NVGcontext* nvg = context->nvgContext;
	if (parent != nullptr) {
		box2px pbounds = parent->getBounds();
		nvgScissor(nvg, pbounds.position.x, pbounds.position.y,
				pbounds.dimensions.x, pbounds.dimensions.y);
	}
	float cornerRadius = 5.0f;
	bool hover=context->isMouseContainedIn(this);
	if(hover){
		nvgBeginPath(nvg);
		NVGpaint shadowPaint = nvgBoxGradient(nvg, bounds.position.x + 1,
				bounds.position.y, bounds.dimensions.x - 2, bounds.dimensions.y,
				cornerRadius, 8, Color(0, 0, 0, 255), Color(255, 255, 255, 0));
		nvgFillPaint(nvg, shadowPaint);
		nvgRoundedRect(nvg, bounds.position.x + 1, bounds.position.y + 4,
				bounds.dimensions.x, bounds.dimensions.y, 4.0f);
		nvgFill(nvg);
	}

	nvgBeginPath(nvg);
	nvgRoundedRect(nvg, bounds.position.x, bounds.position.y,
			bounds.dimensions.x, bounds.dimensions.y, cornerRadius);
	nvgFillColor(nvg, Color(64, 64, 64, 255));
	nvgFill(nvg);

		nvgBeginPath(nvg);
		NVGpaint hightlightPaint = nvgBoxGradient(nvg, bounds.position.x,
				bounds.position.y, bounds.dimensions.x,
				bounds.dimensions.y, cornerRadius, 2, Color(64, 64, 64, 0),
				Color(255,255,255,255));
		nvgFillPaint(nvg, hightlightPaint);
		nvgRoundedRect(nvg, bounds.position.x, bounds.position.y,
				bounds.dimensions.x, bounds.dimensions.y, cornerRadius);
		nvgFill(nvg);
	Composite::draw(context);
}
void Button::drawOnTop(AlloyContext* context) {
	if(isDragEnabled()&&context->isMouseDrag(this)){
		internalDraw(context);
	}
}
void Button::draw(AlloyContext* context) {
	if(!isDragEnabled()||!context->isMouseDrag(this)){
		internalDraw(context);
	}
}
void Button::internalDraw(AlloyContext* context) {
	bool hover = context->isMouseOver(this);
	bool down = context->isMouseDown(this);
	NVGcontext* nvg = context->nvgContext;
	float cornerRadius = 5.0f;
	float lineWidth = 2.0f;

	int xoff = 0;
	int yoff = 0;
	if (down) {
		xoff = 2;
		yoff = 2;
	}
	if (hover || down) {
		if (!down) {
			nvgBeginPath(nvg);
			NVGpaint shadowPaint = nvgBoxGradient(nvg, bounds.position.x + 1,
					bounds.position.y, bounds.dimensions.x - 2,
					bounds.dimensions.y, cornerRadius, 8, Color(0, 0, 0, 255),
					Color(255, 255, 255, 0));
			nvgFillPaint(nvg, shadowPaint);
			nvgRoundedRect(nvg, bounds.position.x + 1, bounds.position.y + 4,
					bounds.dimensions.x, bounds.dimensions.y, 4.0f);
			nvgFill(nvg);
		}

		nvgBeginPath(nvg);
		nvgRoundedRect(nvg, bounds.position.x + xoff, bounds.position.y + yoff,
				bounds.dimensions.x, bounds.dimensions.y, cornerRadius);
		nvgFillColor(nvg, Color(255, 255, 255, 255));
		nvgFill(nvg);

	} else {
		nvgBeginPath(nvg);
		nvgRoundedRect(nvg, bounds.position.x + 1, bounds.position.y + 1,
				bounds.dimensions.x - 2, bounds.dimensions.y - 2, cornerRadius);
		nvgFillColor(nvg, Color(255, 255, 255, 255));
		nvgFill(nvg);
	}

	if (hover) {

		nvgBeginPath(nvg);
		NVGpaint hightlightPaint = nvgBoxGradient(nvg, bounds.position.x + xoff,
				bounds.position.y + yoff, bounds.dimensions.x,
				bounds.dimensions.y, cornerRadius, 4, Color(255, 255, 255, 0),
				Color(64, 64, 64, 255));
		nvgFillPaint(nvg, hightlightPaint);
		nvgRoundedRect(nvg, bounds.position.x + xoff, bounds.position.y + yoff,
				bounds.dimensions.x, bounds.dimensions.y, cornerRadius);
		nvgFill(nvg);
	}

	float th = fontSize.toPixels(bounds.dimensions.y, context->dpmm.y,
			context->pixelRatio);
	nvgFontSize(nvg, th);
	nvgFillColor(nvg, *textColor);
	nvgFontFaceId(nvg, context->getFontHandle(FontType::Bold));
	float tw = nvgTextBounds(nvg, 0, 0, name.c_str(), nullptr, nullptr);
	this->aspect = tw / th;
	nvgTextAlign(nvg, NVG_ALIGN_MIDDLE | NVG_ALIGN_CENTER);
	pixel2 offset(0, 0);
	nvgText(nvg, bounds.position.x + bounds.dimensions.x / 2 + xoff,
			bounds.position.y + bounds.dimensions.y / 2 + yoff, name.c_str(),
			nullptr);

}
}

