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

class TextButton: public Region {
private:
	AColor textColor;
	AUnit1D fontSize;
	bool truncate;
public:
	void setTruncate(bool t) {
		truncate = t;
	}
	TextButton(const std::string& label, const AUnit2D& position,
			const AUnit2D& dimensions,bool truncate=true);
	virtual void draw(AlloyContext* context) override;
	virtual inline ~TextButton() {
	}
};

class TextIconButton: public Composite {
private:
	std::string iconCodeString;
	IconAlignment iconAlignment;
	HorizontalAlignment alignment;
	bool truncate;
public:
	AColor textColor;
	AUnit1D fontSize;
	void setTruncate(bool t) {
		truncate = t;
	}
	TextIconButton(const std::string& label, int iconCode,
			const AUnit2D& position, const AUnit2D& dimensions,
			const HorizontalAlignment& alignment = HorizontalAlignment::Center,
			const IconAlignment& iconAlignment = IconAlignment::Left,bool truncate=true);
	virtual void draw(AlloyContext* context) override;
	virtual inline ~TextIconButton() {
	}
};
enum class IconType {
	CIRCLE, SQUARE
};
class IconButton: public Composite {
private:
	std::string iconCodeString;
	IconType iconType;
	bool truncate;
public:
	AColor foregroundColor;
	AColor iconColor;
	void setTruncate(bool t) {
		truncate = t;
	}
	IconButton(int iconCode, const AUnit2D& position, const AUnit2D& dimensions,
			IconType iconType = IconType::SQUARE,bool truncate=true);
	void setIcon(int iconCode);
	virtual void draw(AlloyContext* context) override;
	virtual inline ~IconButton() {
	}
};
class CheckBox: public Composite {
private:
	TextLabelPtr checkLabel;
	TextLabelPtr valueLabel;
	bool checked;
	bool handleMouseDown(AlloyContext* context, const InputEvent& event);
public:
	std::function<void(bool)> onChange;
	inline bool getValue() {
		return checked;
	}
	void setValue(bool value);
	CheckBox(const std::string& label, const AUnit2D& position,
			const AUnit2D& dimensions, bool checked ,bool showText=true);
	virtual void draw(AlloyContext* context) override;
};
class ToggleBox: public Composite {
private:
	TextLabelPtr toggleLabel;
	TextLabelPtr onLabel;
	TextLabelPtr offLabel;
	CompositePtr clickRegion;
	bool toggledOn;
	bool handleMouseDown(AlloyContext* context, const InputEvent& e);
public:
	inline bool getValue() {
		return toggledOn;
	}
	std::function<void(bool)> onChange;

