#ifndef PLOTFREQRESP_H
#define PLOTFREQRESP_H

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>

class PlotFreqResp : public QwtPlot
{
    Q_OBJECT

public:
    PlotFreqResp();
    void set_magn_data(int channel, double *xvals, double *data, int number);
    void set_phase_data(int channel, double *xvals, double *data, int number);
    void set_groupdelay_data(int channel, double *xvals, double *data, int number);
    void set_mscohere_data(int channel, double *xvals, double *data, int number);
    void set_visible_magn(int channel, bool state);
    void set_visible_phase(int channel, bool state);
    void set_visible_groupdelay(int channel, bool state);
    void set_visible_mscohere(int channel, bool state);
    void zoom_in_left();
    void zoom_out_left();
    void zoom_in_right();
    void zoom_out_right();
    void add_channels(int number);

public slots:
    void change_color(int sysindex, QColor color);

private:
    void rezoom();

    std::vector<QwtPlotCurve *> curve_magn, curve_phase, curve_groupdelay, curve_mscohere;
    QwtPlotGrid *grid_freqResp;

    int zoom_step_left;
    const int zoom_min_left = -5;
    const int zoom_max_left = 3;

    int zoom_step_right;
    const int zoom_min_right = -3;
    const int zoom_max_right = 6;
};

#endif // PLOTFREQRESP_H
