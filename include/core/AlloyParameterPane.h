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
#ifndef ALLOYPARAMETERPANE_H_
#define ALLOYPARAMETERPANE_H_
#include "AlloyUI.h"
#include "AlloyWidget.h"
#include "AlloyExpandBar.h"
#include "AlloyAny.h"
#include <string>
#include <vector>
namespace aly {
	class ParameterPane : public Composite {
	protected:
		static const float SPACING;
		float entryHeight;
		std::vector<std::shared_ptr<AnyInterface>> values;
		void setCommonParameters(const CompositePtr& compRegion,const TextLabelPtr& textLabel,const RegionPtr& regionLabel);
		std::list<RegionPtr> groupQueue;
		CompositePtr lastRegion;
		ExpandBarPtr expandBar;
		float estimatedHeight=0;
		bool lastExpanded = false;
		void updateGroups();
	public:
		AColor entryTextColor;
		AColor entryBackgroundColor;
		AColor entryBorderColor;
		AUnit1D entryBorderWidth;
		virtual void pack(const pixel2& pos, const pixel2& dims, const double2& dpmm, double pixelRatio, bool clamp) override;

		ParameterPane(const std::string& name, const AUnit2D& pos, const AUnit2D& dim,float entryHeight=30.0f);
		void addGroup(const std::string& name,bool expanded);
		void addNumberField(const std::string& label,Number& value,float aspect=3.0f);
		void addSelectionField(const std::string& label,int& selectedIndex,const std::vector<std::string>& options, float aspect = 4.0f);
		void addToggleBox(const std::string& label, bool& value, float aspect = 2.1f);
		void addCheckBox(const std::string& label, bool& value, float aspect = 1.0f);
		void addColorField(const std::string& label,Color& color, float aspect = 3.0f);
		void addFileField(const std::string& label, std::string& file, float aspect = -1.0f);
		void addMultiFileSelector(const std::string& label, std::vector<std::string>& files, float aspect = -1.0f);
	};

	typedef std::shared_ptr<ParameterPane> ParameterPanePtr;
}

#endif