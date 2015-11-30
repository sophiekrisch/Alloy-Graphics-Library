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
#include "AvoidanceRouting.h"
#include "AlloyDataFlow.h"
namespace aly {
	namespace dataflow {

		bool operator==(const std::shared_ptr<AvoidancePath>& a, const std::shared_ptr<AvoidancePath>& b) {
			return (a->path.start == b->path.start&&a->path.end == b->path.end);
		}
		bool operator< (const std::shared_ptr<AvoidancePath>& b, const std::shared_ptr<AvoidancePath>& a) {
			if (a->distToDest == b->distToDest) {
				if (a->pathLength == b->pathLength) {
					return (a->depth < b->depth);
				}
				else {
					return (a->pathLength < b->pathLength);
				}
			}
			else {
				return (a->distToDest < b->distToDest);
			}
		}
		const float AvoidancePath::BORDER_SPACE = 10.0f;
		AvoidancePath::AvoidancePath(std::vector<box2px>& obstacles, const float2& from, const float2& to, Direction direction, AvoidancePath* parent) :obstacles(obstacles), direction(direction), parent(parent), path(from, to) {
			distToDest = std::numeric_limits<float>::max();
			depth = 0;
			pathLength = std::numeric_limits<float>::max();
			updatePath(obstacles, to);
			updateDistToDestination(to);
		}
		void AvoidancePath::addChild(const std::shared_ptr<AvoidancePath>& child) {
			children.push_back(child);
			child->depth = this->depth + 1;
		}
		float2 AvoidancePath::backTrack(std::vector<float2>& pointList) {
			if (parent != nullptr) {
				pointList.push_back(parent->backTrack(pointList));
			}
			else {
				pointList.push_back(path.start);
			}
			return path.end;
		}
		std::vector<float2> AvoidancePath::backTrack() {
			std::vector<float2> path;
			path.push_back(backTrack(path));
			return path;
		}
		float AvoidancePath::updatePathLength() {
			this->pathLength = distance(path.start, path.end) + ((parent != nullptr) ? parent->updatePathLength() : 0);
			return pathLength;
		}
		bool AvoidancePath::createChildren(const float2& to) {
			if ((direction == Direction::North) || (direction == Direction::South)) {
				std::shared_ptr<AvoidancePath> east = std::shared_ptr<AvoidancePath>(new AvoidancePath(obstacles, path.end, to, Direction::East, this));
				std::shared_ptr<AvoidancePath> west = std::shared_ptr<AvoidancePath>(new AvoidancePath(obstacles, path.end, to, Direction::West, this));
				if ((east->distToDest == 0) || (west->distToDest == 0)) {
					if (east->distToDest == 0) {
						addChild(east);
					}
					if (west->distToDest == 0) {
						addChild(west);
					}
					return true;
				}
				else {
					addChild(east);
					addChild(west);
					return false;
				}
			}
			else {
				std::shared_ptr<AvoidancePath> south = std::shared_ptr<AvoidancePath>(new AvoidancePath(obstacles, path.end, to, Direction::South, this));
				std::shared_ptr<AvoidancePath> north = std::shared_ptr<AvoidancePath>(new AvoidancePath(obstacles, path.end, to, Direction::North, this));
				if ((north->distToDest == 0) || (south->distToDest == 0)) {
					if (north->distToDest == 0) {
						addChild(north);
					}
					if (south->distToDest == 0) {
						addChild(south);
					}
					return true;
				}
				else {
					addChild(north);
					addChild(south);
					return false;
				}
			}
		}
		void AvoidancePath::createDescendants(std::vector<std::shared_ptr<AvoidancePath>>& ret, const float2& to, int depth) {
			if (!createChildren(to)) {
				if (depth != 0) {
					for (std::shared_ptr<AvoidancePath> child : children) {
						child->createDescendants(ret, to, depth - 1);
					}
				}
			}
			else {
				ret.insert(ret.end(), children.begin(), children.end());
			}
			if (depth == 0) {
				ret.insert(ret.end(), children.begin(), children.end());
			}
		}
		float2 AvoidancePath::findNextBoundary(const float2 &point) {
			float2 p = point;
			float2 tmp;
			double minDist = std::numeric_limits<float>::min();
			if ((parent != nullptr) && (parent->obstacle.dimensions.x*parent->obstacle.dimensions.y > 0)) {
				box2f rect = parent->obstacle;
				switch (direction) {
				case Direction::North:
					p = float2(point.x, rect.position.y - BORDER_SPACE);
					break;
				case Direction::South:
					p = float2(point.x, rect.position.y + rect.dimensions.y + BORDER_SPACE);
					break;
				case Direction::East:
					p = float2(rect.position.x + rect.dimensions.x + BORDER_SPACE, point.y);
					break;
				case Direction::West:
					p = float2(rect.position.x - BORDER_SPACE, point.y);
					break;
				default:
					break;
				}
				return p;
			}
			else {
				return point;
			}
		}
		void AvoidancePath::getDescendants(std::vector<std::shared_ptr<AvoidancePath>>& ret, int depth) {
			if (depth == 0) {
				return;
			}
			ret.insert(ret.end(), children.begin(), children.end());
			for (std::shared_ptr<AvoidancePath> child : children) {
				child->getDescendants(ret, depth - 1);
			}
		}
		float AvoidancePath::getDistanceToDestination() const {
			return distToDest;
		}
		float AvoidancePath::getPathLength() const {
			return pathLength;
		}
		float AvoidancePath::lineLength() {
			return distance(path.start, path.end);
		}
		float AvoidancePath::updateDistToDestination(float2 target) {
			if (path != line2f::NONE) {
				distToDest = distance(path.end, target);
			}
			else {
				distToDest = std::numeric_limits<float>::max();
			}
			return distToDest;
		}
		line2f AvoidancePath::updatePath(std::vector<box2f>& obstacles, const float2& to) {
			if (direction == Direction::Unkown) {
				return path;
			}
			switch (direction) {
				case Direction::North:
					if (path.end.y >= path.start.y) {
						path = line2f(path.start, findNextBoundary(path.start));
					}
					else {
						path = line2f(path.start, float2(path.start.x, path.end.y));
					}
					break;
				case Direction::South:
					if (path.end.y <= path.start.y) {
						path = line2f(path.start, findNextBoundary(path.start));
					}
					else {
						path = line2f(path.start, float2(path.start.x, path.end.y));
					}
					break;
				case Direction::East:
					if (path.end.x <= path.start.x) {
						path = line2f(path.start, findNextBoundary(path.start));
					}
					else {
						path = line2f(path.start, float2(path.end.x, path.start.y));
					}
					break;
				case Direction::West:
					if (path.end.x >= path.start.x) {
						path = line2f(path.start, findNextBoundary(path.start));
					}
					else {
						path = line2f(path.start, float2(path.end.x, path.start.y));
					}
					break;
			}
			for (box2f obstacle : obstacles) {
				if (path.intersects(obstacle)) {
					switch (direction) {
					case Direction::South:
						path = line2f(path.start, float2(path.start.x, obstacle.position.y
							- BORDER_SPACE));
						break;
					case Direction::North:
						path = line2f(path.start, float2(path.start.x, obstacle.position.y + obstacle.dimensions.y
							+ BORDER_SPACE));
						break;
					case Direction::West:
						path = line2f(path.start, float2(obstacle.position.x + obstacle.dimensions.x + BORDER_SPACE, path.start
							.y));
						break;
					case Direction::East:
						path = line2f(path.start, float2(obstacle.position.x - BORDER_SPACE, path.start
							.y));
						break;
					}
					this->obstacle = obstacle;
				}
			}
			updatePathLength();
			return path;
		}
		void AvoidanceRouting::update() {
			getObstacles(obstacles);
		}
		void AvoidanceRouting::evaluate(std::vector<float2>& path, float2 from, float2 to, Direction direction) {
			//std::cout << "Evaluate " << from << " " << to << " " << direction << " " << obstacles.size() << std::endl;
			path.clear();
			float2 origFrom = from;
			float2 origTo = to;
			switch (direction) {
			case Direction::South:
				from = float2(from.x, from.y + AvoidancePath::BORDER_SPACE);
				to = float2(to.x, to.y - AvoidancePath::BORDER_SPACE);
				break;
			case Direction::North:
				from = float2(from.x, from.y - AvoidancePath::BORDER_SPACE);
				to = float2(to.x, to.y + AvoidancePath::BORDER_SPACE);
				break;
			case Direction::East:
				from = float2(from.x + AvoidancePath::BORDER_SPACE, from.y);
				to = float2(to.x - AvoidancePath::BORDER_SPACE, to.y);
				break;
			case Direction::West:
				from = float2(from.x - AvoidancePath::BORDER_SPACE, from.y);
				to = float2(to.x + AvoidancePath::BORDER_SPACE, to.y);
				break;
			default:
				break;
			}
			std::priority_queue<std::shared_ptr<AvoidancePath>> queue;
			std::shared_ptr<AvoidancePath> root = std::shared_ptr<AvoidancePath>(new AvoidancePath(obstacles, from, to, direction));
			queue.push(root);
			std::shared_ptr<AvoidancePath> optNode = root;
			int count = 0;
			std::list<std::shared_ptr<AvoidancePath>> history;
			std::shared_ptr<AvoidancePath> head;
			while ((queue.size() > 0) && (count < MAX_PATHS)) {
				head = queue.top();
				//std::cout <<"Top "<< *head << std::endl;
				queue.pop();
				if (head->getDistanceToDestination() == 0) {
					optNode = head;
					break;
				}
				count++;
				std::vector<std::shared_ptr<AvoidancePath>> children;
				head->createDescendants(children, to, DEPTH_LIMIT);
				bool allowAdd = false;
				for (std::shared_ptr<AvoidancePath> child : children) {
					allowAdd = true;
					for (std::shared_ptr<AvoidancePath> node : history) {
						if (node == child) {
							allowAdd = false;
							break;
						}
					}
					if (allowAdd) {
						//std::cout << "Add child " << *child << std::endl;
						queue.push(child);
						history.push_back(child);
					}
				}
			}
			path = optNode->backTrack();
			path[0] = origFrom;
			if ((direction == Direction::South) || (direction == Direction::North)) {
				if ((optNode->direction == Direction::South) || (optNode->direction == Direction::North)) {
					path[path.size() - 1] = origTo;
				}
				else {
					path.push_back(origTo);
				}
				simplifyPath(path, obstacles,0);
			}
			else {
				if ((optNode->direction == Direction::East) || (optNode->direction == Direction::West)) {
					path[path.size() - 1] = origTo;
				}
				else {
					path.push_back(origTo);
				}
				simplifyPath(path, obstacles,1);
			}
			if (path.size() == 2) {
				float dx = std::abs(origFrom.x - origTo.x);
				float dy = std::abs(origFrom.y - origTo.y);
				float x2 = origFrom.x + ((origTo.x - origFrom.x) *0.5f);
				float y2 = origFrom.y + ((origTo.y - origFrom.y) *0.5f);
				path.clear();
				path.push_back(origFrom);
				if ((direction == Direction::East) || (direction == Direction::West)) {
					path.push_back(float2(x2, origFrom.y));
					path.push_back(float2(x2, origTo.y));
				}
				else {
					path.push_back(float2(origFrom.x, y2));
					path.push_back(float2(origTo.x, y2));
				}
				path.push_back(origTo);
			}
			

		}
		void AvoidanceRouting::getObstacles(std::vector<box2px>& obst) {
			obst.clear();
			for (NodePtr& node : nodes) {
				box2px box = node->getObstacleBounds();
				obst.push_back(box);
			}
		}
		box2px AvoidanceRouting::getPathBounds(float2 from, float2 to) const {
			float2 minPt = aly::min(from, to);
			float2 maxPt = aly::max(from, to);
			return box2px(minPt, maxPt - minPt);
		}
		void AvoidanceRouting::evaluate(const std::shared_ptr<Connection>& edge) {
			std::vector<float2>& path = edge->path;
			float2 to = edge->destination->getLocation();
			float2 from = edge->source->getLocation();
			Direction direction = Direction::Unkown;
			PortPtr target = edge->destination;
			PortPtr src = edge->source;
			if (src->getType() == PortType::Parent) {
				direction = Direction::East;
			}
			else if (src->getType() == PortType::Child) {
				direction = Direction::West;
			}
			else if (src->getType() == PortType::Output) {
				direction = Direction::South;
			}
			else if (src->getType() == PortType::Input) {
				direction = Direction::North;
			}
			evaluate(path, from, to, direction);
		}
		void AvoidanceRouting::simplifyPath(std::vector<float2>& path, const std::vector<box2f>& obstacles, int parity) {
			float2 st, end, stNext, endNext;
			bool reduce = false;
			line2f l1, l2;
			for (int i = 0; i < (int)path.size() - 4; i++) {
				st = path[i];
				end = path[i + 3];
				if (i % 2 == parity) {
					stNext = float2(st.x, end.y);
				}
				else {
					stNext = float2(end.x, st.y);
				}
				l1 = line2f(path[i + 1], stNext);
				l2 = line2f(stNext, end);
				reduce = true;
				for (box2px obstacle : obstacles) {
					if (l1.intersects(obstacle) || l2.intersects(obstacle)) {
						reduce = false;
						break;
					}
				}
				if (reduce) {
					path[i + 1] = stNext;
					path.erase(path.begin() + i + 2);
					path.erase(path.begin() + i + 2);
					i = -1;
				}
			}
		}

	}
}