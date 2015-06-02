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

#ifndef ALLOYWIDGET_H_
#define ALLOYWIDGET_H_

#include "AlloyUI.h"
namespace aly {
class Widget: public Composite {
public:
	Widget(const std::string& name = "") :
			Composite(name) {
	}
	virtual void onMouseOver(AlloyContext* context, Region* region,
			const InputEvent& event) {
	}
	virtual void onMouseDown(AlloyContext* context, Region* region,
			const InputEvent& event) {
	}
	virtual void onMouseUp(AlloyContext* context, Region* region,
			const InputEvent& event) {
	}
	virtual void onMouseDrag(AlloyContext* context, Region* region,
			const InputEvent& event, const pixel2& lastDragPosition) {
	}
	virtual void add(const std::shared_ptr<Region>& region) override;
};
class Button: public Widget {
private:
	AColor textColor;
	AUnit1D fontSize;
	void internalDraw(AlloyContext* context);
public:
	Button(const std::string& label, const AUnit2D& position,
			const AUnit2D& dimensions);
	virtual void draw(AlloyContext* context) override;
	virtual void drawOnTop(AlloyContext* context) override;
	virtual inline ~Button() {
	}
	;
};
class CheckBox: public Widget {
private:
	TextLabelPtr checkLabel;
	GlyphRegionPtr valueLabel;
	bool checked;
public:
	inline bool getValue(){return checked;}
	CheckBox(const std::string& label, const AUnit2D& position,const AUnit2D& dimensions,bool checked=false);
	virtual void draw(AlloyContext* context) override;
};
class ScrollHandle: public Region {
public:
	ScrollHandle(const std::string& name) :
			Region(name) {
	}
	virtual void draw(AlloyContext* context) override;
};
class ScrollTrack: public Composite {
public:
	ScrollTrack(const std::string& name) :
			Composite(name) {
	}
	virtual void draw(AlloyContext* context) override;
};
class HorizontalSlider: public Widget {
private:
	AColor textColor;
	AUnit1D fontSize;
	Number value;
	Number minValue;
	Number maxValue;

	TextLabelPtr sliderLabel;
	TextLabelPtr valueLabel;
	std::shared_ptr<ScrollHandle> scrollHandle;
	std::shared_ptr<ScrollTrack> scrollTrack;
	std::function<std::string(const Number& value)> labelFormatter =
			[](const Number& value) {return value.toString();};
	void update();
public:
	void setValue(double value);
	inline void setValue(int value) {
		setValue((double) value);
	}
	inline void setValue(float value) {
		setValue((double) value);
	}
	const Number& getValue(){
		return value;
	}
	virtual void onMouseDown(AlloyContext* context, Region* region,
			const InputEvent& event) override;
	virtual void onMouseUp(AlloyContext* context, Region* region,
			const InputEvent& event) override;
	virtual void onMouseDrag(AlloyContext* context, Region* region,
			const InputEvent& event, const pixel2& lastDragPosition) override;
	inline void setLabelFormatter(
			const std::function<const std::string&(const Number& value)>& func) {
		labelFormatter = func;
	}
	HorizontalSlider(const std::string& label, const AUnit2D& position,
			const AUnit2D& dimensions, const Number& minValue = Float(0.0f),
			const Number& maxValue = Float(1.0f),
			const Number& value = Float(0.0f));
	virtual void draw(AlloyContext* context) override;
	virtual inline ~HorizontalSlider() {
	}
	;
};

typedef std::shared_ptr<Button> ButtonPtr;
typedef std::shared_ptr<HorizontalSlider> HSliderPtr;
typedef std::shared_ptr<CheckBox> CheckBoxPtr;
}

#endif /* ALLOYWIDGET_H_ */
