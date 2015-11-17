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
void TreeEx::addDirectory(const std::string& dir,
		const std::shared_ptr<aly::TreeItem>& parent, int depth) {
	for (FileDescription fd : GetDirectoryDescriptionListing(dir)) {
		std::string fileName = GetFileName(fd.fileLocation);
		if (fileName.length() > 0 && fileName[0] == '.')
			continue;
		TreeItemPtr child = TreeItemPtr(new TreeItem(fileName, 0, 20));
		/*
		 for (int d = 0; d < depth; d++) {
		 std::cout << "--";
		 }
		 std::cout << child->getName() << std::endl;
		 */
		if (depth < 4) {
			if (fd.fileType == FileType::Directory) {
				addDirectory(fd.fileLocation, child, depth + 1);
			}
			parent->add(child);
		}
	}
}
bool TreeEx::init(Composite& rootNode) {
	ExpandTreePtr tree = ExpandTreePtr(
			new ExpandTree("Tree", CoordPercent(0.7f, 0.0f),
					CoordPercent(0.3f, 1.0f)));
	std::string homeDir = GetHomeDirectory();
	for (FileDescription fd : GetDirectoryDescriptionListing(homeDir)) {
		std::string fileName = GetFileName(fd.fileLocation);
		if (fileName.length() > 0 && fileName[0] == '.')
			continue;
		TreeItemPtr child = TreeItemPtr(
				new TreeItem(fileName, 0, 20));
		//std::cout << child->getName() << std::endl;
		if (fd.fileType == FileType::Directory) {
			addDirectory(fd.fileLocation, child, 0);
		}
		tree->add(child);
	}
	rootNode.add(tree);
	return true;
}

