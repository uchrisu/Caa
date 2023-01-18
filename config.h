#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <vector>
#include "qcolor.h"

extern int NUM_SYSTEMS;

extern const std::vector<int> list_lengths_N;
extern const std::vector<int> list_lengths_L;
extern const std::vector<int> list_lengths_Nf;
extern const std::vector<int> list_smoothing_per_oct;

extern const int config_stdindex_N;
extern const int config_stdindex_L;
extern const int config_stdindex_Nf;

extern const std::vector<std::string> list_channel_types;
extern const int config_channel_type_live;
extern const int config_channel_type_multiply;
extern const int config_channel_type_divide;
extern const int config_stdindex_channel_type;

extern const std::vector<std::string> list_sysident_methods;
extern const int config_sysident_TDMSE_T;
extern const int config_sysident_DUALFFT;
extern const int config_sysident_NLMS;
extern const int config_stdindex_sysident_methods;

extern QColor get_color(int index, int type);

#endif // CONFIG_H
