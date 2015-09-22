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
#ifndef ALLOYDENSEMATRIX_H_
#define ALLOYDENSEMATRIX_H_
#include "AlloyVector.h"
#include "cereal/types/vector.hpp"
#include "cereal/types/list.hpp"
#include "cereal/types/tuple.hpp"
#include "cereal/types/map.hpp"
#include "cereal/types/memory.hpp"
#include <vector>
#include <list>
#include <map>
namespace aly {

template<class T, int C> struct DenseMatrix {
private:
	std::vector<std::vector<vec<T, C>>>storage;
public:
	int rows, cols;
	template<class Archive> void serialize(Archive & archive) {
		archive(CEREAL_NVP(rows), CEREAL_NVP(cols),
				cereal::make_nvp(
						MakeString() << "matrix"<<C,
						storage));
	}
	std::vector<size_t, vec<T, C>>& operator[](size_t i) {
		if (i >= rows || i < 0)
		throw std::runtime_error(
				MakeString() << "Index (" << i
				<< ",*) exceeds matrix bounds [" << rows << ","
				<< cols << "]");
		return storage[i];
	}
	const std::vector<size_t, vec<T, C>>& operator[](size_t i) const {
		if (i >= rows || i < 0)
		throw std::runtime_error(
				MakeString() << "Index (" << i
				<< ",*) exceeds matrix bounds [" << rows << ","
				<< cols << "]");
		return storage[i];
	}
	DenseMatrix(): rows(0),cols(0) {
	}
	DenseMatrix(size_t rows, size_t cols) :
	rows(rows), cols(cols),storage(rows,std::vector<vec<T,C>>(cols)) {
	}
	void set(size_t i, size_t j, const vec<T, C>& value) {
		if (i >= rows || j >= cols || i < 0 || j < 0)
		throw std::runtime_error(
				MakeString() << "Index (" << i << "," << j
				<< ") exceeds matrix bounds [" << rows << ","
				<< cols << "]");
		storage[i][j] = value;
	}
	void set(size_t i, size_t j, const T& value) {
		if (i >= rows || j >= cols || i < 0 || j < 0)
		throw std::runtime_error(
				MakeString() << "Index (" << i << "," << j
				<< ") exceeds matrix bounds [" << rows << ","
				<< cols << "]");
		storage[i][j] = vec<T, C>(value);
	}
	vec<T, C>& operator()(size_t i, size_t j) {
		if (i >= rows || j >= cols || i < 0 || j < 0)
		throw std::runtime_error(
				MakeString() << "Index (" << i << "," << j
				<< ") exceeds matrix bounds [" << rows << ","
				<< cols << "]");
		return storage[i][j];
	}

	vec<T, C> get(size_t i, size_t j) const {
		if (i >= rows || j >= cols || i < 0 || j < 0)
		throw std::runtime_error(
				MakeString() << "Index (" << i << "," << j
				<< ") exceeds matrix bounds [" << rows << ","
				<< cols << "]");
		return storage[i][j];
	}
	const vec<T, C>& operator()(size_t i, size_t j) const {
		return storage[i][j];
	}
	DenseMatrix<T, C> transpose() const {
		DenseMatrix<T, C> M(cols, rows);
		for (int i = 0; i < rows; i++) {
			for (int j=0;j < cols;j++) {
				M[j][i]=storage[i][j];
			}
		}
		return M;
	}
	static DenseMatrix<T, C> identity(size_t M, size_t N) {
		DenseMatrix<T, C> A(M, N);
		int K = (int) aly::min(M, N);
		for (int k = 0; k < K; k++) {
			A[k][k] = vec<T, C>(T(1));
		}
		return A;
	}
	static DenseMatrix<T, C> diagonal(const Vector<T, C>& v) {
		DenseMatrix<T, C> A(v.size(), v.size());
		for (int k = 0; k < (int) v.size(); k++) {
			A[k][k] = v[k];
		}
		return A;
	}
};
template<class A, class B, class T, int C> std::basic_ostream<A, B> & operator <<(
		std::basic_ostream<A, B> & ss, const DenseMatrix<T, C>& M) {
	ss << "\n";
	for (int n = 0; n < M.rows; n++) {
		ss << "[";
		for (int m = 0; m < M.cols; m++) {
			ss << std::setprecision(10) << std::setw(16) << A(n, m)
					<< ((m < M.cols - 1) ? "," : "]\n");
		}
	}
	return ss;
}

template<class T, int C> Vector<T, C> operator*(const DenseMatrix<T, 1>& A,
		const Vector<T, C>& v) {
	Vector<T, C> out(A.rows);
	for (int i = 0; i < A.rows; i++) {
		vec<T, C> sum(0.0);
		for (int j = 0; j < A.cols; j++) {
			sum += A[i][j].x * v[j];
		}
		out[i] = sum;
	}
	return out;
}
template<class T, int C> Vector<T, C> operator*(const DenseMatrix<T, C>& A,
		const Vector<T, C>& v) {
	Vector<T, C> out(A.rows);
	for (int i = 0; i < A.rows; i++) {
		vec<T, C> sum(0.0);
		for (int j = 0; j < A.cols; j++) {
			sum += A[i][j] * v[j];
		}
		out[i] = sum;
	}
	return out;
}
template<class T, int C> DenseMatrix<T, C> operator*(const DenseMatrix<T, C>& A,
		const DenseMatrix<T, C>& B) {
	if (A.cols != B.rows)
		throw std::runtime_error(
				MakeString()
						<< "Cannot multiply matrices. Inner dimensions do not match. "
						<< "[" << A.rows << "," << A.cols << "] * [" << B.rows
						<< "," << B.cols << "]");
	DenseMatrix<T, C> out(A.rows, B.cols);
	for (int i = 0; i < out.rows; i++) {
		for (int j = 0; j < out.cols; j++) {
			vec<T, C> sum(0.0);
			for (int k = 0; k < A.cols; k++) {
				sum += A[i][k] * B[k][j];
			}
			out[i][j] = sum;
		}
	}
	return out;
}
template<class T, int C> DenseMatrix<T, C> operator*(const DenseMatrix<T, 1>& A,
		const DenseMatrix<T, C>& B) {
	if (A.cols != B.rows)
		throw std::runtime_error(
				MakeString()
						<< "Cannot multiply matrices. Inner dimensions do not match. "
						<< "[" << A.rows << "," << A.cols << "] * [" << B.rows
						<< "," << B.cols << "]");
	DenseMatrix<T, C> out(A.rows, B.cols);
	for (int i = 0; i < out.rows; i++) {
		for (int j = 0; j < out.cols; j++) {
			vec<T, C> sum(0.0);
			for (int k = 0; k < A.cols; k++) {
				sum += A[i][k].x * B[k][j];
			}
			out[i][j] = sum;
		}
	}
	return out;
}
template<class T, int C> DenseMatrix<T, C> operator*(const DenseMatrix<T, C>& B,
		const DenseMatrix<T, 1>& A) {
	if (A.cols != B.rows)
		throw std::runtime_error(
				MakeString()
						<< "Cannot multiply matrices. Inner dimensions do not match. "
						<< "[" << A.rows << "," << A.cols << "] * [" << B.rows
						<< "," << B.cols << "]");
	DenseMatrix<T, C> out(A.rows, B.cols);
	for (int i = 0; i < out.rows; i++) {
		for (int j = 0; j < out.cols; j++) {
			vec<T, C> sum(0.0);
			for (int k = 0; k < A.cols; k++) {
				sum += A[i][k] * B[k][j].x;
			}
			out[i][j] = sum;
		}
	}
	return out;
}
template<class T, int C> DenseMatrix<T, 1> operator*(const DenseMatrix<T, 1>& B,
		const DenseMatrix<T, 1>& A) {
	if (A.cols != B.rows)
		throw std::runtime_error(
				MakeString()
						<< "Cannot multiply matrices. Inner dimensions do not match. "
						<< "[" << A.rows << "," << A.cols << "] * [" << B.rows
						<< "," << B.cols << "]");
	DenseMatrix<T, 1> out(A.rows, B.cols);
	for (int i = 0; i < out.rows; i++) {
		for (int j = 0; j < out.cols; j++) {
			vec<T, 1> sum(0.0);
			for (int k = 0; k < A.cols; k++) {
				sum.x += A[i][k].x * B[k][j].x;
			}
			out[i][j] = sum;
		}
	}
	return out;
}
template<class T, int C> DenseMatrix<T, C> operator-(
		const DenseMatrix<T, C>& A) {
	DenseMatrix<T, C> out(A.rows, A.cols);
	for (int i = 0; i < (int) out.rows; i++) {
		for (int j = 0; j < out.cols; j++) {
			out[i][j] = -A[i][j];
		}
	}
	return out;
}
template<class T, int C> Vector<T, C> operator*(const DenseMatrix<T, C>& A,
		const vec<T, C>& v) {
	DenseMatrix<T, C> out(A.rows, A.cols);
	for (int i = 0; i < A.rows; i++) {
		for (int j = 0; j < A.cols; j++) {
			out[i][j] = A[i][j] * v;
		}
	}
	return out;
}
template<class T, int C> Vector<T, C> operator/(const DenseMatrix<T, C>& A,
		const vec<T, C>& v) {
	DenseMatrix<T, C> out(A.rows, A.cols);
	for (int i = 0; i < A.rows; i++) {
		for (int j = 0; j < A.cols; j++) {
			out[i][j] = A[i][j] / v;
		}
	}
	return out;
}
template<class T, int C> Vector<T, C> operator+(const DenseMatrix<T, C>& A,
		const vec<T, C>& v) {
	DenseMatrix<T, C> out(A.rows, A.cols);
	for (int i = 0; i < A.rows; i++) {
		for (int j = 0; j < A.cols; j++) {
			out[i][j] = A[i][j] + v;
		}
	}
	return out;
}
template<class T, int C> Vector<T, C> operator-(const DenseMatrix<T, C>& A,
		const vec<T, C>& v) {
	DenseMatrix<T, C> out(A.rows, A.cols);
	for (int i = 0; i < A.rows; i++) {
		for (int j = 0; j < A.cols; j++) {
			out[i][j] = A[i][j] - v;
		}
	}
	return out;
}
template<class T, int C> Vector<T, C> operator*(const vec<T, C>& v,
		const DenseMatrix<T, C>& A) {
	DenseMatrix<T, C> out(A.rows, A.cols);
	for (int i = 0; i < A.rows; i++) {
		for (int j = 0; j < A.cols; j++) {
			out[i][j] = v * A[i][j];
		}
	}
	return out;
}
template<class T, int C> Vector<T, C> operator/(const vec<T, C>& v,
		const DenseMatrix<T, C>& A) {
	DenseMatrix<T, C> out(A.rows, A.cols);
	for (int i = 0; i < A.rows; i++) {
		for (int j = 0; j < A.cols; j++) {
			out[i][j] = v / A[i][j];
		}
	}
	return out;
}
template<class T, int C> Vector<T, C> operator+(const vec<T, C>& v,
		const DenseMatrix<T, C>& A) {
	DenseMatrix<T, C> out(A.rows, A.cols);
	for (int i = 0; i < A.rows; i++) {
		for (int j = 0; j < A.cols; j++) {
			out[i][j] = v + A[i][j];
		}
	}
	return out;
}
template<class T, int C> Vector<T, C> operator-(const vec<T, C>& v,
		const DenseMatrix<T, C>& A) {
	DenseMatrix<T, C> out(A.rows, A.cols);
	for (int i = 0; i < A.rows; i++) {
		for (int j = 0; j < A.cols; j++) {
			out[i][j] = v - A[i][j];
		}
	}
	return out;
}
template<class T, int C> DenseMatrix<T, C>& operator*=(
		const DenseMatrix<T, C>& A, const vec<T, C>& v) {
	for (int i = 0; i < A.rows; i++) {
		for (int j = 0; j < A.cols; j++) {
			A[i][j] = A[i][j] * v;
		}
	}
	return A;
}
template<class T, int C> DenseMatrix<T, C>& operator/=(
		const DenseMatrix<T, C>& A, const vec<T, C>& v) {
	for (int i = 0; i < A.rows; i++) {
		for (int j = 0; j < A.cols; j++) {
			A[i][j] = A[i][j] / v;
		}
	}
	return A;
}
template<class T, int C> DenseMatrix<T, C>& operator+=(
		const DenseMatrix<T, C>& A, const vec<T, C>& v) {
	for (int i = 0; i < A.rows; i++) {
		for (int j = 0; j < A.cols; j++) {
			A[i][j] = A[i][j] + v;
		}
	}
	return A;
}
template<class T, int C> DenseMatrix<T, C>& operator-=(
		const DenseMatrix<T, C>& A, const vec<T, C>& v) {
	for (int i = 0; i < A.rows; i++) {
		for (int j = 0; j < A.cols; j++) {
			A[i][j] = A[i][j] - v;
		}
	}
	return A;
}
template<class T, int C> DenseMatrix<T, C>& operator*=(
		const DenseMatrix<T, C>& A, const T& v) {
	for (int i = 0; i < A.rows; i++) {
		for (int j = 0; j < A.cols; j++) {
			A[i][j] = A[i][j] * v;
		}
	}
	return A;
}
template<class T, int C> DenseMatrix<T, C>& operator/=(
		const DenseMatrix<T, C>& A, const T& v) {
	for (int i = 0; i < A.rows; i++) {
		for (int j = 0; j < A.cols; j++) {
			A[i][j] = A[i][j] / v;
		}
	}
	return A;
}
template<class T, int C> DenseMatrix<T, C>& operator+=(
		const DenseMatrix<T, C>& A, const T& v) {
	for (int i = 0; i < A.rows; i++) {
		for (int j = 0; j < A.cols; j++) {
			A[i][j] = A[i][j] + v;
		}
	}
	return A;
}
template<class T, int C> DenseMatrix<T, C>& operator-=(
		const DenseMatrix<T, C>& A, const T& v) {
	for (int i = 0; i < A.rows; i++) {
		for (int j = 0; j < A.cols; j++) {
			A[i][j] = A[i][j] - v;
		}
	}
	return A;
}

/******************************************************************************
 * XLISP-STAT 2.1 Copyright (c) 1990, by Luke Tierney
 * XLISP version 2.1, Copyright (c) 1989, by David Betz.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Luke Tierney and David Betz not be
 * used in advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  Luke Tierney and David Betz
 * make no representations about the suitability of this software for any
 * purpose. It is provided "as is" without express or implied warranty.
 *
 * LUKE TIERNEY AND DAVID BETZ DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
 * IN NO EVENT SHALL LUKE TIERNEY NOR DAVID BETZ BE LIABLE FOR ANY SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * XLISP-STAT AUTHOR:
 *               Luke Tierney
 *               School of Statistics
 *               University of Minnesota
 *               Minneapolis, MN 55455
 *               (612) 625-7843
 *
 *       Email Address:
 *               internet: luke@umnstat.stat.umn.edu
 *
 * XLISP AUTHOR:
 *              David Betz
 *              P.O. Box 144
 *              Peterborough, NH 03458
 *              (603) 924-4145
 ******************************************************************************
 * XLISP-STAT 2.1 was ported to the Amiga by
 *              J.K. Lindsey
 *              Faculty of Economic, Business and Social Sciences,
 *              University of Liege,
 *              Sart Tilman B31,
 *              4000 Liege,
 *              Belgium
 *              32-41-56.29.64
 *
 * The above permission and disclaimer also applies to all of the specifically
 * Amiga portions of this software, with the restriction that the Amiga
 * version not be used for any military-related applications.
 ******************************************************************************

 */

template<class T, int C> void SVD(const DenseMatrix<T, C>& M,
		DenseMatrix<T, C>& U, DenseMatrix<T, C>& D, DenseMatrix<T, C>& Vt) {
	int m = M.rows;
	int n = M.cols;
	double v[n][n];
	double u[m][m];
	double w[n];
	double rv1[n];
	int flag, i, its, j, jj, k, l, nm;
	double c, f, h, s, x, y, z;
	double anorm, g, scale;
	for (int cc = 0; cc < C; cc++) {
		anorm = 0.0, g = 0.0, scale = 0.0;
		if (m < n) {
			throw std::runtime_error(
					"SVD error, rows must be greater than or equal to cols.");
		}
		for (int i = 0; i < m; i++) {
			for (int j = 0; j < n; j++) {
				u[i][j] = M[i][j][cc];
			}
		}
		for (i = 0; i < n; i++) {
			l = i + 1;
			rv1[i] = scale * g;
			g = s = scale = 0.0;
			if (i < m) {
				for (k = i; k < m; k++)
					scale += std::abs((double) u[k][i]);
				if (scale) {
					for (k = i; k < m; k++) {
						u[k][i] = ((double) u[k][i] / scale);
						s += ((double) u[k][i] * (double) u[k][i]);
					}
					f = (double) u[i][i];
					g = -sign(std::sqrt(s), f);
					h = f * g - s;
					u[i][i] = (f - g);
					if (i != n - 1) {
						for (j = l; j < n; j++) {
							for (s = 0.0, k = i; k < m; k++)
								s += ((double) u[k][i] * (double) u[k][j]);
							f = s / h;
							for (k = i; k < m; k++)
								u[k][j] += (f * (double) u[k][i]);
						}
					}
					for (k = i; k < m; k++)
						u[k][i] = ((double) u[k][i] * scale);
				}
			}
			w[i] = (scale * g);
			g = s = scale = 0.0;
			if (i < m && i != n - 1) {
				for (k = l; k < n; k++)
					scale += std::abs((double) u[i][k]);
				if (scale) {
					for (k = l; k < n; k++) {
						u[i][k] = ((double) u[i][k] / scale);
						s += ((double) u[i][k] * (double) u[i][k]);
					}
					f = (double) u[i][l];
					g = -sign(std::sqrt(s), f);
					h = f * g - s;
					u[i][l] = (f - g);
					for (k = l; k < n; k++)
						rv1[k] = (double) u[i][k] / h;
					if (i != m - 1) {
						for (j = l; j < m; j++) {
							for (s = 0.0, k = l; k < n; k++)
								s += ((double) u[j][k] * (double) u[i][k]);
							for (k = l; k < n; k++)
								u[j][k] += (s * rv1[k]);
						}
					}
					for (k = l; k < n; k++)
						u[i][k] = ((double) u[i][k] * scale);
				}
			}
			anorm = aly::max(anorm,
					(std::abs((double) w[i]) + std::abs(rv1[i])));
		}
		for (i = n - 1; i >= 0; i--) {
			if (i < n - 1) {
				if (g) {
					for (j = l; j < n; j++)
						v[j][i] = (((double) u[i][j] / (double) u[i][l]) / g);
					for (j = l; j < n; j++) {
						for (s = 0.0, k = l; k < n; k++)
							s += ((double) u[i][k] * (double) v[k][j]);
						for (k = l; k < n; k++)
							v[k][j] += (s * (double) v[k][i]);
					}
				}
				for (j = l; j < n; j++)
					v[i][j] = v[j][i] = 0.0;
			}
			v[i][i] = 1.0;
			g = rv1[i];
			l = i;
		}
		for (i = n - 1; i >= 0; i--) {
			l = i + 1;
			g = (double) w[i];
			if (i < n - 1)
				for (j = l; j < n; j++)
					u[i][j] = 0.0;
			if (g) {
				g = 1.0 / g;
				if (i != n - 1) {
					for (j = l; j < n; j++) {
						for (s = 0.0, k = l; k < m; k++)
							s += ((double) u[k][i] * (double) u[k][j]);
						f = (s / (double) u[i][i]) * g;
						for (k = i; k < m; k++)
							u[k][j] += (f * (double) u[k][i]);
					}
				}
				for (j = i; j < m; j++)
					u[j][i] = ((double) u[j][i] * g);
			} else {
				for (j = i; j < m; j++)
					u[j][i] = 0.0;
			}
			++u[i][i];
		}
		for (k = n - 1; k >= 0; k--) {
			for (its = 0; its < 30; its++) {
				flag = 1;
				for (l = k; l >= 0; l--) {
					nm = l - 1;
					if (std::abs(rv1[l]) + anorm == anorm) {
						flag = 0;
						break;
					}
					if (std::abs((double) w[nm]) + anorm == anorm)
						break;
				}
				if (flag) {
					c = 0.0;
					s = 1.0;
					for (i = l; i <= k; i++) {
						f = s * rv1[i];
						if (std::abs(f) + anorm != anorm) {
							g = (double) w[i];
							h = pythag(f, g);
							w[i] = h;
							h = 1.0 / h;
							c = g * h;
							s = (-f * h);
							for (j = 0; j < m; j++) {
								y = (double) u[j][nm];
								z = (double) u[j][i];
								u[j][nm] = (y * c + z * s);
								u[j][i] = (z * c - y * s);
							}
						}
					}
				}
				z = (double) w[k];
				if (l == k) {
					if (z < 0.0) {
						w[k] = (-z);
						for (j = 0; j < n; j++)
							v[j][k] = (-v[j][k]);
					}
					int iii, jjj;
					for (iii = k; (iii < n - 1) && (w[iii] < w[iii + 1]);
							iii++) {
						std::swap(w[iii], w[iii + 1]);
						for (jjj = 0; jjj < m; jjj++)
							std::swap(u[jjj][iii], u[jjj][iii + 1]);
						for (jjj = 0; jjj < n; jjj++)
							std::swap(v[jjj][iii], v[jjj][iii + 1]);
					}
					break;
				}
				if (its >= 30) {
					throw std::runtime_error("SVD did not converge.");
				}
				x = (double) w[l];
				nm = k - 1;
				y = (double) w[nm];
				g = rv1[nm];
				h = rv1[k];
				f = ((y - z) * (y + z) + (g - h) * (g + h)) / (2.0 * h * y);
				g = pythag(f, 1.0);
				f = ((x - z) * (x + z) + h * ((y / (f + sign(g, f))) - h)) / x;
				c = s = 1.0;
				for (j = l; j <= nm; j++) {
					i = j + 1;
					g = rv1[i];
					y = (double) w[i];
					h = s * g;
					g = c * g;
					z = pythag(f, h);
					rv1[j] = z;
					c = f / z;
					s = h / z;
					f = x * c + g * s;
					g = g * c - x * s;
					h = y * s;
					y = y * c;
					for (jj = 0; jj < n; jj++) {
						x = (double) v[jj][j];
						z = (double) v[jj][i];
						v[jj][j] = (x * c + z * s);
						v[jj][i] = (z * c - x * s);
					}
					z = pythag(f, h);
					w[j] = z;
					if (z) {
						z = 1.0 / z;
						c = f * z;
						s = h * z;
					}
					f = (c * g) + (s * y);
					x = (c * y) - (s * g);
					for (jj = 0; jj < m; jj++) {
						y = (double) u[jj][j];
						z = (double) u[jj][i];
						u[jj][j] = (y * c + z * s);
						u[jj][i] = (z * c - y * s);
					}
				}
				rv1[l] = 0.0;
				rv1[k] = f;
				w[k] = x;
			}
		}
		for (int i = 0; i < m; i++) {
			for (int j = 0; j < n; j++) {
				if (i == j) {
					D[i][j][cc] = (T) w[j];
				} else {
					D[i][j][cc] = T(0);
				}
			}
		}
		for (int i = 0; i < m; i++) {
			for (int j = 0; j < m; j++) {
				U[i][j][cc] = (T) u[i][j];
			}
		}
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < n; j++) {
				Vt[i][j][cc] = (T) v[j][i];
			}
		}
	}
}
typedef DenseMatrix<float, 4> DenseMatrix4f;
typedef DenseMatrix<float, 3> DenseMatrix3f;
typedef DenseMatrix<float, 2> DenseMatrix2f;
typedef DenseMatrix<float, 1> DenseMatrix1f;

typedef DenseMatrix<double, 4> DenseMatrix4d;
typedef DenseMatrix<double, 3> DenseMatrix3d;
typedef DenseMatrix<double, 2> DenseMatrix2d;
typedef DenseMatrix<double, 1> DenseMatrix1d;
}

#endif