	void setValue(bool value);
	ToggleBox(const std::string& label, const AUnit2D& position,
			const AUnit2D& dimensions, bool toggledOn ,bool showText=true);
	virtual void draw(AlloyContext* context) override;
};
class SliderHandle: public Region {
public:
	SliderHandle(const std::string& name) :
			Region(name) {
	}
	virtual void draw(AlloyContext* context) override;
};
class SliderTrack: public Composite {
protected:
	const Orientation orientation;
public:
	Color startColor, endColor;
	SliderTrack(const std::string& name, Orientation orientColor,
			const Color& st, const Color& ed);
	virtual void draw(AlloyContext* context) override;
};
class ProgressBar: public Composite {
private:
	TextLabelPtr textLabel;
	float value;
	std::string label;
public:
	virtual void draw(AlloyContext* context) override;
	inline void setValue(float p) {
		value = clamp(p, 0.0f, 1.0f);
	}
	inline float getValue() const {
		return value;
	}
	inline std::string getLabel() const {
		return label;
	}
	inline void setValue(const std::string& l) {
		label = l;
	}
	inline void setValue(const std::string& l, float p) {
		label = l;
		value = clamp(p, 0.0f, 1.0f);
	}
	ProgressBar(const std::string& name, const AUnit2D& pt,
			const AUnit2D& dims);
};
class Slider: public Composite {
protected:
	AColor textColor;
	AUnit1D fontSize;
	Number minValue;
	Number maxValue;
	Number value;
	TextLabelPtr sliderLabel;
	TextLabelPtr valueLabel;
	std::shared_ptr<SliderHandle> sliderHandle;
	std::shared_ptr<SliderTrack> sliderTrack;
	std::function<std::string(const Number& value)> labelFormatter;
	std::function<void(const Number& value)> onChangeEvent;
	virtual void update()=0;
	double sliderPosition;
public:
	void setSliderColor(const Color& startColor, const Color& endColor) {
		sliderTrack->startColor = startColor;
		sliderTrack->endColor = endColor;
	}
	Slider(const std::string& name, const Number& min, const Number& max,
			const Number& val) :
			Composite(name), minValue(min), maxValue(max), value(val), sliderPosition(
					0.0) {
		labelFormatter = [](const Number& value) {return value.toString();};
	}
	Slider(const std::string& name, const AUnit2D& pos, const AUnit2D& dims,
			const Number& min, const Number& max, const Number& val) :
			Composite(name, pos, dims), minValue(min), maxValue(max), value(
					val), sliderPosition(0.0) {
		labelFormatter = [](const Number& value) {return value.toString();};
	}
	double getBlendValue() const;
	void setBlendValue(double value);
	virtual void setValue(double value)=0;
	inline void setValue(int value) {
		setValue((double) value);
	}
	inline void setValue(float value) {
		setValue((double) value);
	}
	const Number& getValue() {
		return value;
	}
	inline void setOnChangeEvent(
			const std::function<void(const Number& value)>& func) {
		onChangeEvent = func;
	}
	inline void setLabelFormatter(
			const std::function<std::string(const Number& value)>& func) {
		labelFormatter = func;
	}
};
class HorizontalSlider: public Slider {
protected:
	virtual void update() override;
public:
	virtual void setValue(double value) override;
	bool onMouseDown(AlloyContext* context, Region* region,
			const InputEvent& event);
	bool onMouseDrag(AlloyContext* context, Region* region,
			const InputEvent& event);
	HorizontalSlider(const std::string& label, const AUnit2D& position,
			const AUnit2D& dimensions, bool showLabel,
		const Number& minValue = Float(0.0f),
			const Number& maxValue = Float(1.0f),
			const Number& value = Float(0.0f));
	HorizontalSlider(const std::string& label, const AUnit2D& position,
		const AUnit2D& dimensions,
		const Number& minValue = Float(0.0f),
		const Number& maxValue = Float(1.0f),
		const Number& value = Float(0.0f)):HorizontalSlider(label,position,dimensions,true,minValue,maxValue,value) {

	}
	virtual void draw(AlloyContext* context) override;
	virtual inline ~HorizontalSlider() {
	}
};

class VerticalSlider: public Slider {
protected:
	virtual void update() override;
public:
	virtual void setValue(double value) override;
	bool onMouseDown(AlloyContext* context, Region* region,
			const InputEvent& event);
	bool onMouseDrag(AlloyContext* context, Region* region,
			const InputEvent& event);
	VerticalSlider(const std::string& label, const AUnit2D& position,
			const AUnit2D& dimensions, const Number& minValue = Float(0.0f),
			const Number& maxValue = Float(1.0f),
			const Number& value = Float(0.0f));
	virtual void draw(AlloyContext* context) override;
	virtual inline ~VerticalSlider() {
	}
};
class Selection: public Composite {
private:
	TextLabelPtr selectionLabel;
	TextLabelPtr arrowLabel;
	std::shared_ptr<SelectionBox> selectionBox;
	int selectedIndex;
	void hide(AlloyContext* context);
	void show(AlloyContext* context);
	bool handleMouseClick(AlloyContext* context, const InputEvent& event);
public:
	void setTextColor(const AColor& c);
	std::function<void(int)> onSelect;
	//bool onEventHandler(AlloyContext* context, const InputEvent& event)override;
	inline int getSelectedIndex() const {
		return selectedIndex;
	}
	void setMaxDisplayEntries(int mx) {
		selectionBox->setMaxDisplayEntries(mx);
	}
	std::string getValue() {
		return selectionBox->getSelection(selectedIndex);
	}
	std::string getSelection() {
		return selectionBox->getSelection(selectedIndex);
	}
	std::string getSelection(int index) {
		return selectionBox->getSelection(index);
	}
	void setValue(int selection) {
		selectedIndex = selection;
		selectionBox->setSelectedIndex(selection);
		selectionLabel->setLabel(this->getValue());
		if (onSelect)
			onSelect(selectedIndex);
	}
	void addSelection(const std::string& selection) {
		selectionBox->addSelection(selection);
	}
	void setSelectionIndex(int selection) {
		selectedIndex = selection;
		selectionBox->setSelectedIndex(selection);
		selectionLabel->setLabel(this->getValue());
		if (onSelect)
			onSelect(selectedIndex);
	}
	virtual void draw(AlloyContext* context) override;
	Selection(const std::string& label, const AUnit2D& position,
			const AUnit2D& dimensions, const std::vector<std::string>& options =
					std::vector<std::string>());
};

