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
#include "AlloyMeshIntersector.h"
namespace aly {
	void KDBox::update() {
		minPoint.x = minPoint.y = minPoint.z = 1E30f;
		maxPoint.x = maxPoint.y = maxPoint.z = -1E30f;
		for (std::shared_ptr<KDBox>& child : children) {
			child->update();
			float3 p = child->getMin();
			minPoint.x = aly::min(minPoint.x, p.x);
			minPoint.y = aly::min(minPoint.y, p.y);
			minPoint.z = aly::min(minPoint.z, p.z);
			p = child->getMax();
			maxPoint.x = aly::max(maxPoint.x, p.x);
			maxPoint.y = aly::max(maxPoint.y, p.y);
			maxPoint.z = aly::max(maxPoint.z, p.z);
		}
	}
	double KDBox::distanceToBox(const float3& p) {
		if (inside(p)) {
			return -1;
		}
		float3 ref;
		ref.x = aly::clamp(p.x, maxPoint.x, minPoint.x);
		ref.y = aly::clamp(p.y, maxPoint.y, minPoint.y);
		ref.z = aly::clamp(p.z, maxPoint.z, minPoint.z);
		return distance(p, ref);
	}
	bool KDBox::intersects(const KDBox& test) const {
		double dx = aly::min(test.maxPoint.x, maxPoint.x)
			- aly::max(test.minPoint.x, minPoint.x);
		double dy = aly::min(test.maxPoint.y, maxPoint.y)
			- aly::max(test.minPoint.y, minPoint.y);
		double dz = aly::min(test.maxPoint.z, maxPoint.z)
			- aly::max(test.minPoint.z, minPoint.z);
		return (dx >= 0) && (dy >= 0) && (dz >= 0);
	}
	bool KDBox::inside(const float3& test) const {
		return (test.x >= minPoint.x) && (test.x <= maxPoint.x)
			&& (test.y >= minPoint.y) && (test.y <= maxPoint.y)
			&& (test.z >= minPoint.z) && (test.z <= maxPoint.z);
	}
	bool KDBox::intersectsRay(const float3& org, const float3& dr) const {
		double3 minB = { minPoint.x, minPoint.y, minPoint.z };
		double3 maxB = { maxPoint.x, maxPoint.y, maxPoint.z };
		double3 origin(org);
		double3 dir(dr);
		double3 coord;
		int3 quadrant;
		bool inside = true;
		int i;
		int whichPlane;
		double maxT[] = { 0, 0, 0 };
		double3 candidatePlane;
		for (i = 0; i < 3; i++) {
			if (origin[i] < minB[i]) {
				quadrant[i] = LEFT;
				candidatePlane[i] = minB[i];
				inside = false;
			}
			else if (origin[i] > maxB[i]) {
				quadrant[i] = RIGHT;
				candidatePlane[i] = maxB[i];
				inside = false;
			}
			else {
				quadrant[i] = MIDDLE;
			}
		}
		if (inside) {
			return true;
		}
		/* Calculate T distances to candidate planes */
		for (i = 0; i < 3; i++) {
			if (quadrant[i] != MIDDLE && dir[i] != 0.) {
				maxT[i] = (candidatePlane[i] - origin[i]) / dir[i];
			}
			else {
				maxT[i] = -1.;
			}
		}
		/* Get largest of the maxT's for final choice of intersection */
		whichPlane = 0;
		for (i = 1; i < 3; i++) {
			if (maxT[whichPlane] < maxT[i])
				whichPlane = i;
		}
		/* Check final candidate actually inside box */;
		if (maxT[whichPlane] < 0) {
			return false;
		}
		for (i = 0; i < 3; i++) {
			if (whichPlane != i) {
				coord[i] = origin[i] + maxT[whichPlane] * dir[i];
				if (coord[i] < minB[i] || coord[i] > maxB[i]) {
					return false;
				}
			}
			else {
				coord[i] = candidatePlane[i];
			}
		}
		return true;
	}
	void KDBoxEdge::set(const std::shared_ptr<KDBox>& b, int dim, bool start) {
		box = b;
		this->start = start;
		if (b.get() == nullptr)
			return;
		switch (dim) {
		case 0:
			val = (start) ? b->getMin().x : b->getMax().x;
			break;
		case 1:
			val = (start) ? b->getMin().y : b->getMax().y;
			break;
		case 2:
			val = (start) ? b->getMin().z : b->getMax().z;
			break;
		default:
			val = 0;
		}
	}
	bool KDBox::intersectsSegment(const float3& org, const float3& end) const {
		if (inside(org) || inside(end))
			return true;

		float3 dr = end - org;
		float len = length(dr);
		dr /= len;
		double minB[] = { minPoint.x, minPoint.y, minPoint.z };
		double maxB[] = { maxPoint.x, maxPoint.y, maxPoint.z };
		double origin[] = { org.x, org.y, org.z };
		double dir[] = { dr.x, dr.y, dr.z };
		float3 coord;
		int3 quadrant;
		bool inside = true;
		int i;
		int whichPlane;
		double maxT[] = { 0, 0, 0 };
		double3 candidatePlane;
		for (i = 0; i < 3; i++) {
			if (origin[i] < minB[i]) {
				quadrant[i] = LEFT;
				candidatePlane[i] = minB[i];
				inside = false;
			}
			else if (origin[i] > maxB[i]) {
				quadrant[i] = RIGHT;
				candidatePlane[i] = maxB[i];
				inside = false;
			}
			else {
				quadrant[i] = MIDDLE;
			}
		}
		if (inside) {
			return true;
		}
		/* Calculate T distances to candidate planes */
		for (i = 0; i < 3; i++) {
			if (quadrant[i] != MIDDLE && dir[i] != 0.) {
				maxT[i] = (candidatePlane[i] - origin[i]) / dir[i];
			}
			else {
				maxT[i] = -1.;
			}
		}
		/* Get largest of the maxT's for final choice of intersection */
		whichPlane = 0;
		for (i = 1; i < 3; i++) {
			if (maxT[whichPlane] < maxT[i])
				whichPlane = i;
		}
		/* Check final candidate actually inside box */;
		if (maxT[whichPlane] < 0) {
			return false;
		}
		for (i = 0; i < 3; i++) {
			if (whichPlane != i) {
				coord[i] = (float)(origin[i] + maxT[whichPlane] * dir[i]);
				if (coord[i] < minB[i] || coord[i] > maxB[i]) {
					return false;
				}
			}
			else {
				coord[i] = (float)candidatePlane[i];
			}
		}
		if (distance(org, coord) <= len) {
			return true;
		}
		else {
			return false;
		}
	}
	KDSegment KDSegment::createFromPoints(const float3& p1, const float3& p2) {
		KDSegment seg = KDSegment();
		seg.origin = p1;
		seg.direction = p2 - p1;
		seg.extent = length(seg.direction);
		seg.direction /= seg.extent;
		return seg;
	}
	KDSegment KDSegment::createSegmentFromRay(const float3& p1, const float3& v) {
		KDSegment seg = KDSegment();
		seg.origin = p1;
		seg.direction = v;
		seg.extent = 1E30f;
		return seg;
	}
	double KDSegment::distance(KDSegment& seg) {
		float3 kDiff = origin - seg.origin;
		double fA01 = -dot(direction, seg.direction);
		double fB0 = dot(kDiff, direction);
		double fB1 = -dot(kDiff, seg.direction);
		double fC = lengthSqr(kDiff);
		double fDet = std::abs((double) 1.0 - fA01 * fA01);
		double fS0, fS1, fSqrDist, fExtDet0, fExtDet1, fTmpS0, fTmpS1;
		if (fDet >= ZERO_TOLERANCE) {
			// segments are not parallel
			fS0 = fA01 * fB1 - fB0;
			fS1 = fA01 * fB0 - fB1;
			fExtDet0 = extent * fDet;
			fExtDet1 = seg.extent * fDet;
			if (fS0 >= -fExtDet0) {
				if (fS0 <= fExtDet0) {
					if (fS1 >= -fExtDet1) {
						if (fS1 <= fExtDet1) // region 0 (interior)
						{
							// minimum at two interior points of 3D lines
							double fInvDet = ((double) 1.0) / fDet;
							fS0 *= fInvDet;
							fS1 *= fInvDet;
							fSqrDist = fS0
								* (fS0 + fA01 * fS1 + ((double) 2.0) * fB0)
								+ fS1
								* (fA01 * fS0 + fS1 + ((double) 2.0) * fB1)
								+ fC;
						}
						else // region 3 (side)
						{
							fS1 = seg.extent;
							fTmpS0 = -(fA01 * fS1 + fB0);
							if (fTmpS0 < -extent) {
								fS0 = -extent;
								fSqrDist = fS0
									* (fS0 - ((double) 2.0) * fTmpS0) + fS1
									* (fS1 + ((double) 2.0) * fB1) + fC;
							}
							else if (fTmpS0 <= extent) {
								fS0 = fTmpS0;
								fSqrDist = -fS0 * fS0 + fS1
									* (fS1 + ((double) 2.0) * fB1) + fC;
							}
							else {
								fS0 = extent;
								fSqrDist = fS0
									* (fS0 - ((double) 2.0) * fTmpS0) + fS1
									* (fS1 + ((double) 2.0) * fB1) + fC;
							}
						}
					}
					else // region 7 (side)
					{
						fS1 = -seg.extent;
						fTmpS0 = -(fA01 * fS1 + fB0);
						if (fTmpS0 < -extent) {
							fS0 = -extent;
							fSqrDist = fS0 * (fS0 - ((double) 2.0) * fTmpS0)
								+ fS1 * (fS1 + ((double) 2.0) * fB1) + fC;
						}
						else if (fTmpS0 <= extent) {
							fS0 = fTmpS0;
							fSqrDist = -fS0 * fS0 + fS1
								* (fS1 + ((double) 2.0) * fB1) + fC;
						}
						else {
							fS0 = extent;
							fSqrDist = fS0 * (fS0 - ((double) 2.0) * fTmpS0)
								+ fS1 * (fS1 + ((double) 2.0) * fB1) + fC;
						}
					}
				}
				else {
					if (fS1 >= -fExtDet1) {
						if (fS1 <= fExtDet1) // region 1 (side)
						{
							fS0 = extent;
							fTmpS1 = -(fA01 * fS0 + fB1);
							if (fTmpS1 < -seg.extent) {
								fS1 = -seg.extent;
								fSqrDist = fS1
									* (fS1 - ((double) 2.0) * fTmpS1) + fS0
									* (fS0 + ((double) 2.0) * fB0) + fC;
							}
							else if (fTmpS1 <= seg.extent) {
								fS1 = fTmpS1;
								fSqrDist = -fS1 * fS1 + fS0
									* (fS0 + ((double) 2.0) * fB0) + fC;
							}
							else {
								fS1 = seg.extent;
								fSqrDist = fS1
									* (fS1 - ((double) 2.0) * fTmpS1) + fS0
									* (fS0 + ((double) 2.0) * fB0) + fC;
							}
						}
						else // region 2 (corner)
						{
							fS1 = seg.extent;
							fTmpS0 = -(fA01 * fS1 + fB0);
							if (fTmpS0 < -extent) {
								fS0 = -extent;
								fSqrDist = fS0
									* (fS0 - ((double) 2.0) * fTmpS0) + fS1
									* (fS1 + ((double) 2.0) * fB1) + fC;
							}
							else if (fTmpS0 <= extent) {
								fS0 = fTmpS0;
								fSqrDist = -fS0 * fS0 + fS1
									* (fS1 + ((double) 2.0) * fB1) + fC;
							}
							else {
								fS0 = extent;
								fTmpS1 = -(fA01 * fS0 + fB1);
								if (fTmpS1 < -seg.extent) {
									fS1 = -seg.extent;
									fSqrDist = fS1
										* (fS1 - ((double) 2.0) * fTmpS1)
										+ fS0
										* (fS0 + ((double) 2.0) * fB0) + fC;
								}
								else if (fTmpS1 <= seg.extent) {
									fS1 = fTmpS1;
									fSqrDist = -fS1 * fS1 + fS0
										* (fS0 + ((double) 2.0) * fB0) + fC;
								}
								else {
									fS1 = seg.extent;
									fSqrDist = fS1
										* (fS1 - ((double) 2.0) * fTmpS1)
										+ fS0
										* (fS0 + ((double) 2.0) * fB0) + fC;
								}
							}
						}
					}
					else // region 8 (corner)
					{
						fS1 = -seg.extent;
						fTmpS0 = -(fA01 * fS1 + fB0);
						if (fTmpS0 < -extent) {
							fS0 = -extent;
							fSqrDist = fS0 * (fS0 - ((double) 2.0) * fTmpS0)
								+ fS1 * (fS1 + ((double) 2.0) * fB1) + fC;
						}
						else if (fTmpS0 <= extent) {
							fS0 = fTmpS0;
							fSqrDist = -fS0 * fS0 + fS1
								* (fS1 + ((double) 2.0) * fB1) + fC;
						}
						else {
							fS0 = extent;
							fTmpS1 = -(fA01 * fS0 + fB1);
							if (fTmpS1 > seg.extent) {
								fS1 = seg.extent;
								fSqrDist = fS1
									* (fS1 - ((double) 2.0) * fTmpS1) + fS0
									* (fS0 + ((double) 2.0) * fB0) + fC;
							}
							else if (fTmpS1 >= -seg.extent) {
								fS1 = fTmpS1;
								fSqrDist = -fS1 * fS1 + fS0
									* (fS0 + ((double) 2.0) * fB0) + fC;
							}
							else {
								fS1 = -seg.extent;
								fSqrDist = fS1
									* (fS1 - ((double) 2.0) * fTmpS1) + fS0
									* (fS0 + ((double) 2.0) * fB0) + fC;
							}
						}
					}
				}
			}
			else {
				if (fS1 >= -fExtDet1) {
					if (fS1 <= fExtDet1) // region 5 (side)
					{
						fS0 = -extent;
						fTmpS1 = -(fA01 * fS0 + fB1);
						if (fTmpS1 < -seg.extent) {
							fS1 = -seg.extent;
							fSqrDist = fS1 * (fS1 - ((double) 2.0) * fTmpS1)
								+ fS0 * (fS0 + ((double) 2.0) * fB0) + fC;
						}
						else if (fTmpS1 <= seg.extent) {
							fS1 = fTmpS1;
							fSqrDist = -fS1 * fS1 + fS0
								* (fS0 + ((double) 2.0) * fB0) + fC;
						}
						else {
							fS1 = seg.extent;
							fSqrDist = fS1 * (fS1 - ((double) 2.0) * fTmpS1)
								+ fS0 * (fS0 + ((double) 2.0) * fB0) + fC;
						}
					}
					else // region 4 (corner)
					{
						fS1 = seg.extent;
						fTmpS0 = -(fA01 * fS1 + fB0);
						if (fTmpS0 > extent) {
							fS0 = extent;
							fSqrDist = fS0 * (fS0 - ((double) 2.0) * fTmpS0)
								+ fS1 * (fS1 + ((double) 2.0) * fB1) + fC;
						}
						else if (fTmpS0 >= -extent) {
							fS0 = fTmpS0;
							fSqrDist = -fS0 * fS0 + fS1
								* (fS1 + ((double) 2.0) * fB1) + fC;
						}
						else {
							fS0 = -extent;
							fTmpS1 = -(fA01 * fS0 + fB1);
							if (fTmpS1 < -seg.extent) {
								fS1 = -seg.extent;
								fSqrDist = fS1
									* (fS1 - ((double) 2.0) * fTmpS1) + fS0
									* (fS0 + ((double) 2.0) * fB0) + fC;
							}
							else if (fTmpS1 <= seg.extent) {
								fS1 = fTmpS1;
								fSqrDist = -fS1 * fS1 + fS0
									* (fS0 + ((double) 2.0) * fB0) + fC;
							}
							else {
								fS1 = seg.extent;
								fSqrDist = fS1
									* (fS1 - ((double) 2.0) * fTmpS1) + fS0
									* (fS0 + ((double) 2.0) * fB0) + fC;
							}
						}
					}
				}
				else // region 6 (corner)
				{
					fS1 = -seg.extent;
					fTmpS0 = -(fA01 * fS1 + fB0);
					if (fTmpS0 > extent) {
						fS0 = extent;
						fSqrDist = fS0 * (fS0 - ((double) 2.0) * fTmpS0) + fS1
							* (fS1 + ((double) 2.0) * fB1) + fC;
					}
					else if (fTmpS0 >= -extent) {
						fS0 = fTmpS0;
						fSqrDist = -fS0 * fS0 + fS1
							* (fS1 + ((double) 2.0) * fB1) + fC;
					}
					else {
						fS0 = -extent;
						fTmpS1 = -(fA01 * fS0 + fB1);
						if (fTmpS1 < -seg.extent) {
							fS1 = -seg.extent;
							fSqrDist = fS1 * (fS1 - ((double) 2.0) * fTmpS1)
								+ fS0 * (fS0 + ((double) 2.0) * fB0) + fC;
						}
						else if (fTmpS1 <= seg.extent) {
							fS1 = fTmpS1;
							fSqrDist = -fS1 * fS1 + fS0
								* (fS0 + ((double) 2.0) * fB0) + fC;
						}
						else {
							fS1 = seg.extent;
							fSqrDist = fS1 * (fS1 - ((double) 2.0) * fTmpS1)
								+ fS0 * (fS0 + ((double) 2.0) * fB0) + fC;
						}
					}
				}
			}
		}
		else {
			// The segments are parallel. The average b0 term is designed to
			// ensure symmetry of the function. That is, dist(seg0,seg1) and
			// dist(seg1,seg0) should produce the same number.
			double fE0pE1 = extent + seg.extent;
			double fSign = (fA01 > (double) 0.0 ? (double)-1.0 : (double) 1.0);
			double fB0Avr = ((double) 0.5) * (fB0 - fSign * fB1);
			double fLambda = -fB0Avr;
			if (fLambda < -fE0pE1) {
				fLambda = -fE0pE1;
			}
			else if (fLambda > fE0pE1) {
				fLambda = fE0pE1;
			}

			fS1 = -fSign * fLambda * seg.extent / fE0pE1;
			fS0 = fLambda + fSign * fS1;
			fSqrDist = fLambda * (fLambda + ((double) 2.0) * fB0Avr) + fC;
		}

		float3 m_kClosestPoint0;
		float3 m_kClosestPoint1;

		float3 tmpDir = direction;
		tmpDir *= (float)fS0;
		m_kClosestPoint0 = origin + tmpDir;
		setLastIntersect(m_kClosestPoint1);

		tmpDir = seg.direction;
		tmpDir *= (float)fS1;
		m_kClosestPoint1 = seg.origin + tmpDir;
		seg.setLastIntersect(m_kClosestPoint1);
		return std::sqrt(std::abs(fSqrDist));
	}
	bool KDSegment::intersectsPoint(const float3& test) {
		static const float EPS = 5e-4f;
		float3 v1 = test - origin;
		float3 v2 = direction;
		double dt = dot(v1, direction);
		if (dt <= EPS)
			return false;
		v2 *= (float)dt;
		v1 -= v2;
		if (length(v1) > EPS)
			return false;
		if (dt < extent - EPS)
			return true;
		return false;
	}
	float3 KDSegment::intersectsPlane(const float3& center, const float3& kNormal) {
		static const float EPS = 5e-4f;
		float A = kNormal.x;
		float B = kNormal.y;
		float C = kNormal.z;
		float numer = A * (center.x - origin.x) + B * (center.y - origin.y) + C
			* (center.z - origin.z);
		float denom = (A * direction.x + B * direction.y + C * direction.z);
		if (denom != 0) {
			float u = numer / denom;
			if (u >EPS && u < extent - EPS) {
				return origin + direction*u;
			}
			else {
				return float3(std::numeric_limits<float>::max());
			}
		}
		else {
			return float3(std::numeric_limits<float>::max());
		}
	}
	double3 KDTriangle::toBary(const float3& p) const {
		double a = pts[0].x - pts[2].x;
		double b = pts[1].x - pts[2].x;
		double c = pts[2].x - p.x;

		double d = pts[0].y - pts[2].y;
		double e = pts[1].y - pts[2].y;
		double f = pts[2].y - p.y;

		double g = pts[0].z - pts[2].z;
		double h = pts[1].z - pts[2].z;
		double i = pts[2].z - p.z;

		double l1 = (b * (f + i) - c * (e + h)) / (a * (e + h) - b * (d + g));
		double l2 = (a * (f + i) - c * (d + g)) / (b * (d + g) - a * (e + h));
		if (l1 > 1 || l2 > 1 || l1 + l2 > 1 || l1 < 0 || l2 < 0) {
			l1 = clamp(l1, 0.0, 1.0);
			l2 = clamp(l2, 0.0, 1.0);
			if (l1 + l2 > 1) {
				double diff = 0.5 * (1 - l1 - l2);
				l1 += diff;
				l2 += diff;
			}
		}
		return double3(l1, l2, 1 - l1 - l2);
	}
	float3 KDTriangle::intersectionPoint(const float3& p1, const float3& p2) {
		static const float EPS = 5e-4f;
		// compute the offset origin, edges, and normal
		KDSegment seg = KDSegment::createFromPoints(p1, p2);
		float3 dr = seg.direction;
		float len = seg.extent;
		float3 kDiff = p1 - pts[0];
		float3 kEdge1 = pts[1] - pts[0];
		float3 kEdge2 = pts[2] - pts[0];
		float3 kNormal = cross(kEdge1, kEdge2);
		// Solve Q + t*D = b1*E1 + b2*E2 (Q = kDiff, D = segment direction,
		// E1 = kEdge1, E2 = kEdge2, N = Cross(E1,E2)) by
		// |Dot(D,N)|*b1 = sign(Dot(D,N))*Dot(D,Cross(Q,E2))
		// |Dot(D,N)|*b2 = sign(Dot(D,N))*Dot(D,Cross(E1,Q))
		// |Dot(D,N)|*t = -sign(Dot(D,N))*Dot(Q,N)
		double fDdN = dot(dr, kNormal);
		double fSign;
		if (fDdN > 0) {
			fSign = (double) 1.0;
		}
		else if (fDdN < 0) {
			fSign = (double)-1.0;
			fDdN = -fDdN;
		}
		else {
			return float3(std::numeric_limits<float>::max());
		}
		float3 crs = cross(kDiff, kEdge2);
		double fDdQxE2 = fSign * dot(dr, crs);
		if (fDdQxE2 >= (double) 0.0) {
			crs = cross(kEdge1, kDiff);
			double fDdE1xQ = fSign * dot(dr, crs);
			if (fDdE1xQ >= (double) 0.0) {
				if (fDdQxE2 + fDdE1xQ <= fDdN) {
					// line intersects triangle, check if segment does
					double fQdN = -fSign * dot(kDiff, kNormal);
					double fExtDdN = len * fDdN;
					/*
					* if (-fExtDdN <= fQdN && fQdN <= fExtDdN) {
					*/
					double fInv = ((double) 1.0) / fDdN;
					double m_fSegmentT = fQdN * fInv;

					if (m_fSegmentT >= 0 && m_fSegmentT <= len) {
						return p1 + dr*(float)m_fSegmentT;
					}
				}
			}
		}
		float3 intersect = NO_HIT_PT;
		double d;
		KDSegment e1 = KDSegment::createFromPoints(pts[0], pts[1]);
		d = seg.distance(e1);
		if (d < EPS) {
			intersect = e1.getLastIntersect();
		}
		else {
			KDSegment e2 = KDSegment::createFromPoints(pts[1], pts[2]);
			d = seg.distance(e2);
			if (d < EPS) {
				intersect = e2.getLastIntersect();
			}
			else {
				KDSegment e3 = KDSegment::createFromPoints(pts[2], pts[0]);
				d = seg.distance(e3);
				if (d < EPS) {
					intersect = e3.getLastIntersect();
				}
			}
		}
		if (intersect != NO_HIT_PT) {
			kDiff = intersect - p1;
			double l = dot(seg.direction, kDiff);
			if (l >= 0 && l <= len) {
				lastIntersectionPoint = intersect;
				return intersect;
			}
		}
		lastIntersectionPoint = NO_HIT_PT;
		return NO_HIT_PT;

	}
	float3 KDTriangle::intersectionPointRay(const float3& org, const float3& v) {
		static const float EPS = 5e-4f;
		// compute the offset origin, edges, and normal
		KDSegment seg = KDSegment::createSegmentFromRay(org, v);
		float3 dr = seg.direction;
		float len = seg.extent;
		float3 kDiff = org - pts[0];
		float3 kEdge1 = pts[1] - pts[0];
		float3 kEdge2 = pts[2] - pts[0];
		float3 kNormal = cross(kEdge1, kEdge2);

		// Solve Q + t*D = b1*E1 + b2*E2 (Q = kDiff, D = segment direction,
		// E1 = kEdge1, E2 = kEdge2, N = Cross(E1,E2)) by
		// |Dot(D,N)|*b1 = sign(Dot(D,N))*Dot(D,Cross(Q,E2))
		// |Dot(D,N)|*b2 = sign(Dot(D,N))*Dot(D,Cross(E1,Q))
		// |Dot(D,N)|*t = -sign(Dot(D,N))*Dot(Q,N)
		double fDdN = dot(dr, kNormal);
		double fSign;
		if (fDdN > 0) {
			fSign = (double) 1.0;
		}
		else if (fDdN < 0) {
			fSign = (double)-1.0;
			fDdN = -fDdN;
		}
		else {
			// System.out.println("SEGMENT PARALLEL !");
			// Segment and triangle are parallel, call it a "no intersection"
			// even if the segment does intersect.
			return NO_HIT_PT;
		}
		float3 crs = cross(kDiff, kEdge2);
		double fDdQxE2 = fSign * dot(dr, crs);
		if (fDdQxE2 >= (double) 0.0) {
			crs = cross(kEdge1, kDiff);
			double fDdE1xQ = fSign * dot(dr, crs);
			if (fDdE1xQ >= (double) 0.0) {
				if (fDdQxE2 + fDdE1xQ <= fDdN) {
					// line intersects triangle, check if segment does
					double fQdN = -fSign * dot(kDiff, kNormal);
					double fExtDdN = len * fDdN;
					if (-fExtDdN <= fQdN && fQdN <= fExtDdN) {
						// segment intersects triangle
						double fInv = ((double) 1.0) / fDdN;
						double m_fSegmentT = fQdN * fInv;
						// double m_fTriB1 = fDdQxE2*fInv;
						// double m_fTriB2 = fDdE1xQ*fInv;
						// double m_fTriB0 = (double)1.0 - m_fTriB1 - m_fTriB2;
						return org + dr*(float)m_fSegmentT;
					}
				}
			}
		}
		float3 intersect = NO_HIT_PT;
		double d;
		KDSegment e1 = KDSegment::createFromPoints(pts[0], pts[1]);
		d = seg.distance(e1);
		if (d < EPS) {
			intersect = e1.getLastIntersect();
		}
		else {
			KDSegment e2 = KDSegment::createFromPoints(pts[1], pts[2]);
			d = seg.distance(e2);
			if (d < EPS) {
				intersect = e2.getLastIntersect();
			}
			else {
				KDSegment e3 = KDSegment::createFromPoints(pts[2], pts[0]);
				d = seg.distance(e3);
				if (d < EPS) {
					intersect = e3.getLastIntersect();
				}
			}
		}
		lastIntersectionPoint = intersect;
		return intersect;
	}
	double KDTriangle::distance(const float3& p) {
		float3 p1 = pts[0];
		float3 p2 = pts[1];
		float3 p3 = pts[2];
		float3 kDiff = (p1 - p);
		float3 kEdge0 = (p2 - p1);
		float3 kEdge1 = (p3 - p1);
		float fA00 = (float)lengthSqr(kEdge0);
		float fA01 = dot(kEdge0, kEdge1);
		float fA11 = lengthSqr(kEdge1);
		float fB0 = dot(kDiff, kEdge0);
		float fB1 = dot(kDiff, kEdge1);
		float fC = lengthSqr(kDiff);
		float fDet = std::abs(fA00 * fA11 - fA01 * fA01);
		float fS = fA01 * fB1 - fA11 * fB0;
		float fT = fA01 * fB0 - fA00 * fB1;
		float fSqrDistance;
		if (fS + fT <= fDet) {
			if (fS < (float) 0.0) {
				if (fT < (float) 0.0) // region 4
				{
					if (fB0 < (float) 0.0) {
						fT = (float) 0.0;
						if (-fB0 >= fA00) {
							fS = (float) 1.0;
							fSqrDistance = fA00 + ((float) 2.0) * fB0 + fC;
						}
						else {
							fS = -fB0 / fA00;
							fSqrDistance = fB0 * fS + fC;
						}

					}
					else {
						fS = (float) 0.0;
						if (fB1 >= (float) 0.0) {
							fT = (float) 0.0;
							fSqrDistance = fC;
						}
						else if (-fB1 >= fA11) {
							fT = (float) 1.0;
							fSqrDistance = fA11 + ((float) 2.0) * fB1 + fC;
						}
						else {
							fT = -fB1 / fA11;
							fSqrDistance = fB1 * fT + fC;
						}
					}

				}
				else // region 3
				{
					fS = (float) 0.0;
					if (fB1 >= (float) 0.0) {
						fT = (float) 0.0;
						fSqrDistance = fC;
					}
					else if (-fB1 >= fA11) {
						fT = (float) 1.0;
						fSqrDistance = fA11 + ((float) 2.0) * fB1 + fC;
					}
					else {
						fT = -fB1 / fA11;
						fSqrDistance = fB1 * fT + fC;
					}
				}
			}
			else if (fT < (float) 0.0) // region 5
			{
				fT = (float) 0.0;
				if (fB0 >= (float) 0.0) {
					fS = (float) 0.0;
					fSqrDistance = fC;
				}
				else if (-fB0 >= fA00) {
					fS = (float) 1.0;
					fSqrDistance = fA00 + ((float) 2.0) * fB0 + fC;
				}
				else {
					fS = -fB0 / fA00;
					fSqrDistance = fB0 * fS + fC;
				}
			}
			else // region 0
			{
				// minimum at interior point
				float fInvDet = ((float) 1.0) / fDet;
				fS *= fInvDet;
				fT *= fInvDet;
				fSqrDistance = fS
					* (fA00 * fS + fA01 * fT + ((float) 2.0) * fB0) + fT
					* (fA01 * fS + fA11 * fT + ((float) 2.0) * fB1) + fC;
			}
		}
		else {
			float fTmp0, fTmp1, fNumer, fDenom;

			if (fS < (float) 0.0) // region 2
			{
				fTmp0 = fA01 + fB0;
				fTmp1 = fA11 + fB1;
				if (fTmp1 > fTmp0) {
					fNumer = fTmp1 - fTmp0;
					fDenom = fA00 - 2.0f * fA01 + fA11;
					if (fNumer >= fDenom) {
						fS = (float) 1.0;
						fT = (float) 0.0;
						fSqrDistance = fA00 + ((float) 2.0) * fB0 + fC;
					}
					else {
						fS = fNumer / fDenom;
						fT = (float) 1.0 - fS;
						fSqrDistance = fS
							* (fA00 * fS + fA01 * fT + 2.0f * fB0) + fT
							* (fA01 * fS + fA11 * fT + ((float) 2.0) * fB1)
							+ fC;
					}
				}
				else {
					fS = (float) 0.0;
					if (fTmp1 <= (float) 0.0) {
						fT = (float) 1.0;
						fSqrDistance = fA11 + ((float) 2.0) * fB1 + fC;
					}
					else if (fB1 >= (float) 0.0) {
						fT = (float) 0.0;
						fSqrDistance = fC;
					}
					else {
						fT = -fB1 / fA11;
						fSqrDistance = fB1 * fT + fC;
					}
				}
			}
			else if (fT < (float) 0.0) // region 6
			{
				fTmp0 = fA01 + fB1;
				fTmp1 = fA00 + fB0;
				if (fTmp1 > fTmp0) {
					fNumer = fTmp1 - fTmp0;
					fDenom = fA00 - ((float) 2.0) * fA01 + fA11;
					if (fNumer >= fDenom) {
						fT = (float) 1.0;
						fS = (float) 0.0;
						fSqrDistance = fA11 + ((float) 2.0) * fB1 + fC;
					}
					else {
						fT = fNumer / fDenom;
						fS = (float) 1.0 - fT;
						fSqrDistance = fS
							* (fA00 * fS + fA01 * fT + ((float) 2.0) * fB0)
							+ fT
							* (fA01 * fS + fA11 * fT + ((float) 2.0) * fB1)
							+ fC;
					}
				}
				else {
					fT = (float) 0.0;
					if (fTmp1 <= (float) 0.0) {
						fS = (float) 1.0;
						fSqrDistance = fA00 + ((float) 2.0) * fB0 + fC;
					}
					else if (fB0 >= (float) 0.0) {
						fS = (float) 0.0;
						fSqrDistance = fC;
					}
					else {
						fS = -fB0 / fA00;
						fSqrDistance = fB0 * fS + fC;
					}
				}
			}
			else // region 1
			{
				fNumer = fA11 + fB1 - fA01 - fB0;
				if (fNumer <= (float) 0.0) {
					fS = (float) 0.0;
					fT = (float) 1.0;
					fSqrDistance = fA11 + ((float) 2.0) * fB1 + fC;
				}
				else {
					fDenom = fA00 - 2.0f * fA01 + fA11;
					if (fNumer >= fDenom) {
						fS = (float) 1.0;
						fT = (float) 0.0;
						fSqrDistance = fA00 + ((float) 2.0) * fB0 + fC;
					}
					else {
						fS = fNumer / fDenom;
						fT = (float) 1.0 - fS;
						fSqrDistance = fS
							* (fA00 * fS + fA01 * fT + ((float) 2.0) * fB0)
							+ fT
							* (fA01 * fS + fA11 * fT + ((float) 2.0) * fB1)
							+ fC;
					}
				}
			}
		}

		// account for numerical round-off error
		if (fSqrDistance < (float) 0.0) {
			fSqrDistance = (float) 0.0;
		}
		lastIntersectionPoint = pts[0] + kEdge0*(float)fS + kEdge1*(float)fT;
		return std::sqrt(fSqrDistance);
	}
	void KDTree::build(const Mesh& mesh, int maxDepth) {
		root = std::shared_ptr<KDBox>(new KDBox());
		for (uint3 tri : mesh.triIndexes.data) {
			std::shared_ptr<KDTriangle> t = std::shared_ptr<KDTriangle>(new KDTriangle(1));
			root->getChildren().push_back(t);
		}
		root->update();
		buildTree(root, maxDepth);
	}
	void KDTree::buildTree(const std::shared_ptr<KDBox>& initBox, int maxDepth) {
		std::list<std::shared_ptr<KDBox>> boxes;
		boxes.push_back(initBox);
		std::shared_ptr<KDBox> box = initBox;
		int sz;
		int splitPos;
		std::vector<KDBoxEdge> edges;
		std::shared_ptr<KDBox> leftChild;
		std::shared_ptr<KDBox> rightChild;
		int initSz = (int)box->getChildren().size() * 2;
		edges.resize(initSz);
		int depthCount = 0;
		while (boxes.size()>0) {
			box = boxes.front();
			boxes.pop_front();
			if (box->getDepth() > maxDepth)
				continue;
			if (box->getDepth() > depthCount) {
				depthCount = box->getDepth();
			}
			std::vector<std::shared_ptr<KDBox>>& children = box->getChildren();
			if (children.size() < 2)
				continue;
			sz = (int)children.size() * 2;
			splitPos = splitPosition(children, edges, box);
			if (splitPos <= 0 || splitPos == sz - 1) {
				continue;
			}
			leftChild = std::shared_ptr<KDBox>(new KDBox(box->getDepth() + 1));
			rightChild = std::shared_ptr<KDBox>(new KDBox(box->getDepth() + 1));
			std::vector<std::shared_ptr<KDBox>>& lchildren = leftChild->getChildren();
			std::vector<std::shared_ptr<KDBox>>& rchildren = rightChild->getChildren();
			for (int i = 0; i < splitPos; i++) {
				if (edges[i].isMin()) {
					edges[i].getBox()->setDepth(leftChild->getDepth() + 1);
					lchildren.push_back(edges[i].getBox());
				}
			}
			for (int i = splitPos + 1; i < sz; i++) {
				if (!edges[i].isMin()) {
					edges[i].getBox()->setDepth(rightChild->getDepth() + 1);
					rchildren.push_back(edges[i].getBox());
				}
			}
			if (lchildren.size() >= children.size()
				|| rchildren.size() >= children.size()) {
				continue;
			}
			leftChild->update();
			rightChild->update();
			children.clear();
			children.push_back(leftChild);
			children.push_back(rightChild);
			box->update();
			boxes.push_back(leftChild);
			boxes.push_back(rightChild);
		}
	}
	int KDTree::splitPosition(std::vector<std::shared_ptr<KDBox>>& children, std::vector<KDBoxEdge>& edges, const std::shared_ptr<KDBox>& box) {
		int nAbove = 0;
		int nBelow = 0;
		double pAbove = 0;
		double pBelow = 0;
		double cost = 0;
		int dim = 0;
		int bestSplit = -1;
		double sum = 0;
		double prod = 0;
		double voxel;
		double val;
		double minCost;
		int sz;
		KDBoxEdge e1, e2;
		float3 v = box->getMax() - box->getMin();
		if (v.x > v.y && v.x > v.z)
			dim = 0;
		else if (v.y > v.z)
			dim = 1;
		else
			dim = 2;
		int retry = 3;
		voxel = 1.0 / (v.x + v.y + v.z);
		do {
			bestSplit = -1;
			switch (dim) {
			case 0:
				sum = v.y + v.z;
				prod = v.y * v.z;
				break;
			case 1:
				sum = v.x + v.z;
				prod = v.x * v.z;
				break;
			case 2:
				sum = v.x + v.y;
				prod = v.x * v.y;
				break;
			}
			sz = (int)children.size() * 2;
			e1.set(box, dim, true);
			e2.set(box, dim, false);
			for (int i = 0; i < sz; i++) {
				std::shared_ptr<KDBox> child = children[i / 2];
				edges[i].set(child, dim, (i % 2 == 1) ? true : false);
			}
			std::sort(edges.begin(), edges.end());
			nBelow = 0;
			nAbove = (int)children.size();
			// Do not assume prior minimum cost because the spherical
			// orientation
			// is in some sense optimal when rays are cast from outside the
			// sphere.
			// If minimum cost is assumed, then no branches in kd-tree will be
			// generated
			// minCost=intersectCost*nAbove;
			minCost = 1E30;
			for (int i = 0; i < sz; i++) {
				if (!edges[i].isMin())
					nAbove--;
				val = edges[i].getValue();
				if (val > e1.getValue() && e2.getValue() > val) {
					pBelow = (sum * (val - e1.getValue()) + prod) * voxel;
					pAbove = (sum * (e2.getValue() - val) + prod) * voxel;
					cost = traversalCost
						+ intersectCost
						* (1 - ((nAbove == 0 || nBelow == 0) ? emptyBonus
							: 0)) * (pBelow * nBelow + pAbove * nAbove);
					if (cost < minCost) {
						minCost = cost;
						bestSplit = i;
					}
				}
				if (edges[i].isMin())
					nBelow++;
			}
			dim = (dim + 1) % 3;
			retry--;
		} while (bestSplit == -1 && retry > 0);
		return bestSplit;
	}
}