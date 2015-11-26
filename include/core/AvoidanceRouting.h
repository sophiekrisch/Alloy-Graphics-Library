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
#ifndef INCLUDE_AVOIDANCEROUTING_H_
#define INCLUDE_AVOIDANCEROUTING_H_
#include "AlloyMath.h"
#include "AlloyVector.h"
#include "AlloyUI.h"
#include "AlloyDataFlow.h"
#include <vector>
#include <memory>
#include <queue>
namespace aly {
	namespace dataflow {
		template<class T, int C> struct line {
			vec<T, C> start;
			vec<T, C> end;
			line(const vec<T, C>& start = vec<T, C>(T(0)), const vec<T, C>& end = vec<T, C>(T(0))) :start(start), end(end) {
			}
			inline float length() const {
				return distance(start, end);
			}
			bool intersects(const box2f& box) const {
				return false;
			}
			static const line<T, C> NONE;
		};
		template<class T,int C> const line<T, C> line<T, C>::NONE = line<T, C>(vec<T, C>(std::numeric_limits<T>::min()), vec<T, C>(std::numeric_limits<T>::min()));

		template<class T, int C> bool operator==(const line<T, C>& a, const line<T, C>& b) {
			return (a.start == b.start&&a.end == b.end);
		}
		template<class T, int C> bool operator!=(const line<T, C>& a, const line<T, C>& b) {
			return (a.start != b.start || a.end != b.end);
		}
		template<class T, int K, class C, class R> std::basic_ostream<C, R> & operator <<(
			std::basic_ostream<C, R> & ss, const line<T, K>& line) {
			return ss << "[" << line.start << "->" << line.end << "]";
		}
		typedef line<float, 2> line2f;
		typedef line<float, 3> line3f;
		typedef line<float, 2> line2d;
		typedef line<double, 3> line3d;
		enum class Dir { Unkown, North, South, East, West };
		template<class C, class R> std::basic_ostream<C, R> & operator <<(
			std::basic_ostream<C, R> & ss, const Dir& type) {
			switch (type) {
			case Dir::Unkown:
				return ss << "Unkown";
			case Dir::North:
				return ss << "North";
			case Dir::South:
				return ss << "South";
			case Dir::East:
				return ss << "East";
			case Dir::West:
				return ss << "West";
			}
			return ss;
		}
		struct AvoidancePath {
			static const float BORDER_SPACE;
			std::vector<box2px>& obstacles;
			Dir direction = Dir::Unkown;
			float distToDest;
			float pathLength;
			int depth;
			std::vector<std::shared_ptr<AvoidancePath>> children;
			AvoidancePath* parent;
			line2f path;
			box2f obstacle;
			AvoidancePath(std::vector<box2px>& obstacles,const float2& from, const float2& to, Dir direction, AvoidancePath* parent = nullptr) :obstacles(obstacles),direction(direction), parent(parent), path(from, to) {
				distToDest = std::numeric_limits<float>::max();
				depth = 0;
				pathLength = 0;
				updatePath(obstacles,to);
				updateDistToDestination(to);
			}
			void addChild(const std::shared_ptr<AvoidancePath>& child) {
				children.push_back(child);
				child->depth = this->depth + 1;
			}
			float2 backTrack(std::vector<float2>& pointList) {
				if (parent != nullptr) {
					parent->backTrack(pointList);
				}
				else {
					return path.start;
				}
				return path.end;
			}
			inline std::vector<float2> backTrack() {
				std::vector<float2> path;
				path.push_back(backTrack(path));
				return path;
			}
			float updatePathLength() {
				this->pathLength = distance(path.start, path.end) + ((parent != nullptr) ? parent->updatePathLength() : 0);
				return pathLength;
			}
			bool createChildren(const float2& to) {
				if ((direction == Dir::North) || (direction == Dir::South)) {
					std::shared_ptr<AvoidancePath> east = std::shared_ptr<AvoidancePath>(new AvoidancePath(obstacles,path.end, to, Dir::East, this));
					std::shared_ptr<AvoidancePath> west = std::shared_ptr<AvoidancePath>(new AvoidancePath(obstacles, path.end, to, Dir::West, this));
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
					std::shared_ptr<AvoidancePath> south = std::shared_ptr<AvoidancePath>(new AvoidancePath(obstacles,path.end, to, Dir::South, this));
					std::shared_ptr<AvoidancePath> north = std::shared_ptr<AvoidancePath>(new AvoidancePath(obstacles, path.end, to, Dir::North, this));
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
			void createDescendants(std::vector<std::shared_ptr<AvoidancePath>>& ret, const float2& to, int depth) {
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
			float2 findNextBoundary(const float2 &point) {
				float2 p = point;
				float2 tmp;
				double minDist = std::numeric_limits<float>::min();
				if ((parent != nullptr) && (parent->obstacle.dimensions.x*parent->obstacle.dimensions.y > 0)) {
					box2f rect = parent->obstacle;
					switch (direction) {
					case Dir::North:
						p = float2(point.x, rect.position.y - BORDER_SPACE);
						break;
					case Dir::South:
						p = float2(point.x, rect.position.y + rect.dimensions.y + BORDER_SPACE);
						break;
					case Dir::East:
						p = float2(rect.position.x + rect.dimensions.x + BORDER_SPACE, point.y);
						break;
					case Dir::West:
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
			void getDescendants(std::vector<std::shared_ptr<AvoidancePath>>& ret, int depth) {
				if (depth == 0) {
					return;
				}
				ret.insert(ret.end(), children.begin(), children.end());
				for (std::shared_ptr<AvoidancePath> child : children) {
					child->getDescendants(ret, depth - 1);
				}
			}
			inline float getDistanceToDestination() const {
				return distToDest;
			}
			inline float getPathLength() const {
				return pathLength;
			}

			inline float lineLength() {
				return distance(path.start, path.end);
			}
			float updateDistToDestination(float2 target) {
				if (path != line2f::NONE) {
					distToDest = distance(path.end, target);
				}
				else {
					distToDest = std::numeric_limits<float>::max();
				}
				return distToDest;
			}
			line2f updatePath(std::vector<box2f>& obstacles, const float2& to) {
				if (direction == Dir::Unkown) {
					return path;
				}
				switch (direction) {
				case Dir::North:
					if (path.end.y >= path.start.y) {
						path = line2f(path.start, findNextBoundary(path.start));
					}
					else {
						path = line2f(path.start, float2(path.start.x, path.end.y));
					}
					break;
				case Dir::South:
					if (path.end.y <= path.start.y) {
						path = line2f(path.start, findNextBoundary(path.start));
					}
					else {
						path = line2f(path.start, float2(path.start.x, path.end.y));
					}
					break;
				case Dir::East:
					if (path.end.x <= path.start.x) {
						path = line2f(path.start, findNextBoundary(path.start));
					}
					else {
						path = line2f(path.start, float2(path.end.x, path.start.y));
					}
					break;
				case Dir::West:
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
						case Dir::South:
							path = line2f(path.start, float2(path.start.x, obstacle.position.y
								- BORDER_SPACE));
							break;
						case Dir::North:
							path = line2f(path.start, float2(path.start.x, obstacle.position.y + obstacle.dimensions.y
								+ BORDER_SPACE));
							break;
						case Dir::West:
							path = line2f(path.start, float2(obstacle.position.x + obstacle.dimensions.x + BORDER_SPACE, path.start
								.y));
							break;
						case Dir::East:
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

		};
		const float AvoidancePath::BORDER_SPACE = 10.0f;
		template<class C, class R> std::basic_ostream<C, R> & operator <<(
			std::basic_ostream<C, R> & ss, const AvoidancePath& apath) {
			if ((apath->parent != nullptr) && (apath->parent->obstacle.dimensions.x*apath->parent->obstacle.dimensions.y > 0)) {
				float2 min = apath->parent->obstacle.min();
				float2 max = apath->parent->obstacle.max();
				return ss << "DIR:" << apath->direction << " " << apath->path << " " << min << ":" << max;
			}
			else {
				return ss << "DIR:" << apath->direction << " " << apath->path;
			}
		}

		bool operator==(const std::shared_ptr<AvoidancePath>& a, const std::shared_ptr<AvoidancePath>& b) {
			return (a->path.start == b->path.start&&a->path.end == b->path.end);
		}
		bool operator< (const std::shared_ptr<AvoidancePath>& a, const std::shared_ptr<AvoidancePath>& b) {
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

		class AvoidanceRouting {
		protected:
			std::vector<box2f> obstacles;
			static const int DEPTH_LIMIT = 4;
			static const int MAX_PATHS = 128;
		public:
			void computeRoute(const CompositePtr& cache, std::vector<float2>& path, float2 from, float2 to, Dir direction) {
				path.clear();
				std::shared_ptr<AvoidancePath> root;
				getObstacles(obstacles, cache, from, to);
				float2 origFrom = from;
				float2 origTo = to;
				switch (direction) {
				case Dir::South:
					from = float2(from.x, from.y + AvoidancePath::BORDER_SPACE);
					to = float2(to.x, to.y - AvoidancePath::BORDER_SPACE);
					break;
				case Dir::North:
					from = float2(from.x, from.y - AvoidancePath::BORDER_SPACE);
					to = float2(to.x, to.y + AvoidancePath::BORDER_SPACE);
					break;
				case Dir::East:
					from = float2(from.x + AvoidancePath::BORDER_SPACE, from.y);
					to = float2(to.x - AvoidancePath::BORDER_SPACE, to.y);
					break;
				case Dir::West:
					from = float2(from.x - AvoidancePath::BORDER_SPACE, from.y);
					to = float2(to.x + AvoidancePath::BORDER_SPACE, to.y);
					break;
				default:
					break;
				}
				std::priority_queue<std::shared_ptr<AvoidancePath>> queue;
				root = std::shared_ptr<AvoidancePath>(new AvoidancePath(obstacles,from, to, direction));
				queue.push(root);
				std::shared_ptr<AvoidancePath> optNode = root;
				int count = 0;
				bool intersect;
				std::list<std::shared_ptr<AvoidancePath>> history;
				std::shared_ptr<AvoidancePath> head;
				while ((queue.size() > 0) && (count < MAX_PATHS)) {
					head = queue.top();
					queue.pop();
					intersect = false;
					if (!intersect) {
						if (head->distToDest == 0) {
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
								queue.push(child);
								history.push_back(child);
							}
						}
					}
				}
				path = optNode->backTrack();
				path[0] = origFrom;
				if ((direction == Dir::South) || (direction == Dir::North)) {
					if ((optNode->direction == Dir::South) || (optNode->direction == Dir::North)) {
						path[path.size() - 1] = origTo;
					}
					else {
						path.push_back(origTo);
					}
					simplifyPath(path, 0);
				}
				else {
					if ((optNode->direction == Dir::East) || (optNode->direction == Dir::West)) {
						path[path.size() - 1] = origTo;
					}
					else {
						path.push_back(origTo);
					}
					simplifyPath(path, 1);
				}
				if (path.size() == 2) {
					float dx = std::abs(origFrom.x - origTo.x);
					float dy = std::abs(origFrom.y - origTo.y);
					float x2 = origFrom.x + ((origTo.x - origFrom.x) *0.5f);
					float y2 = origFrom.y + ((origTo.y - origFrom.y) *0.5f);
					path.clear();
					path.push_back(origFrom);
					if (dx > dy) {
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
			void getObstacles(std::vector<box2px>& obst, const CompositePtr& cache, float2 from, float2 to) {
				std::vector<RegionPtr> views = cache->getChildren();
				obst.clear();
				for (RegionPtr& view : views) {
					if (dynamic_cast<Node*>(view.get())) {
						bool leaf = true;
						box2px box = view->getBounds(false);
						box.position += view->getDragOffset();
						obst.push_back(box);
					}
				}
			}
			box2px getPathBounds(float2 from, float2 to) const {
				float2 minPt = aly::min(from, to);
				float2 maxPt = aly::max(from, to);
				return box2px(minPt, maxPt - minPt);
			}
			void routeEdge(const  CompositePtr& cache,std::vector<float2>& path, const std::shared_ptr<Connection>& edge) {
				int n = (int)edge->path.size();
				Dir direction = Dir::Unkown;
				PortPtr target = edge->destination;
				PortPtr src = edge->source;
				float2 to = edge->path[n - 1];
				float2 from = edge->path[0];
				from = src->getLocation();
				to = target->getLocation();
				if (src->getType()==PortType::Parent) {
					direction = Dir::East;
				}
				else if (src->getType() == PortType::Child) {
					direction = Dir::West;
				}
				else if (src->getType() == PortType::Output) {
					direction = Dir::South;
				}
				else if (src->getType() == PortType::Input) {
					direction = Dir::North;
				}
				 computeRoute(cache, path, from, to, direction);
			}
			void simplifyPath(std::vector<float2>& path, int parity) {
				float2 st, end, stNext, endNext;
				bool reduce = false;
				line2f l1, l2;
				for (int i = 0; i < path.size() - 4; i++) {
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
		};
	}
}
#endif