struct ColorWheel: public Composite {
protected:
	Color selectedColor;
	float2 tBounds[3];
	float2 tPoints[3];
	float rInner, rOuter;
	float2 center;
	HSVA hsvColor;
	bool triangleSelected = false;
	bool circleSelected = false;
	std::function<void(const Color& value)> onChangeEvent;
	void updateWheel();
public:
	inline void setOnChangeEvent(
			const std::function<void(const Color& value)>& func) {
		onChangeEvent = func;
	}
	void reset();
	Color getSelectedColor() const {
		return selectedColor;
	}
	void setColor(const Color& c);
	void setColor(const pixel2& cursor);
	ColorWheel(const std::string& name, const AUnit2D& pos,
			const AUnit2D& dims);
	void draw(AlloyContext* context) override;
};

class MessageDialog: public Composite {
protected:
	bool returnValue = false;
	MessageOption option;
	MessageType type;
	std::shared_ptr<Composite> containerRegion;
public:
	bool getValue() const {
		return returnValue;
	}
	void setVisible(bool visible);
	MessageDialog(const std::string& name, const AUnit2D& pos,
			const AUnit2D& dims, bool wrap, const MessageOption& option,
			const MessageType& type);
	MessageDialog(const std::string& name, bool wrap,
			const MessageOption& option, const MessageType& type);
	virtual void draw(AlloyContext* context) override;
	std::function<void(MessageDialog* dialog)> onSelect;
};
class ColorSelector: public Composite {
private:
	TextLabelPtr textLabel;
	GlyphRegionPtr colorLabel;
	CompositePtr colorSelectionPanel;
	std::shared_ptr<VerticalSlider> redSlider;
	std::shared_ptr<VerticalSlider> greenSlider;
	std::shared_ptr<VerticalSlider> blueSlider;
	std::shared_ptr<VerticalSlider> lumSlider;
	std::shared_ptr<VerticalSlider> alphaSlider;
	std::shared_ptr<ColorWheel> colorWheel;
	void updateColorSliders(const Color& c);
public:
	std::function<void(const Color& c)> onSelect;
	void setColor(const Color& color);
	Color getColor();
	inline void setValue(const Color& color) {
		setColor(color);
	}
	inline Color getValue() {
		return getColor();
	}
	ColorSelector(const std::string& name, const AUnit2D& pos,
			const AUnit2D& dims,bool showText=true);
	virtual void draw(AlloyContext* context) override;
};
class ExpandRegion: public Composite {
private:
	std::shared_ptr<TextLabel> selectionLabel;
	std::shared_ptr<TextLabel> arrowIcon;
	std::shared_ptr<Composite> contentRegion;
	std::shared_ptr<Composite> titleContainer;
	bool expanded;
public:
	pixel expandHeight;
	void setExpanded(bool expanded);
	ExpandRegion(const std::string& name,
			const std::shared_ptr<Composite>& region, const AUnit2D& pos,
			const AUnit2D& dims, pixel expandHeight, bool expanded);
	virtual void draw(AlloyContext* context) override;
};
class ExpandTree;
class TreeItem {
protected:
	static const int PADDING;
	std::string name;
	std::string iconCodeString;
	float fontSize = 24;

