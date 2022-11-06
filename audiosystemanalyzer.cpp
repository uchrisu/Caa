#include "audiosystemanalyzer.h"

#include <cassert>
#include <vector>
#include <cmath>
#include <chrono>
#include <iostream>
#include "config.h"


AudioSystemAnalyzer::AudioSystemAnalyzer(JAudioBuffer *buffer, int index)
{
    calc_time_full = 0;
    calc_time_identify = 0;

    this->index = index;
    this->sysident_method = config_stdindex_sysident_methods;

    window = new windowfunc(windowfunc::type_rectangular, 0);
    sysident_window = new windowfunc(windowfunc::type_rectangular, 0);

    this->shutdown = false;
    this->start_calculation = false;
    this->in_calculation = false;
    this->calculation_done = false;

    this->end_position = 0;
    this->L = 0;

    this->audiobuffer = buffer;
    this->additional_offset = 0;
    this->system_delay = 0;
    this->h = nullptr;
    this->f_h = nullptr;
    this->phase = nullptr;
    this->phase_unwrapped = nullptr;
    this->groupdelay = nullptr;
    this->x = nullptr;
    this->y = nullptr;

    // internal calculation (fftw):
    this->data_corr_in = nullptr;
    this->data_corr_out = nullptr;
    f_corr1 = nullptr;
    f_corr2 = nullptr;
    h_fft = nullptr;
    plan_corr_in1 = nullptr;
    plan_corr_in2 = nullptr;
    plan_corr_out = nullptr;
    plan_h2freq = nullptr;

    data_delay_in = nullptr;
    data_delay_out = nullptr;
    f_delay1 = nullptr;
    f_delay2 = nullptr;
    plan_delay_in1 = nullptr;
    plan_delay_in2 = nullptr;
    plan_delay_out = nullptr;

    dualfft_in = nullptr;
    dualfft_fx = nullptr;
    dualfft_fy = nullptr;
    dualfft_out = nullptr;
    plan_dualfft_in_x = nullptr;
    plan_dualfft_in_y = nullptr;
    plan_dualfft_out = nullptr;

    calc_result_N = calc_result_L = calc_result_Nf = 0;


    set_filterlength(STANDARD_FILTERLENGTH);
    set_freq_length(STANDARD_FILTERLENGTH);
    set_analyze_length(STANDARD_ANALYZE_LENGTH);
    set_freq_smooting(0);
    set_expTimeSmoothFactor(0.0);



    calc_thread = new std::thread(&AudioSystemAnalyzer::calc_thread_fun, this);

}

AudioSystemAnalyzer::~AudioSystemAnalyzer()
{
    shutdown = true;
    calc_thread->join();
    delete calc_thread;

    delete window;
    delete sysident_window;

    if (h != nullptr)
        delete[] h;
    if (f_h != nullptr)
        fftw_free(f_h);
    if (phase != nullptr)
        delete[] phase;
    if (phase_unwrapped != nullptr)
        delete[] phase_unwrapped;
    if (groupdelay != nullptr)
        delete[] groupdelay;
    if (x != nullptr)
        delete[] x;
    if (y != nullptr)
        delete[] y;

    if (data_corr_in != nullptr)
        fftw_free(data_corr_in);
    if (data_corr_out != nullptr)
        fftw_free(data_corr_out);
    if (f_corr1 != nullptr)
        fftw_free(f_corr1);
    if (f_corr2 != nullptr)
        fftw_free(f_corr2);
    if (h_fft != nullptr)
        fftw_free(h_fft);
    if (plan_corr_in1 != nullptr)
        fftw_destroy_plan(plan_corr_in1);
    if (plan_corr_in2 != nullptr)
        fftw_destroy_plan(plan_corr_in2);
    if (plan_corr_out != nullptr)
        fftw_destroy_plan(plan_corr_out);
    if (plan_h2freq != nullptr)
        fftw_destroy_plan(plan_h2freq);

    if (data_delay_in != nullptr)
        fftw_free(data_delay_in);
    if (data_delay_out != nullptr)
        fftw_free(data_delay_out);
    if (f_delay1 != nullptr)
        fftw_free(f_delay1);
    if (f_delay2 != nullptr)
        fftw_free(f_delay2);
    if (plan_delay_in1 != nullptr)
        fftw_destroy_plan(plan_delay_in1);
    if (plan_delay_in2 != nullptr)
        fftw_destroy_plan(plan_delay_in2);
    if (plan_delay_out != nullptr)
        fftw_destroy_plan(plan_delay_out);

    if (dualfft_in != nullptr)
        fftw_free(dualfft_in);
    if (dualfft_fx != nullptr)
        fftw_free(dualfft_fx);
    if (dualfft_fy != nullptr)
        fftw_free(dualfft_fy);
    if (dualfft_out != nullptr)
        fftw_free(dualfft_out);
    if (plan_dualfft_in_x != nullptr)
        fftw_destroy_plan(plan_dualfft_in_x);
    if (plan_dualfft_in_y != nullptr)
        fftw_destroy_plan(plan_dualfft_in_y);
    if (plan_dualfft_out != nullptr)
        fftw_destroy_plan(plan_dualfft_out);
}

