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
#include "AlloyParameterPane.h"
#include "AlloyFileUtil.h"
#include "AlloyColorSelector.h"
namespace aly {
	const float ParameterPane::SPACING = 1.0f;
	void ParameterPane::pack(const pixel2& pos, const pixel2& dims, const double2& dpmm, double pixelRatio, bool clamp) {
		updateGroups();
		Composite::pack(pos, dims, dpmm, pixelRatio, clamp);
	}
	void ParameterPane::updateGroups() {
		if(groupQueue.size()>0) {
			if (lastRegion.get() != nullptr) {
				for (RegionPtr region : groupQueue) {
					lastRegion->add(region);
					region->backgroundColor = MakeColor(AlloyDefaultContext()->theme.DARKER);
				}
				groupQueue.clear();
				CompositePtr eregion=expandBar->add(lastRegion, estimatedHeight-SPACING, lastExpanded);
				eregion->setRoundCorners(false);
				lastRegion->setOrientation(Orientation::Vertical, pixel2(0.0f, SPACING), pixel2(0.0f));
				lastRegion->backgroundColor = MakeColor(AlloyDefaultContext()->theme.DARKEST);
				lastRegion->borderColor= MakeColor(AlloyDefaultContext()->theme.DARK);
				lastRegion->setRoundCorners(false);
				estimatedHeight = 0;
			}
			else {
				CompositePtr comp = CompositePtr(new Composite("Parameters", CoordPX(0.0f, 0.0f), CoordPerPX(1.0f, 1.0f, -Composite::scrollBarSize, 0.0f)));
				for(RegionPtr region : groupQueue) {
					comp->add(region);
				}
				Composite::add(comp);
				comp->setOrientation(Orientation::Vertical, pixel2(0.0f, SPACING), pixel2(0.0f));
				setScrollEnabled(true);
				setAlwaysShowVerticalScrollBar(true);
				groupQueue.clear();
			}
		}
	}
	void ParameterPane::addGroup(const std::string& name,bool expanded) {
		if (lastRegion.get() != nullptr&&groupQueue.size() > 0) {
			for (RegionPtr region : groupQueue) {
				lastRegion->add(region);
				region->backgroundColor = MakeColor(AlloyDefaultContext()->theme.DARKER);
			}
			groupQueue.clear();
			CompositePtr eregion = expandBar->add(lastRegion, estimatedHeight - SPACING,lastExpanded);
			eregion->setRoundCorners(false);
			lastRegion->setOrientation(Orientation::Vertical, pixel2(0.0f, SPACING), pixel2(0.0f));
			lastRegion->backgroundColor = MakeColor(AlloyDefaultContext()->theme.DARKEST);
			lastRegion->borderColor = MakeColor(AlloyDefaultContext()->theme.DARK);
			lastRegion->setRoundCorners(false);
			estimatedHeight = 0;
		}
		lastRegion = CompositePtr(new Composite(name, CoordPX(0.0f,0.0f),CoordPercent(1.0f,1.0f)));
		lastExpanded = expanded;
		if (expandBar.get() == nullptr) {
			expandBar = ExpandBarPtr(new ExpandBar("Parameters", CoordPX(0.0f, 0.0f), CoordPercent(1.0f, 1.0f)));
			Composite::add(expandBar);
		}
	}
	ParameterPane::ParameterPane(const std::string& name, const AUnit2D& pos, const AUnit2D& dim, float entryHeight):Composite(name,pos,dim),entryHeight(entryHeight) {
		backgroundColor = MakeColor(AlloyDefaultContext()->theme.DARKER);
		entryBackgroundColor= MakeColor(AlloyDefaultContext()->theme.DARK);
		entryBorderColor= MakeColor(AlloyDefaultContext()->theme.LIGHT);
		entryTextColor = MakeColor(AlloyDefaultContext()->theme.LIGHTER);
		entryBorderWidth = UnitPX(0.0f);
	}
	void ParameterPane::setCommonParameters(const CompositePtr& compRegion,const TextLabelPtr& textLabel, const RegionPtr& valueRegion) {
		valueRegion->setRoundCorners(true);
		compRegion->backgroundColor = entryBackgroundColor;
		compRegion->borderColor = entryBorderColor;
		compRegion->borderWidth = entryBorderWidth;
		valueRegion->backgroundColor = MakeColor(0,0,0,0);
		valueRegion->borderWidth = UnitPX(0.0f);
		textLabel->textColor = entryTextColor;
		textLabel->fontSize = UnitPX(entryHeight - 2);
		compRegion->add(textLabel);
		compRegion->add(valueRegion);
		groupQueue.push_back(compRegion);
	}
	void ParameterPane::addNumberField(const std::string& label, Number& value,float aspect) {
		CompositePtr comp = CompositePtr(new Composite(label+"_param",CoordPX(0,0),CoordPerPX(1.0f,0.0f,0.0f,entryHeight)));
		TextLabelPtr labelRegion = TextLabelPtr(new TextLabel(label, CoordPX(0.0f, 0.0f), CoordPerPX(1.0f, 0.0f, 0.0f, entryHeight)));
		NumberFieldPtr valueRegion = NumberFieldPtr(new NumberField(label, CoordPerPX(1.0f, 0.0f, -aspect*entryHeight, 0.0f), CoordPX(aspect*entryHeight, entryHeight), value.type()));
		if (aspect <= 0) {
			pixel2 labelBounds = labelRegion->getTextDimensions(AlloyDefaultContext().get());
			labelBounds.x += 10;
			valueRegion->position = CoordPX(labelBounds.x, 0.0f);
			valueRegion->dimensions = CoordPerPX(1.0f, 0.0f, -labelBounds.x, entryHeight);
		}
		else {
			labelRegion->position = CoordPX(0.0f, 0.0f);
			labelRegion->dimensions = CoordPerPX(1.0f, 0.0f, -aspect*entryHeight, entryHeight);
		}
		valueRegion->textColor = MakeColor(AlloyDefaultContext()->theme.DARKER);
		valueRegion->setNumberValue(value);

		std::shared_ptr<AnyInterface> ref = std::shared_ptr<AnyInterface>(new AnyValue<Number*>(&value));
		values.push_back(ref);
		valueRegion->onTextEntered = [=](NumberField* field) {
			*(ref->getValue<Number*>())=field->getValue();
		};
		setCommonParameters(comp,labelRegion, valueRegion);
		valueRegion->backgroundColor= MakeColor(AlloyDefaultContext()->theme.LIGHTER);
		valueRegion->setRoundCorners(true);
		estimatedHeight += entryHeight + SPACING;
	}
	void ParameterPane::addColorField(const std::string& label, Color& value, float aspect) {
		CompositePtr comp = CompositePtr(new Composite(label + "_param", CoordPX(0, 0), CoordPerPX(1.0f, 0.0f, 0.0f, entryHeight)));
		TextLabelPtr labelRegion = TextLabelPtr(new TextLabel(label, CoordPX(0.0f, 0.0f), CoordPerPX(1.0f, 0.0f, 0.0f, entryHeight)));
		ColorSelectorPtr valueRegion = ColorSelectorPtr(new ColorSelector(label, CoordPerPX(1.0f, 0.0f, -aspect*entryHeight, 0.0f), CoordPX(aspect*entryHeight, entryHeight),false));
		if (aspect <= 0) {
			pixel2 labelBounds = labelRegion->getTextDimensions(AlloyDefaultContext().get());
			labelBounds.x += 10;
			valueRegion->position = CoordPX(labelBounds.x, 0.0f);
			valueRegion->dimensions = CoordPerPX(1.0f, 0.0f, -labelBounds.x, entryHeight);
		}
		else {
			labelRegion->position = CoordPX(0.0f, 0.0f);
			labelRegion->dimensions = CoordPerPX(1.0f, 0.0f, -aspect*entryHeight, entryHeight);
		}
		valueRegion->setColor(value);
		std::shared_ptr<AnyInterface> ref = std::shared_ptr<AnyInterface>(new AnyValue<Color*>(&value));
		values.push_back(ref);
		valueRegion->onSelect = [=](const Color& c) {
			*(ref->getValue<Color*>()) = c;
		};
		setCommonParameters(comp, labelRegion, valueRegion);
		estimatedHeight += entryHeight + SPACING;

	}
	void ParameterPane::addSelectionField(const std::string& label,int& selectedIndex, const std::vector<std::string>& options, float aspect) {
		CompositePtr comp = CompositePtr(new Composite(label + "_param", CoordPX(0, 0), CoordPerPX(1.0f, 0.0f, 0.0f, entryHeight)));
		TextLabelPtr labelRegion = TextLabelPtr(new TextLabel(label, CoordPX(0.0f, 0.0f), CoordPerPX(1.0f, 0.0f, 0.0f, entryHeight)));
		SelectionPtr valueRegion = SelectionPtr(new Selection(label, CoordPerPX(1.0f, 0.0f, -aspect*entryHeight, 0.0f), CoordPX(aspect*entryHeight, entryHeight), options));
		if (aspect <= 0) {
			pixel2 labelBounds = labelRegion->getTextDimensions(AlloyDefaultContext().get());
			labelBounds.x += 10;
			valueRegion->position = CoordPX(labelBounds.x, 0.0f);
			valueRegion->dimensions = CoordPerPX(1.0f, 0.0f, -labelBounds.x, entryHeight);
		}
		else {
			labelRegion->position = CoordPX(0.0f, 0.0f);
			labelRegion->dimensions = CoordPerPX(1.0f, 0.0f, -aspect*entryHeight, entryHeight);
		}
		valueRegion->setSelectionIndex(selectedIndex);
		std::shared_ptr<AnyInterface> ref = std::shared_ptr<AnyInterface>(new AnyValue<int*>(&selectedIndex));
		values.push_back(ref);
		valueRegion->onSelect=[=](int selection){
			*(ref->getValue<int*>()) = selection;
		};
		setCommonParameters(comp, labelRegion, valueRegion);
		valueRegion->backgroundColor = MakeColor(AlloyDefaultContext()->theme.LIGHTER);
		valueRegion->setRoundCorners(true);
		estimatedHeight += entryHeight + SPACING;
	}
	void ParameterPane::addToggleBox(const std::string& label, bool& value, float aspect) {
		CompositePtr comp = CompositePtr(new Composite(label + "_param", CoordPX(0, 0), CoordPerPX(1.0f, 0.0f, 0.0f, entryHeight)));
		TextLabelPtr labelRegion = TextLabelPtr(new TextLabel(label, CoordPX(0.0f, 0.0f), CoordPerPX(1.0f, 0.0f, 0.0f, entryHeight)));
		ToggleBoxPtr valueRegion = ToggleBoxPtr(new ToggleBox(label, CoordPerPX(1.0f, 0.0f, -aspect*entryHeight, 0.0f), CoordPX(aspect*entryHeight, entryHeight),value,false));
		if (aspect <= 0) {
			pixel2 labelBounds = labelRegion->getTextDimensions(AlloyDefaultContext().get());
			labelBounds.x += 10;
			valueRegion->position = CoordPX(labelBounds.x, 0.0f);
			valueRegion->dimensions = CoordPerPX(1.0f, 0.0f, -labelBounds.x, entryHeight);
		}
		else {
			labelRegion->position = CoordPX(0.0f, 0.0f);
			labelRegion->dimensions = CoordPerPX(1.0f, 0.0f, -aspect*entryHeight, entryHeight);
		}
		std::shared_ptr<AnyInterface> ref = std::shared_ptr<AnyInterface>(new AnyValue<bool*>(&value));
		values.push_back(ref);
		valueRegion->onChange = [=](bool value) {
			*(ref->getValue<bool*>()) = value;
		};
		setCommonParameters(comp, labelRegion, valueRegion);
		estimatedHeight += entryHeight + SPACING;
	}
	void ParameterPane::addCheckBox(const std::string& label, bool& value, float aspect) {
		CompositePtr comp = CompositePtr(new Composite(label + "_param", CoordPX(0, 0), CoordPerPX(1.0f, 0.0f, 0.0f, entryHeight)));
		TextLabelPtr labelRegion = TextLabelPtr(new TextLabel(label, CoordPX(0.0f, 0.0f), CoordPerPX(1.0f, 0.0f, 0.0f, entryHeight)));
		CheckBoxPtr valueRegion = CheckBoxPtr(new CheckBox(label, CoordPerPX(1.0f, 0.0f, -aspect*entryHeight, 0.0f), CoordPX(aspect*entryHeight, entryHeight),value,false));
		if (aspect <= 0) {
			pixel2 labelBounds = labelRegion->getTextDimensions(AlloyDefaultContext().get());
			labelBounds.x += 10;
			valueRegion->position = CoordPX(labelBounds.x, 0.0f);
			valueRegion->dimensions = CoordPerPX(1.0f, 0.0f, -labelBounds.x, entryHeight);
		}
		else {
			labelRegion->position = CoordPX(0.0f, 0.0f);
			labelRegion->dimensions = CoordPerPX(1.0f, 0.0f, -aspect*entryHeight, entryHeight);
		}
		std::shared_ptr<AnyInterface> ref = std::shared_ptr<AnyInterface>(new AnyValue<bool*>(&value));
		values.push_back(ref);
		valueRegion->onChange = [=](bool value) {
			*(ref->getValue<bool*>()) = value;
		};
		setCommonParameters(comp, labelRegion, valueRegion);
		estimatedHeight += entryHeight + SPACING;
	}
	void ParameterPane::addFileField(const std::string& label, std::string& file, float aspect) {
		CompositePtr comp = CompositePtr(new Composite(label + "_param", CoordPX(0, 0), CoordPerPX(1.0f, 0.0f, 0.0f, entryHeight)));
		TextLabelPtr labelRegion = TextLabelPtr(new TextLabel(label, CoordPX(0.0f, 0.0f), CoordPerPX(1.0f, 0.0f, 0.0f, entryHeight)));
		FileSelectorPtr valueRegion = FileSelectorPtr(new FileSelector(label, CoordPerPX(1.0f, 0.0f, -aspect*entryHeight, 0.0f), CoordPX(aspect*entryHeight, entryHeight)));
		valueRegion->setValue(file);
		if (aspect <= 0) {
			pixel2 labelBounds = labelRegion->getTextDimensions(AlloyDefaultContext().get());
			labelBounds.x += 10;
			valueRegion->position = CoordPX(labelBounds.x, 0.0f);
			valueRegion->dimensions = CoordPerPX(1.0f, 0.0f, -labelBounds.x, entryHeight);
		}
		else {
			labelRegion->position = CoordPX(0.0f, 0.0f);
			labelRegion->dimensions = CoordPerPX(1.0f, 0.0f, -aspect*entryHeight, entryHeight);
		}
		std::shared_ptr<AnyInterface> ref = std::shared_ptr<AnyInterface>(new AnyValue<std::string*>(&file));
		values.push_back(ref);
		valueRegion->onChange = [=](const std::string& value) {
			*(ref->getValue<std::string*>()) = value;
		};
		setCommonParameters(comp, labelRegion, valueRegion);
		valueRegion->backgroundColor = MakeColor(AlloyDefaultContext()->theme.LIGHTER);
		valueRegion->setRoundCorners(true);
		estimatedHeight += entryHeight + SPACING;
	}

