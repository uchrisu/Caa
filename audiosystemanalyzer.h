#ifndef AUDIOSYSTEMANALYZER_H
#define AUDIOSYSTEMANALYZER_H

#define STANDARD_FILTERLENGTH 4096
#define STANDARD_ANALYZE_LENGTH 64*1024

#define ASA_RETURN_SUCCESS 0
#define ASA_RETURN_BUSY -1

#include "jaudiobuffer.h"
#include "windowfunc.h"
#include <complex.h>
#include <fftw3.h>
#include <atomic>
#include <thread>
#include <mutex>
#include <vector>

class AudioSystemAnalyzer
{
public:
    AudioSystemAnalyzer(JAudioBuffer *buffer, int index = 0);
    ~AudioSystemAnalyzer();

    void set_delay(int delay);
    void set_offset(int offset);
    void set_filterlength(int len);
    void set_freq_length(int len);
    void set_analyze_length(int len);
    void set_freq_smooting(int steps_per_octave);
    void set_sysident_method(int method);
    void set_expTimeSmoothFactor(double factor);
    int get_freq_smooting();
    int identify_delay(int64_t end_position);

    int calc_all(int64_t end_position, bool blocking = true);
    bool is_calculation_done();
    bool is_in_calculation();
    int result_N();
    int result_L();
    int result_Nf();

    int get_impulse_response(double *ir, int len);
    int get_freq_resp(double *fr, int len);
    int get_freq_resp_db(double *fr, int len);
    int get_phase(double *phase, int len);
    int get_phase_unwrapped(double *phase, int len);
    int get_groupdelay(double *gd, int len);
    int get_mscohere(double *msc, int len);
    std::vector<double> get_smfreq_resp();
    std::vector<double> get_smfreq_resp_db();
    std::vector<double> get_smphase();
    std::vector<double> get_smphase_unwrapped();
    std::vector<double> get_smfreq();
    std::vector<double> get_smgroupdelay();
    std::vector<double> get_smmscohere();

    void set_sysident_window_type(int wintype);
    void set_window_length(int length);
    void set_window_offset(int offset);
    void set_window_type(int wintype);
    void get_window_vals(double *vals, int length);

    int get_calc_time_full();
    int get_calc_time_identify();


private:

    int int_identify_IR(int64_t end_position);
    int int_calc_freq_resp();
    int int_calc_phase();
    int int_calc_mscohere();
    int int_calc_smooth();

    int solve_toepliz(double *x, double *a, double *b, int n);
    int solve_toepliz2(double *x, double *c, double *r, double *b, int n);

    void compl_add_phase(double re_in, double im_in, double &re_out, double &im_out, double add_phase);
    void compl_add_phase(double &re, double &im, double add_phase);
    void compl_divide(double &re, double &im, double divider_re, double divider_im);

    void calc_thread_fun();

    void int_set_filterlength(int len);
    void int_set_freq_length(int len);
    void int_set_analyze_length(int len);
    void int_set_freq_smooting(int steps_per_octave);

    int index;
    int sysident_method;

    JAudioBuffer *audiobuffer;
    windowfunc *window, *sysident_window, *mscohere_window;


    int N, Nf;
    int64_t end_position;
    int L;
    int system_delay, additional_offset;
    double expTimeSmoothFactor;

    double *h;
    double *phase, *phase_unwrapped, *groupdelay;
    double *mscohere;
    
    int steps_per_octave;
    std::vector<double> fsmooth_freq;
    std::vector<double> fsmooth_re;
    std::vector<double> fsmooth_im;
    std::vector<double> fsmooth_amp;
    std::vector<double> fsmooth_amp_db;
    std::vector<double> fsmooth_phase;
    std::vector<double> fsmooth_phase_unwrapped;
    std::vector<double> fsmooth_groupdelay;
    std::vector<double> fsmooth_mscohere;

    float *x, *y;

    std::atomic<bool> calculation_done;
    std::atomic<bool> in_calculation;
    std::atomic<bool> shutdown, start_calculation;

    int calc_result_N, calc_result_L, calc_result_Nf;

    fftw_complex *f_h;

    std::thread *calc_thread;

    std::mutex calc_mtx;

    double *data_corr_in;
    double *data_corr_out;
    fftw_complex *f_corr1, *f_corr2;
    double *h_fft;
    fftw_plan plan_corr_in1;
    fftw_plan plan_corr_in2;
    fftw_plan plan_corr_out;
    fftw_plan plan_h2freq;

    double *data_delay_in;
    double *data_delay_out;
    fftw_complex *f_delay1, *f_delay2;
    fftw_plan plan_delay_in1;
    fftw_plan plan_delay_in2;
    fftw_plan plan_delay_out;

    double *dualfft_in;
    fftw_complex *dualfft_fx, *dualfft_fy;
    double *dualfft_out;
    fftw_plan plan_dualfft_in_x, plan_dualfft_in_y;
    fftw_plan plan_dualfft_out;

    double *mscohere_fft_in;
    fftw_complex *mscohere_fft_outx;
    fftw_complex *mscohere_fft_outy;
    fftw_plan plan_mscohere_x;
    fftw_plan plan_mscohere_y;

    std::atomic<int> calc_time_full;
    std::atomic<int> calc_time_identify;

    // new desired values (active in next calc loop):

    std::atomic<int> next_delay;
    std::atomic<int> next_offset;
    std::atomic<int> next_filterlength;
    std::atomic<int> next_freq_length;
    std::atomic<int> next_analyze_length;
    std::atomic<int> next_freq_smooting;
    std::atomic<int> next_sysident_method;
    std::atomic<double> next_expTimeSmoothFactor;
    std::atomic<int> next_sysident_window_type;
    std::atomic<int> next_window_length;
    std::atomic<int> next_window_offset;
    std::atomic<int> next_window_type;


};

#endif // AUDIOSYSTEMANALYZER_H