void AudioSystemAnalyzer::set_delay(int delay)
{
    this->system_delay = delay;
}

void AudioSystemAnalyzer::set_offset(int offset)
{
    this->additional_offset = offset;
}

void AudioSystemAnalyzer::set_filterlength(int len)
{
    while (true){
        if (calc_mtx.try_lock()){
            if (in_calculation){
                calc_mtx.unlock();
            }
            else {
                break;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }

    this->N = len;

    if (this->h != nullptr)
        delete[] h;
    h = new double[len];

    for (int i = 0; i < len; i++)
        this->h[i] = 0.0;

    if (dualfft_in != nullptr)
        fftw_free(dualfft_in);
    dualfft_in = (double *) fftw_malloc(sizeof(double) * N);
    if (dualfft_in == nullptr)
        perror("malloc failed");

    if (dualfft_fx != nullptr)
        fftw_free(dualfft_fx);
    dualfft_fx = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * N);
    if (dualfft_fx == nullptr)
        perror("malloc failed");

    if (dualfft_fy != nullptr)
        fftw_free(dualfft_fy);
    dualfft_fy = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * N);
    if (dualfft_fy == nullptr)
        perror("malloc failed");

    if (dualfft_out != nullptr)
        fftw_free(dualfft_out);
    dualfft_out = (double *) fftw_malloc(sizeof(double) * N);
    if (dualfft_out == nullptr)
        perror("malloc failed");

    if (plan_dualfft_in_x != nullptr)
        fftw_destroy_plan(plan_dualfft_in_x);
    plan_dualfft_in_x = fftw_plan_dft_r2c_1d(N, dualfft_in, dualfft_fx, FFTW_ESTIMATE);

    if (plan_dualfft_in_y != nullptr)
        fftw_destroy_plan(plan_dualfft_in_y);
    plan_dualfft_in_y = fftw_plan_dft_r2c_1d(N, dualfft_in, dualfft_fy, FFTW_ESTIMATE);

    if (plan_dualfft_out != nullptr)
        fftw_destroy_plan(plan_dualfft_out);
    plan_dualfft_out = fftw_plan_dft_c2r_1d(N, dualfft_fy, dualfft_out, FFTW_ESTIMATE);


    window->set_window_length(2*N);
    window->set_window_offset(-N);

    sysident_window->set_window_length(N);


    calculation_done = false;
    calc_result_N = 0;
    calc_mtx.unlock();

}

