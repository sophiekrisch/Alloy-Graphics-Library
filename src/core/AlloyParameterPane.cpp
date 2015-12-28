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
namespace aly {
	ParameterPane::ParameterPane(const std::string& name, const AUnit2D& pos, const AUnit2D& dim, float entryHeight):Composite(name,pos,dim),entryHeight(entryHeight) {
		setOrientation(Orientation::Vertical,pixel2(0.0f,2.0f),pixel2(0.0f));
		backgroundColor = MakeColor(AlloyDefaultContext()->theme.DARK.toDarker(0.5f));
		entryBackgroundColor= MakeColor(AlloyDefaultContext()->theme.DARK);
		entryBorderColor= MakeColor(AlloyDefaultContext()->theme.LIGHT);
		entryTextColor = MakeColor(AlloyDefaultContext()->theme.LIGHT_TEXT);
		entryBorderWidth = UnitPX(0.0f);
	}
	void ParameterPane::setCommonParameters(const CompositePtr& compRegion,const TextLabelPtr& textLabel, const RegionPtr& valueRegion) {
		valueRegion->setRoundCorners(true);
		compRegion->backgroundColor = entryBackgroundColor;
		compRegion->borderColor = entryBorderColor;
		compRegion->borderWidth = entryBorderWidth;

		valueRegion->backgroundColor = entryBackgroundColor;
		valueRegion->borderWidth = UnitPX(0.0f);
		textLabel->textColor = entryTextColor;
	}
	void ParameterPane::addNumberField(const std::string& label, Number& value,float aspect) {
		float entryWidth = aspect * entryHeight;
		CompositePtr comp = CompositePtr(new Composite(label+"_param",CoordPX(0,0),CoordPerPX(1.0f,0.0f,0.0f,entryHeight)));
		TextLabelPtr labelRegion = TextLabelPtr(new TextLabel(label,CoordPX(0.0f,0.0f),CoordPerPX(1.0f,0.0f,-entryWidth,entryHeight)));
		NumberFieldPtr valueRegion = NumberFieldPtr(new NumberField(label, CoordPerPX(1.0f, 0.0f, -entryWidth, 0.0f), CoordPX(entryWidth, entryHeight), value.type()));
		valueRegion->textColor = MakeColor(AlloyDefaultContext()->theme.DARK_TEXT);
		valueRegion->setNumberValue(value);
		comp->add(labelRegion);
		comp->add(valueRegion);
		std::shared_ptr<AnyInterface> ref = std::shared_ptr<AnyInterface>(new AnyValue<Number*>(&value));
		values.push_back(ref);
		valueRegion->onTextEntered = [=](NumberField* field) {
			*(ref->getValue<Number*>())=field->getValue();
		};
		Composite::add(comp);
		setCommonParameters(comp,labelRegion, valueRegion);
		valueRegion->backgroundColor= MakeColor(AlloyDefaultContext()->theme.LIGHT_TEXT);
		valueRegion->setRoundCorners(true);
	}
	void ParameterPane::addColorField(const std::string& label, Color& value, float aspect) {
		float entryWidth = aspect * entryHeight;
		CompositePtr comp = CompositePtr(new Composite(label + "_param", CoordPX(0, 0), CoordPerPX(1.0f, 0.0f, 0.0f, entryHeight)));
		TextLabelPtr labelRegion = TextLabelPtr(new TextLabel(label, CoordPX(0.0f, 0.0f), CoordPerPX(1.0f, 0.0f, -entryWidth, entryHeight)));
		ColorSelectorPtr valueRegion = ColorSelectorPtr(new ColorSelector(label, CoordPerPX(1.0f, 0.0f, -entryWidth, 0.0f), CoordPX(entryWidth, entryHeight),false));
		valueRegion->setColor(value);
		comp->add(labelRegion);
		comp->add(valueRegion);
		std::shared_ptr<AnyInterface> ref = std::shared_ptr<AnyInterface>(new AnyValue<Color*>(&value));
		values.push_back(ref);
		valueRegion->onSelect = [=](const Color& c) {
			*(ref->getValue<Color*>()) = c;
		};
		Composite::add(comp);
		setCommonParameters(comp, labelRegion, valueRegion);

	}
	void ParameterPane::addSelectionField(const std::string& label,int& selectedIndex, const std::vector<std::string>& options, float aspect) {
		float entryWidth = aspect * entryHeight;
		CompositePtr comp = CompositePtr(new Composite(label + "_param", CoordPX(0, 0), CoordPerPX(1.0f, 0.0f, 0.0f, entryHeight)));
		TextLabelPtr labelRegion = TextLabelPtr(new TextLabel(label, CoordPX(0.0f, 0.0f), CoordPerPX(1.0f, 0.0f, -entryWidth, entryHeight)));
		SelectionPtr valueRegion = SelectionPtr(new Selection(label, CoordPerPX(1.0f, 0.0f, -entryWidth, 0.0f), CoordPX(entryWidth, entryHeight), options));
		valueRegion->setSelectionIndex(selectedIndex);

		comp->add(labelRegion);
		comp->add(valueRegion);
		std::shared_ptr<AnyInterface> ref = std::shared_ptr<AnyInterface>(new AnyValue<int*>(&selectedIndex));
		values.push_back(ref);
		valueRegion->onSelect=[=](int selection){
			*(ref->getValue<int*>()) = selection;
		};
		Composite::add(comp);
		setCommonParameters(comp, labelRegion, valueRegion);
		valueRegion->backgroundColor = MakeColor(AlloyDefaultContext()->theme.LIGHT_TEXT);
		valueRegion->setRoundCorners(true);
	}
	void ParameterPane::addToggleBox(const std::string& label, bool& value, float aspect) {
		float entryWidth = aspect * entryHeight;
		CompositePtr comp = CompositePtr(new Composite(label + "_param", CoordPX(0, 0), CoordPerPX(1.0f, 0.0f, 0.0f, entryHeight)));
		TextLabelPtr labelRegion = TextLabelPtr(new TextLabel(label, CoordPX(0.0f, 0.0f), CoordPerPX(1.0f, 0.0f, -entryWidth, entryHeight)));
		ToggleBoxPtr valueRegion = ToggleBoxPtr(new ToggleBox(label, CoordPerPX(1.0f, 0.0f, -entryWidth, 0.0f), CoordPX(entryWidth, entryHeight),value,false));
		comp->add(labelRegion);
		comp->add(valueRegion);
		std::shared_ptr<AnyInterface> ref = std::shared_ptr<AnyInterface>(new AnyValue<bool*>(&value));
		values.push_back(ref);
		valueRegion->onChange = [=](bool value) {
			*(ref->getValue<bool*>()) = value;
		};
		Composite::add(comp);
		setCommonParameters(comp, labelRegion, valueRegion);
	}
	void ParameterPane::addCheckBox(const std::string& label, bool& value, float aspect) {
		float entryWidth = aspect * entryHeight;
		CompositePtr comp = CompositePtr(new Composite(label + "_param", CoordPX(0, 0), CoordPerPX(1.0f, 0.0f, 0.0f, entryHeight)));
		TextLabelPtr labelRegion = TextLabelPtr(new TextLabel(label, CoordPX(0.0f, 0.0f), CoordPerPX(1.0f, 0.0f, -entryWidth, entryHeight)));
		CheckBoxPtr valueRegion = CheckBoxPtr(new CheckBox(label, CoordPerPX(1.0f, 0.0f, -entryWidth, 0.0f), CoordPX(entryWidth, entryHeight),value,false));
		comp->add(labelRegion);
		comp->add(valueRegion);
		std::shared_ptr<AnyInterface> ref = std::shared_ptr<AnyInterface>(new AnyValue<bool*>(&value));
		values.push_back(ref);
		valueRegion->onChange = [=](bool value) {
			*(ref->getValue<bool*>()) = value;
		};
		Composite::add(comp);
		setCommonParameters(comp, labelRegion, valueRegion);
	}
	void ParameterPane::addFileField(const std::string& label, std::string& file, float aspect) {
		float entryWidth = aspect * entryHeight;
		CompositePtr comp = CompositePtr(new Composite(label + "_param", CoordPX(0, 0), CoordPerPX(1.0f, 0.0f, 0.0f, entryHeight)));
		TextLabelPtr labelRegion = TextLabelPtr(new TextLabel(label, CoordPX(0.0f, 0.0f), CoordPerPX(1.0f, 0.0f, -entryWidth, entryHeight)));
		pixel2 labelBounds = labelRegion->getTextDimensions(AlloyDefaultContext().get());
		labelBounds.x += 4;
		FileSelectorPtr valueRegion = FileSelectorPtr(new FileSelector(label, CoordPerPX(1.0f, 0.0f, -entryWidth, 0.0f), CoordPX(entryWidth, entryHeight)));
		comp->add(labelRegion);
		comp->add(valueRegion);
		std::shared_ptr<AnyInterface> ref = std::shared_ptr<AnyInterface>(new AnyValue<std::string*>(&file));
		values.push_back(ref);
		valueRegion->onChange = [=](const std::string& value) {
			*(ref->getValue<std::string*>()) = value;
		};
		Composite::add(comp);
		setCommonParameters(comp, labelRegion, valueRegion);
		valueRegion->backgroundColor = MakeColor(AlloyDefaultContext()->theme.LIGHT_TEXT);
		valueRegion->setRoundCorners(true);
	}

