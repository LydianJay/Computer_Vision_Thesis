#include "ColorBlob.h"



ColorRange ColorBlob::colorRange = {};


ColorBlob::ColorBlob() {

    maxX = 0, maxY = 0;
    minX = INT32_MAX, minY = INT32_MAX;
}

ColorBlob::ColorBlob(uint32_t _minX, uint32_t _minY, uint32_t _maxX, uint32_t _maxY): minX(_minX), minY(_minY), maxX(_maxX), maxY(_maxY) {
}

void ColorBlob::setColorRange(ColorRange range) {
    colorRange = range;
}

bool ColorBlob::isColorInRange(uint32_t color) {
    unsigned char red = (color & 0x00FF0000) >> 16;
    unsigned char green = (color & 0x0000FF00) >> 8;
    unsigned char blue = ((color & 0x00000FF));
    unsigned char alpha = (color & 0xFF000000) >> 24;


    ColorRange& c = colorRange;
    return (
        red >= c.Rmin && red <= c.Rmax,
        green >= c.Gmin && green <= c.Gmax,
        blue >= c.Bmin && blue <= c.Bmax
        );
}



bool ColorBlob::isInPerimiter(uint32_t x, uint32_t y) {
   

    return (x >= minX-pixelRange && x <= maxX + pixelRange && y >= minY-pixelRange && y <= maxY + pixelRange);
}

void ColorBlob::updatePerimeter(uint32_t x, uint32_t y) {
    
    memberCount++;
    if (x < minX) minX = x;
    if (y < minY) minY = y;
    if (x > maxX) maxX = x;
    if (y > maxY) maxY = y;

}

BlobArea ColorBlob::getArea() {
    uint32_t w = (maxX - minX) + (pixelRange * 2), h = (maxY - minY) + (pixelRange * 2);
    return {minX, minY, w, h };
}

uint32_t ColorBlob::getMemberCount() {
    return memberCount;
}