void AudioSystemAnalyzer::set_freq_length(int len)
{
    while (true){
        if (calc_mtx.try_lock()){
            if (in_calculation){
                calc_mtx.unlock();
            }
            else {
                break;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }

    this->Nf = len;

    if (this->phase != nullptr)
        delete[] phase;
    phase = new double[len];

    if (this->phase_unwrapped != nullptr)
        delete[] phase_unwrapped;
    phase_unwrapped = new double[len];

    if (this->groupdelay != nullptr)
        delete[] groupdelay;
    groupdelay = new double[len];

    if (this->f_h != nullptr)
        fftw_free(f_h);
    f_h = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * len);

    if (h_fft != nullptr)
        fftw_free(h_fft);
    h_fft = (double *) fftw_malloc(sizeof(double) * Nf);

    if (plan_h2freq != nullptr)
        fftw_destroy_plan(plan_h2freq);
    plan_h2freq = fftw_plan_dft_r2c_1d(Nf, h_fft, f_h, FFTW_ESTIMATE);

    for (int i = 0; i < len; i++){
        this->f_h[i][0] = 0.0;
        this->f_h[i][1] = 0.0;
        this->phase[i] = 0.0;
        this->phase_unwrapped[i] = 0.0;
        this->groupdelay[i] = 0.0;
    }
    calculation_done = false;
    calc_result_Nf = 0;
    calc_mtx.unlock();
}

void AudioSystemAnalyzer::set_analyze_length(int len)
{
    while (true){
        if (calc_mtx.try_lock()){
            if (in_calculation){
                calc_mtx.unlock();
            }
            else {
                break;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }

    this->L = len;

    if (x != nullptr){
        delete[] x;
    }
    x = new float[len];

    if (y != nullptr){
        delete[] y;
    }
    y = new float[len];

    // correlation for identify ir

    if (data_corr_in != nullptr)
        fftw_free(data_corr_in);
    data_corr_in = (double *) fftw_malloc(sizeof(double) * 2 * L);

    if (data_corr_out != nullptr)
        fftw_free(data_corr_out);
    data_corr_out = (double *) fftw_malloc(sizeof(double) * 2 * L);

    if (f_corr1 != nullptr)
        fftw_free(f_corr1);
    f_corr1 = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * 2*L);

    if (f_corr2 != nullptr)
        fftw_free(f_corr2);
    f_corr2 = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * 2*L);

    if (plan_corr_in1 != nullptr)
        fftw_destroy_plan(plan_corr_in1);
    plan_corr_in1 = fftw_plan_dft_r2c_1d(2*L, data_corr_in, f_corr1, FFTW_ESTIMATE);

    if (plan_corr_in2 != nullptr)
        fftw_destroy_plan(plan_corr_in2);
    plan_corr_in2 = fftw_plan_dft_r2c_1d(2*L, data_corr_in, f_corr2, FFTW_ESTIMATE);

    if (plan_corr_out != nullptr)
        fftw_destroy_plan(plan_corr_out);
    plan_corr_out = fftw_plan_dft_c2r_1d(2*L, f_corr1, data_corr_out, FFTW_ESTIMATE);


    // correlation for delay

    if (data_delay_in != nullptr)
        fftw_free(data_delay_in);
    data_delay_in = (double *) fftw_malloc(sizeof(double) * 2 * L);

    if (data_delay_out != nullptr)
        fftw_free(data_delay_out);
    data_delay_out = (double *) fftw_malloc(sizeof(double) * 2 * L);

    if (f_delay1 != nullptr)
        fftw_free(f_delay1);
    f_delay1 = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * 2*L);

    if (f_delay2 != nullptr)
        fftw_free(f_delay2);
    f_delay2 = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * 2*L);

    if (plan_delay_in1 != nullptr)
        fftw_destroy_plan(plan_delay_in1);
    plan_delay_in1 = fftw_plan_dft_r2c_1d(2*L, data_delay_in, f_delay1, FFTW_ESTIMATE);

    if (plan_delay_in2 != nullptr)
        fftw_destroy_plan(plan_delay_in2);
    plan_delay_in2 = fftw_plan_dft_r2c_1d(2*L, data_delay_in, f_delay2, FFTW_ESTIMATE);

    if (plan_delay_out != nullptr)
        fftw_destroy_plan(plan_delay_out);
    plan_delay_out = fftw_plan_dft_c2r_1d(2*L, f_delay1, data_delay_out, FFTW_ESTIMATE);

    calculation_done = false;
    calc_result_L = 0;
    calc_mtx.unlock();
}


