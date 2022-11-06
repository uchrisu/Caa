#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <vector>

#define NUM_SYSTEMS 4

extern std::vector<int> list_lengths_N;
extern std::vector<int> list_lengths_L;
extern std::vector<int> list_lengths_Nf;
extern std::vector<int> list_smoothing_per_oct;

extern int config_stdindex_N;
extern int config_stdindex_L;
extern int config_stdindex_Nf;


extern std::vector<std::string> list_sysident_methods;
extern int config_sysident_TDMMSE;
extern int config_sysident_DUALFFT;
extern int config_stdindex_sysident_methods;

#endif // CONFIG_H
