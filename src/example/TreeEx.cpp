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

#include "Alloy.h"
#include "../../include/example/TreeEx.h"
using namespace aly;
TreeEx::TreeEx() :
		Application(800, 600, "Expand Tree Example") {
}
void TreeEx::addLeaf(TreeItem* item, const FileDescription& fd) {
	const float fontSize = 14;
	item->clear();
	item->add(
			LeafItemPtr(
					new LeafItem(
							[this,fontSize,fd](AlloyContext* context,const box2px& bounds) {
								NVGcontext* nvg=context->nvgContext;
								pushScissor(nvg,bounds);
								float yoff=2+bounds.position.y;
								nvgFontSize(nvg,fontSize);
								nvgFontFaceId(nvg,context->getFontHandle(FontType::Normal));
								std::string label;
								label=MakeString()<<"File Size: "<<FormatSize(fd.fileSize);
								drawText(nvg,bounds.position.x,yoff,label.c_str(),FontStyle::Normal,context->theme.LIGHT_TEXT);
								yoff+=fontSize+2;
								label=MakeString()<<"Created: "<<FormatDateAndTime(fd.creationTime);
								drawText(nvg,bounds.position.x,yoff,label.c_str(),FontStyle::Normal,context->theme.LIGHT_TEXT);
								yoff+=fontSize+2;
								label=MakeString()<<"Modified: "<<FormatDateAndTime(fd.lastModifiedTime);
								drawText(nvg,bounds.position.x,yoff,label.c_str(),FontStyle::Normal,context->theme.LIGHT_TEXT);
								yoff+=fontSize+2;
								label=MakeString()<<"Accessed: "<<FormatDateAndTime(fd.lastAccessTime);
								drawText(nvg,bounds.position.x,yoff,label.c_str(),FontStyle::Normal,context->theme.LIGHT_TEXT);
								popScissor(nvg);
							}, pixel2(180, 4 * (fontSize + 2) + 2))));
}
void TreeEx::addDirectory(const std::string& dir, aly::TreeItem* parent) {
	for (FileDescription fd : GetDirectoryDescriptionListing(dir)) {
		std::string fileName = GetFileName(fd.fileLocation);
		if (fileName.length() > 0 && fileName[0] == '.')
			continue;
		TreeItemPtr child;
		std::string fileLocation = fd.fileLocation;
		if (fd.fileType == FileType::Directory) {
			child = TreeItemPtr(new TreeItem(fileName, 0xf115, 22));

			//Use onExpand for lazy tree evaluation.
			child->onExpand = [this,fileLocation](TreeItem* current) {
				current->clear();
				addDirectory(fileLocation, current);
			};
		} else {
			child = TreeItemPtr(new TreeItem(fileName, 0xf016, 18));
			child->onExpand = [this,fd](TreeItem* current) {
				//Display file info when expanded.
				addLeaf(current,fd);
			};
		}
		parent->add(child);
	}
}
bool TreeEx::init(Composite& rootNode) {
	ExpandTreePtr tree = ExpandTreePtr(
			new ExpandTree("Tree", CoordPercent(0.0f, 0.0f),
					CoordPercent(0.35f, 1.0f)));
	std::string homeDir = GetCurrentWorkingDirectory();
	for (FileDescription fd : GetDirectoryDescriptionListing(homeDir)) {
		std::string fileName = GetFileName(fd.fileLocation);
		if (fileName.length() > 0 && fileName[0] == '.')
			continue;
		TreeItemPtr child;
		if (fd.fileType == FileType::Directory) {
			child = TreeItemPtr(new TreeItem(fileName, 0xf115, 22));
			addDirectory(fd.fileLocation, child.get());
		} else {
			child = TreeItemPtr(new TreeItem(fileName, 0xf016, 18));
			child->onExpand = [this,fd](TreeItem* current) {
				addLeaf(current,fd);
			};
		}
		tree->add(child);
	}
	rootNode.add(tree);
	return true;
}

