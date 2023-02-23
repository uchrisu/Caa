#include "windowfunc.h"
#include <cmath>

windowfunc::windowfunc(int window_type, int window_length)
{
    this->window_type = window_type;
    this->N = window_length;
    this->offset = 0;

}

void windowfunc::set_window_type(int windowtype)
{
    this->window_type = windowtype;
}

void windowfunc::set_window_length(int length)
{
    this->N = length;
}

void windowfunc::set_window_offset(int offset)
{
    this->offset = offset;
}

int windowfunc::get_window_type()
{
    return this->window_type;
}

int windowfunc::get_window_length()
{
    return this->N;
}

int windowfunc::get_window_offset()
{
    return this->offset;
}

double windowfunc::get_factor(int n)
{
    auto cosine_sum = [](double n, double N, double a0, double a1, double a2, double a3, double a4)
    {
        return a0 - a1*cos((2*M_PI*n)/N) + a2*cos((4*M_PI*n)/N) - a3*cos((6*M_PI*n)/N) + a4*cos((8*M_PI*n)/N);
    };

    n -= offset;

    if (n < 0)
        return 0.0;
    if (n > N)
        return 0.0;

    double factor = 1.0; // default to rectangular
    switch (window_type) {
        case type_rectangular:
            factor = 1;
            break;
        case type_triangle:
            factor = 1.0 - std::abs((n-N/2.0)/(N/2.0));
            break;
        case type_welch:
            factor = 1.0 - pow(((n-N/2.0)/(N/2.0)),2);
            break;
        case type_sine:
            factor = sin((M_PI*n)/N);
            break;
        case type_hann:
            factor = cosine_sum(n, N, 0.5, 0.5, 0, 0, 0);
            break;
        case type_hamming:
            factor = cosine_sum(n, N, 25.0/46, 21.0/46, 0, 0, 0);
            break;
        case type_blackman:
            factor = cosine_sum(n, N, 7938.0/18608, 9240.0/18608, 1430.0/18608, 0, 0);
            break;
        case type_nuttall:
            factor = cosine_sum(n, N, 0.355768, 0.487396, 0.144232, 0.012604, 0);
            break;
        case type_blackman_nuttall:
            factor = cosine_sum(n, N, 0.3635819, 0.4891775, 0.1365995, 0.0106411, 0);
            break;
        case type_blackman_harris:
            factor = cosine_sum(n, N, 0.35875, 0.48829, 0.14128, 0.01168, 0);
            break;
        case type_flat_top:
            factor = cosine_sum(n, N, 0.21557895, 0.41663158, 0.277263158, 0.083578947, 0.006947368);
            break;
        case type_lanczos:
            factor = (2.0*n)/N - 1;
            if (std::abs(factor) > 1.0e-12)
              factor = sin(M_PI*factor)/(M_PI*factor);
            else
              factor = 1.0;
            break;
    }

    return factor;
}

void windowfunc::apply_window(double *vals, int len)
{
    for (int i = 0; i < len; i++)
    {
        vals[i] *= get_factor(i);
    }
}

void windowfunc::get_window(double *vals, int len, int additional_offset)
{
    for (int i = 0; i < len; i++)
    {
        vals[i] = get_factor(i - additional_offset);
    }
}

std::string windowfunc::get_type_name(int index)
{
    auto names = windowfunc::get_type_names();
    return names[index];
}


