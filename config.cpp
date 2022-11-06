#include <string>
#include <vector>

int config_stdindex_N = 3;
std::vector<int> list_lengths_N = {512, 1024, 2048, 4096, 8192, 16*1024};

int config_stdindex_L = 3;
std::vector<int> list_lengths_L = {8*1024, 16*1024, 32*1024, 64*1024, 128*1024, 256*1024};

int config_stdindex_Nf = 3;
std::vector<int> list_lengths_Nf = {512, 1024, 2048, 4096, 8192, 16*1024, 32*1024, 64*1024, 128*1024};

std::vector<int> list_smoothing_per_oct = {3, 6, 12, 24, 48};

std::vector<std::string> list_sysident_methods = {"TDMMSE", "Dual FFT"};
int config_sysident_TDMMSE = 0;
int config_sysident_DUALFFT = 1;
int config_stdindex_sysident_methods = 0;