	box2px bounds;
	box2px selectionBounds;
	bool expanded;
	std::vector<std::shared_ptr<TreeItem>> children;
public:
	std::function<void(TreeItem* item)> onExpand;
	std::function<void(TreeItem* item)> onCollapse;
	std::function<void(TreeItem* item, const InputEvent& e)> onSelect;
	bool isExpanded() const {
		return expanded;
	}
	std::string getName() const {
		return name;
	}
	void clear() {
		children.clear();
	}
	bool hasChildren() const {
		return (children.size() != 0);
	}

	void setExpanded(bool expand);
	virtual void add(const std::shared_ptr<TreeItem>& item);
	virtual TreeItem* locate(AlloyContext* context, const pixel2& pt);
	TreeItem(const std::string& name = "", int iconCode = 0,
			float fontSize = 24);
	virtual box2px getBounds() const;
	virtual box2px update(AlloyContext* context,
			const pixel2& offset = pixel2(0.0f));
	virtual void draw(ExpandTree* tree, AlloyContext* context,
			const pixel2& offset);
	virtual ~TreeItem() {

	}
};
class LeafItem: public TreeItem {
protected:
	std::function<void(AlloyContext* context, const box2px& bounds)> onDraw;
public:
	LeafItem(
			const std::function<
					void(AlloyContext* context, const box2px& bounds)>& onDraw,
			const pixel2& dimensions);
	virtual box2px getBounds() const;
	virtual void add(const std::shared_ptr<TreeItem>& item) override {
		throw std::runtime_error("Cannot add child to leaf node.");
	}
	virtual box2px update(AlloyContext* context,
			const pixel2& offset = pixel2(0.0f)) override;
	virtual void draw(ExpandTree* tree, AlloyContext* context,
			const pixel2& offset) override;
	virtual TreeItem* locate(AlloyContext* context, const pixel2& pt) override;
	virtual ~LeafItem() {
	}
};
class ExpandTree: public Composite {
protected:
	TreeItem root;
	DrawPtr drawRegion;
	TreeItem* selectedItem;
	bool dirty;
public:
	TreeItem* getSelectedItem() const {
		return selectedItem;
	}
	void setDirty(bool d) {
		dirty = d;
	}
	void update(AlloyContext* context);
	ExpandTree(const std::string& name, const AUnit2D& pos,
			const AUnit2D& dims);
	void add(const std::shared_ptr<TreeItem>& item);
	void pack(const pixel2& pos, const pixel2& dims, const double2& dpmm,
			double pixelRatio, bool clamp);
	virtual void draw(AlloyContext* context) override;
	virtual void drawDebug(AlloyContext* context) override;
};
class ExpandBar: public Composite {
private:
	std::list<std::shared_ptr<ExpandRegion>> expandRegions;
	std::list<std::shared_ptr<Region>> contentRegions;
public:
	CompositePtr add(const std::shared_ptr<Region>&, pixel expandHeight,
			bool expanded);

	ExpandBar(const std::string& name, const AUnit2D& pos, const AUnit2D& dims);
};
class FileDialog;
class ListBox;
class ListEntry: public Region {
protected:
	std::string iconCodeString;
	std::string label;
	bool selected;
	ListBox* dialog;
	float entryHeight;
	AUnit1D fontSize;
public:
	friend class ListBox;
	void setSelected(bool selected);
	bool isSelected();
	void setLabel(const std::string& label);
	void setIcon(int icon);
	ListEntry(ListBox* listBox, const std::string& name, float entryHeight);
	virtual void draw(AlloyContext* context) override;
};
class FileEntry: public ListEntry {
private:
	std::string creationTime;
	std::string lastModifiedTime;
	std::string lastAccessTime;
	std::string fileSize;
public:
	FileDescription fileDescription;
	FileEntry(FileDialog* dialog, const std::string& name, float fontHeight);
	void setValue(const FileDescription& fileDescription);
};
struct FileFilterRule {
	std::string name;
	std::vector<std::string> extensions;
	FileFilterRule(const std::string& name, const std::string& extension) :
			name(name) {
		extensions.push_back(extension);
	}
	;
	FileFilterRule(const std::string& name) :
			name(name) {
	}
	;
	FileFilterRule(const std::string& name,
			std::initializer_list<std::string> extension) :
			name(name) {
		for (std::string ext : extension) {
			extensions.push_back(ext);
		}
	}
	std::string toString();
	virtual bool accept(const std::string& file);
	virtual ~FileFilterRule() {
	}
};
class ListBox: public Composite {
protected:
	bool enableMultiSelection;
	box2px dragBox;
	std::shared_ptr<TimerTask> downTimer, upTimer;
	bool scrollingDown;
	bool scrollingUp;
	std::vector<std::shared_ptr<ListEntry>> listEntries;
	std::list<ListEntry*> lastSelected;
	void addToActiveList(ListEntry* entry) {
		lastSelected.push_back(entry);
	}
	void clearActiveList() {
		lastSelected.clear();
	}
public:
	virtual void pack(const pixel2& pos, const pixel2& dims, const double2& dpmm,double pixelRatio, bool clamp) override;
	box2px getDragBox() const {
		return dragBox;
	}
	std::vector<std::shared_ptr<ListEntry>>& getEntries() {
		return listEntries;
	}
	void addEntry(const std::shared_ptr<ListEntry>& entry) {
		listEntries.push_back(entry);
	}
	void clearEntries() {
		listEntries.clear();
		lastSelected.clear();
	}
	void update();

