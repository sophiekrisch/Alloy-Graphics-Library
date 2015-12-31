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
#include "AlloyTablePane.h"
#include "AlloyApplication.h"
#include "AlloyDrawUtil.h"
namespace aly {
	TableRow::TableRow(TablePane* tablePane, const std::string& name) :
		Composite(name,CoordPX(0.0f,0.0f),CoordPerPX(1.0f,0.0f,0.0f,tablePane->entryHeight)), tablePane(tablePane) {
		this->backgroundColor = MakeColor(AlloyApplicationContext()->theme.DARK);
		this->borderColor = MakeColor(COLOR_NONE);
		this->selected = false;
	}
	TableEntry::TableEntry(const std::string& name, const AUnit2D& pos, const AUnit2D& dims): Composite(name,pos,dims) {

	}

	void TableRow::pack(const pixel2& pos, const pixel2& dims, const double2& dpmm,double pixelRatio, bool clamp) {
		const int cols = tablePane->getColumns();
		pixel offset = 0.0f;
		if (selected) {
			backgroundColor=MakeColor(AlloyApplicationContext()->theme.LINK);
		}
		else {
			backgroundColor = MakeColor(AlloyApplicationContext()->theme.DARK);
		}
		for (std::pair<int, TableEntryPtr> pr : columns) {
			TableEntryPtr entry = pr.second;
			int col = pr.first;
			entry->position = CoordPX(offset, 0.0f);
			pixel w = tablePane->getColumnWidthPixels(col);
			entry->dimensions = CoordPerPX(0.0f,1.0f,w, 0.0f);
			offset += w;
		}
		Composite::pack(pos, dims, dpmm, pixelRatio, clamp);
	}
	std::shared_ptr<TableEntry> TableRow::getColumn(int i) const {
		if (columns.find(i) == columns.end()){
			return std::shared_ptr<TableEntry>();
		}
		else {
			return columns.at(i);
		}
	}
	int TableRow::compare(const std::shared_ptr<TableRow>& row, int column) {
		return getColumn(column)->compare(row->getColumn(column));
	}
	void TablePane::sort(int c) {
		int dir = sortDirections[c];
		if (dir == 0)dir = -1; else dir = -dir;
		for (int i = 0;i < columns;i++) {
			sortDirections[i] = 0;
			columnHeaders[i]->setIcon(0);
		}
		sortDirections[c] = dir;
		if (dir < 0) {
			columnHeaders[c]->setIcon(0xf0d7);
		}
		else if (dir>0) {
			columnHeaders[c]->setIcon(0xf0d8);
		}
		std::sort(rows.begin(), rows.end(), [this,c,dir](const TableRowPtr& a, const TableRowPtr& b) {
			return (a->compare(b, c)*dir>0);
		});
		update();
		AlloyApplicationContext()->requestPack();
	}
	void TableRow::setColumn(int c, const std::shared_ptr<TableEntry>& region) {
		if (columns.find(c) != columns.end()) {
			Composite::erase(columns[c]);
		}
		Composite::add(region);
		columns[c] = region;
	}
	void TablePane::pack(const pixel2& pos, const pixel2& dims, const double2& dpmm,
		double pixelRatio, bool clamp) {
		if (dirty) {
			update();
		}
		Region::pack(pos, dims, dpmm, pixelRatio, clamp);
		box2px bounds = contentRegion->getBounds();
		pixel offset = 0;
		pixel w;
		pixel sw = (contentRegion->isVerticalScrollVisible())?std::max(0.0f,bounds.dimensions.x - Composite::scrollBarSize):bounds.dimensions.x;
		for (int c = 0;c < columns;c++) {
			columnWidthPixels[c] =w= columnWidths[c].toPixels(sw, dpmm.x, pixelRatio);
			columnHeaders[c]->position = CoordPX(offset, 0.0f);
			columnHeaders[c]->dimensions = CoordPX(w, entryHeight);
			offset += w;
		}
		Composite::pack(pos, dims, dpmm, pixelRatio, clamp);
	}
	void TablePane::setColumnWidth(int c, const AUnit1D& unit) {
		columnWidths[c] = unit;
	}

