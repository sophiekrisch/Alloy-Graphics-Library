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
#include "../../include/example/MeshViewer.h"
using namespace aly;
MeshViewer::MeshViewer():Application(1280, 960, "Mesh Viewer"),matcapShader(getFullPath("images/JG_Gold.png")){
}
bool MeshViewer::init(Composite& rootNode) {
	mesh.load(getFullPath("models/icosahedron.ply"));
	mesh.scale(10.0f);
	mesh.transform(MakeRotationY((float) M_PI));

	box3f renderBBox = box3f(float3(-0.5f, -0.5f, -0.5f),
			float3(1.0f, 1.0f, 1.0f));
	camera.setNearFarPlanes(0.01f,10.0f);
	camera.setPose(MakeTransform(mesh.getBoundingBox(), renderBBox));
	exampleImage.load(getFullPath("images/sfsunset.png"), true);
	frameBuffer.initialize(480,480);
	mesh.updateVertexNormals();
	addListener(&camera);
	return true;
}
void MeshViewer::draw(const aly::DrawEvent3D& event) {
	frameBuffer.begin();
	edgeDepthAndNormalShader.draw(mesh, camera,frameBuffer.getViewport());
	frameBuffer.end();
}
void MeshViewer::draw(const aly::DrawEvent2D& event) {
	imageShader.draw(exampleImage, float2(1280-310.0f, 10.0f),float2(300.0f, 200.0f),true);
	normalColorShader.draw(frameBuffer.getTexture(), float2(0.0f, 0.0f),float2(480,480));
	depthColorShader.draw(frameBuffer.getTexture(),camera.computeNormalizedDepthRange(mesh), float2(480.0f, 0.0f),float2(480,480));
	distanceFieldShader.draw(frameBuffer.getTexture(),8, float2(0.0f, 480.0f),float2(480,480));
	//effectsShader.draw(frameBuffer.getTexture(), float2(480.0f, 480.0f),float2(480,480));

}
