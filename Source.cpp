#include <stdio.h>
#include <iostream>
#include "S2D/GLSimp2DGraphics.h"
#include "S2D/GLSimp2DWindow.h"
#include "escapi/escapi.h"
#include <sstream>
#include <vector>
#include <unordered_map>
#include <queue>
#include "ColorBlob.h"

struct BoundingBox {
	uint32_t sFx, sFy, eFx, eFy;
	uint32_t minX, minY, maxX, maxY;
};


static const int th = 150;
static const int d = 10;

void flipPixelData(int* pixel, int w, int h) {

	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {

			int temp = pixel[x + y * w];
		
			unsigned int red = (temp & 0x00FF0000) >> 16;
			unsigned int green = (temp & 0x0000FF00);
			unsigned int blue = (temp & 0x000000FF) << 16;

			temp = 0;

			temp |= red;
			temp |= blue;
			temp |= green;
			pixel[x + y * w] = temp;
			
		}

	}


}


void displayPixelValue(int pixel) {
	DWORD d;
	std::stringstream ss;
	
	unsigned char red = (pixel & 0xFF000000) >> 24;
	unsigned char green = (pixel & 0x00FF0000) >> 16;
	unsigned char blue = ((pixel & 0x0000FF00) >> 8);
	unsigned char alpha = ((pixel & 0x000000FF) >> 0);
	ss << "Red: " << (int)red << " Green: " << (int)green << " Blue: " << (int)blue << " Alpha: " << (int)alpha << "    ";
	
	
	
	WriteConsoleOutputCharacterA(GetStdHandle(STD_OUTPUT_HANDLE), ss.str().c_str(), ss.str().size(), { 0,1 }, &d);

}


bool atThreashold(int t, unsigned int color) {
	
	unsigned char red = (color & 0x00FF0000) >> 16;
	unsigned char green = (color & 0x0000FF00) >> 8;
	unsigned char blue = ((color & 0x00000FF));
	unsigned char alpha = (color & 0xFF000000) >> 24;

	return (red <= 35 && green <= 35 && blue <= 35 /*&& alpha >= t*/);
}



void findBlobs(std::vector<BoundingBox>& b, int * data, uint32_t x, uint32_t y, uint32_t w, uint32_t h) {

	uint32_t color = data[x + y * w];

	if (atThreashold(th, color)) {

		

		for (auto& box : b) {

			if (x >= box.minX && x <= box.maxX &&
				y >= box.minY && y <= box.maxY
				) {


				if (x < box.sFx) { box.sFx = x; box.minX = box.sFx - d; }
				if (y < box.sFy) { box.sFy = y; box.minY = box.sFy - d; }
				if (x > box.eFx) { box.eFx = x; box.maxX = box.eFx + d; }
				if (y > box.eFy) { box.eFy = x; box.maxY = box.eFy + d; }



				
				return;
				
			}

		}


		BoundingBox bx = {
			x, y, x, y,
			x - d, y - d, x + d, y + d
		};

		b.push_back(bx);
	}

}