void AudioSystemAnalyzer::set_freq_smooting(int steps_per_octave)
{
    while (true){
        if (calc_mtx.try_lock()){
            if (in_calculation){
                calc_mtx.unlock();
            }
            else {
                break;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }

    this->steps_per_octave = steps_per_octave;
    if (steps_per_octave <= 0){
        calc_mtx.unlock();
        return;
    }
    int len = 10*steps_per_octave+1;
    fsmooth_freq.resize(len);
    fsmooth_re.resize(len);
    fsmooth_im.resize(len);
    fsmooth_amp.resize(len);
    fsmooth_amp_db.resize(len);
    fsmooth_phase.resize(len);
    fsmooth_phase_unwrapped.resize(len);
    fsmooth_groupdelay.resize(len);
    for (int i = 0; i < len; i++){
        fsmooth_freq[i] = 20*std::pow(2, static_cast<double>(i)/steps_per_octave);
        fsmooth_re[i] = 0.0;
        fsmooth_im[i] = 0.0;
        fsmooth_amp[i] = 0.0;
        fsmooth_amp_db[i] = 0.0;
        fsmooth_phase[i] = 0.0;
        fsmooth_phase_unwrapped[i] = 0.0;
        fsmooth_groupdelay[i] = 0.0;
    }
    calc_mtx.unlock();
}

void AudioSystemAnalyzer::set_sysident_method(int method)
{
    sysident_method = method;
}

void AudioSystemAnalyzer::set_expTimeSmoothFactor(double factor)
{
    double fac = factor;
    if (fac < 0)
        fac = 0.0;
    if (fac > 1)
        fac = 1.0;
    while (true){
        if (calc_mtx.try_lock()){
            if (in_calculation){
                calc_mtx.unlock();
            }
            else {
                break;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }
    expTimeSmoothFactor = fac;
    calc_mtx.unlock();
}

int AudioSystemAnalyzer::get_freq_smooting()
{
    return steps_per_octave;
}


int AudioSystemAnalyzer::int_identify_IR(int64_t end_position)
{
    auto start_t = std::chrono::high_resolution_clock::now();
    // Some comments here of old code for a better explanation of what the algorithm does
    double h_new[N];
    int64_t pos_ref, pos_sig;
    //pos_ref = end_position - L - N + 1; // N - 1 shift, see comment below
    //pos_sig = end_position - L - N + 1;
    pos_ref = end_position - L; // N - 1 shift, see comment below
    pos_sig = end_position - L;


    int full_delay = system_delay - additional_offset;
    if (full_delay > 0)
        pos_ref -= full_delay;
    if (full_delay < 0)
        pos_sig += full_delay;

    //float *x, *y;
    //x = new float[L + N - 1];
    //y = new float[L + N - 1]; // L would be enough, but take same length for clarity

    //audiobuffer->get_samples(0, pos_ref, x, L + N - 1);
    //audiobuffer->get_samples(1, pos_sig, y, L + N - 1);

    audiobuffer->get_samples(index * 2, pos_ref, x, L);
    audiobuffer->get_samples(index * 2 + 1, pos_sig, y, L);


    if (sysident_method == config_sysident_TDMMSE){

        //    int start = N - 1;
        double *toprow = new double[N];
        double *rxy = new double[N];
        for (int i = 0; i < N; i++){
            toprow[i] = 0.0;
            rxy[i] = 0.0;
        }

        //    for (int k = start; k < (start + L); k++){
        //        for (int j = 0; j < N; j++){
        //            toprow[j] += x[k] * x[k-j];
        //            rxy[j] += y[k] * x[k-j];
        //        }
        //    }


        for (int i = 0; i < L; i++)
            data_corr_in[i] = x[i];
        for (int i = L; i < 2*L; i++)
            data_corr_in[i] = 0;
        fftw_execute(plan_corr_in1);
        for (int i = 0; i < L; i++)
            data_corr_in[i] = x[L-i-1];
        for (int i = L; i < 2*L; i++)
            data_corr_in[i] = 0;
        fftw_execute(plan_corr_in2);
        for (int i = 0; i < 2*L; i++){
            double re = f_corr1[i][0]*f_corr2[i][0] - f_corr1[i][1]*f_corr2[i][1];
            double im = f_corr1[i][0]*f_corr2[i][1] + f_corr1[i][1]*f_corr2[i][0];
            f_corr1[i][0] = re;
            f_corr1[i][1] = im;
        }
        fftw_execute(plan_corr_out);
        for (int i = 0; i < N; i++)
            toprow[i] = data_corr_out[L - 1 + i]/(2*L);

        for (int i = 0; i < L; i++)
            data_corr_in[i] = y[i];
        for (int i = L; i < 2*L; i++)
            data_corr_in[i] = 0;
        fftw_execute(plan_corr_in1);
        for (int i = 0; i < L; i++)
            data_corr_in[i] = x[L-i-1];
        for (int i = L; i < 2*L; i++)
            data_corr_in[i] = 0;
        fftw_execute(plan_corr_in2);
        for (int i = 0; i < 2*L; i++){
            double re = f_corr1[i][0]*f_corr2[i][0] - f_corr1[i][1]*f_corr2[i][1];
            double im = f_corr1[i][0]*f_corr2[i][1] + f_corr1[i][1]*f_corr2[i][0];
            f_corr1[i][0] = re;
            f_corr1[i][1] = im;
        }
        fftw_execute(plan_corr_out);
        for (int i = 0; i < N; i++)
            rxy[i] = data_corr_out[L - 1 + i]/(2*L);

        double toepv[2*N-1];

        for (int i = 0; i < N; i++)
            toepv[i] = toprow[N-i-1];
        for (int i = 1; i < N; i++)
            toepv[i+N-1] = toprow[i];

        solve_toepliz(h_new, toepv, rxy, N);


        //solve_toepliz2(h, toprow, toprow, rxy, N);

        delete[] toprow;
        delete[] rxy;
    }
    else if (sysident_method == config_sysident_DUALFFT){
        int num_avg = L/N;
        double dualfft_x[N];
        double dualfft_y[N];
        for (int i = 0; i < N; i++){
            dualfft_x[i] = 0;
            dualfft_y[i] = 0;
        }
        for (int avg = 0; avg < num_avg; avg++){
            for (int i = 0; i < N; i++){
                dualfft_x[i] += x[avg*N+i];
                dualfft_y[i] += y[avg*N+i];
            }
        }

        for (int i = 0; i < N; i++){
            dualfft_in[i] = dualfft_x[i] * sysident_window->get_factor(i); //[avg*N+i];
        }
        fftw_execute(plan_dualfft_in_x);
        for (int i = 0; i < N; i++){
            dualfft_in[i] = dualfft_y[i] * sysident_window->get_factor(i); //y[avg*N+i];
        }
        fftw_execute(plan_dualfft_in_y);
        for (int i = 0; i < N; i++){
            //reinterpret_cast<std::complex<double>>(dualfft_fy[i]) /= dualfft_fx[i];
            compl_divide(dualfft_fy[i][0], dualfft_fy[i][1], dualfft_fx[i][0], dualfft_fx[i][1]);
        }
        fftw_execute(plan_dualfft_out);
        for (int i = 0; i < N; i++){
            h_new[i] = dualfft_out[i]/N;
        }


    }
    else
        return -1;

    for (int i = 0; i < N; i++){
        if (!std::isnormal(h_new[i]))
            h_new[i] = 0.0;
        h[i] = (1-expTimeSmoothFactor) * h_new[i] + expTimeSmoothFactor * h[i];
        if (std::isnan(h[i]))
            std::cout << "NAN" << std::endl;
    }

    auto ende_t  = std::chrono::high_resolution_clock::now();

    calc_time_identify = std::chrono::duration_cast<std::chrono::milliseconds>(ende_t-start_t).count();

    return 0;

}


int AudioSystemAnalyzer::get_impulse_response(double *ir, int len)
{
    if (!calc_mtx.try_lock())
        return ASA_RETURN_BUSY;
    int num = len;
    if (num > calc_result_N)
        num = calc_result_N;
    for (int i = 0; i < num; i++)
        ir[i] = h[i];
    for (int i = num; i < len; i++)
        ir[i] = 0;
    for (int i = 0; i < len; i++)
        if (std::isnan(ir[i]))
            std::cout << "NAN2" << std::endl;
    calc_mtx.unlock();

    return num;
}


int AudioSystemAnalyzer::int_calc_freq_resp()
{
    int lim1 = N;
    if (Nf < N)
        lim1 = Nf;
    for (int i = 0; i < lim1; i++)
        h_fft[i] = h[i] * window->get_factor(i);
    for (int i = lim1; i < Nf; i ++)
        h_fft[i] = 0;

    fftw_execute(plan_h2freq);

    // phase (offset) compensation:

    for (int i = 0; i < Nf; i++){
        compl_add_phase(f_h[i][0], f_h[i][1], 2 * M_PI * i * additional_offset / Nf );
    }

    return 0;
}


int AudioSystemAnalyzer::identify_delay(int64_t end_position)
{
    calc_mtx.lock();
    int64_t pos = end_position - L;

    float x[L], y[L];

    audiobuffer->get_samples(index * 2, pos, x, L);
    audiobuffer->get_samples(index * 2 + 1, pos, y, L);

    for (int i = 0; i < L; i++)
        data_delay_in[i] = x[L-i-1];
    for (int i = L; i < 2*L; i++)
        data_delay_in[i] = 0;
    fftw_execute(plan_delay_in1);
    for (int i = 0; i < L; i++)
        data_delay_in[i] = y[i];
    for (int i = L; i < 2*L; i++)
        data_delay_in[i] = 0;
    fftw_execute(plan_delay_in2);
    for (int i = 0; i < 2*L; i++){
        double re = f_delay1[i][0]*f_delay2[i][0] - f_delay1[i][1]*f_delay2[i][1];
        double im = f_delay1[i][0]*f_delay2[i][1] + f_delay1[i][1]*f_delay2[i][0];
        f_delay1[i][0] = re;
        f_delay1[i][1] = im;
    }
    fftw_execute(plan_delay_out);

   /* double max = 0;
    int max_pos = 0;
    for (int i = 0; i < 2*L; i++){
        if (std::abs(data_delay_out[i]) > max ){
            max = std::abs(data_delay_out[i]);
            max_pos = i;
        }
    }

    for (int i = 0; i < 2*L; i++){
        if (std::abs(data_delay_out[i]) >=  (0.5 * max) ){
            max_pos = i;
            break;
        }
    }*/

    for (int i = 0; i < (2*L - 1); i++){
        data_delay_out[i] -= data_delay_out[i+1];
    }
    for (int i = 0; i < (2*L - 1); i++){
        data_delay_out[i] -= data_delay_out[i+1];
    }
    double max = 0;
    int max_pos = 0;
    for (int i = 2; i < (2*L-2); i++){
        if (std::abs(data_delay_out[i]) > max ){
            max = std::abs(data_delay_out[i]);
            max_pos = i;
        }
    }


    calc_mtx.unlock();

    return max_pos - L + 1;
}

int AudioSystemAnalyzer::int_calc_phase()
{
    if (Nf < 1)
        return -1;

    phase[0] = atan2(f_h[0][1], f_h[0][0]);
    phase_unwrapped[0] = phase[0];
    double offset = 0.0;
    for (int i = 1; i < Nf; i++){
        phase[i] = atan2(f_h[i][1], f_h[i][0]); // + 2 * M_PI * i * additional_offset / Nf;
        phase[i] = fmod((phase[i] + M_PI), 2*M_PI) - M_PI;
        double tmp = phase[i] + offset;
        if ((tmp - phase_unwrapped[i-1]) > M_PI){
            tmp -= 2 * M_PI;
            offset -= 2*M_PI;
        }
        if ((tmp - phase_unwrapped[i-1]) < (- M_PI)){
            tmp += 2 * M_PI;
            offset += 2*M_PI;
        }
        phase_unwrapped[i] = tmp;
    }
    double dw = static_cast<double>(audiobuffer->get_fs()) / Nf * 2 * M_PI;
    groupdelay[0] = 1000 * (phase_unwrapped[1] - phase_unwrapped[0]) / dw;
    for (int i = 1; i < (Nf - 1); i++){
        groupdelay[i] = - 1000 * (phase_unwrapped[i+1] - phase_unwrapped[i-1]) / (2 * dw);
    }
    groupdelay[Nf - 1] = 1000 * (phase_unwrapped[Nf -1] - phase_unwrapped[Nf - 2]) / dw;
    return 0;
}


int AudioSystemAnalyzer::int_calc_smooth()
{
    if (steps_per_octave == 0)
      return 0;
    if (Nf <= 0)
        return -1;
    int len = fsmooth_freq.size();
    int fpos = 0;
    double sum_i, sum_re, sum_im;
    double fs = audiobuffer->get_fs();
    double offset = 0.0;
    for (int i = 0; i < len; i++){
        if (fpos >= Nf) 
            break;
        double lim1 = 20*std::pow(2, (i-0.5)/steps_per_octave);
        double lim2 = 20*std::pow(2, (i+0.5)/steps_per_octave);
        while((static_cast<double>(fpos)/Nf*fs) < lim1){
            fpos++;
        }
        if (fpos >= Nf) 
            break;
        sum_i = sum_re = sum_im = 0;
        while((static_cast<double>(fpos)/Nf*fs) <= lim2){
            sum_i += 1;
            sum_re += f_h[fpos][0];
            sum_im += f_h[fpos][1];
            fpos++;
             if (fpos >= Nf) 
                break;
        }
        if (sum_i > 0) {
            fsmooth_re[i] = sum_re/sum_i;
            fsmooth_im[i] = sum_im/sum_i;
        }
        else {

            if (fpos >= Nf) {
                fsmooth_re[i] = f_h[Nf-1][0];
                fsmooth_im[i] = f_h[Nf-1][1];
            }
            else if (fpos == 0) {
                fsmooth_re[i] = f_h[0][0];
                fsmooth_im[i] = f_h[0][1];
            }
            else {
                double center_freq = 20*std::pow(2, static_cast<double>(i)/steps_per_octave);
                double f_left = static_cast<double>(fpos-1)/Nf*fs;
                double f_right = static_cast<double>(fpos)/Nf*fs;
                double factor_l = (f_right-center_freq)/(f_right-f_left);
                double factor_r = (center_freq-f_left)/(f_right-f_left);
                fsmooth_re[i] = f_h[fpos-1][0]*factor_l + f_h[fpos][0]*factor_r;
                fsmooth_im[i] = f_h[fpos-1][1]*factor_l + f_h[fpos][1]*factor_r;
            }

        }
        
        fsmooth_amp[i] = sqrt(fsmooth_re[i]*fsmooth_re[i] + fsmooth_im[i]*fsmooth_im[i]);
        fsmooth_amp_db[i] = 20*std::log10(fsmooth_amp[i]);
        fsmooth_phase[i] = atan2(fsmooth_im[i], fsmooth_re[i]);
        fsmooth_phase[i] = fmod((fsmooth_phase[i] + M_PI), 2*M_PI) - M_PI;
        if (i == 0)
            fsmooth_phase_unwrapped[i] = fsmooth_phase[i];
        else {
            double tmp = fsmooth_phase[i] + offset;
            if ((tmp - fsmooth_phase_unwrapped[i-1]) > M_PI){
                tmp -= 2 * M_PI;
                offset -= 2*M_PI;
            }
            if ((tmp - fsmooth_phase_unwrapped[i-1]) < (- M_PI)){
                tmp += 2 * M_PI;
                offset += 2*M_PI;
            }
            fsmooth_phase_unwrapped[i] = tmp;
            }
        
    }
    for (int i = 0; i < len; i++){
        if (i == 0){
            fsmooth_groupdelay[i] = - 1000 * (fsmooth_phase_unwrapped[i+1]-fsmooth_phase_unwrapped[0]) / (fsmooth_freq[i+1]-fsmooth_freq[0]) / (2 * M_PI);
        }
        else if (i == (len - 1)){
            fsmooth_groupdelay[i] = - 1000 * (fsmooth_phase_unwrapped[i]-fsmooth_phase_unwrapped[i-1]) / (fsmooth_freq[i]-fsmooth_freq[i-1]) / (2 * M_PI);
        }
        else {
            fsmooth_groupdelay[i] = - 1000 * (fsmooth_phase_unwrapped[i+1]-fsmooth_phase_unwrapped[i-1]) / (fsmooth_freq[i+1]-fsmooth_freq[i-1]) / (2 * M_PI);
        }

    }
    
    return 0;
}


int AudioSystemAnalyzer::calc_all(int64_t end_position, bool blocking)
{
    if (!calc_mtx.try_lock())
        return ASA_RETURN_BUSY;
    this->calculation_done = false;
    this->in_calculation = true;
    this->end_position = end_position;
    start_calculation = true;
    if (blocking)
        while (!calculation_done)
            std::this_thread::sleep_for(std::chrono::milliseconds(2));
    calc_mtx.unlock();
    return (0);
}

bool AudioSystemAnalyzer::is_calculation_done()
{
    return calculation_done;
}

bool AudioSystemAnalyzer::is_in_calculation()
{
    return in_calculation;
}

int AudioSystemAnalyzer::result_N()
{
    return calc_result_N;
}

int AudioSystemAnalyzer::result_L()
{
    return calc_result_L;
}

int AudioSystemAnalyzer::result_Nf()
{
    return calc_result_Nf;
}

int AudioSystemAnalyzer::get_freq_resp(double *fr, int len)
{
    if (!calc_mtx.try_lock())
        return ASA_RETURN_BUSY;
    int num = len;
    if (num > calc_result_Nf)
        num = calc_result_Nf;
    for (int i = 0; i < num; i++){
        double val = sqrt(f_h[i][0]*f_h[i][0] + f_h[i][1]*f_h[i][1]);
        if (std::isnormal(val))
            fr[i] = val;
        else
            fr[i] = 0;
    }
    for (int i = num; i < len; i++)
        fr[i] = 0;
    calc_mtx.unlock();

    return num;
}


int AudioSystemAnalyzer::get_freq_resp_db(double *fr, int len)
{
    if (!calc_mtx.try_lock())
        return ASA_RETURN_BUSY;
    int num = len;
    if (num > calc_result_Nf)
        num = calc_result_Nf;
    for (int i = 0; i < num; i++){
        double val = 20*log10(sqrt(f_h[i][0]*f_h[i][0] + f_h[i][1]*f_h[i][1]));
        if (std::isnormal(val))
            fr[i] = val;
        else
            fr[i] = 0;
    }
    for (int i = num; i < len; i++)
        fr[i] = 0;
    calc_mtx.unlock();

    return num;
}

int AudioSystemAnalyzer::get_phase(double *phase, int len)
{
    if (!calc_mtx.try_lock())
        return ASA_RETURN_BUSY;
    int num = len;
    if (num > calc_result_Nf)
        num = calc_result_Nf;
    for (int i = 0; i < num; i++)
        phase[i] = this->phase[i];
    for (int i = num; i < len; i++)
        phase[i] = 0;
    calc_mtx.unlock();

    return num;
}

int AudioSystemAnalyzer::get_phase_unwrapped(double *phase, int len)
{
    if (!calc_mtx.try_lock())
        return ASA_RETURN_BUSY;
    int num = len;
    if (num > N)
        num = N;
    for (int i = 0; i < num; i++)
        phase[i] = phase_unwrapped[i];
    for (int i = num; i < len; i++)
        phase[i] = 0;
    calc_mtx.unlock();

    return num;
}

int AudioSystemAnalyzer::get_groupdelay(double *gd, int len)
{
    if (!calc_mtx.try_lock())
        return ASA_RETURN_BUSY;
    int num = len;
    if (num > N)
        num = N;
    for (int i = 0; i < num; i++)
        gd[i] = groupdelay[i];
    for (int i = num; i < len; i++)
        gd[i] = 0;
    calc_mtx.unlock();

    return num;
}

std::vector<double> AudioSystemAnalyzer::get_smfreq_resp()
{
    return fsmooth_amp;
}

std::vector<double> AudioSystemAnalyzer::get_smfreq_resp_db()
{
    return fsmooth_amp_db;
}

std::vector<double> AudioSystemAnalyzer::get_smphase()
{
    return fsmooth_phase;
}

std::vector<double> AudioSystemAnalyzer::get_smphase_unwrapped()
{
    return fsmooth_phase_unwrapped;
}

std::vector<double> AudioSystemAnalyzer::get_smfreq()
{
    return fsmooth_freq;
}

std::vector<double> AudioSystemAnalyzer::get_smgroupdelay()
{
    return fsmooth_groupdelay;
}

void AudioSystemAnalyzer::set_sysident_window_type(int wintype)
{
    sysident_window->set_window_type(wintype);
}

void AudioSystemAnalyzer::set_window_length(int length)
{
    if (length <= 0)
        window->set_window_length(2*N);
    else
        window->set_window_length(2*length);
}

void AudioSystemAnalyzer::set_window_offset(int offset)
{
    window->set_window_offset(offset-window->get_window_length()/2);
}

void AudioSystemAnalyzer::set_window_type(int wintype)
{
    window->set_window_type(wintype);
}

void AudioSystemAnalyzer::get_window_vals(double *vals, int length)
{
    window->get_window(vals, length);
}

int AudioSystemAnalyzer::get_calc_time_full()
{
    return calc_time_full;
}

int AudioSystemAnalyzer::get_calc_time_identify()
{
    return calc_time_identify;
}


int AudioSystemAnalyzer::solve_toepliz(double *x, double *a, double *b, int n)
{
    /* Taken and adapted from https://github.com/scipy/scipy/blob/main/scipy/linalg/_solve_toeplitz.pyx
     * (Original author Author: Robert T. McGibbon, December 2014)
     * which itself was adpated from
     * toeplitz.f90 by Alan Miller, accessed at
     * http://jblevins.org/mirror/amiller/toeplitz.f90
     * Released under a Public domain declaration.
     */
    //double x[n];
    double g[n];
    double h[n];
    //double reflection_coeff[n+1];

    double x_num, x_den, g_num, h_num, g_den;
    double gj, gk, hj, hk, c1, c2;

    int nmj, k, m2;

    for (int i = 0; i < n; i++){
        x[i] = 0;
        g[i] = 0;
        h[i] = 0;
        //reflection_coeff[i] = 0;
    }
    //reflection_coeff[n] = 0;

    if (a[n-1] == 0)
        return -1;
    x[0] = b[0] / a[n-1];
    //reflection_coeff[0] = 1;
    //reflection_coeff[1] = x[0];

    if (n == 1)
        return 0;

    g[0] = a[n-2] / a[n-1];
    h[0] = a[n] / a[n-1];

    for (int m = 1; m < n; m++){ // m in range(1, n):
        // Compute numerator and denominator of x[m]
        x_num = -b[m];
        x_den = -a[n-1];
        for (int j = 0; j < m; j++){ // j in range(m):
            nmj = n + m - (j+1);
            x_num = x_num + a[nmj] * x[j];
            x_den = x_den + a[nmj] * g[m-j-1];
        }
        if (x_den == 0){
            //raise LinAlgError('Singular principal minor')
            return(-2);
        }
        x[m] = x_num / x_den;
        //reflection_coeff[m+1] = x[m];

        // Compute x
        for (int j = 0; j < m; j++){ // j in range(m):
            x[j] = x[j] - x[m] * g[m-j-1];
        }
        if (m == n-1)
            return(0);

        // Compute the numerator and denominator of g[m] and h[m]
        g_num = -a[n-m-2];
        h_num = -a[n+m];
        g_den = -a[n-1];
        for (int j = 0; j < m; j++ ){ // j in range(m):
            g_num = g_num + a[n+j-m-1] * g[j];
            h_num = h_num + a[n+m-j-1] * h[j];
            g_den = g_den + a[n+j-m-1] * h[m-j-1];
        }

        if (g_den == 0.0){
            //raise LinAlgError("Singular principal minor")
            return(-2);
        }

        // Compute g and h
        g[m] = g_num / g_den;
        h[m] = h_num / x_den;
        k = m - 1;
        m2 = (m + 1) >> 1;
        c1 = g[m];
        c2 = h[m];
        for (int j = 0; j < m2; j++){ // j in range(m2):
            gj = g[j];
            gk = g[k];
            hj = h[j];
            hk = h[k];
            g[j] = gj - (c1 * hk);
            g[k] = gk - (c1 * hj);
            h[j] = hj - (c2 * gk);
            h[k] = hk - (c2 * gj);
            k -= 1;
        }

    }
    return(-3);

}


void AudioSystemAnalyzer::compl_add_phase(double re_in, double im_in, double &re_out, double &im_out, double add_phase)
{
    double amp = sqrt(re_in*re_in+im_in*im_in);
    double phase = std::atan2(im_in, re_in);
    phase += add_phase;
    re_out = amp * cos(phase);
    im_out = amp * sin(phase);
}

void AudioSystemAnalyzer::compl_add_phase(double &re, double &im, double add_phase)
{
    double amp = sqrt(re*re+im*im);
    double phase = std::atan2(im, re);
    phase += add_phase;
    re = amp * cos(phase);
    im = amp * sin(phase);
}

void AudioSystemAnalyzer::compl_divide(double &re, double &im, double divider_re, double divider_im)
{
    double den = divider_re*divider_re + divider_im*divider_im;
    double tmp_re = re * divider_re + im * divider_im;
    double tmp_im = im * divider_re - re * divider_im;
    re = tmp_re / den;
    im = tmp_im / den;
}


void AudioSystemAnalyzer::calc_thread_fun()
{
    while (!shutdown){
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        if (start_calculation){
            calculation_done = false;
            start_calculation = false;
            if (L > 0){
                calc_mtx.lock();
                auto start_t  = std::chrono::high_resolution_clock::now();
                int_identify_IR(end_position);
                int_calc_freq_resp();
                int_calc_phase();
                int_calc_smooth();
                auto ende_t  = std::chrono::high_resolution_clock::now();
                calc_time_full = std::chrono::duration_cast<std::chrono::milliseconds>(ende_t-start_t).count();
                calc_mtx.unlock();
            }
            calc_result_N = N;
            calc_result_L = L;
            calc_result_Nf = Nf;
            calculation_done = true;
            in_calculation = false;

        }
    }
}