	void ParameterPane::addMultiFileSelector(const std::string& label, std::vector<std::string>& files, float aspect) {
		CompositePtr comp = CompositePtr(new Composite(label + "_param", CoordPX(0, 0), CoordPerPX(1.0f, 0.0f, 0.0f, 4*entryHeight)));
		TextLabelPtr labelRegion = TextLabelPtr(new TextLabel(label, CoordPX(0.0f, 0.0f), CoordPerPX(1.0f, 0.0f,0.0f, entryHeight)));
		MultiFileSelectorPtr valueRegion = MultiFileSelectorPtr(new MultiFileSelector("Multi-File Field", CoordPerPX(1.0f, 0.0f, -aspect*entryHeight, 0.0f), CoordPX(aspect*entryHeight, 4*entryHeight),entryHeight));
		if (aspect <=0) {
			pixel2 labelBounds = labelRegion->getTextDimensions(AlloyDefaultContext().get());
			labelBounds.x += 10;
			valueRegion->position = CoordPX(labelBounds.x, 0.0f);
			valueRegion->dimensions = CoordPerPX(1.0f, 0.0f, -labelBounds.x, 4 * entryHeight);
		}
		else {
			labelRegion->position=CoordPX(0.0f, 0.0f);
			labelRegion->dimensions=CoordPerPX(1.0f, 0.0f, -aspect*entryHeight,4* entryHeight);
		}
		std::shared_ptr<AnyInterface> ref = std::shared_ptr<AnyInterface>(new AnyValue<std::vector<std::string>*>(&files));
		valueRegion->addFiles(files);
		values.push_back(ref);
		valueRegion->onChange = [=](const std::vector<std::string>& value) {
			*(ref->getValue<std::vector<std::string>*>()) = value;
		};
		setCommonParameters(comp, labelRegion, valueRegion);
		valueRegion->backgroundColor = MakeColor(AlloyDefaultContext()->theme.LIGHTER);
		valueRegion->setRoundCorners(true);
		estimatedHeight += 4*entryHeight + SPACING;
	}
}