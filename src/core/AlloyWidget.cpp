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
#include "AlloyDrawUtil.h"
using namespace std;
namespace aly {
CheckBox::CheckBox(const std::string& label, const AUnit2D& position,
		const AUnit2D& dimensions, bool checked) :
		Widget(label,position,dimensions), checked(checked) {
	this->aspectRatio = 4.0f;
	CompositePtr valueContainer = MakeComposite("Check Bounds",
			CoordPerPX(0.0f, 0.0f, 5.0f, 5.0f),
			CoordPerPX(1.0f, 1.0f, -10.0f, -10.0f));
	checkLabel = MakeTextLabel(label, CoordPercent(0.0f, 0.0f),
			CoordPercent(1.0f, 1.0f), FontType::Bold, UnitPercent(1.0f),
			Application::getContext()->theme.LIGHT_TEXT.toRGBA(),
			HorizontalAlignment::Left, VerticalAlignment::Middle);
	valueLabel = MakeGlyphRegion(
			Application::getContext()->createAwesomeGlyph(0xf00c),
			CoordPercent(1.0f, 0.0f), CoordPercent(0.0f, 1.0f), COLOR_NONE,
			Application::getContext()->theme.LIGHT_TEXT.toRGBA());
	valueLabel->setOrigin(Origin::TopRight);
	valueLabel->setAspectRule(AspectRule::FixedHeight);
	valueContainer->add(checkLabel);
	valueContainer->add(valueLabel);
	add(valueContainer);
	this->valueLabel->foregroundColor =
			(this->checked) ?
					MakeColor(Application::getContext()->theme.LIGHT_TEXT) :
					MakeColor(Application::getContext()->theme.DARK);
	valueLabel->onMouseDown =
			[this](AlloyContext* context,const InputEvent& event) {
				if(event.button==GLFW_MOUSE_BUTTON_LEFT) {
					this->checked=!this->checked;
					this->valueLabel->foregroundColor=(this->checked)?MakeColor(Application::getContext()->theme.LIGHT_TEXT):MakeColor(Application::getContext()->theme.DARK);

				}
			};
	checkLabel->onMouseDown =
			[this](AlloyContext* context,const InputEvent& event) {
				if(event.button==GLFW_MOUSE_BUTTON_LEFT) {
					this->checked=!this->checked;
					this->valueLabel->foregroundColor=(this->checked)?MakeColor(Application::getContext()->theme.LIGHT_TEXT):MakeColor(Application::getContext()->theme.DARK);
				}
			};
}
void CheckBox::draw(AlloyContext* context) {
	NVGcontext* nvg = context->nvgContext;
	box2px bounds = getBounds();
	bool hover = context->isMouseContainedIn(this);
	if (hover) {
		nvgBeginPath(nvg);
		NVGpaint shadowPaint = nvgBoxGradient(nvg, bounds.position.x + 1,
				bounds.position.y, bounds.dimensions.x - 2, bounds.dimensions.y,
				context->theme.CORNER_RADIUS, 8, context->theme.SHADOW,
				context->theme.HIGHLIGHT.toSemiTransparent(0));
		nvgFillPaint(nvg, shadowPaint);
		nvgRoundedRect(nvg, bounds.position.x + 1, bounds.position.y + 4,
				bounds.dimensions.x, bounds.dimensions.y,
				context->theme.CORNER_RADIUS);
		nvgFill(nvg);
	}

	nvgBeginPath(nvg);
	nvgRoundedRect(nvg, bounds.position.x, bounds.position.y,
			bounds.dimensions.x, bounds.dimensions.y,
			context->theme.CORNER_RADIUS);
	nvgFillColor(nvg, context->theme.DARK);
	nvgFill(nvg);

	nvgBeginPath(nvg);
	NVGpaint hightlightPaint = nvgBoxGradient(nvg, bounds.position.x,
			bounds.position.y, bounds.dimensions.x, bounds.dimensions.y,
			context->theme.CORNER_RADIUS, 2,
			context->theme.DARK.toSemiTransparent(0), context->theme.HIGHLIGHT);
	nvgFillPaint(nvg, hightlightPaint);
	nvgRoundedRect(nvg, bounds.position.x, bounds.position.y,
			bounds.dimensions.x, bounds.dimensions.y,
			context->theme.CORNER_RADIUS);
	nvgFill(nvg);

	nvgBeginPath(nvg);
	nvgFillColor(nvg, context->theme.NEUTRAL);
	box2px clickbox = valueLabel->getBounds();
	nvgRoundedRect(nvg, clickbox.position.x, clickbox.position.y,
			clickbox.dimensions.x, clickbox.dimensions.y,
			context->theme.CORNER_RADIUS);
	nvgFill(nvg);

	if (context->isMouseOver(valueLabel.get())
			|| context->isMouseOver(checkLabel.get())) {
		nvgBeginPath(nvg);
		nvgStrokeColor(nvg, context->theme.LIGHT_TEXT);
		nvgStrokeWidth(nvg, 2.0f);
		nvgRoundedRect(nvg, clickbox.position.x, clickbox.position.y,
				clickbox.dimensions.x, clickbox.dimensions.y,
				context->theme.CORNER_RADIUS);
		nvgStroke(nvg);
	}

	Composite::draw(context);
}
void ProgressBar::draw(AlloyContext* context) {
	box2px bounds = getBounds();
	NVGcontext* nvg = context->nvgContext;
	float x = bounds.position.x;
	float y = bounds.position.y;
	float w = bounds.dimensions.x;
	float h = bounds.dimensions.y;
	const float FADE = 8;
	NVGpaint shadowPaint = nvgBoxGradient(nvg, x, y, //+1
			w, h, (h) / 2, FADE, context->theme.LIGHT, context->theme.SHADOW);
	nvgBeginPath(nvg);
	nvgRoundedRect(nvg, x, y, w, h, h / 2);
	nvgFillPaint(nvg, shadowPaint);
	nvgFill(nvg);

	NVGpaint gradPaint = nvgLinearGradient(nvg, x, y, x, y + h,
			context->theme.NEUTRAL, context->theme.DARK);
	pushScissor(nvg, x, y, w * value, h);
	nvgBeginPath(nvg);
	nvgRoundedRect(nvg, x, y, w, h, h / 2);
	nvgFillPaint(nvg, gradPaint);
	nvgFill(nvg);
	shadowPaint = nvgBoxGradient(nvg, x,
			y, //+1
			w, h, (h) / 2, FADE, context->theme.LIGHT.toSemiTransparent(0.0f),
			context->theme.SHADOW.toSemiTransparent(1.0f));
	nvgFillPaint(nvg, shadowPaint);
	nvgFill(nvg);
	nvgTextAlign(nvg, NVG_ALIGN_MIDDLE | NVG_ALIGN_CENTER);
	nvgFontFaceId(nvg, context->getFontHandle(FontType::Bold));
	nvgFontSize(nvg, std::max(8.0f, h - FADE));
	drawText(nvg, pixel2(x + 0.5f * w, y + 0.5f * h), label, FontStyle::Normal,
			context->theme.LIGHT_TEXT, context->theme.DARK_TEXT);
	popScissor(nvg);

	pushScissor(nvg, x + w * value, y, w * (1.0f - value), h);
	drawText(nvg, pixel2(x + 0.5f * w, y + 0.5f * h), label, FontStyle::Normal,
			context->theme.DARK_TEXT, context->theme.LIGHT_TEXT);
	popScissor(nvg);
}
ProgressBar::ProgressBar(const std::string& name, const AUnit2D& pt,
		const AUnit2D& dims) :
		Widget(name, pt, dims), value(0), label(name) {

}
Button::Button(const std::string& label, const AUnit2D& position,
		const AUnit2D& dimensions) :
		Widget(label) {
	this->position = position;
	this->dimensions = dimensions;
	backgroundColor = MakeColor(Application::getContext()->theme.HIGHLIGHT);
	textColor = MakeColor(Application::getContext()->theme.DARK_TEXT);
	borderColor = MakeColor(Application::getContext()->theme.LIGHT);
	fontSize = UnitPerPX(1.0f, -10);
	this->aspectRule = AspectRule::FixedHeight;
}
void SliderTrack::draw(AlloyContext* context) {
	NVGcontext* nvg = context->nvgContext;
	box2px bounds = getBounds();
	if(orientation==Orientation::Horizontal){
		nvgBeginPath(nvg);
		nvgMoveTo(nvg, bounds.position.x + bounds.dimensions.y*0.5f-1,
				bounds.position.y  + bounds.dimensions.y * 0.5f);
		nvgLineTo(nvg, bounds.position.x - bounds.dimensions.y*0.5f+1 + bounds.dimensions.x,
				bounds.position.y + bounds.dimensions.y * 0.5f);
		nvgStrokeColor(nvg, context->theme.HIGHLIGHT);
		nvgStrokeWidth(nvg, 10.0f);
		nvgLineCap(nvg, NVG_ROUND);
		nvgStroke(nvg);
	} else if(orientation==Orientation::Vertical){
		nvgBeginPath(nvg);
		nvgMoveTo(nvg,
				bounds.position.x + bounds.dimensions.x * 0.5f,
				bounds.position.y + bounds.dimensions.x*0.5f-1);
		nvgLineTo(nvg,
				bounds.position.x + bounds.dimensions.x * 0.5f,
				bounds.position.y - bounds.dimensions.x*0.5f+1 + bounds.dimensions.y);
		nvgStrokeColor(nvg, context->theme.HIGHLIGHT);
		nvgStrokeWidth(nvg, 10.0f);
		nvgLineCap(nvg, NVG_ROUND);
		nvgStroke(nvg);

	}
	for (std::shared_ptr<Region> ptr : children) {
		ptr->draw(context);
	}
}
void SliderHandle::draw(AlloyContext* context) {
	NVGcontext* nvg = context->nvgContext;
	box2px bounds = getBounds();
	if (context->isMouseOver(this) || context->isMouseDown(this)) {
		nvgBeginPath(nvg);
		nvgCircle(nvg, bounds.position.x + bounds.dimensions.x * 0.5f,
				bounds.position.y + bounds.dimensions.y * 0.5f,
				bounds.dimensions.y * 0.5f);
		nvgFillColor(nvg, context->theme.LIGHT.toSemiTransparent(0.5f));
		nvgFill(nvg);
	}
	nvgBeginPath(nvg);
	nvgCircle(nvg, bounds.position.x + bounds.dimensions.x * 0.5f,
			bounds.position.y + bounds.dimensions.y * 0.5f,
			bounds.dimensions.y * 0.25f);
	nvgFillColor(nvg, context->theme.HIGHLIGHT);
	nvgFill(nvg);

	nvgBeginPath(nvg);
	nvgStrokeWidth(nvg, 2.0f);
	nvgStrokeColor(nvg, context->theme.NEUTRAL.toSemiTransparent(0.5f));
	nvgCircle(nvg, bounds.position.x + bounds.dimensions.x * 0.5f,
			bounds.position.y + bounds.dimensions.y * 0.5f,
			bounds.dimensions.y * 0.25f);
	nvgStroke(nvg);

}
void SelectionBox::draw(AlloyContext* context) {

}
void SelectionBox::drawOnTop(AlloyContext* context) {
	context->setDragObject(this);
	NVGcontext* nvg = context->nvgContext;
	box2px bounds = getBounds();
	pixel lineWidth = borderWidth.toPixels(bounds.dimensions.y, context->dpmm.y,
			context->pixelRatio);
	float entryHeight = std::min(
			context->viewport.dimensions.y / (float) options.size(),
			bounds.dimensions.y);
	float boxHeight = (options.size()) * entryHeight;

	float yOffset = std::min(bounds.position.y + boxHeight + entryHeight,
			(float) context->viewport.dimensions.y) - boxHeight;
	if (backgroundColor->a > 0) {
		nvgBeginPath(nvg);
		nvgRect(nvg, bounds.position.x, yOffset, bounds.dimensions.x,
				boxHeight);
		nvgFillColor(nvg, *backgroundColor);
		nvgFill(nvg);
	}

	float th = entryHeight - 2;
	nvgFontSize(nvg, th);
	nvgFillColor(nvg, *textColor);

	float tw = 0.0f;
	for (std::string& label : options) {
		tw = std::max(tw,
				nvgTextBounds(nvg, 0, 0, label.c_str(), nullptr, nullptr));
	}
	nvgTextAlign(nvg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
	pixel2 offset(0, 0);

	nvgFillColor(nvg, context->theme.DARK);
	int index = 0;
	nvgFontFaceId(nvg, context->getFontHandle(FontType::Normal));

	selectedIndex = -1;
	for (std::string& label : options) {
		if (context->isMouseContainedIn(
				pixel2(bounds.position.x, yOffset) + offset,
				pixel2(bounds.dimensions.x, entryHeight))) {
			nvgBeginPath(nvg);
			nvgRect(nvg, bounds.position.x + offset.x, yOffset + offset.y,
					bounds.dimensions.x, entryHeight);
			nvgFillColor(nvg, context->theme.NEUTRAL);
			nvgFill(nvg);
			selectedIndex = index;
		}
		index++;
		nvgFillColor(nvg, *textColor);
		nvgText(nvg, bounds.position.x + offset.x + lineWidth,
				yOffset + entryHeight / 2 + offset.y, label.c_str(), nullptr);
		offset.y += entryHeight;
	}
	if (borderColor->a > 0) {
		nvgBeginPath(nvg);
		nvgRect(nvg, bounds.position.x + lineWidth * 0.5f,
				yOffset + lineWidth * 0.5f, bounds.dimensions.x - lineWidth,
				boxHeight - lineWidth);
		nvgStrokeColor(nvg, *borderColor);
		nvgStrokeWidth(nvg, lineWidth);
		nvgStroke(nvg);
	}
}
SelectionBox::SelectionBox(const std::string& name,
		const std::vector<std::string>& labels) :
		Region(name), options(labels), label(name) {
}
Selection::Selection(const std::string& label, const AUnit2D& position,
		const AUnit2D& dimensions, const std::vector<std::string>& options) :
		Widget(label) {
	this->position = position;
	this->dimensions = dimensions;
	CompositePtr valueContainer = MakeComposite(label,
			CoordPerPX(0.0f, 0.0f, 5.0f, 5.0f),
			CoordPerPX(1.0f, 1.0f, -10.0f, -10.0f));
	selectionLabel = MakeTextLabel(label, CoordPercent(0.0f, 0.0f),
			CoordPercent(1.0f, 1.0f), FontType::Bold, UnitPercent(1.0f),
			Application::getContext()->theme.LIGHT_TEXT.toRGBA(),
			HorizontalAlignment::Left, VerticalAlignment::Middle);
	arrowLabel = MakeGlyphRegion(
			Application::getContext()->createAwesomeGlyph(0xf13a),
			CoordPercent(1.0f, 0.0f), CoordPercent(0.0f, 1.0f),
			Application::getContext()->theme.DARK.toRGBA(),
			Application::getContext()->theme.LIGHT_TEXT.toRGBA());
	selectionBox = SelectionBoxPtr(new SelectionBox("Selection", options));
	selectionBox->setPosition(CoordPercent(0.0f, 0.0f));
	selectionBox->setDimensions(CoordPercent(1.0f, 1.0f));
	selectionBox->fontSize = selectionLabel->fontSize;
	selectionBox->backgroundColor = MakeColor(
			Application::getContext()->theme.DARK);
	selectionBox->borderColor = MakeColor(
			Application::getContext()->theme.HIGHLIGHT);
	selectionBox->borderWidth = UnitPX(1.0f);
	selectionBox->setVisible(false);
	selectionBox->textColor = MakeColor(
			Application::getContext()->theme.LIGHT_TEXT);
	selectionBox->textAltColor = MakeColor(
			Application::getContext()->theme.DARK_TEXT);
	arrowLabel->setOrigin(Origin::TopRight);
	arrowLabel->setAspectRule(AspectRule::FixedHeight);
	valueContainer->add(selectionLabel);
	valueContainer->add(arrowLabel);
	add(valueContainer);
	add(selectionBox);
	selectionLabel->onMouseDown =
			[this](AlloyContext* context,const InputEvent& event) {
				if(event.button==GLFW_MOUSE_BUTTON_LEFT) {
					selectionBox->setVisible(true);
					context->setOnTopRegion(selectionBox.get());
				}
			};
	arrowLabel->onMouseDown =
			[this](AlloyContext* context,const InputEvent& event) {
				if(event.button==GLFW_MOUSE_BUTTON_LEFT) {
					selectionBox->setVisible(true);
					context->setOnTopRegion(selectionBox.get());
				}
			};
	selectionBox->onMouseUp =
			[this](AlloyContext* context,const InputEvent& event) {
				if(event.button==GLFW_MOUSE_BUTTON_LEFT) {
					selectionBox->setVisible(false);
					int newSelection=selectionBox->getSelectedIndex();
					if(newSelection<0) {
						selectionBox->setSelectedIndex(selectedIndex);
					} else {
						selectedIndex=selectionBox->getSelectedIndex();
						selectionBox->setSelectedIndex(selectedIndex);
					}
					selectionLabel->label=this->getSelection();

				}
			};
}
void Selection::draw(AlloyContext* context) {
	NVGcontext* nvg = context->nvgContext;
	bool hover = context->isMouseContainedIn(this);
	box2px bounds = getBounds();
	if (hover) {
		nvgBeginPath(nvg);
		NVGpaint shadowPaint = nvgBoxGradient(nvg, bounds.position.x + 1,
				bounds.position.y, bounds.dimensions.x - 2, bounds.dimensions.y,
				context->theme.CORNER_RADIUS, 8, context->theme.SHADOW,
				context->theme.HIGHLIGHT.toSemiTransparent(0.0f));
		nvgFillPaint(nvg, shadowPaint);
		nvgRoundedRect(nvg, bounds.position.x + 1, bounds.position.y + 4,
				bounds.dimensions.x, bounds.dimensions.y,
				context->theme.CORNER_RADIUS);
		nvgFill(nvg);
	}

	nvgBeginPath(nvg);
	nvgRoundedRect(nvg, bounds.position.x, bounds.position.y,
			bounds.dimensions.x, bounds.dimensions.y,
			context->theme.CORNER_RADIUS);
	nvgFillColor(nvg, context->theme.DARK);
	nvgFill(nvg);

	nvgBeginPath(nvg);
	NVGpaint hightlightPaint = nvgBoxGradient(nvg, bounds.position.x,
			bounds.position.y, bounds.dimensions.x, bounds.dimensions.y,
			context->theme.CORNER_RADIUS, 2,
			context->theme.SHADOW.toSemiTransparent(0.0f),
			context->theme.HIGHLIGHT);
	nvgFillPaint(nvg, hightlightPaint);
	nvgRoundedRect(nvg, bounds.position.x, bounds.position.y,
			bounds.dimensions.x, bounds.dimensions.y,
			context->theme.CORNER_RADIUS);
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
	this->aspectRatio = 4.0f;
	textColor = MakeColor(Application::getContext()->theme.DARK_TEXT);
	borderColor = MakeColor(Application::getContext()->theme.HIGHLIGHT);
	sliderHandle = std::shared_ptr<SliderHandle>(
			new SliderHandle("Scroll Handle"));

	sliderHandle->setPosition(CoordPercent(0.0, 0.0));
	sliderHandle->setDimensions(CoordPX(handleSize, handleSize));
	sliderHandle->backgroundColor = MakeColor(
			Application::getContext()->theme.LIGHT);
	sliderHandle->setEnableDrag(true);

	sliderTrack = std::shared_ptr<SliderTrack>(new SliderTrack("Scroll Track",Orientation::Horizontal));

	sliderTrack->setPosition(CoordPerPX(0.0f, 1.0f, 0.0f, -handleSize));
	sliderTrack->setDimensions(CoordPerPX(1.0f, 0.0f, 0.0f, handleSize));

	sliderTrack->backgroundColor = MakeColor(
			Application::getContext()->theme.DARK);
	sliderTrack->add(sliderHandle);
	sliderTrack->onMouseDown =
			[this](AlloyContext* context,const InputEvent& e) {this->onMouseDown(context,sliderTrack.get(),e);};
	sliderHandle->onMouseDown =
			[this](AlloyContext* context,const InputEvent& e) {this->onMouseDown(context,sliderHandle.get(),e);};
	sliderHandle->onMouseUp =
			[this](AlloyContext* context,const InputEvent& e) {this->onMouseUp(context,sliderHandle.get(),e);};
	sliderHandle->onMouseDrag =
			[this](AlloyContext* context,const InputEvent& e,const pixel2& lastDragPosition) {this->onMouseDrag(context,sliderHandle.get(),e,lastDragPosition);};

	add(
			sliderLabel = MakeTextLabel(label,
					CoordPerPX(0.0f, 0.0f, trackPadding, 2.0f),
					CoordPerPX(0.5f, 1.0f, 0,
							-(handleSize - trackPadding * 0.75f)),
					FontType::Bold, UnitPerPX(1.0f, 0),
					Application::getContext()->theme.LIGHT_TEXT.toRGBA(),
					HorizontalAlignment::Left, VerticalAlignment::Bottom));
	add(
			valueLabel = MakeTextLabel("Value",
					CoordPerPX(0.0f, 0.0f, 0.0f, 2.0f),
					CoordPerPX(1.0f, 1.0f, -trackPadding,
							-(handleSize - trackPadding * 0.75f)),
					FontType::Normal, UnitPerPX(1.0f, -2),
					Application::getContext()->theme.LIGHT_TEXT.toRGBA(),
					HorizontalAlignment::Right, VerticalAlignment::Bottom));
	add(sliderTrack);
	this->onPack = [this]() {
		this->setValue(this->value.toDouble());
	};
}
void HorizontalSlider::setValue(double value) {
	double interp = clamp(
			(value - minValue.toDouble())
					/ (maxValue.toDouble() - minValue.toDouble()), 0.0, 1.0);
	double xoff = sliderTrack->getBoundsPositionX()
			+ interp
					* (sliderTrack->getBoundsDimensionsX()
							- sliderHandle->getBoundsDimensionsX());
	sliderHandle->setDragOffset(
			pixel2(xoff, sliderHandle->getBoundsDimensionsY()),
			pixel2(0.0f, 0.0f));
}
void HorizontalSlider::update() {
	double interp = (sliderHandle->getBoundsPositionX()
			- sliderTrack->getBoundsPositionX())
			/ (double) (sliderTrack->getBoundsDimensionsX()
					- sliderHandle->getBoundsDimensionsX());
	double val = (1.0 - interp) * minValue.toDouble()
			+ interp * maxValue.toDouble();
	value.setValue(val);
}
void HorizontalSlider::onMouseDown(AlloyContext* context, Region* region,
		const InputEvent& event) {
	if (event.button == GLFW_MOUSE_BUTTON_LEFT) {
		if (region == sliderTrack.get()) {
			sliderHandle->setDragOffset(event.cursor,
					sliderHandle->getBoundsDimensions() * 0.5f);
			context->setDragObject(sliderHandle.get());
			update();
		} else if (region == sliderHandle.get()) {
			update();
		}
	}
}
void HorizontalSlider::onMouseUp(AlloyContext* context, Region* region,
		const InputEvent& event) {
}
void HorizontalSlider::onMouseDrag(AlloyContext* context, Region* region,
		const InputEvent& event, const pixel2& lastCursorLocation) {
	if (region == sliderHandle.get()) {
		region->setDragOffset(event.cursor, lastCursorLocation);
		update();
	}
}
void HorizontalSlider::draw(AlloyContext* context) {

	valueLabel->label = labelFormatter(value);
	NVGcontext* nvg = context->nvgContext;
	box2px bounds = getBounds();
	bool hover = context->isMouseContainedIn(this);
	if (hover) {
		nvgBeginPath(nvg);
		NVGpaint shadowPaint = nvgBoxGradient(nvg, bounds.position.x + 1,
				bounds.position.y, bounds.dimensions.x - 2, bounds.dimensions.y,
				context->theme.CORNER_RADIUS, 8, context->theme.DARK,
				context->theme.HIGHLIGHT.toSemiTransparent(0.0f));
		nvgFillPaint(nvg, shadowPaint);
		nvgRoundedRect(nvg, bounds.position.x + 1, bounds.position.y + 4,
				bounds.dimensions.x, bounds.dimensions.y,
				context->theme.CORNER_RADIUS);
		nvgFill(nvg);
	}

	nvgBeginPath(nvg);
	nvgRoundedRect(nvg, bounds.position.x, bounds.position.y,
			bounds.dimensions.x, bounds.dimensions.y,
			context->theme.CORNER_RADIUS);
	nvgFillColor(nvg, context->theme.DARK);
	nvgFill(nvg);

	nvgBeginPath(nvg);
	NVGpaint hightlightPaint = nvgBoxGradient(nvg, bounds.position.x,
			bounds.position.y, bounds.dimensions.x, bounds.dimensions.y,
			context->theme.CORNER_RADIUS, 2,
			context->theme.DARK.toSemiTransparent(0.0f),
			context->theme.HIGHLIGHT);
	nvgFillPaint(nvg, hightlightPaint);
	nvgRoundedRect(nvg, bounds.position.x, bounds.position.y,
			bounds.dimensions.x, bounds.dimensions.y,
			context->theme.CORNER_RADIUS);
	nvgFill(nvg);
	for (std::shared_ptr<Region>& region : children) {
		if (region->isVisible()) {
			region->draw(context);
		}
	}
}
////
VerticalSlider::VerticalSlider(const std::string& label,
		const AUnit2D& position, const AUnit2D& dimensions, const Number& min,
		const Number& max, const Number& value) :
		Widget(label), minValue(min), maxValue(max), value(value) {
	this->position = position;
	this->dimensions = dimensions;
	float handleSize = 30.0f;
	float trackPadding = 10.0f;
	this->aspectRatio = 4.0f;
	textColor = MakeColor(Application::getContext()->theme.DARK_TEXT);
	borderColor = MakeColor(Application::getContext()->theme.HIGHLIGHT);
	sliderHandle = std::shared_ptr<SliderHandle>(
			new SliderHandle("Scroll Handle"));

	sliderHandle->setPosition(CoordPercent(0.0, 0.0));
	sliderHandle->setDimensions(CoordPX(handleSize, handleSize));
	sliderHandle->backgroundColor = MakeColor(
			Application::getContext()->theme.LIGHT);
	sliderHandle->setEnableDrag(true);

	sliderTrack = std::shared_ptr<SliderTrack>(new SliderTrack("Scroll Track",Orientation::Vertical));

	sliderTrack->setPosition(CoordPerPX(0.5f, 0.1f, -handleSize*0.5f,2));
	sliderTrack->setDimensions(CoordPerPX(0.0f, 0.8f, handleSize,-4));

	sliderTrack->backgroundColor = MakeColor(
			Application::getContext()->theme.DARK);
	sliderTrack->add(sliderHandle);
	sliderTrack->onMouseDown =
			[this](AlloyContext* context,const InputEvent& e) {this->onMouseDown(context,sliderTrack.get(),e);};
	sliderHandle->onMouseDown =
			[this](AlloyContext* context,const InputEvent& e) {this->onMouseDown(context,sliderHandle.get(),e);};
	sliderHandle->onMouseUp =
			[this](AlloyContext* context,const InputEvent& e) {this->onMouseUp(context,sliderHandle.get(),e);};
	sliderHandle->onMouseDrag =
			[this](AlloyContext* context,const InputEvent& e,const pixel2& lastDragPosition) {this->onMouseDrag(context,sliderHandle.get(),e,lastDragPosition);};

	add(
			sliderLabel = MakeTextLabel(label,
					CoordPercent(0.0f, 0.0f),
					CoordPercent(1.0f, 0.1f),
					FontType::Bold, UnitPerPX(1.0f, 0),
					Application::getContext()->theme.LIGHT_TEXT.toRGBA(),
					HorizontalAlignment::Center, VerticalAlignment::Top));
	add(
			valueLabel = MakeTextLabel("Value",
					CoordPercent(0.0f, 1.0f),
					CoordPercent(1.0f, 0.1f),
					FontType::Normal, UnitPerPX(1.0f, -2),
					Application::getContext()->theme.LIGHT_TEXT.toRGBA(),
					HorizontalAlignment::Center, VerticalAlignment::Bottom));
	valueLabel->setOrigin(Origin::BottomLeft);
	add(sliderTrack);
	this->onPack = [this]() {
		this->setValue(this->value.toDouble());
	};
}
void VerticalSlider::setValue(double value) {
	double interp = 1.0f-clamp(
			(value - minValue.toDouble())
					/ (maxValue.toDouble() - minValue.toDouble()), 0.0, 1.0);
	double yoff = sliderTrack->getBoundsPositionY()+ interp* (sliderTrack->getBoundsDimensionsY()- sliderHandle->getBoundsDimensionsY());
	sliderHandle->setDragOffset(
			pixel2(sliderHandle->getBoundsDimensionsX(),yoff),
			pixel2(0.0f, 0.0f));
}
void VerticalSlider::update() {
	double interp = (sliderHandle->getBoundsPositionY()
			- sliderTrack->getBoundsPositionY())
			/ (double) (sliderTrack->getBoundsDimensionsY()
					- sliderHandle->getBoundsDimensionsY());
	double val = interp*minValue.toDouble()
			+ (1.0 - interp) *  maxValue.toDouble();
	value.setValue(val);
}
void VerticalSlider::onMouseDown(AlloyContext* context, Region* region,
		const InputEvent& event) {
	if (event.button == GLFW_MOUSE_BUTTON_LEFT) {
		if (region == sliderTrack.get()) {
			sliderHandle->setDragOffset(event.cursor,
					sliderHandle->getBoundsDimensions() * 0.5f);
			context->setDragObject(sliderHandle.get());
			update();
		} else if (region == sliderHandle.get()) {
			update();
		}
	}
}
void VerticalSlider::onMouseUp(AlloyContext* context, Region* region,
		const InputEvent& event) {
}
void VerticalSlider::onMouseDrag(AlloyContext* context, Region* region,
		const InputEvent& event, const pixel2& lastCursorLocation) {
	if (region == sliderHandle.get()) {
		region->setDragOffset(event.cursor, lastCursorLocation);
		update();
	}
}
void VerticalSlider::draw(AlloyContext* context) {
	valueLabel->label = labelFormatter(value);
	NVGcontext* nvg = context->nvgContext;
	box2px bounds = getBounds();
	bool hover = context->isMouseContainedIn(this);
	if (hover) {
		nvgBeginPath(nvg);
		NVGpaint shadowPaint = nvgBoxGradient(nvg, bounds.position.x + 1,
				bounds.position.y, bounds.dimensions.x - 2, bounds.dimensions.y,
				context->theme.CORNER_RADIUS, 8, context->theme.DARK,
				context->theme.HIGHLIGHT.toSemiTransparent(0.0f));
		nvgFillPaint(nvg, shadowPaint);
		nvgRoundedRect(nvg, bounds.position.x + 1, bounds.position.y + 4,
				bounds.dimensions.x, bounds.dimensions.y,
				context->theme.CORNER_RADIUS);
		nvgFill(nvg);
	}

	nvgBeginPath(nvg);
	nvgRoundedRect(nvg, bounds.position.x, bounds.position.y,
			bounds.dimensions.x, bounds.dimensions.y,
			context->theme.CORNER_RADIUS);
	nvgFillColor(nvg, context->theme.DARK);
	nvgFill(nvg);

	nvgBeginPath(nvg);
	NVGpaint hightlightPaint = nvgBoxGradient(nvg, bounds.position.x,
			bounds.position.y, bounds.dimensions.x, bounds.dimensions.y,
			context->theme.CORNER_RADIUS, 2,
			context->theme.DARK.toSemiTransparent(0.0f),
			context->theme.HIGHLIGHT);
	nvgFillPaint(nvg, hightlightPaint);
	nvgRoundedRect(nvg, bounds.position.x, bounds.position.y,
			bounds.dimensions.x, bounds.dimensions.y,
			context->theme.CORNER_RADIUS);
	nvgFill(nvg);
	for (std::shared_ptr<Region>& region : children) {
		if (region->isVisible()) {
			region->draw(context);
		}
	}
}
ColorSelector::ColorSelector(const std::string& name,const AUnit2D& pos,const AUnit2D& dims):Widget(name,pos,dims){
	CompositePtr valueContainer = MakeComposite("Check Bounds",
			CoordPerPX(0.0f, 0.0f, 5.0f, 5.0f),
			CoordPerPX(1.0f, 1.0f, -10.0f, -10.0f));
	textLabel = MakeTextLabel(name, CoordPercent(0.0f, 0.0f),
			CoordPercent(1.0f, 1.0f), FontType::Bold, UnitPercent(1.0f),
			Application::getContext()->theme.LIGHT_TEXT,
			HorizontalAlignment::Left, VerticalAlignment::Middle);
	colorLabel = MakeRegion(
			"Color",
			CoordPerPX(1.0f, 0.0f,-4.0f,4.0f), CoordPerPX(0.0f, 1.0f,0.0f,-8.0f), Color(255,0,0),Application::getContext()->theme.LIGHT,UnitPX(1.0f));
	colorLabel->setAspectRatio(1.0f);
	colorLabel->setAspectRule(AspectRule::FixedHeight);
	colorLabel->setOrigin(Origin::TopRight);
	colorLabel->onMouseDown=[this](AlloyContext* context,const InputEvent& e){
		if(e.button == GLFW_MOUSE_BUTTON_LEFT){
			this->colorWheel->setVisible(true);
			context->setOnTopRegion(colorWheel.get());
		}
	};

	colorWheel=ColorWheelPtr(new ColorWheel("Color Wheel",CoordPerPX(0,1.0,0.0f,2.0f),CoordPX(400,300)));
	colorWheel->setOrigin(Origin::TopLeft);

	add(textLabel);
	add(colorLabel);
	add(colorWheel);
}
ColorWheel::ColorWheel(const std::string& name,const AUnit2D& pos,const AUnit2D& dims):Composite(name,pos,dims){

	setColor(Color(32,64,255));
	this->onPack=[this](){
		this->updateWheel();
	};
	this->onMouseDown=[this](AlloyContext* context,const InputEvent& e){
		if(e.button==GLFW_MOUSE_BUTTON_LEFT&&e.isDown()){
			float r2=distanceSqr(e.cursor,center);
			if(r2<rInner*rInner){
				triangleSelected=true;
				circleSelected=false;
			} else if(r2<rOuter*rOuter){
				triangleSelected=false;
				circleSelected=true;
			} else {
				circleSelected=false;
				triangleSelected=false;
			}
			this->setColor(e.cursor);
		}
	};
	this->onMouseOver=[this](AlloyContext* context,const InputEvent& e){
		if(context->isLeftMouseButtonDown()){
			this->setColor(e.cursor);
		}
	};
}
void ColorWheel::updateWheel(){
	box2px bounds = getBounds();
	float x=bounds.position.x;
	float y=bounds.position.y;
	float w=bounds.dimensions.x;
	float h=bounds.dimensions.y;

	float cx = x + w * 0.5f;
	float cy = y + h * 0.5f;
	rOuter = (w < h ? w : h) * 0.5f - 5.0f;
	rInner = rOuter - 20.0f;
	center=float2(cx,cy);
	float r = rInner - 6;
	tPoints[0]=float2(r,0);
	tPoints[1]=float2(cos(120.0f / 180.0f * NVG_PI) * r,sin(120.0f / 180.0f * NVG_PI) * r);
	tPoints[2]=float2(cosf(-120.0f / 180.0f * NVG_PI) * r,sin(-120.0f / 180.0f * NVG_PI) * r);
	float angle=-hsvColor.x * NVG_PI * 2;
	for(int i=0;i<3;i++){
		tBounds[i]=Rotate(tPoints[i],angle)+center;
	}
}
void ColorWheel::setColor(const Color& c){
	selectedColor=c;
	hsvColor=c.toHSV();
	updateWheel();
}
void ColorWheel::setColor(const pixel2& cursor){
	float2 pt=cursor-tBounds[0];
	if(triangleSelected){
		float v=cross(pt,tBounds[2]-tBounds[0])/std::max(1E-6f,cross(pt,tBounds[2]-tBounds[1]));
		float s=dot(pt,pt)/dot(pt,(tBounds[2]-tBounds[0])+v*(tBounds[1]-tBounds[2]));
		v=clamp(v,0.0f,1.0f);
		s=clamp(s,0.0f,1.0f);
		hsvColor.z=v;
		hsvColor.y=s;
		selectedColor=HSVtoColor(hsvColor);
		updateWheel();
	} else if(circleSelected){
		float2 vec=cursor-center;
		hsvColor.x=(atan2(vec.y,vec.x))/(2.0f*NVG_PI);
		if(hsvColor.x<0.0f){
			hsvColor.x+=1.0f;
		}
		selectedColor=HSVtoColor(hsvColor);
		updateWheel();
	}
}
void ColorWheel::drawOnTop(AlloyContext* context){
	NVGcontext* nvg = context->nvgContext;
	box2px bounds = getBounds();

	float x=bounds.position.x;
	float y=bounds.position.y;
	float w=bounds.dimensions.x;
	float h=bounds.dimensions.y;
	float t=0.3f;

	nvgBeginPath(nvg);
	nvgRoundedRect(nvg, bounds.position.x, bounds.position.y,
			bounds.dimensions.x, bounds.dimensions.y,
			context->theme.CORNER_RADIUS);
	nvgFillColor(nvg, context->theme.LIGHT);
	nvgFill(nvg);

	int i;
	float r0, r1, ax, ay, bx, by, cx, cy, aeps, r;

	float hue = hsvColor.x;

	NVGpaint paint;
	nvgSave(nvg);
	aeps = 0.5f / rOuter;	// half a pixel arc length in radians (2pi cancels out).
	for (i = 0; i < 6; i++) {
		float a0 = (float) i / 6.0f * NVG_PI * 2.0f - aeps;
		float a1 = (float) (i + 1.0f) / 6.0f * NVG_PI * 2.0f + aeps;
		nvgBeginPath(nvg);
		nvgArc(nvg, center.x, center.y, rInner, a0, a1, NVG_CW);
		nvgArc(nvg, center.x, center.y, rOuter, a1, a0, NVG_CCW);
		nvgClosePath(nvg);
		ax = center.x + cosf(a0) * (rInner + rOuter) * 0.5f;
		ay = center.y + sinf(a0) * (rInner + rOuter) * 0.5f;
		bx = center.x + cosf(a1) * (rInner + rOuter) * 0.5f;
		by = center.y + sinf(a1) * (rInner + rOuter) * 0.5f;
		paint = nvgLinearGradient(nvg, ax, ay, bx, by,
				nvgHSLA(a0 / (NVG_PI * 2), 1.0f, 0.55f, 255),
				nvgHSLA(a1 / (NVG_PI * 2), 1.0f, 0.55f, 255));
		nvgFillPaint(nvg, paint);
		nvgFill(nvg);
	}

	nvgBeginPath(nvg);
	nvgCircle(nvg, center.x, center.y, rInner - 0.5f);
	nvgCircle(nvg, center.x, center.y, rOuter + 0.5f);
	nvgStrokeColor(nvg, nvgRGBA(0, 0, 0, 64));
	nvgStrokeWidth(nvg, 1.0f);
	nvgStroke(nvg);

	// Selector
	nvgSave(nvg);
	nvgTranslate(nvg, center.x, center.y);
	nvgRotate(nvg, hue * NVG_PI * 2);
	// Marker on
	nvgStrokeWidth(nvg, 2.0f);
	nvgBeginPath(nvg);
	nvgRect(nvg, rInner - 1, -3, rOuter - rInner + 2, 6);
	nvgStrokeColor(nvg, nvgRGBA(255, 255, 255, 192));
	nvgStroke(nvg);

	paint = nvgBoxGradient(nvg, rInner - 3, -5, rOuter - rInner + 6, 10, 2, 4,
			nvgRGBA(0, 0, 0, 128), nvgRGBA(0, 0, 0, 0));
	nvgBeginPath(nvg);
	nvgRect(nvg, rInner - 2 - 10, -4 - 10, rOuter - rInner + 4 + 20, 8 + 20);
	nvgRect(nvg, rInner - 2, -4, rOuter - rInner + 4, 8);
	nvgPathWinding(nvg, NVG_HOLE);
	nvgFillPaint(nvg, paint);
	nvgFill(nvg);

	// Center triangle
	/*
	r = rInner - 6;
	ax = cosf(120.0f / 180.0f * NVG_PI) * r;
	ay = sinf(120.0f / 180.0f * NVG_PI) * r;
	bx = cosf(-120.0f / 180.0f * NVG_PI) * r;
	by = sinf(-120.0f / 180.0f * NVG_PI) * r;
	*/
	nvgBeginPath(nvg);
	nvgMoveTo(nvg, tPoints[0].x, tPoints[0].y);
	nvgLineTo(nvg, tPoints[1].x, tPoints[1].y);
	nvgLineTo(nvg, tPoints[2].x, tPoints[2].y);
	nvgClosePath(nvg);
	paint = nvgLinearGradient(nvg,
		tPoints[0].x, tPoints[0].y, tPoints[1].x, tPoints[1].y,
		nvgHSLA(hue, 1.0f, 0.5f, 255),
			nvgRGBA(255, 255, 255, 255));
	nvgFillPaint(nvg, paint);
	nvgFill(nvg);
	paint = nvgLinearGradient(nvg,
			(tPoints[0].x+tPoints[1].x) * 0.5f,
			(tPoints[0].y+tPoints[1].y) * 0.5f,
			tPoints[2].x, tPoints[2].y,
			nvgRGBA(0, 0, 0, 0), nvgRGBA(0, 0, 0, 255));
	nvgFillPaint(nvg, paint);
	nvgFill(nvg);
	nvgStrokeColor(nvg, nvgRGBA(0, 0, 0, 64));
	nvgStroke(nvg);
	float2 bary=mix(tPoints[0],mix(tPoints[2],tPoints[1],hsvColor.z),hsvColor.y);
	ax=bary.x;
	ay=bary.y;
	nvgStrokeWidth(nvg, 2.0f);
	nvgBeginPath(nvg);
	nvgCircle(nvg, ax, ay, 5);
	nvgStrokeColor(nvg, nvgRGBA(255, 255, 255, 192));
	nvgStroke(nvg);
	paint = nvgRadialGradient(nvg, ax, ay, 7, 9, nvgRGBA(0, 0, 0, 64),
			nvgRGBA(0, 0, 0, 0));
	nvgBeginPath(nvg);
	nvgRect(nvg, ax - 20, ay - 20, 40, 40);
	nvgCircle(nvg, ax, ay, 7);
	nvgPathWinding(nvg, NVG_HOLE);
	nvgFillPaint(nvg, paint);
	nvgFill(nvg);

	nvgRestore(nvg);

	nvgRestore(nvg);

	/*
	nvgBeginPath(nvg);
	nvgStrokeColor(nvg, Color(0,64,0));
	nvgStrokeWidth(nvg,2.0f);
	nvgMoveTo(nvg,tBounds[0].x,tBounds[0].y);
	nvgLineTo(nvg,tBounds[1].x,tBounds[1].y);
	nvgLineTo(nvg,tBounds[2].x,tBounds[2].y);
	nvgClosePath(nvg);
	nvgStroke(nvg);

	nvgBeginPath(nvg);
	nvgStrokeColor(nvg, Color(0,64,0));
	nvgStrokeWidth(nvg,2.0f);
	nvgCircle(nvg,center.x,center.y,rInner);
	nvgCircle(nvg,center.x,center.y,rOuter);
	nvgStroke(nvg);
	*/
}
void ColorSelector::draw(AlloyContext* context){
	NVGcontext* nvg = context->nvgContext;
	box2px bounds = getBounds();
	bool hover = context->isMouseContainedIn(this);
	if(colorWheel->isVisible()){
		*colorLabel->backgroundColor=colorWheel->getSelectedColor();
	}
	if (hover) {
		nvgBeginPath(nvg);
		NVGpaint shadowPaint = nvgBoxGradient(nvg, bounds.position.x + 1,
				bounds.position.y, bounds.dimensions.x - 2, bounds.dimensions.y,
				context->theme.CORNER_RADIUS, 8, context->theme.SHADOW,
				context->theme.HIGHLIGHT.toSemiTransparent(0));
		nvgFillPaint(nvg, shadowPaint);
		nvgRoundedRect(nvg, bounds.position.x + 1, bounds.position.y + 4,
				bounds.dimensions.x, bounds.dimensions.y,
				context->theme.CORNER_RADIUS);
		nvgFill(nvg);
	}
	if(context->isMouseOver(colorLabel.get())){
		colorLabel->borderWidth=UnitPX(2.0f);
		colorLabel->borderColor=MakeColor(context->theme.HIGHLIGHT);
	}else {
		colorLabel->borderWidth=UnitPX(1.0f);
		colorLabel->borderColor=MakeColor(context->theme.NEUTRAL);
	}
	nvgBeginPath(nvg);
	nvgRoundedRect(nvg, bounds.position.x, bounds.position.y,
			bounds.dimensions.x, bounds.dimensions.y,
			context->theme.CORNER_RADIUS);
	nvgFillColor(nvg, context->theme.DARK);
	nvgFill(nvg);

	nvgBeginPath(nvg);
	NVGpaint hightlightPaint = nvgBoxGradient(nvg, bounds.position.x,
			bounds.position.y, bounds.dimensions.x, bounds.dimensions.y,
			context->theme.CORNER_RADIUS, 2,
			context->theme.DARK.toSemiTransparent(0), context->theme.HIGHLIGHT);
	nvgFillPaint(nvg, hightlightPaint);
	nvgRoundedRect(nvg, bounds.position.x, bounds.position.y,
			bounds.dimensions.x, bounds.dimensions.y,
			context->theme.CORNER_RADIUS);
	nvgFill(nvg);

	nvgBeginPath(nvg);
	nvgFillColor(nvg, context->theme.NEUTRAL);
	box2px clickbox = colorLabel->getBounds();
	nvgRoundedRect(nvg, clickbox.position.x, clickbox.position.y,
			clickbox.dimensions.x, clickbox.dimensions.y,
			context->theme.CORNER_RADIUS);
	nvgFill(nvg);
	Composite::draw(context);
}
void Button::drawOnTop(AlloyContext* context) {
	if (isDragEnabled() && context->isMouseDrag(this)) {
		internalDraw(context);
	}
}
void Button::draw(AlloyContext* context) {
	if (!isDragEnabled() || !context->isMouseDrag(this)) {
		internalDraw(context);
	}
}
void Button::internalDraw(AlloyContext* context) {
	bool hover = context->isMouseOver(this);
	bool down = context->isMouseDown(this);
	NVGcontext* nvg = context->nvgContext;
	box2px bounds = getBounds();
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
					bounds.dimensions.y, context->theme.CORNER_RADIUS, 8,
					context->theme.SHADOW,
					context->theme.HIGHLIGHT.toSemiTransparent(0.0f));
			nvgFillPaint(nvg, shadowPaint);
			nvgRoundedRect(nvg, bounds.position.x + 1, bounds.position.y + 4,
					bounds.dimensions.x, bounds.dimensions.y,
					context->theme.CORNER_RADIUS);
			nvgFill(nvg);
		}