	ListEntry* getLastSelected() {
		if (lastSelected.size() > 0)
			return lastSelected.back();
		else
			return nullptr;
	}
	bool isDraggingOver(ListEntry* entry);
	ListBox(const std::string& name, const AUnit2D& pos, const AUnit2D& dims);
	virtual void draw(AlloyContext* context) override;
	void setEnableMultiSelection(bool enable) {
		enableMultiSelection = enable;
	}
	bool onMouseDown(ListEntry* entry, AlloyContext* context,
			const InputEvent& e);
	std::function<void(ListEntry*, const InputEvent&)> onSelect;
};
class FileDialog: public Composite {
private:
	std::vector<std::shared_ptr<FileFilterRule>> filterRules;
	std::shared_ptr<FileField> fileLocation;
	std::shared_ptr<Composite> directoryTree;
	std::shared_ptr<ListBox> directoryList;
	std::shared_ptr<Selection> fileTypeSelect;
	std::shared_ptr<TextIconButton> actionButton;
	std::shared_ptr<IconButton> upDirButton;

	std::shared_ptr<IconButton> cancelButton;
	std::shared_ptr<BorderComposite> containerRegion;

	void setSelectedFile(const std::string& file);
	const FileDialogType type;
	pixel fileEntryHeight;
	bool valid = false;
	void updateDirectoryList();
	bool updateValidity();
public:

	void addFileExtensionRule(const std::string& name,
			const std::string& extension);
	void addFileExtensionRule(const std::string& name,
			const std::initializer_list<std::string> & extension);
	friend class FileEntry;
	std::function<void(const std::vector<std::string>&)> onSelect;
	virtual void draw(AlloyContext* context) override;
	FileDialog(const std::string& name, const AUnit2D& pos, const AUnit2D& dims,
			const FileDialogType& type, pixel fileEntryHeight = 30);

	void setValue(const std::string& file);
	std::string getValue() const;
	void setFileExtensionRule(int index) {
		fileTypeSelect->setValue(index);
		updateDirectoryList();
	}
};
class FileSelector: public BorderComposite {
private:
	std::shared_ptr<FileField> fileLocation;
	std::shared_ptr<IconButton> openIcon;
	std::shared_ptr<FileDialog> fileDialog;
public:
	void setIconColor(const AColor& c);
	std::function<void(const std::string& file)> onChange;
	void addFileExtensionRule(const std::string& name,
			const std::string& extension) {
		fileDialog->addFileExtensionRule(name, extension);
	}
	void addFileExtensionRule(const std::string& name,
			const std::initializer_list<std::string>& extension) {
		fileDialog->addFileExtensionRule(name, extension);
	}
	void setFileExtensionRule(int index) {
		fileDialog->setFileExtensionRule(index);
	}
	FileSelector(const std::string& name, const AUnit2D& pos,
			const AUnit2D& dims);
	void setValue(const std::string& file);
	std::string getValue() {
		return fileLocation->getValue();
	}
	void openFileDialog(AlloyContext* context,
			const std::string& workingDirectory = GetCurrentWorkingDirectory());
};
class FileButton: public IconButton {
private:
	std::shared_ptr<FileDialog> fileDialog;
public:

