#ifndef WINDOWFUNC_H
#define WINDOWFUNC_H

#include <string>
#include <vector>


class windowfunc
{
public:
    windowfunc(int window_type, int window_length);

    void set_window_type(int windowtype);
    void set_window_length(int length);
    void set_window_offset(int offset);
    int get_window_type();
    int get_window_length();
    int get_window_offset();
    double get_factor(int n);
    void apply_window(double *vals, int len);
    void get_window(double *vals, int len, int additional_offset = 0);

    static std::string get_type_name(int index);

    static std::vector<std::string> get_type_names() {
        std::vector<std::string> type_names = {
                "Rectangular",
                "Triangle",
                "Welch",
                "Sine",
                "Hann",
                "Hamming",
                "Blackman",
                "Nuttall",
                "Blackman Nuttall",
                "Blackman Harris",
                "Flat Top",
                "Lanczos" };
        return type_names;
    }


    static const int type_rectangular = 0;
    static const int type_triangle = 1;
    static const int type_welch = 2;
    static const int type_sine = 3;
    static const int type_hann = 4;
    static const int type_hamming = 5;
    static const int type_blackman = 6;
    static const int type_nuttall = 7;
    static const int type_blackman_nuttall = 8;
    static const int type_blackman_harris = 9;
    static const int type_flat_top = 10;
    static const int type_lanczos = 11;

    static const int num_types = 12;

private:


    int window_type;
    int N; // Window Length
    int offset;
};

#endif // WINDOWFUNC_H