	void ParameterPane::addMultiFileSelector(const std::string& label, std::vector<std::string>& files, float aspect) {
		float entryWidth = aspect * entryHeight;
		CompositePtr comp = CompositePtr(new Composite(label + "_param", CoordPX(0, 0), CoordPerPX(1.0f, 0.0f, 0.0f, 4*entryHeight)));
		TextLabelPtr labelRegion = TextLabelPtr(new TextLabel(label, CoordPX(0.0f, 0.0f), CoordPerPX(1.0f, 0.0f,0.0f, entryHeight)));
		pixel2 labelBounds=labelRegion->getTextDimensions(AlloyDefaultContext().get());
		labelBounds.x += 4;
		MultiFileSelectorPtr valueRegion = MultiFileSelectorPtr(new MultiFileSelector("Multi-File Field", CoordPX(labelBounds.x,0.0f), CoordPerPX(1.0f,0.0f,-labelBounds.x, 4*entryHeight),entryHeight));
		std::shared_ptr<AnyInterface> ref = std::shared_ptr<AnyInterface>(new AnyValue<std::vector<std::string>*>(&files));
		valueRegion->addFiles(files);
		values.push_back(ref);
		comp->add(labelRegion);
		comp->add(valueRegion);
		Composite::add(comp);
		
		setCommonParameters(comp, labelRegion, valueRegion);
		valueRegion->backgroundColor = MakeColor(AlloyDefaultContext()->theme.LIGHT_TEXT);
		valueRegion->setRoundCorners(true);

	}
}