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
#ifndef ALLOYDENSESOLVER_H_
#define ALLOYDENSESOLVER_H_
#include "AlloyMath.h"
#include "AlloyImage.h"
namespace aly {
bool SANITY_CHECK_DENSE();

void PoissonBlend(const Image4f& in, Image4f& out, int iterations, int levels,float lambda = 0.99f);
void PoissonBlend(const Image4f& in, Image4f& out, int iterations,float lambda = 0.99f);
void PoissonBlend(const Image2f& in, Image2f& out, int iterations, int levels,float lambda = 0.99f);
void PoissonBlend(const Image2f& in, Image2f& out, int iterations,float lambda = 0.99f);
void PoissonInpaint(const Image4f& source,const Image4f& target,  Image4f& out, int iterations, int levels,float lambda = 0.99f);
void PoissonInpaint(const Image4f& source,const Image4f& target,  Image4f& out, int iterations,float lambda = 0.99f);
void PoissonInpaint(const Image2f& source,const Image2f& target,  Image2f& out, int iterations, int levels,float lambda = 0.99f);
void PoissonInpaint(const Image2f& source,const Image2f& target,  Image2f& out, int iterations,float lambda = 0.99f);
void LaplaceFill(const Image4f& sourceImg, Image4f& targetImg, int iterations,int levels, float lambda = 0.99f);
void LaplaceFill(const Image4f& sourceImg, Image4f& targetImg, int iterations,float lambda = 0.99f);
void LaplaceFill(const Image2f& sourceImg,Image2f& targetImg,int iterations, float lambda=0.99f);
void LaplaceFill(const Image2f& sourceImg,Image2f& targetImg,int iterations,int levels, float lambda=0.99f);

}
#endif
