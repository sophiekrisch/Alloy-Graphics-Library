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
#include "AlloyPointLocator.h"
using namespace libkdtree;
namespace aly {
	const float2i KDTreeLocator2f::NO_POINT_FOUND = float2i(float2(std::numeric_limits<float>::max()), -1);
	const float3i KDTreeLocator3f::NO_POINT_FOUND = float3i(float3(std::numeric_limits<float>::max()), -1);
	void KDTreeLocator2f::insert(const float2i& pt) {
		indexCount = std::max(indexCount,(int64_t)(pt.index + 1));
		locator.insert(pt);
	}
	int64_t KDTreeLocator2f::insert(const float2& pt) {
		float2i pti(pt, indexCount++);
		locator.insert(pti);
		return pti.index;
	}
	void KDTreeLocator2f::insert(const std::vector<float2>& pts) {
		std::vector<float2i> ptsi;
		ptsi.reserve(pts.size());
		for (float2 pt : pts) {
			float2i pti(pt, indexCount++);
			ptsi.push_back(pti);
		}
		locator.insert(ptsi.begin(), ptsi.end());
	}
	void KDTreeLocator2f::insert(const std::vector<float2i>& ptsi) {
		for (float2i pt : ptsi) {
			indexCount = std::max(indexCount, (int64_t)(pt.index + 1));
		}
		locator.insert(ptsi.begin(), ptsi.end());
	}
	float2i KDTreeLocator2f::closestPoint(float2 pt, float maxDistance) const {
		float2i query(pt,-1);
		auto result = locator.find_nearest(query,maxDistance);
		if (result.first != locator.end()) {
			return *(result.first);
		}
		else {
			return NO_POINT_FOUND;
		}
	}
	float2i KDTreeLocator2f::closestPointExact(float2i query) const {
		auto result = locator.find_exact(query);
		if (result != locator.end()) {
			return *(result);
		}
		else {
			return NO_POINT_FOUND;
		}
	}
	float2i KDTreeLocator2f::closestPoint(float2 pt) const {
		float2i query(pt, -1);
		auto result = locator.find_nearest(query);
		if (result.first != locator.end()) {
			return *(result.first);
		}
		else {
			return NO_POINT_FOUND;
		}
	}
	void KDTreeLocator2f::findNearest(float2 pt,float maxDistance,std::vector<float2i>& pts) const {
		float2i query(pt, -1);
		pts.clear();
		locator.find_within_range(query,maxDistance, std::back_insert_iterator<std::vector<float2i> >(pts));
		std::sort(pts.begin(), pts.end(), [=](const float2i& a, const float2i& b) {
			return (distanceSqr(pt, a) < distanceSqr(pt, b));
		});
	}

	void KDTreeLocator3f::insert(const float3i& pt) {
		indexCount = std::max(indexCount, (int64_t)(pt.index + 1));
		locator.insert(pt);
	}
	int64_t KDTreeLocator3f::insert(const float3& pt) {
		float3i pti(pt, indexCount++);
		locator.insert(pti);
		return pti.index;
	}
	void KDTreeLocator3f::insert(const std::vector<float3>& pts) {
		std::vector<float3i> ptsi;
		ptsi.reserve(pts.size());
		for (float3 pt : pts) {
			float3i pti(pt, indexCount++);
			ptsi.push_back(pti);
		}
		locator.insert(ptsi.begin(), ptsi.end());
	}
	void KDTreeLocator3f::insert(const std::vector<float3i>& ptsi) {
		for (float3i pt : ptsi) {
			indexCount = std::max(indexCount, (int64_t)(pt.index + 1));
		}
		locator.insert(ptsi.begin(), ptsi.end());
	}
	float3i KDTreeLocator3f::closestPoint(float3 pt, float maxDistance) const {
		float3i query(pt, -1);
		auto result = locator.find_nearest(query, maxDistance);
		if (result.first != locator.end()) {
			return *(result.first);
		}
		else {
			return NO_POINT_FOUND;
		}
	}
	float3i KDTreeLocator3f::closestPoint(float3 pt) const {
		float3i query(pt, -1);
		auto result = locator.find_nearest(query);
		if (result.first != locator.end()) {
			return *(result.first);
		}
		else {
			return NO_POINT_FOUND;
		}
	}
	float3i KDTreeLocator3f::closestPointExact(float3i query) const {
		auto result = locator.find_exact(query);
		if (result != locator.end()) {
			return *(result);
		}
		else {
			return NO_POINT_FOUND;
		}
	}
	void KDTreeLocator3f::findNearest(float3 pt, float maxDistance, std::vector<float3i>& pts) const {
		float3i query(pt, -1);
		pts.clear();
		locator.find_within_range(query, maxDistance, std::back_insert_iterator<std::vector<float3i> >(pts));
		std::sort(pts.begin(),pts.end(), [=](const float3i& a, const float3i& b) {
			return (distanceSqr(pt, a) < distanceSqr(pt, b));
		});
	}
}