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
#include "AlloyContext.h"
#include <future>
#include <cstdlib>
#include <cctype>

using namespace std;
namespace aly {

bool CheckBox::handleMouseDown(AlloyContext* context, const InputEvent& event) {
	if (event.button == GLFW_MOUSE_BUTTON_LEFT) {
		this->checked = !this->checked;
		this->valueLabel->textColor =
				(this->checked) ?
						MakeColor(AlloyApplicationContext()->theme.LIGHTER) :
						MakeColor(AlloyApplicationContext()->theme.DARK);
		if (onChange)
			onChange(this->checked);
		return true;
	}
	return false;
}
CheckBox::CheckBox(const std::string& label, const AUnit2D& position,
		const AUnit2D& dimensions, bool checked,bool showText) :
		Composite(label, position, dimensions), checked(checked) {
	this->aspectRatio = 4.0f;
	backgroundColor = MakeColor(AlloyApplicationContext()->theme.DARK);
	borderColor = MakeColor(AlloyApplicationContext()->theme.LIGHT);
	borderWidth = UnitPX(1.0f);
	setRoundCorners(true);
	CompositePtr valueContainer = MakeComposite("Check Bounds",
			CoordPerPX(0.0f, 0.0f, 5.0f, 5.0f),
			CoordPerPX(1.0f, 1.0f, -10.0f, -10.0f));

	DrawPtr checkBoundsDraw;
	if (showText) {
		checkLabel = MakeTextLabel(label, CoordPercent(0.0f, 0.0f),
			CoordPercent(1.0f, 1.0f), FontType::Bold, UnitPercent(1.0f),
			AlloyApplicationContext()->theme.LIGHTER.toRGBA(),
			HorizontalAlignment::Left, VerticalAlignment::Middle);
		valueLabel = MakeTextLabel(CodePointToUTF8(0xf00c),
			CoordPercent(1.0f, 0.0f), CoordPercent(0.0f, 1.0f), FontType::Icon,
			UnitPercent(1.0f),
			AlloyApplicationContext()->theme.LIGHTER.toRGBA(),
			HorizontalAlignment::Center, VerticalAlignment::Middle);
		valueLabel->setAspectRatio(1.0f);
		valueLabel->setOrigin(Origin::TopRight);
		valueLabel->setAspectRule(AspectRule::FixedHeight);
		checkBoundsDraw = DrawPtr(new Draw("Check Bounds", CoordPercent(1.0f, 0.0f), CoordPercent(0.0f, 1.0f)));
		checkBoundsDraw->setAspectRatio(1.0f);
		checkBoundsDraw->setOrigin(Origin::TopRight);
		checkBoundsDraw->setAspectRule(AspectRule::FixedHeight);
	}
	else {
		valueLabel = MakeTextLabel(CodePointToUTF8(0xf00c),
			CoordPercent(0.5f, 0.0f), CoordPercent(0.0f, 1.0f), FontType::Icon,
			UnitPercent(1.0f),
			AlloyApplicationContext()->theme.LIGHTER.toRGBA(),
			HorizontalAlignment::Center, VerticalAlignment::Middle);
		valueLabel->setAspectRatio(1.0f);
		valueLabel->setOrigin(Origin::TopCenter);
		valueLabel->setAspectRule(AspectRule::FixedHeight);
		checkBoundsDraw = DrawPtr(new Draw("Check Bounds", CoordPercent(0.5f, 0.0f), CoordPercent(0.0f, 1.0f)));
		checkBoundsDraw->setAspectRatio(1.0f);
		checkBoundsDraw->setOrigin(Origin::TopCenter);
		checkBoundsDraw->setAspectRule(AspectRule::FixedHeight);
	}
	checkBoundsDraw->onDraw = [this](AlloyContext* context, const box2px& clickbox) {
		NVGcontext* nvg = context->nvgContext;
		nvgBeginPath(nvg);
		nvgFillColor(nvg, context->theme.NEUTRAL);
		nvgRoundedRect(nvg, clickbox.position.x, clickbox.position.y,
			clickbox.dimensions.x, clickbox.dimensions.y,
			context->theme.CORNER_RADIUS);
		nvgFill(nvg);
		bool hover = context->isMouseContainedIn(this);
		if (hover) {
			nvgBeginPath(nvg);
			nvgStrokeColor(nvg, context->theme.LIGHTEST);
			nvgStrokeWidth(nvg, 2.0f);
			nvgRoundedRect(nvg, clickbox.position.x, clickbox.position.y,
				clickbox.dimensions.x, clickbox.dimensions.y,
				context->theme.CORNER_RADIUS);
			nvgStroke(nvg);
		}
	};

	checkBoundsDraw->setIgnoreCursorEvents(true);
	if (showText) {
		valueContainer->add(checkLabel);
	}
	valueContainer->add(checkBoundsDraw);
	valueContainer->add(valueLabel);
	
	add(valueContainer);
	this->valueLabel->textColor =
			(this->checked) ?
					MakeColor(AlloyApplicationContext()->theme.LIGHTER) :
					MakeColor(AlloyApplicationContext()->theme.DARK);
	valueLabel->onMouseDown =
			[this](AlloyContext* context, const InputEvent& event) {
				return handleMouseDown(context, event);
			};
	Region::onMouseDown =
			[this](AlloyContext* context, const InputEvent& event) {
				return handleMouseDown(context, event);
			};
	valueContainer->onMouseDown =
		[this](AlloyContext* context, const InputEvent& event) {
		return handleMouseDown(context, event);
	};
	if (showText) {
		checkLabel->onMouseDown =
			[this](AlloyContext* context, const InputEvent& event) {
			return handleMouseDown(context, event);
		};
	} 
}
void CheckBox::setValue(bool value) {
	this->checked = value;
	this->valueLabel->textColor =
			(this->checked) ?
					MakeColor(AlloyApplicationContext()->theme.LIGHTER) :
					MakeColor(AlloyApplicationContext()->theme.DARK);
}
void CheckBox::draw(AlloyContext* context) {
	NVGcontext* nvg = context->nvgContext;
	box2px bounds = getBounds();
	bool hover = context->isMouseContainedIn(this);
	if (hover) {
	} else {
		if(checkLabel.get()!=nullptr)checkLabel->textColor = MakeColor(context->theme.LIGHTER);
	}
	Composite::draw(context);
}
bool ToggleBox::handleMouseDown(AlloyContext* context,
		const InputEvent& event) {
	if (event.button == GLFW_MOUSE_BUTTON_LEFT) {
		this->toggledOn = !this->toggledOn;
		onLabel->setVisible(this->toggledOn);
		offLabel->setVisible(!this->toggledOn);
		if (onChange)
			onChange(this->toggledOn);
		return true;
	}
	return false;
}
ToggleBox::ToggleBox(const std::string& label, const AUnit2D& position,
		const AUnit2D& dimensions, bool checked,bool showText) :
		Composite(label, position, dimensions), toggledOn(checked) {
	this->aspectRatio = 4.0f;
	backgroundColor = MakeColor(AlloyApplicationContext()->theme.DARK);
	borderColor = MakeColor(AlloyApplicationContext()->theme.LIGHT);
	borderWidth = UnitPX(1.0f);
	setRoundCorners(true);

	CompositePtr valueContainer=MakeComposite("Check Bounds",
			CoordPerPX(0.0f, 0.0f, 5.0f, 5.0f),
			CoordPerPX(1.0f, 1.0f, -10.0f, -10.0f));
	if (showText) {
		toggleLabel = MakeTextLabel(label, CoordPercent(0.0f, 0.0f),
			CoordPercent(1.0f, 1.0f), FontType::Bold, UnitPercent(1.0f),
			AlloyApplicationContext()->theme.LIGHTER.toRGBA(),
			HorizontalAlignment::Left, VerticalAlignment::Middle);
	}
	onLabel = MakeTextLabel("ON", CoordPercent(0.2f, 0.0f),
			CoordPercent(0.3f, 1.0f), FontType::Bold, UnitPerPX(1.0f, -4.0f),
			AlloyApplicationContext()->theme.LIGHTER,
			HorizontalAlignment::Center, VerticalAlignment::Middle);
	offLabel = MakeTextLabel("OFF", CoordPercent(0.5f, 0.0f),
			CoordPercent(0.3f, 1.0f), FontType::Bold, UnitPerPX(1.0f, -4.0f),
			AlloyApplicationContext()->theme.DARK,
			HorizontalAlignment::Center, VerticalAlignment::Middle);
	onLabel->setTruncate(false);
	offLabel->setTruncate(false);

	DrawPtr toggleRegion = DrawPtr(new Draw("Toggle Region", CoordPX(0.0f,0.0f),CoordPercent(1.0f,1.0f)));
	toggleRegion->setIgnoreCursorEvents(true);
	toggleRegion->onDraw = [this](AlloyContext* context, const box2px& clickbounds) {
		NVGcontext* nvg = context->nvgContext;
		nvgBeginPath(nvg);
		nvgFillColor(nvg, context->theme.NEUTRAL);
		box2px clickbox = clickRegion->getBounds();
		float radius = clickbox.dimensions.y / 2;
		nvgRoundedRect(nvg, clickbox.position.x, clickbox.position.y,
			clickbox.dimensions.x, clickbox.dimensions.y, radius);
		nvgFill(nvg);
		bool hover = context->isMouseContainedIn(clickRegion.get());
		float pos;
		if (toggledOn) {
			pos = clickbox.position.x + clickbox.dimensions.x - radius;
		}
		else {
			pos = clickbox.position.x + radius;
		}
		if (hover) {
			nvgBeginPath(nvg);
			nvgStrokeColor(nvg, context->theme.LIGHTEST);
			nvgStrokeWidth(nvg, 2.0f);
			nvgRoundedRect(nvg, clickbox.position.x, clickbox.position.y,
				clickbox.dimensions.x, clickbox.dimensions.y, radius);
			nvgStroke(nvg);
		}
		nvgBeginPath(nvg);
		nvgFillColor(nvg, context->theme.LIGHT);
		nvgCircle(nvg, pos, clickbox.position.y + radius, radius - 4);
		nvgFill(nvg);
	};

	if (showText) {
		clickRegion = MakeComposite("tog select", CoordPercent(1.0f, 0.0f),
			CoordPercent(0.42f, 1.0f));
		clickRegion->setOrigin(Origin::TopRight);
		clickRegion->setAspectRatio(2.5f);
		clickRegion->setAspectRule(AspectRule::FixedHeight);
		toggleRegion->setOrigin(Origin::TopRight);
		toggleRegion->setAspectRatio(2.5f);
		toggleRegion->setAspectRule(AspectRule::FixedHeight);
	}
	else {
		clickRegion = MakeComposite("tog select", CoordPercent(0.5f, 0.0f),
			CoordPercent(1.0f, 1.0f));
		clickRegion->setOrigin(Origin::TopCenter);
		clickRegion->setAspectRatio(2.5f);
		clickRegion->setAspectRule(AspectRule::FixedHeight);
	}
	clickRegion->add(onLabel);
	clickRegion->add(offLabel);
	if (showText) {
		valueContainer->add(toggleLabel);
	}
	valueContainer->add(toggleRegion);
	valueContainer->add(clickRegion);
	add(valueContainer);
	onLabel->setVisible(this->toggledOn);
	offLabel->setVisible(!this->toggledOn);

	onLabel->onMouseDown =
			[this](AlloyContext* context, const InputEvent& event) {
				return handleMouseDown(context, event);
			};
	offLabel->onMouseDown =
			[this](AlloyContext* context, const InputEvent& event) {
				return handleMouseDown(context, event);
			};
	clickRegion->onMouseDown =
			[this](AlloyContext* context, const InputEvent& event) {
				return handleMouseDown(context, event);
			};
	if (showText) {
		toggleLabel->onMouseDown =
			[this](AlloyContext* context, const InputEvent& event) {
			return handleMouseDown(context, event);
		};
	}
	Region::onMouseDown =
			[this](AlloyContext* context, const InputEvent& event) {
				return handleMouseDown(context, event);
			};

}
void ToggleBox::setValue(bool value) {
	this->toggledOn = value;
	onLabel->setVisible(this->toggledOn);
	offLabel->setVisible(!this->toggledOn);
}
void ToggleBox::draw(AlloyContext* context) {
	NVGcontext* nvg = context->nvgContext;
	box2px bounds = getBounds();
	bool hover = context->isMouseContainedIn(this);
	if (toggleLabel.get()!=nullptr) {
		if (hover) {
			toggleLabel->textColor = MakeColor(context->theme.LIGHTEST);
		}
		else {
			toggleLabel->textColor = MakeColor(context->theme.LIGHTER);
		}
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
	box2px cbounds = getCursorBounds();
	NVGpaint shadowPaint = nvgBoxGradient(nvg, x, y, //+1
			w , h, (h) / 2, FADE, context->theme.LIGHT, context->theme.DARKEST);
	nvgBeginPath(nvg);
	nvgRoundedRect(nvg, x, y, w, h, h / 2);
	nvgFillPaint(nvg, shadowPaint);
	nvgFill(nvg);

	NVGpaint gradPaint = nvgLinearGradient(nvg, x, y, x, y + h,
			context->theme.NEUTRAL, context->theme.DARK);
	pushScissor(nvg, cbounds.position.x, cbounds.position.y, std::min(cbounds.dimensions.x,w * value), cbounds.dimensions.y);
	nvgBeginPath(nvg);
	nvgRoundedRect(nvg, x, y, w, h, h / 2);
	nvgFillPaint(nvg, gradPaint);
	nvgFill(nvg);
	shadowPaint = nvgBoxGradient(nvg, x,
			y, //+1
			w, h, (h) / 2, FADE, context->theme.LIGHT.toSemiTransparent(0.0f),
			context->theme.DARKEST.toSemiTransparent(1.0f));
	nvgFillPaint(nvg, shadowPaint);
	nvgFill(nvg);
	nvgTextAlign(nvg, NVG_ALIGN_MIDDLE | NVG_ALIGN_CENTER);
	nvgFontFaceId(nvg, context->getFontHandle(FontType::Bold));
	nvgFontSize(nvg, std::max(8.0f, h - FADE));
	drawText(nvg, pixel2(x + 0.5f * w, y + 0.5f * h), label, FontStyle::Normal,
			context->theme.LIGHTER, context->theme.DARK);
	popScissor(nvg);
	float xx= std::max(cbounds.position.x, x + w * value);
	pushScissor(nvg, xx, cbounds.position.y, std::min(cbounds.dimensions.x + x - xx,w * (1.0f - value)+x-xx), cbounds.dimensions.y);
	drawText(nvg, pixel2(x + 0.5f * w, y + 0.5f * h), label, FontStyle::Normal,
			context->theme.DARK, context->theme.LIGHTER);
	popScissor(nvg);
}
ProgressBar::ProgressBar(const std::string& name, const AUnit2D& pt,
		const AUnit2D& dims) :
		Composite(name, pt, dims), value(0), label(name) {

}
TextButton::TextButton(const std::string& label, const AUnit2D& position,
		const AUnit2D& dimensions,bool truncate) :
		Region(label),truncate(truncate) {
	this->position = position;
	this->dimensions = dimensions;
	backgroundColor = MakeColor(AlloyApplicationContext()->theme.LIGHTEST);
	textColor = MakeColor(AlloyApplicationContext()->theme.DARK);
	borderColor = MakeColor(AlloyApplicationContext()->theme.LIGHT);
	fontSize = UnitPerPX(1.0f, -10);
	this->aspectRule = AspectRule::FixedHeight;
}
void TextButton::draw(AlloyContext* context) {
	bool hover = context->isMouseOver(this);
	bool down = context->isMouseDown(this);
	NVGcontext* nvg = context->nvgContext;
	box2px bounds = getBounds();

	int xoff = 0;
	int yoff = 0;
	if (down) {
		xoff = 2;
		yoff = 2;
	}
	if (hover) {
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
				context->theme.LIGHTEST.toSemiTransparent(0.0f),
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
	nvgTextAlign(nvg, NVG_ALIGN_MIDDLE | NVG_ALIGN_CENTER);
	float tw = nvgTextBounds(nvg, 0, 0, name.c_str(), nullptr, nullptr);

	pixel2 offset(0, 0);
	if (truncate) {
		pushScissor(nvg, getCursorBounds());
	}
	nvgText(nvg, bounds.position.x + bounds.dimensions.x / 2 + xoff,
			bounds.position.y + bounds.dimensions.y / 2 + yoff, name.c_str(),
			nullptr);
	if (truncate) {
		popScissor(nvg);
	}

}
TextIconButton::TextIconButton(const std::string& label, int iconCode,
		const AUnit2D& position, const AUnit2D& dimensions,
		const HorizontalAlignment& alignment,
		const IconAlignment& iconAlignment,bool truncate) :
		Composite(label), iconCodeString(CodePointToUTF8(iconCode)), iconAlignment(
				iconAlignment), alignment(alignment),truncate(truncate),label(label) {
	this->position = position;
	this->dimensions = dimensions;
	backgroundColor = MakeColor(AlloyApplicationContext()->theme.LIGHTER);
	textColor = MakeColor(AlloyApplicationContext()->theme.DARK);
	borderColor = MakeColor(AlloyApplicationContext()->theme.LIGHT);
	fontSize = UnitPerPX(1.0f, -10);
}

void TextIconButton::setLabel(const std::string& label) {
	this->label = label;
}
void TextIconButton::setIcon(int code) {
	iconCodeString = CodePointToUTF8(code);
}
void TextIconButton::draw(AlloyContext* context) {
	bool hover = context->isMouseOver(this);
	bool down = context->isMouseDown(this);
	NVGcontext* nvg = context->nvgContext;
	box2px bounds = getBounds();
	int xoff = 0;
	int yoff = 0;
	if (down) {
		xoff = 2;
		yoff = 2;
	}
	float th = fontSize.toPixels(bounds.dimensions.y, context->dpmm.y,
		context->pixelRatio);
	if (hover) {

		nvgBeginPath(nvg);
		if (roundCorners) {
			nvgRoundedRect(nvg, bounds.position.x + xoff, bounds.position.y + yoff,
				bounds.dimensions.x, bounds.dimensions.y,
				context->theme.CORNER_RADIUS);
		}
		else {
			nvgRect(nvg, bounds.position.x + xoff, bounds.position.y + yoff,
				bounds.dimensions.x, bounds.dimensions.y);
		}
		nvgFillColor(nvg, *backgroundColor);
		nvgFill(nvg);
	}
	else {
		nvgBeginPath(nvg);
		if (roundCorners) {
			nvgRoundedRect(nvg, bounds.position.x + 1, bounds.position.y + 1,
				bounds.dimensions.x - 2, bounds.dimensions.y - 2,
				context->theme.CORNER_RADIUS);
		}
		else {
			nvgRect(nvg, bounds.position.x + 1, bounds.position.y + 1,
				bounds.dimensions.x - 2, bounds.dimensions.y - 2);
		}
		nvgFillColor(nvg, *backgroundColor);
		nvgFill(nvg);
	}
	nvgFillColor(nvg, *textColor);
	nvgFontSize(nvg, th);
	nvgFontFaceId(nvg, context->getFontHandle(FontType::Bold));
	float tw = nvgTextBounds(nvg, 0, 0,label.c_str(), nullptr, nullptr);

	nvgFontFaceId(nvg, context->getFontHandle(FontType::Icon));
	float iw = nvgTextBounds(nvg, 0, 0, iconCodeString.c_str(), nullptr,
			nullptr);

	float ww = tw + iw + AlloyApplicationContext()->theme.SPACING.x;
	pixel2 offset(0, 0);
	pixel xoffset = (pixel) xoff;
	if (alignment == HorizontalAlignment::Center) {
		xoffset += (bounds.dimensions.x - ww) / 2;
	} else if (alignment == HorizontalAlignment::Right) {
		xoffset += bounds.dimensions.x - ww
				- AlloyApplicationContext()->theme.SPACING.x;
	} else {
		xoffset += AlloyApplicationContext()->theme.SPACING.x;
	}
	nvgTextAlign(nvg, NVG_ALIGN_MIDDLE | NVG_ALIGN_LEFT);
	if (truncate) {
		pushScissor(nvg, getCursorBounds());
	}
	if (iconAlignment == IconAlignment::Right) {
		nvgFontFaceId(nvg, context->getFontHandle(FontType::Bold));
		nvgText(nvg, bounds.position.x + xoffset,
				bounds.position.y + bounds.dimensions.y / 2 + yoff,
			label.c_str(), nullptr);

		nvgFontFaceId(nvg, context->getFontHandle(FontType::Icon));
		nvgText(nvg,
				bounds.position.x + xoffset
						+ AlloyApplicationContext()->theme.SPACING.x + tw,
				bounds.position.y + bounds.dimensions.y / 2 + yoff,
				iconCodeString.c_str(), nullptr);
	} else if (iconAlignment == IconAlignment::Left) {
		nvgFontFaceId(nvg, context->getFontHandle(FontType::Bold));
		nvgText(nvg,
				bounds.position.x + xoffset
						+ AlloyApplicationContext()->theme.SPACING.x + iw,
				bounds.position.y + bounds.dimensions.y / 2 + yoff,
			label.c_str(), nullptr);

		nvgFontFaceId(nvg, context->getFontHandle(FontType::Icon));
		nvgText(nvg, bounds.position.x + xoffset,
				bounds.position.y + bounds.dimensions.y / 2 + yoff,
				iconCodeString.c_str(), nullptr);
	}
	if (truncate) {
		popScissor(nvg);
	}
}
IconButton::IconButton(int iconCode, const AUnit2D& position,
		const AUnit2D& dimensions, IconType iconType,bool truncate) :
		Composite("Icon", position, dimensions), iconCodeString(
				CodePointToUTF8(iconCode)), iconType(iconType),truncate(truncate) {
	this->position = position;
	this->dimensions = dimensions;
	backgroundColor = MakeColor(AlloyApplicationContext()->theme.DARK);
	foregroundColor = MakeColor(AlloyApplicationContext()->theme.DARK);
	borderColor = MakeColor(AlloyApplicationContext()->theme.LIGHT);
	iconColor = MakeColor(AlloyApplicationContext()->theme.LIGHT);

	this->aspectRatio = 1.0f;
	this->aspectRule = AspectRule::FixedHeight;
}
void IconButton::setIcon(int iconCode) {
	iconCodeString = CodePointToUTF8(iconCode);
}
void IconButton::draw(AlloyContext* context) {
	bool hover = context->isMouseOver(this);
	bool down = context->isMouseDown(this);
	NVGcontext* nvg = context->nvgContext;
	box2px bounds = getBounds();
	pixel2 center = bounds.position + HALF_PIX(bounds.dimensions);
	pixel2 radii = HALF_PIX(bounds.dimensions);
	pixel2 offset;
	if (down) {
		offset = pixel2(1, 1);
	} else {
		offset = pixel2(0, 0);
	}
	float hoverOffset = (hover) ? 1.0f : 0.0f;
	Color bgColor = *backgroundColor;
	if (bgColor.a > 0) {
		nvgBeginPath(nvg);
		nvgFillColor(nvg,bgColor);
		nvgRect(nvg, bounds.position.x,
			bounds.position.y ,
			bounds.dimensions.x,
			bounds.dimensions.y);
		nvgFill(nvg);
	}
	if (iconType == IconType::CIRCLE) {
		nvgBeginPath(nvg);
		nvgEllipse(nvg, center.x + offset.x, center.y + offset.y,
				radii.x - 1 + hoverOffset, radii.y - 1 + hoverOffset);
		nvgFillColor(nvg, *foregroundColor);
		nvgFill(nvg);
	} else {
		nvgBeginPath(nvg);
		if (roundCorners) {
			nvgRoundedRect(nvg, bounds.position.x + 1 + offset.x - hoverOffset,
				bounds.position.y + 1 + offset.y - hoverOffset,
				bounds.dimensions.x - 2 + hoverOffset * 2,
				bounds.dimensions.y - 2 + hoverOffset * 2,
				context->theme.CORNER_RADIUS);
		}
		else {
			nvgRect(nvg, bounds.position.x + 1 + offset.x - hoverOffset,
				bounds.position.y + 1 + offset.y - hoverOffset,
				bounds.dimensions.x - 2 + hoverOffset * 2,
				bounds.dimensions.y - 2 + hoverOffset * 2);
		}
		nvgFillColor(nvg, *foregroundColor);
		nvgFill(nvg);

	}
	pixel lineWidth = borderWidth.toPixels(bounds.dimensions.y, context->dpmm.y,
			context->pixelRatio);
	box2px ibounds = bounds;
	ibounds.position += offset;
	float th = ibounds.dimensions.y - 2 * lineWidth - ((hover) ? 2 : 4);
	nvgFontSize(nvg, th);
	nvgFontFaceId(nvg, context->getFontHandle(FontType::Icon));
	nvgTextAlign(nvg, NVG_ALIGN_MIDDLE | NVG_ALIGN_CENTER);
	if (truncate) {
		pushScissor(nvg, getCursorBounds());
	}
	drawText(nvg, ibounds.position + HALF_PIX(ibounds.dimensions),
			iconCodeString, FontStyle::Normal,
			(hover &&borderColor->a>0) ?
					*borderColor: *iconColor, *backgroundColor,
			nullptr);
	if (truncate) {
		popScissor(nvg);
	}
	if (borderColor->a > 0) {
		if (iconType == IconType::CIRCLE) {

			nvgBeginPath(nvg);
			nvgEllipse(nvg, center.x + offset.x, center.y + offset.y,
					radii.x - HALF_PIX(lineWidth) + hoverOffset,
					radii.y - HALF_PIX(lineWidth) + hoverOffset);
			nvgStrokeColor(nvg,
					(hover) ? *borderColor: *iconColor);
			nvgStrokeWidth(nvg, lineWidth);
			nvgStroke(nvg);
		} else {
			nvgBeginPath(nvg);
			if (roundCorners) {
				nvgRoundedRect(nvg,
					bounds.position.x + offset.x + lineWidth - hoverOffset,
					bounds.position.y + offset.y + lineWidth - hoverOffset,
					bounds.dimensions.x - 2 * lineWidth + hoverOffset * 2.0f,
					bounds.dimensions.y - 2 * lineWidth + hoverOffset * 2.0f,
					context->theme.CORNER_RADIUS);
			}
			else {
				nvgRect(nvg,
					bounds.position.x + offset.x + lineWidth - hoverOffset,
					bounds.position.y + offset.y + lineWidth - hoverOffset,
					bounds.dimensions.x - 2 * lineWidth + hoverOffset * 2.0f,
					bounds.dimensions.y - 2 * lineWidth + hoverOffset * 2.0f);
			}
			nvgStrokeColor(nvg,
					(hover) ? context->theme.LIGHTEST : *borderColor);
			nvgStrokeWidth(nvg, lineWidth);
			nvgStroke(nvg);
		}
	}
}
SliderTrack::SliderTrack(const std::string& name, Orientation orient,
		const Color& st, const Color& ed) :
		Composite(name), orientation(orient), startColor(st), endColor(ed) {
}

void SliderTrack::draw(AlloyContext* context) {
	NVGcontext* nvg = context->nvgContext;
	box2px bounds = getBounds();
	float ax, ay, bx, by;
	if (orientation == Orientation::Horizontal) {
		nvgBeginPath(nvg);
		nvgMoveTo(nvg, ax =
				(bounds.position.x + bounds.dimensions.y * 0.5f - 1),
				ay = (bounds.position.y + bounds.dimensions.y * 0.5f));
		nvgLineTo(nvg,
				bx = (bounds.position.x - bounds.dimensions.y * 0.5f + 1
						+ bounds.dimensions.x),
				by = (bounds.position.y + bounds.dimensions.y * 0.5f));
		NVGpaint paint = nvgLinearGradient(nvg, ax, ay, bx, by, endColor,
				startColor);
		nvgStrokePaint(nvg, paint);
		nvgStrokeWidth(nvg, 10.0f);
		nvgLineCap(nvg, NVG_ROUND);
		nvgStroke(nvg);
	} else if (orientation == Orientation::Vertical) {
		nvgBeginPath(nvg);
		nvgMoveTo(nvg, ax = (bounds.position.x + bounds.dimensions.x * 0.5f),
				ay = (bounds.position.y + bounds.dimensions.x * 0.5f - 1));
		nvgLineTo(nvg, bx = (bounds.position.x + bounds.dimensions.x * 0.5f),
				by = (bounds.position.y - bounds.dimensions.x * 0.5f + 1
						+ bounds.dimensions.y));
		NVGpaint paint = nvgLinearGradient(nvg, ax, ay, bx, by, endColor,
				startColor);
		nvgStrokePaint(nvg, paint);
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
	nvgFillColor(nvg, context->theme.LIGHTEST);
	nvgFill(nvg);

	nvgBeginPath(nvg);
	nvgStrokeWidth(nvg, 2.0f);
	nvgStrokeColor(nvg, context->theme.NEUTRAL.toSemiTransparent(0.5f));
	nvgCircle(nvg, bounds.position.x + bounds.dimensions.x * 0.5f,
			bounds.position.y + bounds.dimensions.y * 0.5f,
			bounds.dimensions.y * 0.25f);
	nvgStroke(nvg);

}
bool Selection::handleMouseClick(AlloyContext* context,
		const InputEvent& event) {
	if (event.button == GLFW_MOUSE_BUTTON_LEFT) {
		box2px bounds = getBounds(false);
		selectionBox->pack(bounds.position, bounds.dimensions, context->dpmm,
				context->pixelRatio);
		selectionBox->setSelectionOffset(0);
		selectionBox->setSelectedIndex(0);
		show(context);
		return true;
	} else if (event.button == GLFW_MOUSE_BUTTON_RIGHT) {
		hide(context);
	}
	return false;
}
void Selection::hide(AlloyContext* context) {
	context->removeOnTopRegion(selectionBox.get());
	selectionBox->setVisible(false);
}
void Selection::show(AlloyContext* context) {
	context->setOnTopRegion(selectionBox.get());
	selectionBox->setVisible(true);
}
void Selection::setTextColor(const AColor& c) {
	selectionLabel->textColor = c;
	selectionLabel->textAltColor = c;
	arrowLabel->textColor = c;
	arrowLabel->textAltColor = c;
}
Selection::Selection(const std::string& label, const AUnit2D& position,
		const AUnit2D& dimensions, const std::vector<std::string>& options) :
		Composite(label), selectedIndex(-1) {
	this->position = position;
	this->dimensions = dimensions;
	borderColor = MakeColor(COLOR_NONE);
	backgroundColor = MakeColor(AlloyApplicationContext()->theme.LIGHTER);
	setRoundCorners(true);
	CompositePtr valueContainer = MakeComposite(label,
			CoordPerPX(0.0f, 0.0f, 5.0f, 5.0f),
			CoordPerPX(1.0f, 1.0f, -10.0f, -10.0f));
	selectionLabel = MakeTextLabel(label, CoordPercent(0.0f, 0.0f),
			CoordPercent(1.0f, 1.0f), FontType::Bold, UnitPercent(1.0f),
			AlloyApplicationContext()->theme.DARK.toRGBA(),
			HorizontalAlignment::Left, VerticalAlignment::Middle);
	arrowLabel = MakeTextLabel(CodePointToUTF8(0xf13a),
			CoordPercent(1.0f, 0.0f), CoordPercent(0.0f, 1.0f), FontType::Icon,
			UnitPercent(1.0f),
			AlloyApplicationContext()->theme.DARK.toRGBA(),
			HorizontalAlignment::Center, VerticalAlignment::Middle);
	selectionBox = SelectionBoxPtr(new SelectionBox(label, options));
	selectionBox->setDetached(true);
	selectionBox->setVisible(false);
	selectionBox->position = CoordPercent(0.0f, 0.0f);
	selectionBox->dimensions = CoordPercent(1.0f, 0.8f);
	selectionBox->backgroundColor = MakeColor(
			AlloyApplicationContext()->theme.DARK);
	selectionBox->borderColor = MakeColor(AlloyApplicationContext()->theme.LIGHTER);
	selectionBox->borderWidth = UnitPX(1.0f);
	selectionBox->textColor = MakeColor(
			AlloyApplicationContext()->theme.LIGHTER);
	selectionBox->textAltColor = MakeColor(
			AlloyApplicationContext()->theme.LIGHTER);
	arrowLabel->setAspectRatio(1.0f);
	arrowLabel->setOrigin(Origin::TopRight);
	arrowLabel->setAspectRule(AspectRule::FixedHeight);
	valueContainer->add(selectionLabel);
	valueContainer->add(arrowLabel);
	add(valueContainer);
	add(selectionBox);

	Region::onMouseDown =
			[this](AlloyContext* context, const InputEvent& event) {
				return handleMouseClick(context, event);
			};
	selectionLabel->onMouseDown =
			[this](AlloyContext* context, const InputEvent& event) {
				return handleMouseClick(context, event);
			};
	arrowLabel->onMouseDown =
			[this](AlloyContext* context, const InputEvent& event) {
				return handleMouseClick(context, event);
			};
	selectionBox->onMouseUp =
			[this](AlloyContext* context, const InputEvent& event) {
				if (event.button == GLFW_MOUSE_BUTTON_LEFT) {
					hide(context);
					int newSelection = selectionBox->getSelectedIndex();
					if (newSelection < 0) {
						selectionBox->setSelectedIndex(selectedIndex);
					}
					else {
						selectedIndex = selectionBox->getSelectedIndex();
						selectionBox->setSelectedIndex(selectedIndex);

					}
					if (selectionBox->onSelect) {
						selectionBox->onSelect(selectionBox.get());
					}
					selectionLabel->setLabel( this->getValue());
					return true;
				}
				else if (event.button == GLFW_MOUSE_BUTTON_RIGHT) {
					hide(context);
				}
				return false;
			};
	selectionBox->onSelect = [this](SelectionBox* box) {
		AlloyApplicationContext()->removeOnTopRegion(selectionBox.get());
		selectionBox->setVisible(false);
		int newSelection = selectionBox->getSelectedIndex();
		if (newSelection < 0) {
			selectionBox->setSelectedIndex(selectedIndex);
		}
		else {
			selectedIndex = selectionBox->getSelectedIndex();
			selectionBox->setSelectedIndex(selectedIndex);
		}
		selectionLabel->setLabel( this->getValue());
		if (this->onSelect) {
			this->onSelect(selectedIndex);
		}
		return true;
	};

}

void Selection::draw(AlloyContext* context) {
	NVGcontext* nvg = context->nvgContext;
	bool hover = context->isMouseContainedIn(this);
	box2px bounds = getBounds();
	if (!hover && selectionBox->isVisible()
			&& !context->isLeftMouseButtonDown()) {
		hide(context);
	}
	Composite::draw(context);
}
HorizontalSlider::HorizontalSlider(const std::string& label,
		const AUnit2D& position, const AUnit2D& dimensions,bool showLabel, const Number& min,
		const Number& max, const Number& value) :
		Slider(label, min, max, value) {
	this->position = position;
	this->dimensions = dimensions;
	float handleSize = 30.0f;
	float trackPadding = 10.0f;
	this->aspectRatio = 4.0f;
		
	sliderPosition = value.toDouble();
	textColor = MakeColor(AlloyApplicationContext()->theme.LIGHTER);
	backgroundColor = MakeColor(AlloyApplicationContext()->theme.DARK);
	borderColor = MakeColor(AlloyApplicationContext()->theme.LIGHT);
	borderWidth = UnitPX(1.0f);
	setRoundCorners(true);

	sliderHandle = std::shared_ptr<SliderHandle>(
			new SliderHandle("Scroll Handle"));

	sliderHandle->position = CoordPercent(0.0, 0.0);
	sliderHandle->dimensions = CoordPX(handleSize, handleSize);
	sliderHandle->backgroundColor = MakeColor(
			AlloyApplicationContext()->theme.LIGHT);
	sliderHandle->setDragEnabled(true);

	sliderTrack = std::shared_ptr<SliderTrack>(
			new SliderTrack("Scroll Track", Orientation::Horizontal,
					AlloyApplicationContext()->theme.LIGHTEST,
					AlloyApplicationContext()->theme.LIGHTEST));


	sliderTrack->backgroundColor = MakeColor(
			AlloyApplicationContext()->theme.DARK);
	sliderTrack->add(sliderHandle);
	sliderTrack->onMouseDown =
			[this](AlloyContext* context, const InputEvent& e) {return this->onMouseDown(context, sliderTrack.get(), e);};
	sliderHandle->onMouseDown =
			[this](AlloyContext* context, const InputEvent& e) {return this->onMouseDown(context, sliderHandle.get(), e);};
	sliderHandle->onMouseDrag =
			[this](AlloyContext* context, const InputEvent& e) {
				return this->onMouseDrag(context, sliderHandle.get(), e);};
	if (showLabel) {
		sliderTrack->position = CoordPerPX(0.0f, 1.0f, 0.0f, -handleSize);
		sliderTrack->dimensions = CoordPerPX(1.0f, 0.0f, 0.0f, handleSize);
		add(
			sliderLabel = MakeTextLabel(label,
				CoordPerPX(0.0f, 0.0f, trackPadding, 2.0f),
				CoordPerPX(0.5f, 1.0f, 0.0f,
					-(handleSize - trackPadding * 0.75f)),
				FontType::Bold, UnitPerPX(1.0f, 0.0f),
				AlloyApplicationContext()->theme.LIGHTER.toRGBA(),
				HorizontalAlignment::Left, VerticalAlignment::Bottom));
		add(
			valueLabel = MakeTextLabel("Value",
				CoordPerPX(0.0f, 0.0f, 0.0f, 2.0f),
				CoordPerPX(1.0f, 1.0f, -trackPadding,
					-(handleSize - trackPadding * 0.75f)),
				FontType::Normal, UnitPerPX(1.0f, -2),
				AlloyApplicationContext()->theme.LIGHTER.toRGBA(),
				HorizontalAlignment::Right, VerticalAlignment::Bottom));
	}
	else {
		sliderTrack->position = CoordPerPX(0.0f,0.5f,0.0f,-0.5f*handleSize);
		sliderTrack->dimensions = CoordPerPX(1.0f, 0.0f, 0.0f, handleSize);
	}

	add(sliderTrack);
	this->onPack = [this]() {
		this->setValue(sliderPosition);
	};
	this->onEvent =
			[this](AlloyContext* context, const InputEvent& event) {
				if (event.type == InputType::Scroll&&isVisible() && context->isMouseContainedIn(this)) {
					double oldV = getBlendValue();
					double newV = clamp(event.scroll.y*0.1f + oldV, 0.0, 1.0);
					if (newV != oldV) {
						this->setBlendValue(newV);
						if (onChangeEvent)onChangeEvent(this->value);
						return true;
					}
				}
				return false;
			};
	Application::addListener(this);
}
void HorizontalSlider::setValue(double value) {
	double interp = clamp(
			(value - minValue.toDouble())
					/ (maxValue.toDouble() - minValue.toDouble()), 0.0, 1.0);
	float xoff = (float) (sliderTrack->getBoundsPositionX()
			+ interp
					* (sliderTrack->getBoundsDimensionsX()
							- sliderHandle->getBoundsDimensionsX()));
	sliderHandle->setDragOffset(
			pixel2(xoff, sliderHandle->getBoundsDimensionsY()),
			pixel2(0.0f, 0.0f));
	sliderPosition = value;
	this->value.setValue(
			clamp(value, minValue.toDouble(), maxValue.toDouble()));
}
void HorizontalSlider::update() {
	double interp = (sliderHandle->getBoundsPositionX()
			- sliderTrack->getBoundsPositionX())
			/ (double) (sliderTrack->getBoundsDimensionsX()
					- sliderHandle->getBoundsDimensionsX());
	double val = (double) ((1.0 - interp) * minValue.toDouble()
			+ interp * maxValue.toDouble());
	sliderPosition = val;
	value.setValue(clamp(val, minValue.toDouble(), maxValue.toDouble()));
}
bool HorizontalSlider::onMouseDown(AlloyContext* context, Region* region,
		const InputEvent& event) {
	if (event.button == GLFW_MOUSE_BUTTON_LEFT) {
		if (region == sliderTrack.get()) {
			sliderHandle->setDragOffset(event.cursor,
					sliderHandle->getBoundsDimensions() * 0.5f);
			context->setDragObject(sliderHandle.get());
			update();
			if (onChangeEvent)
				onChangeEvent(value);
			return true;
		} else if (region == sliderHandle.get()) {
			update();
			if (onChangeEvent)
				onChangeEvent(value);
			return true;
		}
	}
	return false;
}
void Slider::setBlendValue(double value) {
	value = clamp(value, 0.0, 1.0);
	setValue(
			value * (maxValue.toDouble() - minValue.toDouble())
					+ minValue.toDouble());
}

double Slider::getBlendValue() const {
	return (sliderPosition - minValue.toDouble())
			/ (maxValue.toDouble() - minValue.toDouble());
}
bool HorizontalSlider::onMouseDrag(AlloyContext* context, Region* region,
		const InputEvent& event) {
	if (region == sliderHandle.get()) {
		region->setDragOffset(event.cursor,
				context->getRelativeCursorDownPosition());
		update();
		if (onChangeEvent)
			onChangeEvent(value);
		return true;
	}
	return false;
}
void HorizontalSlider::draw(AlloyContext* context) {
	if (valueLabel.get() != nullptr) {
		valueLabel->setLabel(labelFormatter(value));
	}
	Composite::draw(context);
}
////
VerticalSlider::VerticalSlider(const std::string& label,
		const AUnit2D& position, const AUnit2D& dimensions, const Number& min,
		const Number& max, const Number& value) :
		Slider(label, min, max, value) {
	this->position = position;
	this->dimensions = dimensions;
	float handleSize = 30.0f;
	this->aspectRatio = 4.0f;
	sliderPosition = value.toDouble();
	textColor = MakeColor(AlloyApplicationContext()->theme.LIGHTER);
	backgroundColor = MakeColor(AlloyApplicationContext()->theme.DARK);
	borderColor = MakeColor(AlloyApplicationContext()->theme.LIGHT);
	borderWidth = UnitPX(1.0f);
	setRoundCorners(true);
	sliderHandle = std::shared_ptr<SliderHandle>(
			new SliderHandle("Scroll Handle"));

	sliderHandle->position = CoordPercent(0.0, 0.0);
	sliderHandle->dimensions = CoordPX(handleSize, handleSize);
	sliderHandle->backgroundColor = MakeColor(
			AlloyApplicationContext()->theme.LIGHT);
	sliderHandle->setDragEnabled(true);

	sliderTrack = std::shared_ptr<SliderTrack>(
			new SliderTrack("Scroll Track", Orientation::Vertical,
					AlloyApplicationContext()->theme.LIGHTEST,
					AlloyApplicationContext()->theme.LIGHTEST));

	sliderTrack->position = CoordPerPX(0.5f, 0.1f, -handleSize * 0.5f, 2.0f);
	sliderTrack->dimensions = CoordPerPX(0.0f, 0.8f, handleSize, -4.0f);

	sliderTrack->backgroundColor = MakeColor(
			AlloyApplicationContext()->theme.DARK);
	sliderTrack->add(sliderHandle);
	sliderTrack->onMouseDown =
			[this](AlloyContext* context, const InputEvent& e) {return this->onMouseDown(context, sliderTrack.get(), e);};
	sliderHandle->onMouseDown =
			[this](AlloyContext* context, const InputEvent& e) {
				return this->onMouseDown(context, sliderHandle.get(), e);};
	sliderHandle->onMouseDrag =
			[this](AlloyContext* context, const InputEvent& e) {
				return this->onMouseDrag(context, sliderHandle.get(), e);};

	add(
			sliderLabel = MakeTextLabel(label, CoordPercent(0.0f, 0.0f),
					CoordPercent(1.0f, 0.1f), FontType::Bold,
					UnitPerPX(1.0f, 0),
					AlloyApplicationContext()->theme.LIGHTER.toRGBA(),
					HorizontalAlignment::Center, VerticalAlignment::Top));
	add(
			valueLabel = MakeTextLabel("Value", CoordPercent(0.0f, 1.0f),
					CoordPercent(1.0f, 0.1f), FontType::Normal,
					UnitPerPX(1.0f, -2),
					AlloyApplicationContext()->theme.LIGHTER.toRGBA(),
					HorizontalAlignment::Center, VerticalAlignment::Bottom));
	valueLabel->setOrigin(Origin::BottomLeft);
	add(sliderTrack);
	this->onPack = [this]() {
		this->setValue(sliderPosition);
	};
	this->onEvent =
			[this](AlloyContext* context, const InputEvent& event) {
				if (event.type == InputType::Scroll&&isVisible() && context->isMouseContainedIn(this)) {
					double oldV = getBlendValue();
					double newV = clamp(event.scroll.y*0.1f + oldV, 0.0, 1.0);
					if (newV != oldV) {
						this->setBlendValue(newV);
						if (onChangeEvent)onChangeEvent(this->value);
						return true;
					}
				}
				return false;
			};
	Application::addListener(this);
}

void VerticalSlider::setValue(double value) {
	double interp = 1.0f
			- clamp(
					(value - minValue.toDouble())
							/ (maxValue.toDouble() - minValue.toDouble()), 0.0,
					1.0);
	float yoff = (float) (sliderTrack->getBoundsPositionY()
			+ interp
					* (sliderTrack->getBoundsDimensionsY()
							- sliderHandle->getBoundsDimensionsY()));
	sliderHandle->setDragOffset(
			pixel2(sliderHandle->getBoundsDimensionsX(), yoff),
			pixel2(0.0f, 0.0f));
	sliderPosition = value;
	this->value.setValue(
			clamp(value, minValue.toDouble(), maxValue.toDouble()));
}
void VerticalSlider::update() {
	double interp = (sliderHandle->getBoundsPositionY()
			- sliderTrack->getBoundsPositionY())
			/ (double) (sliderTrack->getBoundsDimensionsY()
					- sliderHandle->getBoundsDimensionsY());
	double val = (double) (interp * minValue.toDouble()
			+ (1.0 - interp) * maxValue.toDouble());
	sliderPosition = val;
	value.setValue(clamp(val, minValue.toDouble(), maxValue.toDouble()));
}
bool VerticalSlider::onMouseDown(AlloyContext* context, Region* region,
		const InputEvent& event) {
	if (event.button == GLFW_MOUSE_BUTTON_LEFT) {
		if (region == sliderTrack.get()) {
			sliderHandle->setDragOffset(event.cursor,
					sliderHandle->getBoundsDimensions() * 0.5f);
			context->setDragObject(sliderHandle.get());
			update();
			if (onChangeEvent)
				onChangeEvent(value);
			return true;
		} else if (region == sliderHandle.get()) {
			update();
			if (onChangeEvent)
				onChangeEvent(value);
			return true;
		}
	}
	return false;
}
bool VerticalSlider::onMouseDrag(AlloyContext* context, Region* region,
		const InputEvent& event) {
	if (region == sliderHandle.get()) {
		region->setDragOffset(event.cursor,
				context->getRelativeCursorDownPosition());
		update();
		if (onChangeEvent)
			onChangeEvent(value);
		return true;
	}
	return false;
}
void VerticalSlider::draw(AlloyContext* context) {
	if (valueLabel.get() != nullptr) {
		valueLabel->setLabel(labelFormatter(value));
	}
	Composite::draw(context);
}
ColorSelector::ColorSelector(const std::string& name, const AUnit2D& pos,
		const AUnit2D& dims,bool showText) :
		Composite(name, pos, dims) {
	CompositePtr valueContainer = MakeComposite("Color Bounds",
			CoordPerPX(0.0f, 0.0f, 5.0f, 5.0f),
			CoordPerPX(1.0f, 1.0f, -10.0f, -10.0f));
	backgroundColor = MakeColor(AlloyApplicationContext()->theme.DARK);
	borderColor = MakeColor(AlloyApplicationContext()->theme.LIGHT);
	borderWidth = UnitPX(1.0f);
	setRoundCorners(true);

	if (showText) {
		textLabel = MakeTextLabel(name, CoordPercent(0.0f, 0.0f),
			CoordPercent(1.0f, 1.0f), FontType::Bold, UnitPercent(1.0f),
			AlloyApplicationContext()->theme.LIGHTER,
			HorizontalAlignment::Left, VerticalAlignment::Middle);
	}
	static std::shared_ptr<CheckerboardGlyph> checkerboard;

	if (checkerboard.get() == nullptr) {
		if (showText) {
			checkerboard = std::shared_ptr<CheckerboardGlyph>(
				new CheckerboardGlyph(64, 64, 8, 8,
					AlloyApplicationContext().get()));
		}
		else {
			checkerboard = std::shared_ptr<CheckerboardGlyph>(
				new CheckerboardGlyph(64*3, 64, 8*3, 8,
					AlloyApplicationContext().get()));
		}
	}
	colorLabel = std::shared_ptr<GlyphRegion>(
			new GlyphRegion("Color", checkerboard));

	colorLabel->glyph = checkerboard;
	colorLabel->backgroundColor = MakeColor(COLOR_BLACK);
	colorLabel->foregroundColor = MakeColor(255, 128, 32, 255);
	colorLabel->borderColor = MakeColor(AlloyApplicationContext()->theme.LIGHT);
	colorLabel->borderWidth = UnitPX(1.0f);

	if (showText) {
		colorLabel->position = CoordPerPX(1.0f, 0.0f, -4.0f, 4.0f);
		colorLabel->dimensions = CoordPerPX(0.0f, 1.0f, 0.0f, -8.0f);
		colorLabel->setOrigin(Origin::TopRight);
		colorLabel->setAspectRatio(1.0f);
		colorLabel->setAspectRule(AspectRule::FixedHeight);
	}
	else {
		colorLabel->position = CoordPX(2.0f, 2.0f);
		colorLabel->dimensions = CoordPerPX(1.0f, 1.0f, -4.0f, -4.0f);
	}
	colorWheel = ColorWheelPtr(
			new ColorWheel("Color Wheel", CoordPX(0.0f, 0.0f),
					CoordPerPX(1.0f, 0.0f, 0.0f, 300.0f)));
	colorWheel->setAspectRatio(1.0f);
	colorWheel->setAspectRule(AspectRule::FixedHeight);
	colorLabel->onMouseDown =
			[this](AlloyContext* context, const InputEvent& e) {
				if (e.button == GLFW_MOUSE_BUTTON_LEFT) {
					if (!colorSelectionPanel->isVisible()) {
						colorWheel->reset();
					}
					colorSelectionPanel->setVisible(true);
					context->getGlassPane()->setVisible(true);
					return true;
				}
				return false;
			};
	if (showText) {
		textLabel->onMouseDown =
			[this](AlloyContext* context, const InputEvent& e) {
			if (e.button == GLFW_MOUSE_BUTTON_LEFT) {
				if (!colorSelectionPanel->isVisible()) {
					colorWheel->reset();
				}
				colorSelectionPanel->setVisible(true);
				context->getGlassPane()->setVisible(true);
				return true;
			}
			return false;
		};
	}
	colorSelectionPanel = MakeComposite("Color Selection Panel",
			CoordPerPX(0.5f, 0.5, 0.0f, 0.0f),
			CoordPX(
					18 + 300.0f
							+ (60 + AlloyApplicationContext()->theme.SPACING.x)
									* 5, 22 + 300.0f), COLOR_NONE);
	colorSelectionPanel->setVisible(false);
	colorSelectionPanel->setOrigin(Origin::MiddleCenter);

	redSlider = std::shared_ptr<VerticalSlider>(
			new VerticalSlider("R", CoordPX(0.0f, 0.0f),
					CoordPerPX(0.0f, 1.0f, 60.0f, 0.0f), Float(0.0), Float(1.0),
					Float(0.5)));
	redSlider->setLabelFormatter(
			[](const Number& value) {
				std::string str = MakeString() << (int)std::floor(100.0f*value.toFloat()) << "%";
				return str;
			});
	redSlider->setOnChangeEvent([this](const Number& value) {
		Color c = colorWheel->getSelectedColor();
		c.r = value.toFloat();
		colorWheel->setColor(c);
		HSV hsv = c.toHSV();
		lumSlider->setValue(hsv.z);
		updateColorSliders(c);
	});

	greenSlider = std::shared_ptr<VerticalSlider>(
			new VerticalSlider("G", CoordPX(0.0f, 0.0f),
					CoordPerPX(0.0f, 1.0f, 60.0f, 0.0f), Float(0.0), Float(1.0),
					Float(0.5)));
	greenSlider->setOnChangeEvent([this](const Number& value) {
		Color c = colorWheel->getSelectedColor();
		c.g = value.toFloat();
		colorWheel->setColor(c);
		HSV hsv = c.toHSV();
		lumSlider->setValue(hsv.z);
		updateColorSliders(c);
	});
	greenSlider->setLabelFormatter(
			[](const Number& value) {
				string str = MakeString() << (int)std::floor(100.0f*value.toFloat()) << "%";
				return str;
			});
	blueSlider = std::shared_ptr<VerticalSlider>(
			new VerticalSlider("B", CoordPX(0.0f, 0.0f),
					CoordPerPX(0.0f, 1.0f, 60.0f, 0.0f), Float(0.0), Float(1.0),
					Float(0.5)));
	blueSlider->setOnChangeEvent([this](const Number& value) {
		Color c = colorWheel->getSelectedColor();
		c.b = value.toFloat();
		colorWheel->setColor(c);
		HSV hsv = c.toHSV();
		lumSlider->setValue(hsv.z);
		updateColorSliders(c);
	});
	blueSlider->setLabelFormatter(
			[](const Number& value) {
				string str = MakeString() << (int)std::floor(100.0f*value.toFloat()) << "%";
				return str;
			});
	lumSlider = std::shared_ptr<VerticalSlider>(
			new VerticalSlider("L", CoordPX(0.0f, 0.0f),
					CoordPerPX(0.0f, 1.0f, 60.0f, 0.0f), Float(0.0), Float(1.0),
					Float(0.5)));
	lumSlider->setLabelFormatter(
			[](const Number& value) {
				string str = MakeString() << (int)std::floor(100.0f*value.toFloat()) << "%";
				return str;
			});
	lumSlider->setOnChangeEvent([this](const Number& value) {
		Color c = colorWheel->getSelectedColor();
		HSVA hsv = c.toHSVA();
		hsv.z = value.toFloat();
		c = HSVAtoRGBAf(hsv);
		colorWheel->setColor(c);
		redSlider->setValue(c.r);
		greenSlider->setValue(c.g);
		blueSlider->setValue(c.b);
		updateColorSliders(c);
	});

	alphaSlider = std::shared_ptr<VerticalSlider>(
			new VerticalSlider("A", CoordPX(0.0f, 0.0f),
					CoordPerPX(0.0f, 1.0f, 60.0f, 0.0f), Float(0.0), Float(1.0),
					Float(0.5)));
	alphaSlider->setLabelFormatter(
			[](const Number& value) {
				string str = MakeString() << (int)std::floor(100.0f*value.toFloat()) << "%";
				return str;
			});
	alphaSlider->setOnChangeEvent([this](const Number& value) {
		Color c = colorWheel->getSelectedColor();
		c.a = value.toFloat();
		colorWheel->setColor(c);
		updateColorSliders(c);
	});
	colorWheel->setOnChangeEvent([this](const Color& c) {

		redSlider->setValue(c.r);
		greenSlider->setValue(c.g);
		blueSlider->setValue(c.b);
		HSV hsv = c.toHSV();
		lumSlider->setValue(hsv.z);
		updateColorSliders(c);
	});
	IconButtonPtr cancelButton = std::shared_ptr<IconButton>(
			new IconButton(0xf00d, CoordPerPX(1.0, 0.0, -30, 30),
					CoordPX(30, 30), IconType::CIRCLE));
	cancelButton->setOrigin(Origin::BottomLeft);
	cancelButton->backgroundColor = MakeColor(COLOR_NONE);
	cancelButton->borderColor=MakeColor(AlloyApplicationContext()->theme.LIGHTEST);
	cancelButton->onMouseDown =
			[this](AlloyContext* context, const InputEvent& event) {
				colorSelectionPanel->setVisible(false);
				context->getGlassPane()->setVisible(false);
				return true;
			};
	CompositePtr hContainer = MakeComposite("Horizontal Layout",
			CoordPX(0.0f, 18.0f), CoordPerPX(1.0f, 1.0f, -30.0f, -22.0f),
			AlloyApplicationContext()->theme.LIGHT);

	hContainer->setRoundCorners(true);
	hContainer->setOrientation(Orientation::Horizontal);
	hContainer->add(colorWheel);
	hContainer->add(redSlider);
	hContainer->add(greenSlider);
	hContainer->add(blueSlider);
	hContainer->add(lumSlider);
	hContainer->add(alphaSlider);
	colorSelectionPanel->add(hContainer);
	colorSelectionPanel->onEvent =
			[=](AlloyContext* context, const InputEvent& e) {
				if (colorSelectionPanel->isVisible()) {
					if (e.type == InputType::MouseButton&&e.isDown() && !context->isMouseContainedIn(hContainer->getBounds())) {
						colorSelectionPanel->setVisible(false);
						context->getGlassPane()->setVisible(false);
						return true;
					}
				}
				return false;
			};
	colorSelectionPanel->add(cancelButton);
	Application::addListener(colorSelectionPanel.get());
	if (showText) {
		add(textLabel);
	}
	add(colorLabel);
	AlloyApplicationContext()->getGlassPane()->add(colorSelectionPanel);
	setColor(*colorLabel->foregroundColor);
}
void ColorSelector::updateColorSliders(const Color& c) {
	redSlider->setSliderColor(Color(0.0f, c.g, c.b), Color(1.0f, c.g, c.b));
	greenSlider->setSliderColor(Color(c.r, 0.0f, c.b), Color(c.r, 1.0f, c.b));
	blueSlider->setSliderColor(Color(c.r, c.g, 0.0f), Color(c.r, c.g, 1.0f));
	HSV hsv = c.toHSV();
	lumSlider->setSliderColor(HSVtoColor(HSV(hsv.x, hsv.y, 0.0f)),
			HSVtoColor(HSV(hsv.x, hsv.y, 1.0f)));
	alphaSlider->setSliderColor(Color(c.r, c.g, c.b, 0.0f),
			Color(c.r, c.g, c.b, 1.0f));

}
void ColorSelector::setColor(const Color& c) {
	*colorLabel->foregroundColor = c;
	HSVA hsv = c.toHSVA();
	colorWheel->setColor(c);
	redSlider->setValue(c.r);
	greenSlider->setValue(c.g);
	blueSlider->setValue(c.b);
	lumSlider->setValue(hsv.z);
	alphaSlider->setValue(c.a);
	updateColorSliders(c);
	if (onSelect) {
		onSelect(c);
	}
}
Color ColorSelector::getColor() {
	return colorWheel->getSelectedColor();
}
void ColorWheel::reset() {
	circleSelected = false;
	triangleSelected = false;
	updateWheel();
}
ColorWheel::ColorWheel(const std::string& name, const AUnit2D& pos,
		const AUnit2D& dims) :
		Composite(name, pos, dims) {

	setColor(Color(32, 64, 255));
	this->onPack = [this]() {
		this->updateWheel();
	};
	this->onMouseDown = [this](AlloyContext* context, const InputEvent& e) {
		if (e.button == GLFW_MOUSE_BUTTON_LEFT&&e.isDown()) {
			float r2 = distanceSqr(e.cursor, center);
			if (r2 < rInner*rInner) {
				triangleSelected = true;
				circleSelected = false;
			}
			else if (r2 < rOuter*rOuter) {
				triangleSelected = false;
				circleSelected = true;
			}
			else {
				circleSelected = false;
				triangleSelected = false;
			}
			this->setColor(e.cursor);
			return true;
		}
		return false;
	};
	this->onScroll = [this](AlloyContext* context, const InputEvent& e) {
		hsvColor.x += e.scroll.y*0.01f;
		if (hsvColor.x < 0.0f)hsvColor.x += 1.0f;
		if (hsvColor.x > 1.0f)hsvColor.x -= 1.0f;
		setColor(HSVAtoColor(hsvColor));
		updateWheel();
		if (onChangeEvent)onChangeEvent(selectedColor);
		return true;
	};
	this->onMouseOver = [this](AlloyContext* context, const InputEvent& e) {
		if (context->isLeftMouseButtonDown()) {
			this->setColor(e.cursor);
			return true;
		}
		return false;
	};
	Application::addListener(this);
}

void ColorWheel::updateWheel() {
	box2px bounds = getBounds();
	float x = bounds.position.x;
	float y = bounds.position.y;
	float w = bounds.dimensions.x;
	float h = bounds.dimensions.y;

	float cx = x + w * 0.5f;
	float cy = y + h * 0.5f;
	rOuter = (w < h ? w : h) * 0.5f - 5.0f;
	rInner = rOuter - 20.0f;
	center = float2(cx, cy);
	float r = rInner - 6;
	tPoints[0] = float2(r, 0);
	tPoints[1] = float2(cos(120.0f / 180.0f * NVG_PI) * r,
			sin(120.0f / 180.0f * NVG_PI) * r);
	tPoints[2] = float2(cosf(-120.0f / 180.0f * NVG_PI) * r,
			sin(-120.0f / 180.0f * NVG_PI) * r);
	float angle = -hsvColor.x * NVG_PI * 2;
	for (int i = 0; i < 3; i++) {
		tBounds[i] = Rotate(tPoints[i], angle) + center;
	}
}
void ColorWheel::setColor(const Color& c) {
	selectedColor = c;
	hsvColor = c.toHSVA();
	updateWheel();
}
void ColorWheel::setColor(const pixel2& cursor) {
	if (triangleSelected) {

		float2 mid = 0.5f * (tBounds[0] + tBounds[1]);
		float u = clamp(
				dot(cursor - tBounds[0], tBounds[1] - tBounds[0])
						/ lengthSqr(tBounds[1] - tBounds[0]), 0.0f, 1.0f);
		float v = clamp(
				dot(cursor - tBounds[2], mid - tBounds[2])
						/ lengthSqr(mid - tBounds[2]), 0.0f, 1.0f);
		RGBAf hc = HSVAtoRGBAf(HSVA(hsvColor.x, 1.0f, 1.0f, 1.0f));
		RGBAf c = v * (hc + u * (1.0f - hc));
		HSVA hsv = Color(c).toHSVA();
		hsvColor.y = hsv.y;
		hsvColor.z = hsv.z;
		selectedColor = HSVAtoColor(hsvColor);
		updateWheel();
		if (onChangeEvent)
			onChangeEvent(selectedColor);
	} else if (circleSelected) {
		float2 vec = cursor - center;
		hsvColor.x = (atan2(vec.y, vec.x)) / (2.0f * NVG_PI);
		if (hsvColor.x < 0.0f) {
			hsvColor.x += 1.0f;
		}
		selectedColor = HSVAtoColor(hsvColor);
		updateWheel();
		if (onChangeEvent)
			onChangeEvent(selectedColor);
	}
}
void ColorWheel::draw(AlloyContext* context) {
	NVGcontext* nvg = context->nvgContext;
	box2px bounds = getBounds();

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
			context->theme.LIGHTEST);
	nvgFillPaint(nvg, hightlightPaint);
	nvgRoundedRect(nvg, bounds.position.x, bounds.position.y,
			bounds.dimensions.x, bounds.dimensions.y,
			context->theme.CORNER_RADIUS);
	nvgFill(nvg);

	int i;
	float ax, ay, bx, by, aeps;

	float hue = hsvColor.x;

	NVGpaint paint;
	nvgSave(nvg);
	aeps = 0.5f / rOuter; // half a pixel arc length in radians (2pi cancels out).
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
	nvgStrokeColor(nvg, context->theme.NEUTRAL.toSemiTransparent(0.5f));
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
	nvgStrokeColor(nvg, context->theme.LIGHTEST.toSemiTransparent(0.9f));
	nvgStroke(nvg);

	paint = nvgBoxGradient(nvg, rInner - 3, -5, rOuter - rInner + 6, 10, 2, 4,
			nvgRGBA(0, 0, 0, 128), nvgRGBA(0, 0, 0, 0));
	nvgBeginPath(nvg);
	nvgRect(nvg, rInner - 2 - 10, -4 - 10, rOuter - rInner + 4 + 20, 8 + 20);
	nvgRect(nvg, rInner - 2, -4, rOuter - rInner + 4, 8);
	nvgPathWinding(nvg, NVG_HOLE);
	nvgFillPaint(nvg, paint);
	nvgFill(nvg);

	nvgBeginPath(nvg);
	nvgMoveTo(nvg, tPoints[0].x, tPoints[0].y);
	nvgLineTo(nvg, tPoints[1].x, tPoints[1].y);
	nvgLineTo(nvg, tPoints[2].x, tPoints[2].y);
	nvgClosePath(nvg);
	NVGcolor chue = nvgHSLA(hue, 1.0f, 0.5f, 255);
	paint = nvgLinearGradient(nvg, tPoints[0].x, tPoints[0].y, tPoints[1].x,
			tPoints[1].y, chue, nvgRGBA(255, 255, 255, 255));
	nvgFillPaint(nvg, paint);
	nvgFill(nvg);
	paint = nvgLinearGradient(nvg, (tPoints[0].x + tPoints[1].x) * 0.5f,
			(tPoints[0].y + tPoints[1].y) * 0.5f, tPoints[2].x, tPoints[2].y,
			nvgRGBA(0, 0, 0, 0), nvgRGBA(0, 0, 0, 255));
	nvgFillPaint(nvg, paint);
	nvgFill(nvg);
	nvgStrokeColor(nvg, context->theme.NEUTRAL.toSemiTransparent(0.5f));
	nvgStroke(nvg);

	RGBf c3(0.0f, 0.0f, 0.0f);
	RGBf c2(1.0f, 1.0f, 1.0f);
	RGBf hc(chue.r, chue.g, chue.b);
	RGBf c = selectedColor.toRGBf();

	float2 bvec, pt;
	float u, v;

	v = std::max(std::max(c.x, c.y), c.z);
	if (v > 0.0f) {
		u = dot(c / v - hc, 1.0f - hc) / lengthSqr(1.0f - hc);

		float2 mid = 0.5f * (tPoints[0] + tPoints[1]);
		bvec.x = dot(tPoints[0], tPoints[1] - tPoints[0])
				+ lengthSqr(tPoints[1] - tPoints[0]) * u;
		bvec.y = dot(tPoints[2], mid - tPoints[2])
				+ lengthSqr(mid - tPoints[2]) * v;
		float2x2 M;
		float2 row1 = tPoints[1] - tPoints[0];
		float2 row2 = mid - tPoints[2];
		M(0, 0) = row1.x;
		M(0, 1) = row1.y;
		M(1, 0) = row2.x;
		M(1, 1) = row2.y;
		pt = inverse(M) * bvec;

		if (crossMag(pt - tPoints[2], tPoints[1] - tPoints[2]) < 0) {
			pt = dot(pt - tPoints[2], tPoints[1] - tPoints[2])
					* (tPoints[1] - tPoints[2])
					/ lengthSqr(tPoints[1] - tPoints[2]) + tPoints[2];
		} else if (crossMag(tPoints[0] - tPoints[2], pt - tPoints[2]) < 0) {
			pt = dot(pt - tPoints[2], tPoints[0] - tPoints[2])
					* (tPoints[0] - tPoints[2])
					/ lengthSqr(tPoints[0] - tPoints[2]) + tPoints[2];
		}
	} else {
		pt = tPoints[2];
	}
	ax = pt.x;
	ay = pt.y;

	nvgStrokeWidth(nvg, 2.0f);
	nvgBeginPath(nvg);
	nvgCircle(nvg, ax, ay, 5);
	nvgStrokeColor(nvg, context->theme.LIGHTEST.toSemiTransparent(0.9f));
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
}
void ColorSelector::draw(AlloyContext* context) {
	NVGcontext* nvg = context->nvgContext;
	box2px bounds = getBounds();
	bool hover = context->isMouseContainedIn(this);
	if (colorWheel->isVisible()) {
		*colorLabel->foregroundColor = colorWheel->getSelectedColor();
	}
	if (hover) {
		if(textLabel.get()!=nullptr)textLabel->textColor = MakeColor(context->theme.LIGHTEST);
		colorLabel->borderWidth = UnitPX(2.0f);
		colorLabel->borderColor = MakeColor(context->theme.LIGHTEST);
	} else {
		if (textLabel.get() != nullptr)textLabel->textColor = MakeColor(context->theme.LIGHTER);
		colorLabel->borderWidth = UnitPX(1.0f);
		colorLabel->borderColor = MakeColor(context->theme.LIGHTER);
	}

	Composite::draw(context);
}


void FileSelector::setIconColor(const AColor& c) {
	openIcon->iconColor = c;
	openIcon->borderColor=c;
}
FileSelector::FileSelector(const std::string& name, const AUnit2D& pos,
		const AUnit2D& dims) :
		BorderComposite(name, pos, dims) {
	backgroundColor = MakeColor(AlloyApplicationContext()->theme.LIGHTER);
	borderColor= MakeColor(AlloyApplicationContext()->theme.LIGHTER);
	borderWidth = UnitPX(0.0f);
	setRoundCorners(true);
	std::shared_ptr<Composite> &glassPanel =
			AlloyApplicationContext()->getGlassPane();
	fileDialog = std::shared_ptr<FileDialog>(
			new FileDialog("Open File",
					CoordPerPX(0.5, 0.5, -350 + 7.5f, -250 - 7.5f),
					CoordPX(700, 500), FileDialogType::OpenFile));
	fileDialog->setVisible(false);
	glassPanel->add(fileDialog);
	fileLocation = std::shared_ptr<FileField>(
			new FileField("None", CoordPX(0, 0), CoordPercent(1.0f, 1.0f)));
	fileLocation->borderColor = MakeColor(0, 0, 0, 0);
	fileLocation->backgroundColor = MakeColor(0, 0, 0, 0);
	fileLocation->borderWidth = UnitPX(0.0f);
	fileDialog->onSelect = [this](const std::vector<std::string>& file) {
		fileLocation->setValue(file.front());

		if (onChange)onChange(file.front());
	};
	openIcon = std::shared_ptr<IconButton>(
			new IconButton(0xf115, CoordPerPX(0.0f, 0.0f, 2.0f, 4.0f),
					CoordPerPX(1.0f, 1.0f, 0.0f, -4.0f)));
	openIcon->foregroundColor = MakeColor(COLOR_NONE);
	openIcon->borderColor=MakeColor(AlloyApplicationContext()->theme.DARK);
	openIcon->borderWidth=UnitPX(0.0f);
	openIcon->backgroundColor = MakeColor(COLOR_NONE);
	openIcon->iconColor = MakeColor(AlloyApplicationContext()->theme.DARK);
	setCenter(fileLocation);
	setEast(openIcon, UnitPX(32.0f));
	openIcon->onMouseDown =
			[this](AlloyContext* context, const InputEvent& event) {
				if (event.button == GLFW_MOUSE_BUTTON_LEFT) {
					fileLocation->hideDropDown(context);
					std::string file = getValue();
					AlloyApplicationContext()->setMouseFocusObject(nullptr);
					if (FileExists(file)) {
						openFileDialog(context, file);
					}
					else {
						std::string parent = GetParentDirectory(file);
						if (FileExists(parent)) {
							openFileDialog(context, parent);
						}
						else {
							openFileDialog(context, GetCurrentWorkingDirectory());
						}
					}
					return true;
				}
				return false;
			};
	fileLocation->setValue(GetCurrentWorkingDirectory());
	fileLocation->onTextEntered = [this](TextField* field) {
		fileDialog->setValue(field->getValue());
	};
}
void FileSelector::setValue(const std::string& file) {
	fileLocation->setValue(file);
	fileDialog->setValue(file);
}
void FileSelector::openFileDialog(AlloyContext* context,
		const std::string& workingDirectory) {
	if (!fileDialog->isVisible()) {
		fileDialog->setVisible(true);
		context->getGlassPane()->setVisible(true);
	} else {
		fileDialog->setVisible(false);
		context->getGlassPane()->setVisible(false);
	}
	fileDialog->setValue(workingDirectory);
}

FileButton::FileButton(const std::string& name, const AUnit2D& pos,
		const AUnit2D& dims, const FileDialogType& type) :
		IconButton((type == FileDialogType::SaveFile) ? 0xF0C7 : 0xf115, pos,
				dims) {
	setRoundCorners(true);
	std::shared_ptr<Composite> &glassPanel =
			AlloyApplicationContext()->getGlassPane();
	fileDialog = std::shared_ptr<FileDialog>(
			new FileDialog("File Dialog",
					CoordPerPX(0.5, 0.5, -350 + 7.5f, -250 - 7.5f),
					CoordPX(700, 500), type));
	fileDialog->setVisible(false);
	glassPanel->add(fileDialog);
	if (type == FileDialogType::SaveFile) {
		fileDialog->onSelect = [this](const std::vector<std::string>& file) {
			if (onSave)onSave(file.front());
		};
	} else {
		fileDialog->onSelect = [this](const std::vector<std::string>& file) {
			if (onOpen)onOpen(file);
		};
	}

	foregroundColor = MakeColor(AlloyApplicationContext()->theme.LIGHTER);
	iconColor = MakeColor(AlloyApplicationContext()->theme.DARK);
	borderColor=MakeColor(0,0,0,0);
	borderWidth = UnitPX(2.0f);
	backgroundColor = MakeColor(COLOR_NONE);
	setRoundCorners(true);
	onMouseDown = [this](AlloyContext* context, const InputEvent& event) {
		if (event.button == GLFW_MOUSE_BUTTON_LEFT) {
			std::string file = getValue();
			AlloyApplicationContext()->setMouseFocusObject(nullptr);
			if (FileExists(file)) {
				openFileDialog(context, file);
			}
			else {
				std::string parent = GetParentDirectory(file);
				if (FileExists(parent)) {
					openFileDialog(context, parent);
				} else {
					openFileDialog(context, GetCurrentWorkingDirectory());
				}
			}
			return true;
		}
		return false;
	};
}
void FileButton::setValue(const std::string& file) {

	fileDialog->setValue(file);
}
void FileButton::openFileDialog(AlloyContext* context,
		const std::string& workingDirectory) {
	if (!fileDialog->isVisible()) {
		fileDialog->setVisible(true);
		context->getGlassPane()->setVisible(true);
	} else {
		fileDialog->setVisible(false);
		context->getGlassPane()->setVisible(false);
	}
	fileDialog->setValue(workingDirectory);
}
ListEntry::ListEntry(ListBox* listBox, const std::string& name,
		float entryHeight) :
		Region(name), dialog(listBox), entryHeight(entryHeight) {
	this->backgroundColor = MakeColor(AlloyApplicationContext()->theme.NEUTRAL);
	this->borderColor = MakeColor(COLOR_NONE);
	this->selected = false;
	iconCodeString = "";
	setLabel(name);
	this->onMouseDown = [this](AlloyContext* context, const InputEvent& e) {
		return dialog->onMouseDown(this, context, e);
	};
}
void ListEntry::setLabel(const std::string& label) {
	this->label = label;
	float th = entryHeight - 2 * TextField::PADDING;
	fontSize = UnitPX(th);
	AlloyContext* context = AlloyApplicationContext().get();
	NVGcontext* nvg = context->nvgContext;
	nvgFontSize(nvg, th);
	nvgTextAlign(nvg, NVG_ALIGN_MIDDLE | NVG_ALIGN_LEFT);
	nvgFontFaceId(nvg, context->getFontHandle(FontType::Bold));
	float tw = nvgTextBounds(nvg, 0, 0, label.c_str(), nullptr, nullptr);
	if (iconCodeString.size() > 0) {
		nvgFontFaceId(nvg, context->getFontHandle(FontType::Icon));
		tw += nvgTextBounds(nvg, 0, 0, iconCodeString.c_str(), nullptr, nullptr)
				+ 3 * context->theme.SPACING.x;
	} else {
		tw += 2 * context->theme.SPACING.x;
	}
	position = CoordPX(0.0f, 0.0f);
	dimensions = CoordPX(tw, entryHeight);
}
void ListEntry::setIcon(int icon) {
	this->iconCodeString = CodePointToUTF8(icon);
	float th = entryHeight - 2 * TextField::PADDING;
	fontSize = UnitPX(th);
	AlloyContext* context = AlloyApplicationContext().get();
	NVGcontext* nvg = context->nvgContext;
	nvgFontSize(nvg, th);
	nvgTextAlign(nvg, NVG_ALIGN_MIDDLE | NVG_ALIGN_LEFT);
	nvgFontFaceId(nvg, context->getFontHandle(FontType::Bold));
	float tw = nvgTextBounds(nvg, 0, 0, label.c_str(), nullptr, nullptr);
	if (iconCodeString.size() > 0) {
		nvgFontFaceId(nvg, context->getFontHandle(FontType::Icon));
		tw += nvgTextBounds(nvg, 0, 0, iconCodeString.c_str(), nullptr, nullptr)
				+ 3 * context->theme.SPACING.x;
	} else {
		tw += 2 * context->theme.SPACING.x;
	}
	position = CoordPX(0.0f, 0.0f);
	dimensions = CoordPX(tw, entryHeight);
}
bool ListBox::onMouseDown(ListEntry* entry, AlloyContext* context,
		const InputEvent& e) {
	if (e.isDown()) {
		if (e.button == GLFW_MOUSE_BUTTON_LEFT) {
			if (enableMultiSelection) {
				if (entry->isSelected() && e.clicks == 1) {
					entry->setSelected(false);
					for (auto iter = lastSelected.begin();
							iter != lastSelected.end(); iter++) {
						if (*iter == entry) {
							lastSelected.erase(iter);
							break;
						}
					}
				} else {
					entry->setSelected(true);
					lastSelected.push_back(entry);
				}
			} else {
				if (!entry->isSelected()) {
					for (ListEntry* child : lastSelected) {
						child->setSelected(false);
					}
					entry->setSelected(true);
					lastSelected.clear();
					lastSelected.push_back(entry);
				}
			}
			if (onSelect)
				onSelect(entry, e);
			return true;
		} else if (e.button == GLFW_MOUSE_BUTTON_RIGHT) {
			for (ListEntry* child : lastSelected) {
				child->setSelected(false);
			}
			if (onSelect)
				onSelect(nullptr, e);
			return true;
		}
	}
	return false;
}
FileEntry::FileEntry(FileDialog* dialog, const std::string& name,
		float fontHeight) :
		ListEntry(dialog->directoryList.get(), name, fontHeight), fileDescription() {
}
void FileEntry::setValue(const FileDescription& description) {
	this->fileDescription = description;
	iconCodeString =
			(fileDescription.fileType == FileType::Directory) ?
					CodePointToUTF8(0xf07b) : CodePointToUTF8(0xf15b);
	fileSize = FormatSize(fileDescription.fileSize);
	creationTime = FormatDateAndTime(fileDescription.creationTime);
	lastAccessTime = FormatDateAndTime(fileDescription.lastModifiedTime);
	lastModifiedTime = FormatDateAndTime(fileDescription.lastModifiedTime);
	setLabel(GetFileName(fileDescription.fileLocation));
}
void ListEntry::setSelected(bool selected) {
	this->selected = selected;
}
bool ListEntry::isSelected() {
	return selected;
}
void ListEntry::draw(AlloyContext* context) {
	box2px bounds = getBounds();
	NVGcontext* nvg = context->nvgContext;
	bool hover = context->isMouseOver(this);
	bool down = context->isMouseDown(this);
	bool selected = this->selected || dialog->isDraggingOver(this);
	int xoff = 0;
	int yoff = 0;
	if (down) {
		xoff = 2;
		yoff = 2;
	}
	if (hover || down) {
		nvgBeginPath(nvg);
		nvgRoundedRect(nvg, bounds.position.x + xoff, bounds.position.y + yoff,
				bounds.dimensions.x, bounds.dimensions.y,
				context->theme.CORNER_RADIUS);
		if (selected) {
			nvgFillColor(nvg, context->theme.LINK);
		} else {
			nvgFillColor(nvg, *backgroundColor);
		}
		nvgFill(nvg);
	} else {
		nvgBeginPath(nvg);
		nvgRoundedRect(nvg, bounds.position.x + 1, bounds.position.y + 1,
				bounds.dimensions.x - 2, bounds.dimensions.y - 2,
				context->theme.CORNER_RADIUS);
		if (selected) {
			nvgFillColor(nvg, context->theme.LINK);
		} else {
			nvgFillColor(nvg, Color(0,0,0,0));
		}
		nvgFill(nvg);
	}
	float th = fontSize.toPixels(bounds.dimensions.y, context->dpmm.y,
			context->pixelRatio);
	nvgFontSize(nvg, th);
	nvgTextAlign(nvg, NVG_ALIGN_MIDDLE | NVG_ALIGN_LEFT);
	nvgFontFaceId(nvg, context->getFontHandle(FontType::Icon));
	float iw =
			(iconCodeString.size() > 0) ?
					nvgTextBounds(nvg, 0, 0, iconCodeString.c_str(), nullptr,
							nullptr)
							+ AlloyApplicationContext()->theme.SPACING.x :
					0;
	pixel2 offset(0, 0);

	if (selected) {
		if (hover) {

			nvgFillColor(nvg, context->theme.LIGHTEST);
		} else {
			nvgFillColor(nvg, context->theme.LIGHTER);
		}
	} else {
		if (hover) {

			nvgFillColor(nvg, context->theme.LIGHTEST);
		} else {
			nvgFillColor(nvg, context->theme.DARK);
		}
	}

	box2px labelBounds = getCursorBounds();
	pushScissor(nvg, labelBounds);
	if (iconCodeString.size() > 0) {
		nvgFontFaceId(nvg, context->getFontHandle(FontType::Icon));
		nvgText(nvg,
				AlloyApplicationContext()->theme.SPACING.x + bounds.position.x
						+ xoff,
				bounds.position.y + bounds.dimensions.y / 2 + yoff,
				iconCodeString.c_str(), nullptr);
	}
	nvgFontFaceId(nvg, context->getFontHandle(FontType::Bold));
	nvgText(nvg,
			AlloyApplicationContext()->theme.SPACING.x + bounds.position.x + iw
					+ xoff, bounds.position.y + bounds.dimensions.y / 2 + yoff,
			label.c_str(), nullptr);
	popScissor(nvg);
}

void FileDialog::updateDirectoryList() {
	setSelectedFile(fileLocation->getValue());
}
bool FileDialog::updateValidity() {
	FileFilterRule* rule =
			(fileTypeSelect->getSelectedIndex() >= 0) ?
					filterRules[fileTypeSelect->getSelectedIndex()].get() :
					nullptr;
	if (type == FileDialogType::SaveFile) {
		std::string file = fileLocation->getValue();
		std::string fileName = GetFileName(file);
		if (fileName.size() > 0 && !IsDirectory(file)
				&& (rule == nullptr || rule->accept(file))) {
			valid = true;
			actionButton->backgroundColor = MakeColor(
					AlloyApplicationContext()->theme.LIGHTER);
		} else {
			actionButton->backgroundColor = MakeColor(
					AlloyApplicationContext()->theme.LIGHTER.toDarker(0.5f));
			valid = false;
		}
	} else if (type == FileDialogType::OpenFile) {
		std::string file = fileLocation->getValue();
		if (FileExists(file) && IsFile(file)
				&& (rule == nullptr || rule->accept(file))) {
			valid = true;
			actionButton->backgroundColor = MakeColor(
					AlloyApplicationContext()->theme.LIGHTER);
		} else {
			actionButton->backgroundColor = MakeColor(
					AlloyApplicationContext()->theme.LIGHTER.toDarker(0.5f));
			valid = false;
		}
	} else if (type == FileDialogType::OpenMultiFile) {
		valid = true;
		int count = 0;
		for (std::shared_ptr<ListEntry> entry : directoryList->getEntries()) {
			if (entry->isSelected()) {
				count++;
				std::string file =
						dynamic_cast<FileEntry*>(entry.get())->fileDescription.fileLocation;
				if (FileExists(file) && IsFile(file)
						&& (rule == nullptr || rule->accept(file))) {
				} else {
					valid = false;
					break;
				}
			}
		}
		valid &= (count > 0);
		if (valid) {
			actionButton->backgroundColor = MakeColor(
					AlloyApplicationContext()->theme.LIGHTER);
		} else {
			actionButton->backgroundColor = MakeColor(
					AlloyApplicationContext()->theme.LIGHTER.toDarker(0.5f));
		}
	}
	return valid;
}
void FileDialog::setSelectedFile(const std::string& file) {
	std::string dir;
	bool select = false;
	if (IsDirectory(file)) {
		dir = file;
	} else {
		dir = RemoveTrailingSlash(GetParentDirectory(file));
		select = true;
	}
	std::vector<FileDescription> descriptions = GetDirectoryDescriptionListing(
			dir);
	int i = 0;
	FileFilterRule* rule =
			(fileTypeSelect->getSelectedIndex() >= 0) ?
					filterRules[fileTypeSelect->getSelectedIndex()].get() :
					nullptr;

	if (!AlloyApplicationContext()->hasDeferredTasks()) {
		directoryList->clearEntries();
		//Fixes bug in padding out entry width.
		AlloyApplicationContext()->getGlassPane()->pack();
		for (FileDescription& fd : descriptions) {
			if (rule != nullptr && fd.fileType == FileType::File
					&& !rule->accept(fd.fileLocation)) {
				continue;
			}
			FileEntry* entry = new FileEntry(this,
					MakeString() << "Entry " << i, fileEntryHeight);
			directoryList->addEntry(std::shared_ptr<FileEntry>(entry));
			entry->setValue(fd);
			if (select && entry->fileDescription.fileLocation == file) {
				entry->setSelected(true);
			}
			i++;
		}
		updateValidity();
		directoryList->update();
	}
}

void ListBox::pack(const pixel2& pos, const pixel2& dims, const double2& dpmm,double pixelRatio, bool clamp){
	AlloyContext* context = AlloyApplicationContext().get();
	Region::pack(pos,dims, dpmm, pixelRatio, clamp);
	pixel2 maxDim = pixel2(this->getBoundsDimensionsX(), 0.0f);
	NVGcontext* nvg = context->nvgContext;
	box2px bounds = getBounds();

	for (std::shared_ptr<ListEntry> entry : listEntries) {
		float th = entry->fontSize.toPixels(bounds.dimensions.y, context->dpmm.y, context->pixelRatio);
		nvgFontSize(nvg, th);
		nvgFontFaceId(nvg, context->getFontHandle(FontType::Bold));
		float tw = nvgTextBounds(nvg, 0, 0, entry->getName().c_str(), nullptr, nullptr)+10;
		maxDim = aly::max(pixel2(tw,entry->entryHeight), maxDim);
	}
	for (std::shared_ptr<ListEntry> entry : listEntries) {
		entry->dimensions = CoordPX(maxDim);
	}
	Composite::pack(pos, dims, dpmm, pixelRatio, clamp);
}
void ListBox::update() {
	clear();
	lastSelected.clear();
	AlloyContext* context = AlloyApplicationContext().get();
	for (std::shared_ptr<ListEntry> entry : listEntries) {
		if (entry->parent == nullptr) {
			add(entry);
		}
		if (entry->isSelected()) {
			lastSelected.push_back(entry.get());
		}
	}
	context->requestPack();
}
ListBox::ListBox(const std::string& name, const AUnit2D& pos,
		const AUnit2D& dims) :
		Composite(name, pos, dims) {
	enableMultiSelection = false;
	scrollingDown = false;
	scrollingUp = false;
	backgroundColor = MakeColor(AlloyApplicationContext()->theme.LIGHTER);
	borderColor = MakeColor(AlloyApplicationContext()->theme.DARK);
	borderWidth = UnitPX(1.0f);
	setOrientation(Orientation::Vertical, pixel2(0, 2), pixel2(0, 2));
	setScrollEnabled(true);
	dragBox = box2px(float2(0, 0), float2(0, 0));
	onEvent =
			[this](AlloyContext* context, const InputEvent& e) {
				if (!context->isMouseOver(this, true))return false;
				if (e.type == InputType::Cursor || e.type == InputType::MouseButton) {
					if (context->isMouseDrag()) {
						if (enableMultiSelection) {
							float2 cursorDown = context->getCursorDownPosition();
							float2 stPt = aly::min(cursorDown, e.cursor);
							float2 endPt = aly::max(cursorDown, e.cursor);
							dragBox.position = stPt;
							dragBox.dimensions = endPt - stPt;
							dragBox.intersect(getBounds());
						}
					}
					else if (!context->isMouseDown() && e.type == InputType::MouseButton) {
						if (enableMultiSelection) {
							for (std::shared_ptr<ListEntry> entry : listEntries) {
								if (!entry->isSelected()) {
									if (dragBox.intersects(entry->getBounds())) {
										lastSelected.push_back(entry.get());
										entry->setSelected(true);
									}
								}
							}
						}
						dragBox = box2px(float2(0, 0), float2(0, 0));
					}
					else {
						dragBox = box2px(float2(0, 0), float2(0, 0));
					}
				}
				if (e.type == InputType::Cursor) {
					box2px bounds = this->getBounds();
					box2px lastBounds = bounds, firstBounds = bounds;
					float entryHeight = 30;
					lastBounds.position.y = bounds.position.y + bounds.dimensions.y - entryHeight;
					lastBounds.dimensions.y = entryHeight;
					firstBounds.dimensions.y = entryHeight;
					if ((!isHorizontalScrollVisible() && lastBounds.contains(e.cursor)) || (dragBox.dimensions.x*dragBox.dimensions.y > 0 && e.cursor.y > bounds.dimensions.y + bounds.position.y)) {
						if (downTimer.get() == nullptr) {
							downTimer = std::shared_ptr<TimerTask>(new TimerTask([this] {
												double deltaT = 200;
												scrollingDown = true;
												while (scrollingDown) {
													if (!addVerticalScrollPosition(10.0f))break;
													std::this_thread::sleep_for(std::chrono::milliseconds((long)deltaT));
													deltaT = std::max(30.0, 0.75*deltaT);
												}
											}, nullptr, 500, 30));
							downTimer->execute();
						}
					}
					else {
						if (downTimer.get() != nullptr) {
							scrollingDown = false;
							downTimer.reset();
						}
					}
					if (firstBounds.contains(e.cursor) || (dragBox.dimensions.x*dragBox.dimensions.y > 0 && e.cursor.y < bounds.position.y)) {
						if (upTimer.get() == nullptr) {
							upTimer = std::shared_ptr<TimerTask>(new TimerTask([this] {
												double deltaT = 200;
												scrollingUp = true;
												while (scrollingUp) {
													if (!addVerticalScrollPosition(-10.0f))break;
													std::this_thread::sleep_for(std::chrono::milliseconds((long)deltaT));
													deltaT = std::max(30.0, 0.75*deltaT);
												}
											}, nullptr, 500, 30));
							upTimer->execute();
						}
					}
					else {
						if (upTimer.get() != nullptr) {
							scrollingUp = false;
							upTimer.reset();
						}
					}
				}

				return false;
			};
}
void ListBox::draw(AlloyContext* context) {
	pushScissor(context->nvgContext, getCursorBounds());
	Composite::draw(context);
	popScissor(context->nvgContext);
	NVGcontext* nvg = context->nvgContext;
	if (dragBox.dimensions.x > 0 && dragBox.dimensions.y > 0) {
		nvgBeginPath(nvg);
		nvgRect(nvg, dragBox.position.x, dragBox.position.y,
				dragBox.dimensions.x, dragBox.dimensions.y);
		nvgFillColor(nvg, context->theme.DARK.toSemiTransparent(0.5f));
		nvgFill(nvg);

		nvgBeginPath(nvg);
		nvgRect(nvg, dragBox.position.x, dragBox.position.y,
				dragBox.dimensions.x, dragBox.dimensions.y);
		nvgStrokeWidth(nvg, 2.0f);
		nvgStrokeColor(nvg, context->theme.DARK);
		nvgStroke(nvg);
	}
}
bool ListBox::isDraggingOver(ListEntry* entry) {
	if (entry->isSelected() || dragBox.intersects(entry->getBounds())) {
		return true;
	} else {
		return false;
	}
}
FileDialog::FileDialog(const std::string& name, const AUnit2D& pos,
		const AUnit2D& dims, const FileDialogType& type, pixel fileEntryHeight) :
		Composite(name, pos, dims), type(type), fileEntryHeight(fileEntryHeight) {
	containerRegion = std::shared_ptr<BorderComposite>(
			new BorderComposite("Container", CoordPX(0, 15),
					CoordPerPX(1.0, 1.0, -15, -15)));
	actionButton = std::shared_ptr<TextIconButton>(
			new TextIconButton(
					(type == FileDialogType::SaveFile) ? "Save" : "Open",
					0xf115, CoordPerPX(1.0f, 0.0f, -10.0f, 5.0f),
					CoordPX(100, 30)));
	actionButton->onMouseDown =
			[this](AlloyContext* context, const InputEvent& event) {
				if (event.button == GLFW_MOUSE_BUTTON_LEFT) {
					if (valid) {
						if (this->onSelect)
						{
							std::vector<std::string> files;
							if (this->type != FileDialogType::OpenMultiFile) {
								files.push_back(this->getValue());
							}
							else {
								for (std::shared_ptr<ListEntry> entry : directoryList->getEntries()) {
									if (entry->isSelected()) {
										files.push_back(dynamic_cast<FileEntry*>(entry.get())->fileDescription.fileLocation);
									}
								}
							}
							if (files.size() > 0)this->onSelect(files);
						}
						this->setVisible(false);
						context->getGlassPane()->setVisible(false);
						return true;
					}
					else {
						return false;
					}
				}
				return false;
			};

	fileTypeSelect = std::shared_ptr<Selection>(
			new Selection("File Type", CoordPerPX(0.0f, 0.0f, 10.0f, 5.0f),
					CoordPerPX(1.0f, 0.0f, -125.0f, 30.0f)));

	std::shared_ptr<FileFilterRule> filterRule =
			std::shared_ptr<FileFilterRule>(new FileFilterRule("All Files"));
	filterRules.push_back(filterRule);
	fileTypeSelect->addSelection(filterRule->toString());

	fileTypeSelect->setSelectionIndex(0);
	fileTypeSelect->onSelect = [this](int index) {
		this->updateDirectoryList();
	};
	actionButton->setOrigin(Origin::TopRight);
	fileLocation = std::shared_ptr<FileField>(
			new FileField("File Location", CoordPX(10, 7),
					CoordPerPX(1.0f, 0.0f, -55.0f, 30.0f)));
	if (type == FileDialogType::SaveFile
			|| type == FileDialogType::OpenMultiFile) {
		fileLocation->setEnableAutoSugest(false);
	}
	fileLocation->backgroundColor = MakeColor(
			AlloyApplicationContext()->theme.LIGHT);

	fileLocation->onTextEntered = [this](TextField* field) {
		this->updateDirectoryList();
	};
	fileLocation->onKeyInput =
			[this](TextField* field) {
				if (this->type == FileDialogType::SaveFile || this->type == FileDialogType::OpenMultiFile) {
					this->updateDirectoryList();
				}
			};
	upDirButton = std::shared_ptr<IconButton>(
			new IconButton(0xf062, CoordPerPX(1.0, 0.0, -40, 7),
					CoordPX(30, 30)));
	upDirButton->foregroundColor = MakeColor(
			AlloyApplicationContext()->theme.LIGHTER);
	upDirButton->borderWidth=UnitPX(0.0f);
	upDirButton->backgroundColor = MakeColor(0,0,0,0);
	upDirButton->setRoundCorners(true);
	upDirButton->iconColor = MakeColor(
			AlloyApplicationContext()->theme.DARK);
	upDirButton->borderColor = MakeColor(
			AlloyApplicationContext()->theme.DARK);
	upDirButton->onMouseDown =
			[this](AlloyContext* context, const InputEvent& event) {
				std::string file=RemoveTrailingSlash(this->getValue());
				if(IsFile(file)) {
					this->setValue(GetParentDirectory(RemoveTrailingSlash(GetParentDirectory(file))));
				} else {
					this->setValue(GetParentDirectory(file));
				}
				return true;
			};
	cancelButton = std::shared_ptr<IconButton>(
			new IconButton(0xf00d, CoordPerPX(1.0, 0.0, -30, 30),
					CoordPX(30, 30), IconType::CIRCLE));
	cancelButton->setOrigin(Origin::BottomLeft);
	cancelButton->borderColor=MakeColor(AlloyApplicationContext()->theme.LIGHTEST);
	cancelButton->backgroundColor = MakeColor(COLOR_NONE);
	cancelButton->onMouseDown =
			[this](AlloyContext* context, const InputEvent& event) {
				this->setVisible(false);
				context->getGlassPane()->setVisible(false);
				return true;
			};
	CompositePtr southRegion = MakeComposite("File Options", CoordPX(0, 0),
			CoordPercent(1.0f, 1.0f));
	CompositePtr northRegion = MakeComposite("Selection Bar", CoordPX(0, 0),
			CoordPercent(1.0f, 1.0f));
	southRegion->add(actionButton);
	southRegion->add(fileTypeSelect);
	northRegion->add(fileLocation);
	northRegion->add(upDirButton);

	directoryTree = std::shared_ptr<Composite>(
			new Composite("Container", CoordPX(10, 0),
					CoordPerPX(1.0, 1.0, -10, 0)));

	TextIconButtonPtr homeDir = TextIconButtonPtr(
			new TextIconButton("Home", 0xf015, CoordPX(0.0f, 0.0f),
					CoordPerPX(1.0f, 0.0f, -2.0f, 30.0f),
					HorizontalAlignment::Left));
	TextIconButtonPtr docsDir = TextIconButtonPtr(
			new TextIconButton("Documents", 0xf115, CoordPX(0.0f, 0.0f),
					CoordPerPX(1.0f, 0.0f, -2.0f, 30.0f),
					HorizontalAlignment::Left));
	TextIconButtonPtr downloadDir = TextIconButtonPtr(
			new TextIconButton("Downloads", 0xf019, CoordPX(0.0f, 0.0f),
					CoordPerPX(1.0f, 0.0f, -2.0f, 30.0f),
					HorizontalAlignment::Left));
	TextIconButtonPtr desktopDir = TextIconButtonPtr(
			new TextIconButton("Desktop", 0xf108, CoordPX(0.0f, 0.0f),
					CoordPerPX(1.0f, 0.0f, -2.0f, 30.0f),
					HorizontalAlignment::Left));

	homeDir->onMouseDown = [this](AlloyContext* context, const InputEvent& e) {
		if (e.button == GLFW_MOUSE_BUTTON_LEFT) {
			this->setValue(GetHomeDirectory());
			return true;
		}
		return false;
	};
	directoryTree->add(homeDir);
	docsDir->onMouseDown = [this](AlloyContext* context, const InputEvent& e) {
		if (e.button == GLFW_MOUSE_BUTTON_LEFT) {
			this->setValue(GetDocumentsDirectory());
			return true;
		}
		return false;
	};
	directoryTree->add(docsDir);
	downloadDir->onMouseDown =
			[this](AlloyContext* context, const InputEvent& e) {
				if (e.button == GLFW_MOUSE_BUTTON_LEFT) {
					this->setValue(GetDownloadsDirectory());
					return true;
				}
				return false;
			};
	directoryTree->add(downloadDir);
	desktopDir->onMouseDown =
			[this](AlloyContext* context, const InputEvent& e) {
				if (e.button == GLFW_MOUSE_BUTTON_LEFT) {
					this->setValue(GetDesktopDirectory());
					return true;
				}
				return false;
			};
	directoryTree->add(desktopDir);

	std::vector<std::string> drives = GetDrives();
	for (std::string file : drives) {
		TextIconButtonPtr diskDir = TextIconButtonPtr(
				new TextIconButton(
						GetFileName(
								RemoveTrailingSlash(file)) + ALY_PATH_SEPARATOR,
						0xf0a0, CoordPX(0.0f, 0.0f),
						CoordPerPX(1.0f, 0.0f, -2.0f, 30.0f),
						HorizontalAlignment::Left));
		diskDir->onMouseDown =
				[this, file](AlloyContext* context, const InputEvent& e) {
					if (e.button == GLFW_MOUSE_BUTTON_LEFT) {
						this->setValue(file);
						return true;
					}
					return false;
				};
		directoryTree->add(diskDir);
	}
	directoryList = std::shared_ptr<ListBox>(
			new ListBox("Container", CoordPX(0, 0),
					CoordPerPX(1.0f, 1.0, -10.0f, 0.0f)));
	directoryList->setEnableMultiSelection(
			type == FileDialogType::OpenMultiFile);
	directoryList->onSelect =
			[this](ListEntry* lentry, const InputEvent& e) {
				if (e.clicks == 2) {
					actionButton->onMouseDown(AlloyApplicationContext().get(), e);
				}
				else {
					if (lentry != nullptr) {
						FileEntry* entry = dynamic_cast<FileEntry*>(lentry);
						if (this->type == FileDialogType::OpenMultiFile) {
							if (entry->fileDescription.fileType == FileType::Directory) {
								std::string fileName = entry->fileDescription.fileLocation;
								setSelectedFile(fileName);
								fileLocation->setValue(fileName);
							}
							updateValidity();

						}
						else if (this->type == FileDialogType::OpenFile) {
							std::string fileName = entry->fileDescription.fileLocation;
							setSelectedFile(fileName);
							fileLocation->setValue(fileName);
							updateValidity();
						}
						else if (this->type == FileDialogType::SaveFile) {
							if (entry->fileDescription.fileType == FileType::Directory) {
								std::string fileName = entry->fileDescription.fileLocation;
								setSelectedFile(fileName);
								fileLocation->setValue(fileName);
							}
							else {
								fileLocation->setValue(entry->fileDescription.fileLocation);
							}
							updateValidity();
						}
					}
					else {
						if (this->type != FileDialogType::OpenMultiFile) {
							ListEntry* entry = directoryList->getLastSelected();
							if (entry != nullptr) {
								fileLocation->setValue(
										GetParentDirectory(dynamic_cast<FileEntry*>(entry)->fileDescription.fileLocation));
							}
						}
						updateValidity();
					}
				}
			};
	directoryTree->borderColor = MakeColor(
			AlloyApplicationContext()->theme.DARK);
	directoryTree->borderWidth = UnitPX(1.0f);
	directoryTree->setOrientation(Orientation::Vertical);
	directoryTree->setScrollEnabled(true);

	containerRegion->setNorth(northRegion, UnitPX(40));
	containerRegion->setSouth(southRegion, UnitPX(40));
	containerRegion->setWest(directoryTree, UnitPX(140.0f));
	containerRegion->setCenter(directoryList);
	add(containerRegion);
	add(cancelButton);
}
std::string FileFilterRule::toString() {
	std::stringstream ss;
	if (extensions.size() == 0) {
		ss << name << " (*.*)";
		return ss.str();
	}
	ss << name << " (";
	int index = 0;
	for (std::string ext : extensions) {
		ss << "*." << ext;
		if (index < (int) extensions.size() - 1) {
			ss << ", ";
		}
		index++;
	}
	ss << ")";
	return ss.str();
}
bool FileFilterRule::accept(const std::string& file) {
	if (extensions.size() == 0)
		return true;
	std::string ext = GetFileExtension(file);
	for (char& c : ext) {
		c = std::tolower(c);
	}
	for (std::string extension : extensions) {
		if (ext == extension)
			return true;
	}
	return false;
}
void FileDialog::addFileExtensionRule(const std::string& name,
		const std::string& extension) {
	using extensions = std::initializer_list<std::string>;
	filterRules.push_back(
			std::shared_ptr<FileFilterRule>(
					new FileFilterRule(name, extensions { extension })));
	fileTypeSelect->addSelection(filterRules.back()->toString());
}
void FileDialog::addFileExtensionRule(const std::string& name,
		const std::initializer_list<std::string> & extension) {
	filterRules.push_back(
			std::shared_ptr<FileFilterRule>(
					new FileFilterRule(name, extension)));
	fileTypeSelect->addSelection(filterRules.back()->toString());
}

void FileDialog::setValue(const std::string& file) {
	fileLocation->setValue(file);
	setSelectedFile(file);
}
std::string FileDialog::getValue() const {
	return fileLocation->getValue();
}

void FileDialog::draw(AlloyContext* context) {
	NVGcontext* nvg = context->nvgContext;
	box2px bounds = containerRegion->getBounds();

	NVGpaint shadowPaint = nvgBoxGradient(nvg, bounds.position.x,
			bounds.position.y, bounds.dimensions.x, bounds.dimensions.y,
			context->theme.CORNER_RADIUS, 8, context->theme.DARKEST,
			context->theme.LIGHTEST.toSemiTransparent(0.0f));

	nvgBeginPath(nvg);
	nvgFillPaint(nvg, shadowPaint);

	nvgRoundedRect(nvg, bounds.position.x + 2, bounds.position.y + 2,
			bounds.dimensions.x + 2, bounds.dimensions.y + 2,
			context->theme.CORNER_RADIUS);
	nvgFill(nvg);

	nvgBeginPath(nvg);
	nvgRoundedRect(nvg, bounds.position.x, bounds.position.y,
			bounds.dimensions.x, bounds.dimensions.y,
			context->theme.CORNER_RADIUS);
	nvgFillColor(nvg, context->theme.DARK);
	nvgFill(nvg);

	nvgBeginPath(nvg);
	nvgRoundedRect(nvg, bounds.position.x, bounds.position.y,
			bounds.dimensions.x, bounds.dimensions.y,
			context->theme.CORNER_RADIUS);
	pixel lineWidth = borderWidth.toPixels(bounds.dimensions.y, context->dpmm.y,
			context->pixelRatio);
	nvgStrokeWidth(nvg, lineWidth);
	nvgStrokeColor(nvg, context->theme.LIGHT);
	nvgStroke(nvg);

	Composite::draw(context);

}

Graph::Graph(const std::string& name, const AUnit2D& pos, const AUnit2D& dims) :
		Region(name, pos, dims) {
	graphBounds = box2f(float2(0.0f), float2(-1.0f));
	backgroundColor = MakeColor(AlloyApplicationContext()->theme.DARK);
	setRoundCorners(true);
	xAxisLabel = "x";
	yAxisLabel = "y=f(x)";
	cursorPosition.x = -1;
	cursorPosition.y = -1;
	onEvent = [this](const AlloyContext* context, const InputEvent& e) {
		Graph* region = this;
		if (context->isMouseOver(region)) {
			cursorPosition = e.cursor;
		}
		else {
			cursorPosition = float2(-1, -1);
		}
		return false;
	};
	Application::addListener(this);
}
void Graph::draw(AlloyContext* context) {
	Region::draw(context);
	box2px rbounds = getBounds();
	NVGcontext* nvg = context->nvgContext;
	box2px gbounds = rbounds;
	const float LARGE_TEXT = 18.0f;
	const float MEDIUM_TEXT = 16.0f;
	const float SMALL_TEXT = 12.0f;
	float2 gpos(-1, -1);
	gbounds.position = pixel2(rbounds.position.x + GRAPH_PADDING,
			rbounds.position.y + GRAPH_PADDING);
	gbounds.dimensions = pixel2(rbounds.dimensions.x - GRAPH_PADDING * 2,
			rbounds.dimensions.y - GRAPH_PADDING * 2);
	if (graphBounds.dimensions.x < 0 || graphBounds.dimensions.y < 0) {
		updateGraphBounds();
	}
	nvgBeginPath(nvg);
	nvgRoundedRect(nvg, gbounds.position.x - 2, gbounds.position.y - 2,
			gbounds.dimensions.x + 4, gbounds.dimensions.y + 4,
			context->theme.CORNER_RADIUS);
	nvgFillColor(nvg, context->theme.LIGHTEST);
	nvgFill(nvg);
	//Draw vertical line for x=0
	if (graphBounds.position.x < 0
			&& graphBounds.position.x + graphBounds.dimensions.x > 0) {
		float xpos = -graphBounds.position.x / graphBounds.dimensions.x;
		nvgBeginPath(nvg);
		nvgMoveTo(nvg, xpos * gbounds.dimensions.x + gbounds.position.x,
				gbounds.position.y);
		nvgLineTo(nvg, xpos * gbounds.dimensions.x + gbounds.position.x,
				gbounds.position.y + gbounds.dimensions.y);
		nvgStrokeWidth(nvg, 2.0f);
		nvgStrokeColor(nvg, context->theme.DARK.toSemiTransparent(0.75f));
		nvgStroke(nvg);
	}
	//Draw horizontal line for y=0
	if (graphBounds.position.y < 0
			&& graphBounds.position.y + graphBounds.dimensions.y > 0) {
		float ypos = -graphBounds.position.y / graphBounds.dimensions.y;
		nvgBeginPath(nvg);
		nvgMoveTo(nvg, gbounds.position.x,
				ypos * gbounds.dimensions.y + gbounds.position.y);
		nvgLineTo(nvg, gbounds.position.x + gbounds.dimensions.x,
				ypos * gbounds.dimensions.y + gbounds.position.y);
		nvgStrokeWidth(nvg, 2.0f);
		nvgStrokeColor(nvg, context->theme.DARK.toSemiTransparent(0.75f));
		nvgStroke(nvg);
	}
	if (gbounds.contains(cursorPosition)) {
		context->setCursor(&Cursor::CrossHairs);
		gpos = (cursorPosition - gbounds.position) / gbounds.dimensions;
		gpos.y = 1 - gpos.y;
		gpos = gpos * graphBounds.dimensions + graphBounds.position;

		nvgBeginPath(nvg);
		nvgMoveTo(nvg, cursorPosition.x, gbounds.position.y);
		nvgLineTo(nvg, cursorPosition.x,
				gbounds.position.y + gbounds.dimensions.y);
		nvgStrokeWidth(nvg, 1.0f);
		nvgStrokeColor(nvg, context->theme.DARK.toSemiTransparent(0.25f));
		nvgStroke(nvg);

		nvgBeginPath(nvg);
		nvgMoveTo(nvg, gbounds.position.x, cursorPosition.y);
		nvgLineTo(nvg, gbounds.position.x + gbounds.dimensions.x,
				cursorPosition.y);
		nvgStrokeWidth(nvg, 1.0f);
		nvgStrokeColor(nvg, context->theme.DARK.toSemiTransparent(0.25f));
		nvgStroke(nvg);

	}
	for (GraphDataPtr& curve : curves) {
		std::vector<float2> points = curve->points;
		if (points.size() > 1 && graphBounds.dimensions.x > 0.0f
				&& graphBounds.dimensions.y > 0.0f) {
			NVGcontext* nvg = context->nvgContext;
			float2 last = points[0];
			last = (last - graphBounds.position) / graphBounds.dimensions;
			last.y = 1.0f - last.y;
			last = last * gbounds.dimensions + gbounds.position;
			nvgBeginPath(nvg);
			nvgMoveTo(nvg, last.x, last.y);
			for (int i = 1; i < (int) points.size(); i++) {
				float2 pt = points[i];
				pt = (pt - graphBounds.position) / graphBounds.dimensions;
				pt.y = 1.0f - pt.y;
				pt = pt * gbounds.dimensions + gbounds.position;
				nvgLineTo(nvg, pt.x, pt.y);
				last = pt;
			}
			nvgStrokeWidth(nvg, 2.0f);
			nvgStrokeColor(nvg, curve->color);
			nvgStroke(nvg);
		}
	}

	nvgFontFaceId(nvg, context->getFontHandle(FontType::Bold));
	nvgFontSize(nvg, LARGE_TEXT);
	nvgTextAlign(nvg, NVG_ALIGN_CENTER | NVG_ALIGN_TOP);
	drawText(nvg, rbounds.position + float2(rbounds.dimensions.x / 2, 2.0f),
			name, FontStyle::Outline, context->theme.LIGHTEST,
			context->theme.DARK);
	nvgFontSize(nvg, MEDIUM_TEXT);
	nvgFontFaceId(nvg, context->getFontHandle(FontType::Bold));
	nvgTextAlign(nvg, NVG_ALIGN_CENTER | NVG_ALIGN_BOTTOM);
	drawText(nvg,
			rbounds.position
					+ float2(rbounds.dimensions.x / 2,
							rbounds.dimensions.y - 4.0f), xAxisLabel,
			FontStyle::Outline, context->theme.LIGHTEST, context->theme.DARK);
	nvgTextAlign(nvg, NVG_ALIGN_CENTER | NVG_ALIGN_TOP);
	nvgSave(nvg);
	pixel2 center = rbounds.position
			+ float2(2.0f, rbounds.dimensions.y * 0.5f);
	nvgTranslate(nvg, center.x, center.y);
	nvgRotate(nvg, -ALY_PI * 0.5f);
	drawText(nvg, pixel2(0, 2), yAxisLabel, FontStyle::Outline,
			context->theme.LIGHTEST, context->theme.DARK);
	nvgRestore(nvg);
	nvgFontSize(nvg, SMALL_TEXT);
	nvgTextAlign(nvg, NVG_ALIGN_RIGHT | NVG_ALIGN_TOP);
	drawText(nvg, rbounds.position + float2(GRAPH_PADDING, GRAPH_PADDING),
			MakeString() << std::setprecision(2)
					<< (graphBounds.position.y + graphBounds.dimensions.y),
			FontStyle::Outline, context->theme.LIGHTER, context->theme.DARK);
	nvgTextAlign(nvg, NVG_ALIGN_RIGHT | NVG_ALIGN_BOTTOM);
	drawText(nvg,
			rbounds.position
					+ float2(GRAPH_PADDING,
							rbounds.dimensions.y - GRAPH_PADDING),
			MakeString() << std::setprecision(2) << graphBounds.position.y,
			FontStyle::Outline, context->theme.LIGHTER, context->theme.DARK);
	nvgTextAlign(nvg, NVG_ALIGN_RIGHT | NVG_ALIGN_TOP);
	drawText(nvg,
			rbounds.position
					+ float2(rbounds.dimensions.x - GRAPH_PADDING,
							rbounds.dimensions.y - GRAPH_PADDING + 2),
			MakeString() << std::setprecision(2)
					<< (graphBounds.position.x + graphBounds.dimensions.x),
			FontStyle::Outline, context->theme.LIGHTER, context->theme.DARK);
	nvgTextAlign(nvg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
	drawText(nvg,
			rbounds.position
					+ float2(GRAPH_PADDING,
							rbounds.dimensions.y - GRAPH_PADDING + 2),
			MakeString() << std::setprecision(2) << graphBounds.position.x,
			FontStyle::Outline, context->theme.LIGHTER, context->theme.DARK);

	if (cursorPosition.x >= 0) {
		float minDist = 1E30f;
		float bestY = 0;
		GraphDataPtr closestCurve;
		for (GraphDataPtr& curve : curves) {
			float y = curve->interpolate(gpos.x);
			if (y != GraphData::NO_INTERSECT) {
				if (std::abs(y - gpos.y) < minDist) {
					minDist = std::abs(y - gpos.y);
					bestY = y;
					closestCurve = curve;
				}
			}
		}
		if (closestCurve.get() != nullptr) {
			nvgBeginPath(nvg);
			nvgStrokeWidth(nvg, 2.0f);
			nvgFillColor(nvg, closestCurve->color);
			nvgStrokeColor(nvg, context->theme.LIGHTER);
			float2 pt(gpos.x, bestY);
			pt = (pt - graphBounds.position) / graphBounds.dimensions;
			pt.y = 1.0f - pt.y;
			pt = pt * gbounds.dimensions + gbounds.position;
			nvgCircle(nvg, pt.x, pt.y, 4);
			nvgFill(nvg);
			nvgStroke(nvg);

			nvgBeginPath(nvg);
			nvgFillColor(nvg, context->theme.DARK);
			nvgCircle(nvg, cursorPosition.x, cursorPosition.y, 2);
			nvgFill(nvg);

			nvgTextAlign(nvg, NVG_ALIGN_RIGHT | NVG_ALIGN_MIDDLE);
			nvgFontSize(nvg, MEDIUM_TEXT);
			drawText(nvg, float2(pt.x - 8, pt.y), closestCurve->name,
					FontStyle::Outline, context->theme.LIGHTEST,
					context->theme.DARK);
			nvgTextAlign(nvg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
			drawText(nvg, float2(pt.x + 8, pt.y),
					MakeString() << "(" << std::setprecision(2) << gpos.x
							<< ", " << std::setprecision(2) << bestY << ")",
					FontStyle::Outline, context->theme.LIGHTEST,
					context->theme.DARK);

		} else {

			nvgBeginPath(nvg);
			nvgFillColor(nvg, context->theme.DARK);
			nvgCircle(nvg, cursorPosition.x, cursorPosition.y, 2);
			nvgFill(nvg);
		}
	}
}
void Graph::add(const GraphDataPtr& curve) {
	curves.push_back(curve);
}
std::shared_ptr<GraphData> Graph::add(const GraphData& curve) {
	std::shared_ptr<GraphData> curvePtr = std::shared_ptr<GraphData>(
			new GraphData(curve));
	curves.push_back(curvePtr);
	return curvePtr;
}
box2f Graph::updateGraphBounds() {
	float2 minPt(std::numeric_limits<float>::max());
	float2 maxPt(std::numeric_limits<float>::min());
	for (GraphDataPtr& curve : curves) {
		for (float2& pt : curve->points) {
			minPt = aly::min(pt, minPt);
			maxPt = aly::max(pt, maxPt);
		}
	}
	graphBounds = box2f(minPt, maxPt - minPt);
	return graphBounds;
}
const float GraphData::NO_INTERSECT = std::numeric_limits<float>::max();
float GraphData::interpolate(float x) const {
	if (points.size() < 2) {
		return NO_INTERSECT;
	}
	if (x < points.front().x || x > points.back().x) {
		return NO_INTERSECT;
	}
	float y = 0;
	int startX = 0;
	int endX = (int) points.size() - 1;
	for (int i = 1; i < (int) points.size(); i++) {
		if (x < points[i].x) {
			startX = i - 1;
			break;
		}
	}
	for (int i = (int) points.size() - 2; i >= 0; i--) {
		if (x > points[i].x) {
			endX = i + 1;
			break;
		}
	}
	if (startX == endX) {
		return points[startX].y;
	} else if (endX - startX > 0) {
		float diff = (points[endX].x - points[startX].x);
		if (diff < 1E-10f) {
			return 0.5f * (points[endX].y + points[startX].y);
		} else {
			y = points[startX].y
					+ (x - points[startX].x)
							* (points[endX].y - points[startX].y) / diff;
			return y;
		}
	}
	return NO_INTERSECT;
}
void WindowPane::setMaximize(bool max) {
	maximized = max;
	if (this->maximized) {
		maximizeIcon->setIcon(0xf066);
	} else {
		maximizeIcon->setIcon(0xf065);
	}
}
void WindowPane::draw(AlloyContext* context) {
	AdjustableComposite::draw(context);
	if (context->getCursor() == nullptr) {
		if (context->isMouseOver(label.get())) {
			context->setCursor(&Cursor::Position);
		}
	}
}
bool WindowPane::onEventHandler(AlloyContext* context, const InputEvent& e) {
	if (dragging && e.type == InputType::Cursor && !isResizing()) {
		box2px pbounds = parent->getBounds();
		this->setDragOffset(pbounds.clamp(e.cursor), cursorDownPosition);
		setMaximize(false);
		context->requestPack();
	} else if (e.type == InputType::MouseButton && e.isUp()) {
		context->requestPack();
		dragging = false;
	}
	return AdjustableComposite::onEventHandler(context, e);
}

WindowPane::WindowPane(const RegionPtr& content) :
		AdjustableComposite(content->name, content->position,
				content->dimensions, true), maximized(
		false), dragging(false) {
	cellSpacing = pixel2(2, 2);
	cellPadding = pixel2(8, 8);
	titleRegion = CompositePtr(
			new Composite("Title", CoordPX(cellPadding.x, cellPadding.y),
					CoordPerPX(1.0f, 0.0f, -2.0f * cellPadding.x, 30.0f)));
	//titleRegion->backgroundColor = MakeColor(AlloyApplicationContext()->theme.LIGHT);
	label = TextLabelPtr(
			new TextLabel(content->name, CoordPX(0.0f, 0.0f),
					CoordPerPX(1.0f, 1.0f, 0.0f, 0.0f)));
	label->textColor = MakeColor(AlloyApplicationContext()->theme.LIGHTER);
	titleRegion->add(label);
	contentRegion = CompositePtr(
			new Composite("Content",
					CoordPX(cellPadding.x,
							30.0f + cellSpacing.y + cellPadding.y),
					CoordPerPX(1.0f, 1.0f, -2.0f * cellPadding.x,
							-30.0f - 2 * cellPadding.y - cellSpacing.y)));
	contentRegion->setScrollEnabled(true);
	contentRegion->add(content);
	content->position = CoordPX(0.0f, 0.0f);
	content->dimensions = CoordPercent(1.0f, 1.0f);
	backgroundColor = MakeColor(AlloyApplicationContext()->theme.DARK);
	setRoundCorners(true);
	Composite::add(titleRegion);
	Composite::add(contentRegion);
	this->setClampDragToParentBounds(false);
	label->onMouseDown = [this](AlloyContext* context, const InputEvent& e) {
		if (e.button == GLFW_MOUSE_BUTTON_LEFT&&!isResizing()) {
			cursorDownPosition = e.cursor - this->getBoundsPosition();
			dragging = true;
		}
		return false;
	};
	onResize = [this](AdjustableComposite* composite,const box2px& bounds) {
		setMaximize(false);
	};
	maximizeIcon = IconButtonPtr(
			new IconButton(0xf0fe, CoordPerPX(1.0f, 0.0f, -24.0f, 0.0f),
					CoordPX(24.0f, 24.0f)));
	maximizeIcon->borderWidth = UnitPX(0.0f);
	maximizeIcon->borderColor = MakeColor(COLOR_NONE);
	maximizeIcon->foregroundColor = MakeColor(COLOR_NONE);
	maximizeIcon->iconColor = MakeColor(
			AlloyApplicationContext()->theme.LIGHTER);
	titleRegion->add(maximizeIcon);
	maximizeIcon->onMouseDown =
			[this](AlloyContext* context, const InputEvent& event) {
				if (event.button == GLFW_MOUSE_BUTTON_LEFT) {
					setMaximize(!this->maximized);
					if (maximized) {
						windowInitialBounds = getBounds(false);
						this->setDragOffset(pixel2(0.0f, 0.0f));
						this->position = CoordPX(0.0f,0.0f);
						this->dimensions = CoordPercent(1.0f,1.0f);
						dynamic_cast<Composite*>(this->parent)->resetScrollPosition();
						dynamic_cast<Composite*>(this->parent)->putLast(this);
						context->requestPack();
					}
					else {
						this->position = CoordPX(windowInitialBounds.position);
						this->dimensions = CoordPX(windowInitialBounds.dimensions);
						context->requestPack();
					}
					return true;
				}
				return false;
			};
	setMaximize(false);
	Application::addListener(this);
}
MessageDialog::MessageDialog(const std::string& name, const AUnit2D& pos,
		const AUnit2D& dims, bool wrap, const MessageOption& option,
		const MessageType& type) :
		Composite(name, pos, dims), option(option), type(type) {
	setVisible(false);
	containerRegion = std::shared_ptr<Composite>(
			new Composite("Container", CoordPX(0, 15),
					CoordPerPX(1.0, 1.0, -15, -15)));

	TextButtonPtr actionButton = std::shared_ptr<TextButton>(
			new TextButton(
					(option == MessageOption::Okay
							|| option == MessageOption::OkayCancel) ?
							"Okay" : "Yes",
					CoordPerPX(0.5f, 1.0f,
							(option == MessageOption::Okay) ? 0.0f : -5.0f,
							-40.0f), CoordPX(100, 30)));
	actionButton->setAspectRule(AspectRule::Unspecified);
	actionButton->onMouseDown =
			[this](AlloyContext* context, const InputEvent& event) {
				this->returnValue=true;
				this->setVisible(false);
				if(onSelect) {
					onSelect(this);
				}
				return true;
			};
	containerRegion->add(actionButton);
	if (option == MessageOption::Okay) {
		actionButton->setOrigin(Origin::TopCenter);

	} else {
		actionButton->setOrigin(Origin::TopRight);

	}
	if (option == MessageOption::OkayCancel || option == MessageOption::YesNo) {
		TextButtonPtr inactionButton = std::shared_ptr<TextButton>(
				new TextButton(
						(option == MessageOption::OkayCancel) ? "Cancel" : "No",
						CoordPerPX(0.5f, 1.0f, 5.0f, -40.0f),
						CoordPX(100, 30)));
		inactionButton->setAspectRule(AspectRule::Unspecified);
		inactionButton->onMouseDown =
				[this](AlloyContext* context, const InputEvent& event) {
					this->returnValue=false;
					this->setVisible(false);
					if(onSelect) {
						onSelect(this);
					}
					return true;
				};
		containerRegion->add(inactionButton);

	}
	int code = 0;
	switch (type) {
	case MessageType::Error:
		code = 0xf056;
		break;
	case MessageType::Warning:
		code = 0xf06a;
		break;
	case MessageType::Information:
		code = 0xf05a;
		break;
	case MessageType::Question:
		code = 0xf059;
		break;
	default:
		code = 0;
	}
	GlyphRegionPtr glyphRegion = GlyphRegionPtr(
			new GlyphRegion("icon",
					AlloyApplicationContext()->createAwesomeGlyph(code,
							FontStyle::Normal, 50.0f),
					CoordPerPX(0.0f, 0.5f, 10.0f, (wrap) ? -50.0f : -40.0f),
					CoordPX(50.0f, 50.0f)));

	glyphRegion->setAspectRule(AspectRule::FixedHeight);
	glyphRegion->setOrigin(Origin::TopLeft);

	IconButtonPtr cancelButton = std::shared_ptr<IconButton>(
			new IconButton(0xf00d, CoordPerPX(1.0, 0.0, -30, 30),
					CoordPX(30, 30), IconType::CIRCLE));
	cancelButton->setOrigin(Origin::BottomLeft);
	cancelButton->backgroundColor = MakeColor(COLOR_NONE);
	cancelButton->borderColor=MakeColor(AlloyApplicationContext()->theme.LIGHTEST);
	cancelButton->onMouseDown =
			[this](AlloyContext* context, const InputEvent& event) {
				returnValue=false;
				this->setVisible(false);
				return true;
			};
	containerRegion->add(glyphRegion);
	if (wrap) {
		TextRegionPtr textLabel = TextRegionPtr(
				new TextRegion(name, CoordPerPX(0.0f, 0.5f, 60.0f, -50.0f),
						CoordPerPX(1.0f, 0.0f, -70.0f, 50.0f)));
		containerRegion->add(textLabel);
	} else {
		TextLabelPtr textLabel = TextLabelPtr(
				new TextLabel(name, CoordPerPX(0.0f, 0.5f, 60.0f, -40.0f),
						CoordPerPX(1.0f, 0.0f, -70.0f, 50.0f)));
		containerRegion->add(textLabel);
		textLabel->verticalAlignment = VerticalAlignment::Middle;
	}
	add(containerRegion);
	add(cancelButton);
}
void MessageDialog::draw(AlloyContext* context) {
	NVGcontext* nvg = context->nvgContext;
	box2px bounds = containerRegion->getBounds();
	NVGpaint shadowPaint = nvgBoxGradient(nvg, bounds.position.x,
			bounds.position.y, bounds.dimensions.x, bounds.dimensions.y,
			context->theme.CORNER_RADIUS, 8, context->theme.DARKEST,
			context->theme.LIGHTEST.toSemiTransparent(0.0f));

	nvgBeginPath(nvg);
	nvgFillPaint(nvg, shadowPaint);

	nvgRoundedRect(nvg, bounds.position.x + 2, bounds.position.y + 2,
			bounds.dimensions.x + 2, bounds.dimensions.y + 2,
			context->theme.CORNER_RADIUS);
	nvgFill(nvg);

	nvgBeginPath(nvg);
	nvgRoundedRect(nvg, bounds.position.x, bounds.position.y,
			bounds.dimensions.x, bounds.dimensions.y,
			context->theme.CORNER_RADIUS);
	nvgFillColor(nvg, context->theme.DARK);
	nvgFill(nvg);

	nvgBeginPath(nvg);
	nvgRoundedRect(nvg, bounds.position.x, bounds.position.y,
			bounds.dimensions.x, bounds.dimensions.y,
			context->theme.CORNER_RADIUS);
	pixel lineWidth = borderWidth.toPixels(bounds.dimensions.y, context->dpmm.y,
			context->pixelRatio);
	nvgStrokeWidth(nvg, lineWidth);
	nvgStrokeColor(nvg, context->theme.LIGHT);
	nvgStroke(nvg);

	Composite::draw(context);
}
void MessageDialog::setVisible(bool visible) {
	if (!Composite::isVisible()) {
		Composite::setVisible(true);
		AlloyApplicationContext()->getGlassPane()->setVisible(true);
	} else {
		Composite::setVisible(false);
		AlloyApplicationContext()->getGlassPane()->setVisible(false);
	}
}
MessageDialog::MessageDialog(const std::string& name, bool wrap,
		const MessageOption& option, const MessageType& type) :
		MessageDialog(name, CoordPerPX(0.5, 0.5, -200 + 7.5f, -100 - 7.5f),
				CoordPX(400, 200), wrap, option, type) {

}

MultiFileEntry::MultiFileEntry(ListBox* listBox, const std::string& name, float fontHeight) :ListEntry(listBox, name, fontHeight) {

}
void MultiFileEntry::setValue(const std::string& file) {
	this->fileName = file;
}
void MultiFileSelector::clearEntries() {
	valueRegion->clearEntries();
}
void MultiFileSelector::update() {
	for (ListEntryPtr entry : valueRegion->getEntries()) {
		entry->parent = nullptr;
	}
	valueRegion->update();
}
void MultiFileSelector::addFiles(const std::vector<std::string>& newFiles) {
	for (std::string file : newFiles) {
		MultiFileEntryPtr entry = MultiFileEntryPtr(new MultiFileEntry(valueRegion.get(), GetFileName(file), this->entryHeight));
		entry->setValue(file);
		valueRegion->addEntry(entry);
	}
	update();
	fireEvent();
}
void MultiFileSelector::fireEvent() {
	if (onChange) {
		std::vector<std::string> files;
		for (ListEntryPtr entry : valueRegion->getEntries()) {
			MultiFileEntryPtr newEntry = std::dynamic_pointer_cast<MultiFileEntry>(entry);
			files.push_back(newEntry->getValue());
		}
		onChange(files);
	}
}
MultiFileSelector::MultiFileSelector(const std::string& name, const AUnit2D& pos, const AUnit2D& dims,float entryHeight) : Composite(name, pos, dims) , entryHeight(entryHeight){
	valueRegion = ListBoxPtr(new ListBox(name, CoordPX(0.0f, 0.0f), CoordPerPX(1.0f,1.0f,-entryHeight-3.0f,0.0f)));
	RegionPtr bgRegion = RegionPtr(new Region(name, CoordPerPX(1.0f, 0.0f, -entryHeight - 3,0.0f), CoordPerPX(0.0f, 1.0f, 2.0f, 0.0f)));
	bgRegion->backgroundColor= MakeColor(AlloyDefaultContext()->theme.DARK);
	bgRegion->setRoundCorners(false);
	bgRegion->setIgnoreCursorEvents(true);
	openFileButton = FileButtonPtr(new FileButton("Open Multi-File", CoordPerPX(1.0f, 0.0f, -entryHeight+2.0f, 2.0f), CoordPX(entryHeight-4, entryHeight-4), FileDialogType::OpenMultiFile));
	upButton = IconButtonPtr(new IconButton(0xf0d8, CoordPerPX(1.0f, 0.0f, -entryHeight + 2.0f, entryHeight + 2.0f), CoordPX(entryHeight - 4, entryHeight - 4)));
	downButton = IconButtonPtr(new IconButton(0xf0d7, CoordPerPX(1.0f, 0.0f, -entryHeight + 2.0f, 2 * entryHeight + 2.0f), CoordPX(entryHeight - 4, entryHeight - 4)));
	eraseButton = IconButtonPtr(new IconButton(0xf00d, CoordPerPX(1.0f, 0.0f, -entryHeight + 2.0f, 3 * entryHeight + 2.0f), CoordPX(entryHeight - 4, entryHeight - 4)));
	valueRegion->setRoundCorners(true);
	valueRegion->borderWidth = UnitPX(0.0f);
	valueRegion->backgroundColor = MakeColor(0, 0, 0, 0);
	openFileButton->backgroundColor = MakeColor(0, 0, 0, 0);
	openFileButton->foregroundColor = MakeColor(0, 0, 0, 0);
	openFileButton->borderWidth = UnitPX(0.0f);
	openFileButton->iconColor = MakeColor(AlloyDefaultContext()->theme.DARK);
	upButton->backgroundColor = MakeColor(0, 0, 0, 0);
	upButton->foregroundColor = MakeColor(0, 0, 0, 0);
	upButton->iconColor = MakeColor(AlloyDefaultContext()->theme.DARK);
	upButton->borderWidth = UnitPX(0.0f);
	upButton->borderColor = MakeColor(AlloyDefaultContext()->theme.DARK);

	downButton->backgroundColor = MakeColor(0, 0, 0, 0);
	downButton->foregroundColor = MakeColor(0, 0, 0, 0);
	downButton->iconColor = MakeColor(AlloyDefaultContext()->theme.DARK);
	downButton->borderWidth = UnitPX(0.0f);
	downButton->borderColor = MakeColor(AlloyDefaultContext()->theme.DARK);

	eraseButton->backgroundColor = MakeColor(0, 0, 0, 0);
	eraseButton->foregroundColor = MakeColor(0, 0, 0, 0);
	eraseButton->iconColor = MakeColor(AlloyDefaultContext()->theme.DARK);
	eraseButton->borderWidth = UnitPX(0.0f);
	eraseButton->borderColor = MakeColor(AlloyDefaultContext()->theme.DARK);
	borderWidth = UnitPX(0.0f);
	backgroundColor = MakeColor(AlloyDefaultContext()->theme.LIGHTER);
	setRoundCorners(true);

	Composite::add(valueRegion);
	Composite::add(openFileButton);
	Composite::add(upButton);
	Composite::add(downButton);
	Composite::add(eraseButton);
	Composite::add(bgRegion);
	eraseButton->onMouseDown=[this](AlloyContext* context, const InputEvent& e) {
		if (e.button == GLFW_MOUSE_BUTTON_LEFT) {
			std::vector<ListEntryPtr>& entries = valueRegion->getEntries();
			bool removed = false;
			ListEntryPtr next;
			for (auto iter = entries.begin();iter != entries.end();iter++) {
				ListEntryPtr entry = *iter;
				if (entry->isSelected()) {
					if((iter + 1) != entries.end()) {
						next=(*(iter + 1));
					}
					entries.erase(iter);
					iter--;
					removed = true;
				}
			}
			if (next.get() != nullptr) {
				next->setSelected(true);
			}
			if (removed) {
				update();
				context->requestPack();
				fireEvent();
			}
		}
		return false;
	};
	upButton->onMouseDown = [this](AlloyContext* context, const InputEvent& e) {
		if (e.button == GLFW_MOUSE_BUTTON_LEFT) {
			std::vector<ListEntryPtr>& entries = valueRegion->getEntries();
			int N =(int) entries.size();
			for (int i = 1;i < N;i++) {
				if (entries[i]->isSelected()) {
					std::swap(entries[std::max(i-1 , 0)], entries[i]);
					update();
					context->requestPack();
					fireEvent();
					break;
				}
			}
			return true;
		}
		return false;
	};	
	downButton->onMouseDown = [this](AlloyContext* context, const InputEvent& e) {
		std::vector<ListEntryPtr>& entries = valueRegion->getEntries();
		int N = (int)entries.size();
		for (int i = 0;i < N-1;i++) {
			if (entries[i]->isSelected()) {
				std::swap(entries[std::min(i +1 ,N-1)], entries[i]);
				update();
				context->requestPack();
				fireEvent();
				break;
			}
		}
		return false;
	};
	openFileButton->onOpen = [this](const std::vector<std::string>& newFiles) {
		addFiles(newFiles);
	};
}
}