int main() {

	int deviceCount = setupESCAPI();
	int deviceSelected = 0;
	for (int i = 0; i < deviceCount; i++) {
		char buffer[64] = {};
		memset(buffer, 0, 64);
		getCaptureDeviceName(i, buffer, 64);
		std::cout << "Device Name: " << buffer << '\n';
		
	}
	std::cout << "Enter device number: ";
	std::cin >> deviceSelected;
	SimpleCapParams camCap;
	
	camCap.mHeight = 400, camCap.mWidth = 400;

	
	
	


	camCap.mTargetBuf = new int[camCap.mHeight * camCap.mWidth];
	
	if (!initCapture(deviceSelected, &camCap)) {
		std::cout << "Capture failed\n";
	}


	for (int i = 0; i < 10; i++) {

		doCapture(deviceSelected);

		while (!isCaptureDone(deviceSelected)){}
			
	}
	int winW = 800, winH = 800;

	
	s2d::S2DWindow window(winW, winH, "WebCam", S2D_WINDOW_NO_RESIZE);
	s2d::S2DGraphics graphics(window);
	

	

	s2d::Texture tex = graphics.createTexture((unsigned char*)camCap.mTargetBuf, camCap.mWidth, camCap.mHeight);
	
	
	float mX = 0, mY = 0;

	float lx = 0, ly = 0;
	s2d::S2D_COLOR lColor = s2d::PXLCLR_WHITE;
	

	std::vector<ColorBlob> bbox;
	std::vector<s2d::Vec2f> colorLoc;
	std::vector<s2d::S2D_COLOR> cl;
	colorLoc.reserve(100);
	ColorBlob b;
	ColorRange range = {
	10, 35,
	10, 35,
	10, 35
	};
	b.setColorRange(range);

	bbox.reserve(10);
	bbox.push_back(b);

	while (!window.WindowShouldClose()) {


		
		s2d::KeyState ks = window.GetKeyState();
		//s2d::MouseState ms = window.GetMouseState();
		
		
		
		/*if (ms.state == S2D_MOUSE_CLICK) {
			lx = ms.pos.x;
			ly = ms.pos.y;

			uint32_t color = camCap.mTargetBuf[(int)ms.pos.x + (int)ms.pos.y * camCap.mWidth];
			displayPixelValue(color);
			
			unsigned char red = (color & 0x00FF0000) >> 16;
			unsigned char green = (color & 0x0000FF00) >> 8;
			unsigned char blue = ((color & 0x00000FF));

			lColor = { red, green, blue, 255 };
		}*/
			
		
		
		if (ks.state == S2D_KEY_RELEASED && ks.key == VK_SPACE) {
			colorLoc.clear();
			std::cout << "Capture!\n";
			doCapture(deviceSelected);
			while (!isCaptureDone(deviceSelected)) {}
			bbox.clear();
			ColorBlob b;
			cl.clear();
			ColorRange range = {
			0, 12,
			0, 12,
			0, 12
			};


			b.setColorRange(range);
			
			uint32_t colorCount = 0;
			for (int y = 0; y < camCap.mHeight; y++) {
				for (int x = 0; x < camCap.mWidth; x++) {
					uint32_t idx = x + y * camCap.mWidth;
					uint32_t pixel = camCap.mTargetBuf[idx];





					//for (auto& blob : bbox) {

					//	if (blob.isColorInRange(pixel)) {
					//		colorCount++;
					//		colorLoc.push_back({(float)x,(float)y});

					//		unsigned char r = (pixel & 0x00FF0000) >> 16;
					//		unsigned char g = (pixel & 0x0000FF00) >> 8;
					//		unsigned char b = ((pixel & 0x00000FF));
					//		unsigned char a = (pixel & 0xFF000000) >> 24;

					//		cl.push_back({r,g,b,a});
					//	}

					//}

					if (ColorBlob::isColorInRange(pixel)) {


						if (bbox.empty()) {
							bbox.push_back(ColorBlob(x, y, x, y));
						}
						else {

							bool isIn = false;
							for (auto& b : bbox) {

								if (b.isInPerimiter(x,y)) {
									b.updatePerimeter(x, y);
									isIn = true;
									break;
								}


							}

							if (!isIn) {
								bbox.push_back(ColorBlob(x, y, x, y));
							}

						}

					}

					//for (auto& blob : bbox) {
					//	
					//	if (blob.isColorInRange(pixel)) {
					//		colorCount++;
					//		colorLoc.push_back({(float)x,(float)y});
					//		if (blob.isInPerimiter(x, y)) {
					//			blob.updatePerimeter(x, y);
					//			std::cout << "Perimeter updated\n";
					//			break;
					//		}
					//		else {
					//			bbox.push_back(ColorBlob(x, y, x, y));
					//			break;
					//		}
					//	}
					//	

					//}



				}
			}

			std::cout << "BoxCount: " << bbox.size() << " colorCount: " << colorCount << '\n';

			



			flipPixelData(camCap.mTargetBuf, camCap.mWidth, camCap.mHeight);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, camCap.mWidth, camCap.mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*)camCap.mTargetBuf);





		}


		




		


		
		
		graphics.drawRect(0, 0, winW, winH, s2d::PXLCLR_WHITE, tex);
		//graphics.drawRect(lx, ly, 20, 20, lColor);

		for (auto& b : bbox) {
			BlobArea area = b.getArea();
			
			if (b.getMemberCount() <= 5)continue;

			s2d::Vec2f pos = { (float)((float)area.x/camCap.mWidth) * winW, 
			(float)((float)area.y / camCap.mHeight)* winH
			};
			graphics.drawWireFrameRect( pos, { (float)area.w, (float)area.h }, 1.5f, s2d::PXLCLR_GREEN);
		}

		

		/*if (!bbox.empty()) {
			BoundingBox& b = bbox[0];
			s2d::Vec2f pos = { (float)b.sFx, (float)b.sFy };
			s2d::Vec2f sz = { (float)b.eFx - b.sFx, (float)b.eFy - b.sFy };
			graphics.drawWireFrameRect(pos, sz, 2.0f, s2d::PXLCLR_GREEN);
		}*/
		

		//for (uint32_t i = 0; i < colorLoc.size(); i++) {
		//	const auto& loc = colorLoc[i];
		//	//const auto& col = cl[i];
		//	graphics.drawRect( ((float)loc.x/camCap.mWidth) * winW, ((float)loc.y / camCap.mHeight)* winH, 1, 1, s2d::PXLCLR_BLUE);
		//}

		


		//graphics.drawWireFrameRect({ 0,0 }, {50,50}, 4.0f, s2d::PXLCLR_GREEN);

		/*graphics.drawWireFrameRect({ (float)sFx, (float)sFy }, { (float)eX - sFx, (float)eY - sFy }, 2, s2d::PXLCLR_GREEN);*/

		graphics.flushBuffer();
		window.SwapWindowBuffers();
		window.ProcessMessage();


		
	}


	deinitCapture(deviceSelected);
}