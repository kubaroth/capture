# pragma once

#include <vector>

namespace vpl{
/// Encode direction to look up neighbor pixels
enum Direction {up, right, down, left};

/// A simple struct to share common information
struct PageInfo{
    int width;
    int height;
    std::string filename = "";
    int page_num = 0;
    bool test_ppm=false;
    bool preview_segments=false;
    int threshold = 240;
    PageInfo(int w, int h) : width(w), height(h) {}
    PageInfo() : width(0), height(0) {} // uninitialized dimensions
};

/// TODO: Bad things may happen if we don't initialize coordinates, nor the index - Improve!!!
struct RGB{
    int r,g,b;
    int x = -1;
    int y = -1;
    long index = 0; // used by views to refer to original data
    RGB(int r, int g, int b) : r(r), g(g), b(b) {}
    RGB(int r, int g, int b, int x, int y, long index) : r(r), g(g), b(b), x(x), y(y), index(index){}
    bool operator==(const RGB& rhs) {return (this->r + this->g + this->b == rhs.r + rhs.g + rhs.b) ? true : false;}
    bool operator!=(const RGB& rhs)  {return (!(*this == rhs)) ? true : false;}
    bool operator<=(const RGB& rhs) {return (this->r + this->g + this->b <= rhs.r + rhs.g + rhs.b) ? true : false;}
    bool operator>(const RGB& rhs)  {return (!(*this <= rhs)) ? true : false;}
    bool operator>=(const RGB& rhs) {return (this->r + this->g + this->b >= rhs.r + rhs.g + rhs.b) ? true : false;}
    bool operator<(const RGB& rhs)  {return (!(*this >= rhs)) ? true : false;}

    // TODO: implement remaing const versions
    bool operator==(const RGB& rhs) const {return (this->r + this->g + this->b == rhs.r + rhs.g + rhs.b) ? true : false;}
    bool operator!=(const RGB& rhs) const {return (!(*this == rhs)) ? true : false;}
    RGB operator-(const RGB& rhs){
        return RGB(this->r - rhs.r, this->g - rhs.g, this->b - rhs.b);
    }
    RGB operator-(int rhs){
        return RGB(this->r - rhs, this->g - rhs, this->b - rhs);
    }

    friend std::ostream& operator<<(std:: ostream& ostr, RGB const & rgb){
        ostr << rgb.r<<','<<rgb.g << ','<<rgb.b << " [" << rgb.x <<','<<rgb.y << "] index: "<< rgb.index;
        return ostr;
    }
};

/// This is not used any more, as we shifted to handle pointers (sorting 'the view')
bool compareRGB(const RGB& lhs, const RGB& rhs) {
    return lhs.r + lhs.g + lhs.b < rhs.r + rhs.g + rhs.b;
}
/// functior to sort vector of pointers
bool compareRGB_Ptrs(RGB* lhs, RGB* rhs) {
    return lhs->r + lhs->g + lhs->b < rhs->r + rhs->g + rhs->b;
}


// RGB is passed by copy as we'd like to construct RGB in place
long setRGB(std::vector<RGB>& rgbs, RGB value, const PageInfo& info){
    long index = (value.y * info.width) + value.x;
    rgbs[index] = value;
    // cout << "setting color:" << value << endl;
    return index;
}

/// TODO: write test for all the directions
const RGB* getPixel(const std::vector<RGB>& rgbs, const RGB& pixel, Direction dir, const PageInfo& info){
    if ((dir == up)    && (pixel.index - info.width > 0))  return &rgbs[pixel.index - info.width];
    if ((dir == down)  && (pixel.index + info.width < info.width * info.height))  return &rgbs[pixel.index + info.width];
    if ((dir == left)  && (pixel.index % info.width > 0))  return &rgbs[pixel.index - 1];
    if ((dir == right) && (pixel.index + 1 < info.width * info.height))  return &rgbs[pixel.index + 1];
    else return nullptr;
}

} // end namepace vpl