	std::function<void(const std::vector<std::string>& file)> onOpen;
	std::function<void(const std::string& file)> onSave;
	void addFileExtensionRule(const std::string& name,
			const std::string& extension) {
		fileDialog->addFileExtensionRule(name, extension);
	}
	void addFileExtensionRule(const std::string& name,
			const std::initializer_list<std::string>& extension) {
		fileDialog->addFileExtensionRule(name, extension);
	}
	void setFileExtensionRule(int index) {
		fileDialog->setFileExtensionRule(index);
	}
	FileButton(const std::string& name, const AUnit2D& pos, const AUnit2D& dims,
			const FileDialogType& type);
	void setValue(const std::string& file);
	std::string getValue() {
		return fileDialog->getValue();
	}
	void openFileDialog(AlloyContext* context,
			const std::string& workingDirectory = GetCurrentWorkingDirectory());
};
struct GraphData {
	std::string name;
	Color color;
	std::vector<float2> points;
	static const float NO_INTERSECT;
	GraphData(const std::string& name = "", Color color = Color(200, 64, 64)) :
			name(name), color(color) {

	}
	float interpolate(float x) const;
};
typedef std::shared_ptr<GraphData> GraphDataPtr;
class Graph: public Region {
protected:
	std::vector<GraphDataPtr> curves;
	box2f graphBounds;
	float2 cursorPosition;
	const float GRAPH_PADDING = 24.0f;
public:
	std::string xAxisLabel;
	std::string yAxisLabel;
	void add(const GraphDataPtr& curve);
	std::shared_ptr<GraphData> add(const GraphData& curve);
	void clear() {
		curves.clear();
	}
	box2f updateGraphBounds();
	void setGraphBounds(const box2f& r) {
		graphBounds = r;
	}
	Graph(const std::string& name, const AUnit2D& pos, const AUnit2D& dims);
	virtual void draw(AlloyContext* context) override;
};

class WindowPane: public AdjustableComposite {
protected:
	CompositePtr titleRegion;
	CompositePtr contentRegion;
	bool maximized;
	bool dragging;
	std::shared_ptr<IconButton> maximizeIcon;
	TextLabelPtr label;
public:
	void setMaximize(bool max);
	virtual bool onEventHandler(AlloyContext* context, const InputEvent& event)
			override;
	WindowPane(const RegionPtr& content);
	virtual void draw(AlloyContext* context) override;
};
class TabBar;
struct TabPane;
class TabHeader: public Composite{
protected:
	TextLabelPtr textLabel;
	TabPane* parentPane;
	bool focused;
public:
	friend struct TabPane;
	TabHeader(const std::string& name,TabPane* parent=nullptr);
	virtual void draw(AlloyContext* context) override;
	TabBar* getBar() const ;
	void setSelected() const ;
	bool isSelected() const;
	pixel2 getPreferredDimensions(AlloyContext* context) const;
};
struct TabPane {
	std::shared_ptr<TabHeader> header;
	std::shared_ptr<Composite> region;
	TabBar* parent;
	box2px bounds;
	bool isFocused() const {
		return header->focused;
	}
	TabPane(const std::shared_ptr<Composite>& region);
};
class TabBar: public Composite {
protected:
	std::vector<std::shared_ptr<TabPane>> panes;
	std::shared_ptr<Composite> barRegion;
	std::shared_ptr<Composite> contentRegion;
	std::shared_ptr<SelectionBox> selectionBox;
	std::shared_ptr<IconButton> tabDropButton;
	pixel2 cursorDownPosition;
	TabPane* selectedPane;
	TabPane* dragPane;
	void sortPanes();
public:
	static const float TAB_SPACING;
	static const float TAB_HEIGHT;
	std::function<bool(TabPane*)> onClose;
	void close(TabPane* pane);
	void setSelected(TabPane* s);
	bool isSelected(TabPane* s) const {
		return (s!=nullptr&&s == selectedPane);
	}
	void add(const std::shared_ptr<TabPane>& tabPane);
	TabBar(const std::string& name, const AUnit2D& position,
			const AUnit2D& dimensions);
	virtual bool onEventHandler(AlloyContext* context, const InputEvent& event)override;
	virtual void pack(const pixel2& pos, const pixel2& dims, const double2& dpmm,double pixelRatio, bool clamp = false) override;
	virtual void draw(AlloyContext* context) override;
};
class MultiFileEntry : public ListEntry {
private:
	std::string fileName;
public:
	MultiFileEntry(ListBox* listBox, const std::string& name, float fontHeight);
	void setValue(const std::string& file);
	std::string getValue() const {
		return fileName;
	}
};

