#pragma once

#include "S2D/GLSimp2DTypes.h"
#include <cstdint>

struct ColorRange {
	uint8_t Rmin, Rmax;
	uint8_t Gmin, Gmax;
	uint8_t Bmin, Bmax;
};


struct BlobArea {
	uint32_t x, y, w, h;
};

class ColorBlob {
	
private:
	uint32_t minX, minY, maxX, maxY;
	uint32_t pixelRange = 20;
	uint32_t memberCount = 0;
	static ColorRange colorRange;
public:
	ColorBlob();
	ColorBlob(uint32_t _minX, uint32_t _minY, uint32_t _maxX, uint32_t _maxY);
	void setColorRange(ColorRange range);
	static bool isColorInRange(uint32_t color);
	bool isInPerimiter(uint32_t x, uint32_t y);
	void updatePerimeter(uint32_t x, uint32_t y);
	uint32_t getMemberCount();
	BlobArea getArea();
	

};