		nvgBeginPath(nvg);
		nvgRoundedRect(nvg, bounds.position.x + xoff, bounds.position.y + yoff,
				bounds.dimensions.x, bounds.dimensions.y,
				context->theme.CORNER_RADIUS);
		nvgFillColor(nvg, *backgroundColor);
		nvgFill(nvg);

	} else {
		nvgBeginPath(nvg);
		nvgRoundedRect(nvg, bounds.position.x + 1, bounds.position.y + 1,
				bounds.dimensions.x - 2, bounds.dimensions.y - 2,
				context->theme.CORNER_RADIUS);
		nvgFillColor(nvg, *backgroundColor);
		nvgFill(nvg);
	}

	if (hover) {

		nvgBeginPath(nvg);
		NVGpaint hightlightPaint = nvgBoxGradient(nvg, bounds.position.x + xoff,
				bounds.position.y + yoff, bounds.dimensions.x,
				bounds.dimensions.y, context->theme.CORNER_RADIUS, 4,
				context->theme.HIGHLIGHT.toSemiTransparent(0.0f),
				context->theme.DARK);
		nvgFillPaint(nvg, hightlightPaint);
		nvgRoundedRect(nvg, bounds.position.x + xoff, bounds.position.y + yoff,
				bounds.dimensions.x, bounds.dimensions.y,
				context->theme.CORNER_RADIUS);
		nvgFill(nvg);
	}

	float th = fontSize.toPixels(bounds.dimensions.y, context->dpmm.y,
			context->pixelRatio);
	nvgFontSize(nvg, th);
	nvgFillColor(nvg, *textColor);
	nvgFontFaceId(nvg, context->getFontHandle(FontType::Bold));
	float tw = nvgTextBounds(nvg, 0, 0, name.c_str(), nullptr, nullptr);
	this->aspectRatio = tw / th;
	nvgTextAlign(nvg, NVG_ALIGN_MIDDLE | NVG_ALIGN_CENTER);
	pixel2 offset(0, 0);
	nvgText(nvg, bounds.position.x + bounds.dimensions.x / 2 + xoff,
			bounds.position.y + bounds.dimensions.y / 2 + yoff, name.c_str(),
			nullptr);

}
}