	void TablePane::setColumnLabel(int c, const std::string& label) {
		columnHeaders[c]->setLabel(label);
	}
	AUnit1D TablePane::getColumnWidth(int c) const {
		return columnWidths[c];
	}
	pixel TablePane::getColumnWidthPixels(int c) const {
		return columnWidthPixels[c];
	}
	std::shared_ptr<TableRow> TablePane::addRow(const std::string& name) {
		TableRowPtr row=TableRowPtr(new TableRow(this, name));
		addRow(row);
		return row;
	}
	bool TablePane::onMouseDown(TableRow* entry, AlloyContext* context,
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
					}
					else {
						entry->setSelected(true);
						lastSelected.push_back(entry);
					}
				}
				else {
					if (!entry->isSelected()) {
						for (TableRow* child : lastSelected) {
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
			}
			else if (e.button == GLFW_MOUSE_BUTTON_RIGHT) {
				for (TableRow* child : lastSelected) {
					child->setSelected(false);
				}
				if (onSelect)
					onSelect(nullptr, e);
				return true;
			}
		}
		return false;
	}

	void TablePane::update() {
		contentRegion->clear();
		lastSelected.clear();
		AlloyContext* context = AlloyApplicationContext().get();
		for (std::shared_ptr<TableRow> entry : rows) {
			if (entry->parent == nullptr) {
				contentRegion->add(entry);
			}
			if (entry->isSelected()) {
				lastSelected.push_back(entry.get());
			}
		}

		dirty = false;
		context->requestPack();
	}
	bool TablePane::onEventHandler(AlloyContext* context, const InputEvent& e) {
		
		if (!context->isMouseOver(this, true))return false;
		if (e.type == InputType::MouseButton) {
			for (TableRowPtr row : rows) {
				if (context->isMouseDown(row.get(), true)) {
					onMouseDown(row.get(), context, e);
					break;
				}
			}
		}
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
					for (std::shared_ptr<TableRow> entry : rows) {
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
			if ((!contentRegion->isHorizontalScrollVisible() && lastBounds.contains(e.cursor)) || (dragBox.dimensions.x*dragBox.dimensions.y > 0 && e.cursor.y > bounds.dimensions.y + bounds.position.y)) {
				if (downTimer.get() == nullptr) {
					downTimer = std::shared_ptr<TimerTask>(new TimerTask([this] {
						double deltaT = 200;
						scrollingDown = true;
						while (scrollingDown) {
							if (!contentRegion->addVerticalScrollPosition(10.0f))break;
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
							if (!contentRegion->addVerticalScrollPosition(-10.0f))break;
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
	}
	TablePane::TablePane(const std::string& name, const AUnit2D& pos,
		const AUnit2D& dims,int columns,float entryHeight) :
		Composite(name, pos, dims),columns(columns),columnHeaders(columns), sortDirections(columns), columnWidths(columns,UnitPercent(1.0f/columns)),columnWidthPixels(columns,0.0f), entryHeight(entryHeight){
		dirty = false;
		enableMultiSelection = false;
		scrollingDown = false;
		scrollingUp = false;
		setRoundCorners(false);
		backgroundColor = MakeColor(AlloyApplicationContext()->theme.LIGHTER);
		borderColor = MakeColor(AlloyApplicationContext()->theme.DARK);
		borderWidth = UnitPX(1.0f);
		contentRegion = CompositePtr(new Composite(name,CoordPX(0.0f,entryHeight),CoordPerPX(1.0f,1.0f,0.0f,-entryHeight)));
		contentRegion->setRoundCorners(false);
		contentRegion->setOrientation(Orientation::Vertical, pixel2(0, 2), pixel2(0, 0));
		contentRegion->setScrollEnabled(true);
		contentRegion->setAlwaysShowVerticalScrollBar(true);
		contentRegion->borderWidth = UnitPX(1.0f);
		contentRegion->borderColor = MakeColor(AlloyApplicationContext()->theme.DARK);
		contentRegion->backgroundColor = MakeColor(AlloyApplicationContext()->theme.DARKER);
		dragBox = box2px(float2(0, 0), float2(0, 0));
		for (int c = 0;c < columns;c++) {
			TextIconButtonPtr textIcon = TextIconButtonPtr(new TextIconButton(MakeString()<<"Column "<<c, 0,CoordPX(2.0f,2.0f),CoordPerPX(1.0f,1.0f,-4.0f,entryHeight-4.0f),HorizontalAlignment::Center,IconAlignment::Right));
			textIcon->setRoundCorners(false);
			textIcon->borderWidth = UnitPX(0.0f);
			textIcon->backgroundColor = MakeColor(0, 0, 0, 0);
			textIcon->onMouseDown = [this,c](AlloyContext* context, const InputEvent& e) {
				if (e.button == GLFW_MOUSE_BUTTON_LEFT) {
					sort(c);
					return true;
				}
				return false;
			};
			columnHeaders[c] = textIcon;
			sortDirections[c] = 0;
			Composite::add(textIcon);
		}
		Composite::add(contentRegion);
		Application::addListener(this);
	}
	void TablePane::draw(AlloyContext* context) {
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
	bool TablePane::isDraggingOver(TableRow* entry) {
		if (entry->isSelected() || dragBox.intersects(entry->getBounds())) {
			return true;
		}
		else {
			return false;
		}
	}
	TableStringEntry::TableStringEntry(const std::string& name, const std::string& label,bool modifiable,const HorizontalAlignment& alignment) :TableEntry(name, CoordPX(0.0f, 0.0f), CoordPercent(1.0f, 1.0f)) {
		value = ModifiableLabelPtr(new ModifiableLabel(name, CoordPX(2.0f, 0.0f), CoordPerPX(1.0f, 1.0f, -4.0f, 0.0f), modifiable));
		value->backgroundColor = MakeColor(0, 0, 0, 0);
		value->borderColor = MakeColor(0, 0, 0, 0);
		value->borderWidth = UnitPX(0.0f);
		value->setAlignment(alignment, VerticalAlignment::Middle);
		value->setValue(label);
		Composite::add(value);
	}
	int TableStringEntry::compare(const std::shared_ptr<TableEntry>& entry) const {
		TableStringEntryPtr other = std::dynamic_pointer_cast<TableStringEntry>(entry);
		std::string a = getValue();
		std::string b = other->getValue();
		if (a == b)return 0;
		return (std::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end()))?-1:1;
	}

	TableNumberEntry::TableNumberEntry(const std::string& name, const Number& init,bool modifiable) :TableEntry(name, CoordPX(0.0f, 0.0f), CoordPercent(1.0f, 1.0f)) {
		value = ModifiableNumberPtr(new ModifiableNumber(name, CoordPX(2.0f, 0.0f), CoordPerPX(1.0f, 1.0f, -4.0f, 0.0f), init.type(),modifiable));
		value->backgroundColor = MakeColor(0, 0, 0, 0);
		value->borderColor = MakeColor(0, 0, 0, 0);
		value->borderWidth = UnitPX(0.0f);
		value->setAlignment(HorizontalAlignment::Center, VerticalAlignment::Middle);
		value->setNumberValue(init);
		Composite::add(value);
	}
	int TableNumberEntry::compare(const std::shared_ptr<TableEntry>& entry) const {
		TableNumberEntryPtr other = std::dynamic_pointer_cast<TableNumberEntry>(entry);
		return (int)aly::sign(getValue().toDouble()-other->getValue().toDouble());
	}

	TableCheckBoxEntry::TableCheckBoxEntry(const std::string& name, bool init) :TableEntry(name, CoordPX(0.0f, 0.0f), CoordPercent(1.0f, 1.0f)) {
		value = CheckBoxPtr(new CheckBox(name, CoordPercent(0.5f, 0.0f), CoordPercent(1.0f, 1.0f),init,false));
		value->setAspectRule(AspectRule::FixedHeight);
		value->setAspectRatio(1.0f);
		value->setOrigin(Origin::TopCenter);
		value->backgroundColor = MakeColor(0,0,0,0);
		value->borderColor = MakeColor(0, 0, 0, 0);
		value->borderWidth = UnitPX(0.0f);
		Composite::add(value);
	}
	int TableCheckBoxEntry::compare(const std::shared_ptr<TableEntry>& entry) const {
		TableCheckBoxEntryPtr other = std::dynamic_pointer_cast<TableCheckBoxEntry>(entry);
		bool a = getValue();
		bool b = other->getValue();
		return (((a)?1:0)-((b)?1:0));
	}
	TableToggleBoxEntry::TableToggleBoxEntry(const std::string& name, bool init) :TableEntry(name, CoordPX(0.0f, 0.0f), CoordPercent(1.0f, 1.0f)) {
		value = ToggleBoxPtr(new ToggleBox(name, CoordPercent(0.5f, 0.0f), CoordPercent(1.0f, 1.0f), init, false));
		value->setAspectRule(AspectRule::FixedHeight);
		value->setAspectRatio(2.0f);
		value->setOrigin(Origin::TopCenter);
		value->backgroundColor = MakeColor(0, 0, 0, 0);
		value->borderColor = MakeColor(0, 0, 0, 0);
		value->borderWidth = UnitPX(0.0f);
		Composite::add(value);
	}
	int TableToggleBoxEntry::compare(const std::shared_ptr<TableEntry>& entry) const {
		TableToggleBoxEntryPtr other = std::dynamic_pointer_cast<TableToggleBoxEntry>(entry);
		bool a = getValue();
		bool b = other->getValue();
		return (((a) ? 1 : 0) - ((b) ? 1 : 0));
	}
	TableSelectionEntry::TableSelectionEntry(const std::string& name, const std::vector<std::string>& options, int init) :TableEntry(name, CoordPX(0.0f, 0.0f), CoordPercent(1.0f, 1.0f)) {
		value = SelectionPtr(new Selection(name, CoordPX(0.0f, 0.0f), CoordPercent(1.0f, 1.0f), options));
		value->borderColor = MakeColor(0, 0, 0, 0);
		value->borderWidth = UnitPX(0.0f);
		value->setValue(init);
		Composite::add(value);
	}
	int TableSelectionEntry::compare(const std::shared_ptr<TableEntry>& entry) const {
		TableSelectionEntryPtr other = std::dynamic_pointer_cast<TableSelectionEntry>(entry);
		int a = getValue();
		int b = other->getValue();
		return (a-b);
	}

	TableColorEntry::TableColorEntry(const std::string& name,const Color& init) :TableEntry(name, CoordPX(0.0f, 0.0f), CoordPercent(1.0f, 1.0f)) {
		value = ColorSelectorPtr(new ColorSelector(name, CoordPercent(0.5f, 0.0f), CoordPercent(1.0f, 1.0f),false));
		value->setAspectRatio(1.0f);
		value->setAspectRule(AspectRule::FixedHeight);
		value->setOrigin(Origin::TopCenter);
		value->borderWidth = UnitPX(0.0f);
		value->setValue(init);
		Composite::add(value);
	}
	int TableColorEntry::compare(const std::shared_ptr<TableEntry>& entry) const {
		TableColorEntryPtr other = std::dynamic_pointer_cast<TableColorEntry>(entry);
		Color a = getValue();
		Color b = other->getValue();
		return (int)aly::sign((a.r+a.g+a.b)-(b.r+b.g+b.b));
	}
	TableProgressBarEntry::TableProgressBarEntry(const std::string& name, float progress) :TableEntry(name, CoordPX(0.0f, 0.0f), CoordPercent(1.0f, 1.0f)) {
		value = ProgressBarPtr(new ProgressBar(name, CoordPX(0.0f, 0.0f), CoordPercent(1.0f, 1.0f)));
		value->backgroundColor = MakeColor(0, 0, 0, 0);
		value->borderColor = MakeColor(0, 0, 0, 0);
		value->borderWidth = UnitPX(0.0f);

		value->setAspectRule(AspectRule::Unspecified);
		value->setValue(progress);
		Composite::add(value);
	}
	int TableProgressBarEntry::compare(const std::shared_ptr<TableEntry>& entry) const {
		TableProgressBarEntryPtr other = std::dynamic_pointer_cast<TableProgressBarEntry>(entry);
		float a = getValue();
		float b = other->getValue();
		return (int)aly::sign(a-b);
	}
	void TableRow::setSelected(bool selected) {
		this->selected = selected;
	}
	bool TableRow::isSelected() {
		return selected;
	}
}