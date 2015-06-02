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

#ifndef ALLOYUI_H_
#define ALLOYUI_H_
#include "AlloyMath.h"
#include "AlloyContext.h"
#include "AlloyUnits.h"
#include "nanovg.h"
#include  "GLTexture.h"
#include <iostream>
#include <memory>
#include <list>
#include <vector>
namespace aly {
bool SANITY_CHECK_UI();

struct Region {
protected:
	void drawBoundsLabel(AlloyContext* context, const std::string& name,
			int font);
	Region* mouseOverRegion = nullptr;
	Region* mouseDownRegion = nullptr;
	static uint64_t REGION_COUNTER;
	AUnit2D position = CoordPercent(0.0f, 0.0f);
	AUnit2D dimensions = CoordPercent(1.0f, 1.0f);
	bool visible = true;
	bool dragEnabled = false;
	pixel2 dragOffset = pixel2(0, 0);
	box2px bounds;
public:
	Origin origin = Origin::TopLeft;
	const std::string name;
	AColor backgroundColor = MakeColor(COLOR_NONE);
	AColor borderColor = MakeColor(COLOR_NONE);
	AUnit1D borderWidth = UnitPX(2);

	std::function<void()> onPack;
	std::function<void(AlloyContext* context, const InputEvent& event)> onMouseDown;
	std::function<void(AlloyContext* context, const InputEvent& event)> onMouseUp;
	std::function<void(AlloyContext* context, const InputEvent& event)> onMouseOver;
	std::function<
			void(AlloyContext* context, const InputEvent& event,
					const pixel2& mouseDownOffset)> onMouseDrag;
	void setDragOffset(const pixel2& cursor, const pixel2& delta);
	inline void setEnableDrag(bool enabled) {
		dragEnabled = enabled;
	}
	bool isDragEnabled() const {
		return dragEnabled;
	}
	AUnit2D& getPosition() {
		return position;
	}
	AUnit2D& getDimensions() {
		return dimensions;
	}
	box2px getBounds() const {
		return bounds;
	}
	pixel2 getBoundsPosition() const {
		return bounds.position;
	}
	pixel2 getBoundsDimensions() const {
		return bounds.dimensions;
	}
	pixel getBoundsPositionX() const {
		return bounds.position.x;
	}
	pixel getBoundsDimensionsX() const {
		return bounds.dimensions.x;
	}
	pixel getBoundsPositionY() const {
		return bounds.position.y;
	}
	pixel getBoundsDimensionsY() const {
		return bounds.dimensions.y;
	}
	const AUnit2D& getPosition() const {
		return position;
	}
	const AUnit2D& getDimensions() const {
		return dimensions;
	}
	void setPosition(const AUnit2D& pt);
	void setDimensions(const AUnit2D& dims);
	void setVisible(bool vis);
	AspectRatio aspectRatio = AspectRatio::Unspecified;
	double aspect = -1.0; //Less than zero indicates undetermined. Will be computed at next pack() event.
	Region* parent = nullptr;
	Region(
			const std::string& name = MakeString() << "r" << std::setw(8)
					<< std::setfill('0') << (REGION_COUNTER++));
	virtual void pack(const pixel2& pos, const pixel2& dims,
			const double2& dpmm, double pixelRatio);
	virtual void draw(AlloyContext* context);
	inline virtual void drawOnTop(AlloyContext* context){};
	virtual void update(CursorLocator* cursorLocator);
	virtual void drawDebug(AlloyContext* context);
	bool isVisible();
	virtual inline ~Region() {
	}
	;
};

struct Composite: public Region {
	std::vector<std::shared_ptr<Region>> children;
	Orientation orientation = Orientation::Unspecified;
	Composite(
			const std::string& name = MakeString() << "c" << std::setw(8)
					<< std::setfill('0') << (REGION_COUNTER++)) :
			Region(name) {
	}

