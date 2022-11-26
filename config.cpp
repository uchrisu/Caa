#include <string>
#include <vector>
#include "qcolor.h"

int NUM_SYSTEMS = 2;

extern const int config_stdindex_N = 3;
extern const std::vector<int> list_lengths_N = {512, 1024, 2048, 4096, 8192, 16*1024};

extern const int config_stdindex_L = 3;
extern const std::vector<int> list_lengths_L = {8*1024, 16*1024, 32*1024, 64*1024, 128*1024, 256*1024};

extern const int config_stdindex_Nf = 3;
extern const std::vector<int> list_lengths_Nf = {512, 1024, 2048, 4096, 8192, 16*1024, 32*1024, 64*1024, 128*1024};

extern const std::vector<int> list_smoothing_per_oct = {3, 6, 12, 24, 48};

extern const std::vector<std::string> list_sysident_methods = {"TDMSE-T", "Dual FFT"};
extern const int config_sysident_TDMSE_T = 0;
extern const int config_sysident_DUALFFT = 1;
extern const int config_stdindex_sysident_methods = 0;

QColor get_color(int index, int type)
{
    int color_line0[3] = {0, 0, 255};
    int color_line1[3] = {255, 0, 0};
    int color_line2[3] = {0, 0, 0};
    int color_line3[3] = {127, 127, 0};

    int r, g, b;

    switch (index) {
    case 1:
        r = color_line1[0];
        g = color_line1[1];
        b = color_line1[2];
        break;
    case 2:
        r = color_line2[0];
        g = color_line2[1];
        b = color_line2[2];
        break;
    case 3:
        r = color_line3[0];
        g = color_line3[1];
        b = color_line3[2];
        break;
    default:  // case 0 and else
        r = color_line0[0];
        g = color_line0[1];
        b = color_line0[2];
        break;
    }

    if (type == 1){
        r = static_cast<int>((r+255)/2);
        g = static_cast<int>((g+255)/2);
        b = static_cast<int>((b+255)/2);
    }

    return QColor(r,g,b);
}