class MultiFileSelector: public Composite{
protected:
	std::shared_ptr<ListBox> valueRegion;
	std::shared_ptr<FileButton> openFileButton;
	std::shared_ptr<IconButton> upButton;
	std::shared_ptr<IconButton> downButton;
	std::shared_ptr<IconButton> eraseButton;
	float entryHeight;
	void update();
	void fireEvent();
public:
	std::function<void(const std::vector<std::string>& files)> onChange;
	void addFileExtensionRule(const std::string& name,
		const std::string& extension) {
		openFileButton->addFileExtensionRule(name, extension);
	}
	void addFileExtensionRule(const std::string& name,
		const std::initializer_list<std::string>& extension) {
		openFileButton->addFileExtensionRule(name, extension);
	}
	void setFileExtensionRule(int index) {
		openFileButton->setFileExtensionRule(index);
	}
	void addFiles(const std::vector<std::string>& files);
	void clearEntries();
	MultiFileSelector(const std::string& name, const AUnit2D& pos, const AUnit2D& dims, float entryHeight=30.0f);
};
typedef std::shared_ptr<TextButton> TextButtonPtr;
typedef std::shared_ptr<HorizontalSlider> HSliderPtr;
typedef std::shared_ptr<VerticalSlider> VSliderPtr;
typedef std::shared_ptr<HorizontalSlider> HorizontalSliderPtr;
typedef std::shared_ptr<VerticalSlider> VerticalSliderPtr;
typedef std::shared_ptr<ColorSelector> ColorSelectorPtr;
typedef std::shared_ptr<CheckBox> CheckBoxPtr;
typedef std::shared_ptr<ToggleBox> ToggleBoxPtr;
typedef std::shared_ptr<Selection> SelectionPtr;
typedef std::shared_ptr<ProgressBar> ProgressBarPtr;
typedef std::shared_ptr<ColorWheel> ColorWheelPtr;
typedef std::shared_ptr<ExpandBar> ExpandBarPtr;
typedef std::shared_ptr<ExpandRegion> ExpandRegionPtr;
typedef std::shared_ptr<FileSelector> FileSelectorPtr;
typedef std::shared_ptr<FileDialog> FileDialogPtr;
typedef std::shared_ptr<FileButton> FileButtonPtr;
typedef std::shared_ptr<TextIconButton> TextIconButtonPtr;
typedef std::shared_ptr<IconButton> IconButtonPtr;
typedef std::shared_ptr<ListBox> ListBoxPtr;
typedef std::shared_ptr<ListEntry> ListEntryPtr;
typedef std::shared_ptr<Graph> GraphPtr;
typedef std::shared_ptr<WindowPane> WindowPanePtr;
typedef std::shared_ptr<MessageDialog> MessageDialogPtr;
typedef std::shared_ptr<TreeItem> TreeItemPtr;
typedef std::shared_ptr<LeafItem> LeafItemPtr;
typedef std::shared_ptr<ExpandTree> ExpandTreePtr;
typedef std::shared_ptr<TabHeader> TabHeaderPtr;
typedef std::shared_ptr<TabPane> TabPanePtr;
typedef std::shared_ptr<TabBar> TabBarPtr;
typedef std::shared_ptr<MultiFileEntry> MultiFileEntryPtr;
typedef std::shared_ptr<MultiFileSelector>  MultiFileSelectorPtr;
}

#endif /* ALLOYWIDGET_H_ */