	virtual void draw(AlloyContext* context) override;
	virtual void drawOnTop(AlloyContext* context) override;
	virtual void drawDebug(AlloyContext* context) override;
	virtual void update(CursorLocator* cursorLocator) override;
	void pack(const pixel2& pos, const pixel2& dims, const double2& dpmm,
			double pixelRatio) override;
	void pack(AlloyContext* context);
	virtual void add(const std::shared_ptr<Region>& region);
	void add(Region* region); //After add(), composite will own region and be responsible for destroying it.
	void pack();
	void draw();
};

struct GlyphRegion: public Region {
	AColor foregroundColor = MakeColor(COLOR_NONE);
	std::shared_ptr<Glyph> glyph;
	GlyphRegion(
			const std::string& name = MakeString() << "g" << std::setw(8)
					<< std::setfill('0') << (REGION_COUNTER++)) :
			Region(name) {
		aspectRatio = AspectRatio::FixedHeight;
	}
	;
	virtual void drawDebug(AlloyContext* context) override;
	void draw(AlloyContext* context);
};

struct TextLabel: public Region {
	HorizontalAlignment horizontalAlignment = HorizontalAlignment::Left;
	VerticalAlignment verticalAlignment = VerticalAlignment::Top;
	FontStyle fontStyle = FontStyle::Normal;
	FontType fontType = FontType::Normal;
	AUnit1D fontSize = UnitPX(14);
	AColor textColor = MakeColor(COLOR_WHITE);
	AColor textAltColor = MakeColor(COLOR_BLACK);
	std::string label;
	TextLabel(
			const std::string& name = MakeString() << "t" << std::setw(8)
					<< std::setfill('0') << (REGION_COUNTER++)) :
			Region(name), label(name) {
	}
	;
	void draw(AlloyContext* context);
};
std::shared_ptr<Composite> MakeComposite(const std::string& name,
		const AUnit2D& position, const AUnit2D& dimensions,
		const RGBA& bgColor = COLOR_NONE, const Orientation& orientation =
				Orientation::Unspecified);
std::shared_ptr<GlyphRegion> MakeGlyphRegion(
		const std::shared_ptr<ImageGlyph>& glyph, const AUnit2D& position,
		const AUnit2D& dimensions, const AspectRatio& aspectRatio =
				AspectRatio::Unspecified, const RGBA& bgColor = COLOR_NONE,
		const RGBA& fgColor = COLOR_NONE, const RGBA& borderColor = COLOR_NONE,
		const AUnit1D& borderWidth = UnitPX(2));
std::shared_ptr<GlyphRegion> MakeGlyphRegion(
		const std::shared_ptr<AwesomeGlyph>& glyph, const AUnit2D& position,
		const AUnit2D& dimensions, const RGBA& bgColor = COLOR_NONE,
		const RGBA& fgColor = COLOR_NONE, const RGBA& borderColor = COLOR_NONE,
		const AUnit1D& borderWidth = UnitPX(2));
std::shared_ptr<TextLabel> MakeTextLabel(const std::string& name,
		const AUnit2D& position, const AUnit2D& dimensions,
		const FontType& fontType, const AUnit1D& fontSize = UnitPT(14.0f),
		const RGBA& fontColor = COLOR_WHITE, const HorizontalAlignment& halign =
				HorizontalAlignment::Left, const VerticalAlignment& valign =
				VerticalAlignment::Top);
std::shared_ptr<Region> MakeRegion(const std::string& name,
		const AUnit2D& position, const AUnit2D& dimensions,
		const RGBA& bgColor = COLOR_NONE, const RGBA& lineColor = COLOR_WHITE,
		const AUnit1D& lineWidth = UnitPX(2.0f));
template<class C, class R> std::basic_ostream<C, R> & operator <<(
		std::basic_ostream<C, R> & ss, const Region & region) {
	ss << "Region: " << region.name << std::endl;
	ss << "\tOrigin: " << region.origin << std::endl;
	ss << "\tRelative Position: " << region.getPosition() << std::endl;
	ss << "\tRelative Dimensions: " << region.getDimensions() << std::endl;
	ss << "\tBounds: " << region.bounds << std::endl;
	ss << "\tAspect Ratio: " << region.aspectRatio << std::endl;
	ss << "\tBackground Color: " << region.backgroundColor << std::endl;
	if (region.parent != nullptr)
		ss << "\tParent: " << region.parent->name << std::endl;
	return ss;
}
template<class C, class R> std::basic_ostream<C, R> & operator <<(
		std::basic_ostream<C, R> & ss, const GlyphRegion & region) {
	ss << "Glyph Region: " << region.name << std::endl;
	if (region.glyph.get() != nullptr)
		ss << "\t" << *region.glyph << std::endl;
	ss << "\tOrigin: " << region.origin << std::endl;
	ss << "\tRelative Position: " << region.getPosition() << std::endl;
	ss << "\tRelative Dimensions: " << region.getDimensions() << std::endl;
	ss << "\tBounds: " << region.getBounds() << std::endl;
	ss << "\tAspect Ratio: " << region.aspectRatio << " (" << region.aspect
			<< ")" << std::endl;
	if (region.parent != nullptr)
		ss << "\tParent: " << region.parent->name << std::endl;
	return ss;
}
template<class C, class R> std::basic_ostream<C, R> & operator <<(
		std::basic_ostream<C, R> & ss, const TextLabel & region) {
	ss << "Text Label: " << region.name << std::endl;
	ss << "\tOrigin: " << region.origin << std::endl;
	ss << "\tHorizontal Alignment: " << region.horizontalAlignment << std::endl;
	ss << "\tVertical Alignment: " << region.verticalAlignment << std::endl;
	ss << "\tRelative Position: " << region.getPosition() << std::endl;
	ss << "\tRelative Dimensions: " << region.getDimensions() << std::endl;
	ss << "\tBounds: " << region.getBounds() << std::endl;
	ss << "\tFont Type: " << region.fontType << std::endl;
	ss << "\tFont Size: " << region.fontSize << std::endl;
	ss << "\tFont Color: " << region.textColor << std::endl;
	if (region.parent != nullptr)
		ss << "\tParent: " << region.parent->name << std::endl;
	return ss;
}

template<class C, class R> std::basic_ostream<C, R> & operator <<(
		std::basic_ostream<C, R> & ss, const Composite& region) {
	ss << "Composite: " << region.name << std::endl;
	ss << "\tOrigin: " << region.origin << std::endl;
	ss << "\tOrientation: " << region.orientation << std::endl;
	ss << "\tRelative Position: " << region.getPosition() << std::endl;
	ss << "\tRelative Dimensions: " << region.getDimensions() << std::endl;
	ss << "\tBackground Color: " << region.backgroundColor << std::endl;
	ss << "\tBounds: " << region.getBounds() << std::endl;
	int counter = 0;
	for (const std::shared_ptr<Region>& child : region.children) {
		ss << "\tChild[" << counter << "]: " << child->name << " "
				<< child->getBounds() << std::endl;
	}
	return ss;
}
typedef std::shared_ptr<TextLabel> TextLabelPtr;
typedef std::shared_ptr<Composite> CompositePtr;
typedef std::shared_ptr<GlyphRegion> GlyphRegionPtr;
typedef std::shared_ptr<Region> RegionPtr;
}
#endif /* ALLOYUI_H_ */
