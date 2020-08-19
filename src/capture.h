#pragma once

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <cstdlib>  // srand

#include <cassert>

#include "utils.h"
#include "rgb.h"
#include "png.h"

namespace vpl{

/// Captures screen - by default entire screen but can be configured to
/// just capture selection
std::vector<RGB> capture_screen(PageInfo& info, Window * window = nullptr) {

    using namespace std;

    // Bump file name to the next avaialble digit;
    auto list = glob("page_capture*.png");
    int total_files = list.size();

    info.page_num = total_files + 1;

    stringstream filename;
    filename << "page_capture";
    filename << info.page_num;
    filename << ".png";
    info.filename = filename.str();
    cout << "Capture screen... " << filename.str() <<endl;

    int lx, ly;

    Display *display = 0;
    Screen * screen = 0;
    XWindowAttributes xwa;

    if( !display ) {
        display = XOpenDisplay( 0 );
        screen = XDefaultScreenOfDisplay(display);
        if (!window){
            // window = DefaultRootWindow(display);
            window = &RootWindow(display, DefaultScreen(display));
        }
        XGetWindowAttributes(display, *window, &xwa);
    }

    int width = xwa.width;
    int height = xwa.height;
    info.width = xwa.width;
    info.height = xwa.height;

    cout << "width " << width  << " height " << height <<endl;

    XImage * m = XGetImage( display, *window, 0, 0, width, height, AllPlanes, XYPixmap);

    if( m->bitmap_pad != 32 ){
        throw std::runtime_error("Bad bitmap pad\n");
    }

    unsigned char* image = new unsigned char[3 * width * height];
    std::vector<RGB> rgbs;

    long pixelId = 0;
    for( ly = 0; ly < height; ly++ ) {
        for( lx = 0; lx < width; lx++ ) {
            uint32_t px = XGetPixel( m, lx, ly );
            unsigned char x, y, z;

            size_t pixel_index = ly * width + lx;

            //(*(pb++)) = (px>>16) | (px&0xff00) | ((px&0xff)<<16);

            image[pixel_index*3] = px>>16;
            image[pixel_index*3+1] = px>>8;
            image[pixel_index*3+2] = px;

            rgbs.emplace_back(RGB(image[pixel_index*3],
                                  image[pixel_index*3+1],
                                  image[pixel_index*3+2],
                                  lx,
                                  ly,
                                  pixelId));
            pixelId++;

            // cout << (int)x << " "<< (int)y << " " << (int)z <<endl;
        }
    }
    XDestroyImage( m );

    // NOTE: This savePng routine does not work with libpng used in libPdfWriter
    //       Use it a preview only
    // Png(filename.str().c_str(), width, height, image);   //save png

    delete [] image;

    return rgbs;
}

/// Captures just a specified window - this is what we are ultimately after
std::vector<RGB> capture_window(vpl::PageInfo& info, const std::string& window_name){
    using namespace std;

    Display *display = XOpenDisplay(NULL);

    Window rootWindow = RootWindow(display, DefaultScreen(display));
    Window DOSBoxWindow;
    Atom atom = XInternAtom(display, "_NET_CLIENT_LIST", true);

    int format;
    Atom actualType;
    unsigned long numItems;
    unsigned long bytesAfter;
    unsigned char *data;
    Window *list;

    int status = XGetWindowProperty(display, rootWindow, atom, 0L, (~0L), false, AnyPropertyType, &actualType, &format, &numItems, &bytesAfter, &data);
    list = (Window*)data;

    XTextProperty text_prop_return;

    std::vector<RGB> rgbs;

    for (int i = 0; i < numItems; ++i){
        XGetWMName(display, list[i], &text_prop_return);
        string s(text_prop_return.value, text_prop_return.value+text_prop_return.nitems);
        //cout << s <<endl;

        if (s.find(window_name) != string::npos){
            cout << "..." << i << " " << s<<endl;

            XWindowAttributes xwa;

            XGetWindowAttributes( display, list[i], &xwa );
            cout << xwa.width << " " << xwa.height << endl;

            info.width = xwa.width;
            info.height = xwa.height;

            rgbs = vpl::capture_screen(info, &list[i]);
        }
    }
    return rgbs;
}


void test_neighbor_pixels(std::vector<RGB> rgbs, std::vector<RGB*>& to_process, RGB* value, PageInfo& info){
    using namespace std;

    // Test for pixel neigbors
    cout << "to_process:" << to_process[0] <<endl;

    auto up_px    = getPixel(rgbs, (*to_process[0]), up   , info);
    auto right_px = getPixel(rgbs, (*to_process[0]), right, info);
    auto down_px  = getPixel(rgbs, (*to_process[0]), down , info);
    auto left_px  = getPixel(rgbs, (*to_process[0]), left , info);

    if (up_px){
        cout << "up " << *up_px <<endl;
    }
    if (right_px){
        cout << "right " << *right_px <<endl;
    }
    if (down_px){
        cout << "down " << *down_px <<endl;
    }
    if (left_px){
        cout << "left " << *left_px <<endl;
    }
    setRGB(rgbs, RGB(0,255,0, up_px->x, up_px->y, -1),info);
    setRGB(rgbs, RGB(0,255,0, left_px->x, left_px->y, -1),info);
    setRGB(rgbs, RGB(0,255,0, down_px->x, down_px->y, -1),info);
    setRGB(rgbs, RGB(0,255,0, right_px->x, right_px->y, -1),info);

    /////////////////// test save PNG
    RGB newValue(255,0,0, value->x, value->y, -1);
    setRGB(rgbs, newValue, info);
    savePng(rgbs, "test_neighbor_pixels.png", info);

}

///
struct SegmentsInfo{
    std::unordered_map<long, int> groups;
    int group_index;
};

SegmentsInfo boundary_tracing(PageInfo& info, std::vector<RGB>& rgbs){
    using namespace std;

    int width = info.width;
    int height = info.height;

    if (info.test_ppm){
        auto image = loadPpm(info.filename);
        cout << "Loading:"<< info.filename <<endl;

        long index = 0;
        rgbs.clear();
        long pixelId = 0;
        while (index < image.size()){
            int x = pixelId % width;
            int y = pixelId / width;
            rgbs.emplace_back(RGB(image[index], image[index+1], image[index+2], x, y, pixelId));
            index+=3;
            pixelId++;
        }
        cout <<rgbs[0] <<endl;
        cout <<rgbs[rgbs.size()-1] <<endl;
        cout << compareRGB(RGB(0,0,1), RGB(0,0,1)) <<endl;

    }
    cout << width << " " << height <<  " " << width * height << endl;
    cout << rgbs.size() << endl;
    assert  (rgbs.size() == width*height);


    unordered_set<RGB*> white_set;
    // TODO: at the moment we take into the account only bright pixels
    //       - provide the threshold from the command line
    auto threshold = RGB(240,240,240, 0, 0, 0);
    for (auto &px : rgbs){
        if (px >= threshold){
            white_set.insert(&px);
        }
    }

    cout << "unorderset size " << white_set.size() <<endl;

    // need to reuse same pointers - if we want to compare them
    // (Do we want to compare? - we need to check 2 cases
    // - already visited in the set
    // - if the neigbor has the same color (need to access values)
    vector<RGB*> to_process;
    for(auto &x : white_set){
        to_process.push_back(x);
        // to_process.push_back(rgbs[x->index]);
    }

    // auto ii = white_set.find(to_process[0]);
    // if (ii != white_set.end()){
    //     cout<< "found elemwnt" <<endl;
    // }


    std::queue<RGB*> cue;
    // NOTE: in this initilaliztion all pixels get goup 0
    //       we need to exclude that
    std::unordered_map<long, int> groups;
    int group_number = 0;

    for (auto pixel : to_process){  /// TODO: ------ this is a different pointer (pixel) then the white_set stuff we iterate over!!!!
        auto it = white_set.find(pixel);

        // cout << *pixel <<endl;
        // cout << (white_set.find(pixel) != white_set.end() )<< endl;
        // cout << **it <<endl;
        // cout <<( white_set.find(to_process[0]) != white_set.end())<< endl;
        // cout << (**it == *pixel) << endl;
        // return;

        if (it != white_set.end()){
            white_set.erase(it);

            cue.push(pixel);
            // cout << "group:" << group_number << endl;

            // recursively explore neighbors
            while (!cue.empty()) {
                pixel = cue.front();

                // for each direction check if there is a valid pixle with the same color
                for (int dir = up; dir <= left; dir++){
                    auto px = getPixel(rgbs, *pixel, static_cast<Direction>(dir), info);
                    // Do not modify orignal pixel earlier as we compare values here
                    // to check if they are the same color
                    if ((px) && (*px == *pixel)){  // compare values not pointers
                        it = white_set.find(const_cast<RGB*>(px));
                        if (it != white_set.end()){  // if we havent visited this earlier
                            white_set.erase(it);     // remove as it is already visited

                            // populate queue with neigbor elements
                            cue.push(const_cast<RGB*>(px));  // TODO this is getting annoying
                            // we are woring on active set (no const) in case spec changes
                            // but that does not work well with constness
                        }
                    }
                }
                // cout << "cue.size:" << cue.size() <<endl;

                // As the last step - set color (since earlier we were comparing these values)
                // setRGB(rgbs, RGB(0,255,0, pixel->x, pixel->y, pixel->index), info); // DEBUG

                groups[pixel->index] = group_number;
                // TODO: for easier lookup pixel group we may want to store this aa an atribute
                cue.pop();

            } // queue end

            group_number++;  // bump number of connected component
            // cout << "group_number: " << group_number << endl;

        }
        else {
            // cout << "already visisted" <<endl;
        }

    }

    // color groups
    srand(1);
    std::vector<RGB> random_colors;
    for (auto i=0; i < group_number; ++i){
        int r = rand() % 256;
        int g = rand() % 256;
        int b = rand() % 256;
        random_colors.emplace_back(RGB(r,g,b));
    }

    std::cout << "total groups: " << groups.size() <<endl;

    std::vector<int> groups_histogram(groups.size());
    for (auto g : groups){
        auto index = g.first;
        auto group_number = g.second;

        // set colors to show blocks of color
        if (info.preview_segments) {
            rgbs[index].r = random_colors[group_number].r;
            rgbs[index].g = random_colors[group_number].g;
            rgbs[index].b = random_colors[group_number].b;
        }

        groups_histogram[group_number] += 1;
    }

    // find the group with the largest number

    int max_index = std::distance(groups_histogram.begin(),std::max_element(groups_histogram.begin(),  groups_histogram.end()));

    cout << "total groups: " << groups.size() << " max_index "<< max_index << " total elements in max group:" << groups_histogram[max_index] << endl;

    // A the momoment we dont store group number on the pixel, need to access it thru the map
    if (info.preview_segments){
        for (auto & _pair : groups){ // this is maping pixel to group in the entire image
            if (_pair.second != max_index)
                continue;
            rgbs[_pair.first].r = 255;
            rgbs[_pair.first].g = 0;
            rgbs[_pair.first].b = 0;
        }
    }
    SegmentsInfo seg_info;
    seg_info.groups = groups;
    seg_info.group_index = max_index;
    return seg_info;
}

void crop(PageInfo& info, std::vector<RGB>& rgbs, SegmentsInfo const & seg_info ){
    using namespace std;

    auto groups = seg_info.groups;
    auto max_index = seg_info.group_index;

    // iterate over largest group and find Bounding box
    int top = info.height;
    int left = info.width;
    int bottom = 0;
    int right = 0;
    std::vector<RGB> rgbs_crop;
    for (auto it=groups.begin(); it!=groups.end(); ++it){
        // if (it->second == 0)
        //     continue;
        if (it->second == max_index){
            auto index = it->first;
            auto px = rgbs[index];
            px.r = 0;
            px.g = 255;
            px.b = 0;

            if (px.y < top) top = px.y;
            if (px.x < left) left = px.x;
            if (px.y > bottom) bottom = px.y;
            if (px.x > right) right = px.x;

        }
    }
    cout << "bbox: " << '[' << left <<','<<top<<"],["<<right<<','<<bottom<<']'<<endl;
    cout << "imagemagick: " <<right-left<<'x'<<bottom-top<<'+'<<left <<'+'<<top<<endl;

    setRGB(rgbs, RGB(255,0,0, left, top, -1), info);
    setRGB(rgbs, RGB(255,0,0, right, bottom, -1), info);

    // Crop image
    for (const auto & pixel : rgbs){
        if ((pixel.x > left) && (pixel.y > top) && (pixel.x < right) && (pixel.y < bottom)){
            rgbs_crop.emplace_back(pixel);
        }
    }

    // // test_neighbor_pixels(rgbs, to_process, value);

    if (info.preview_segments) {
        savePng(rgbs, "preview_segments.png", info);
        return;
    }

    stringstream filename;

    // NOTE: This savePng routine does not work with libpng used in libPdfWriter
    filename << "crop_" << info.page_num << ".png";

    info.filename = filename.str();

    // Update PageInfo
    info.width = right-left-1;
    info.height = bottom-top;

    rgbs = rgbs_crop;
    savePng(rgbs_crop, filename.str(), PageInfo(right-left-1, bottom-top));

}
} // end namepace vpl
