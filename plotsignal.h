#ifndef PLOTSIGNAL_H
#define PLOTSIGNAL_H

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>

class PlotSignal : public QwtPlot
{
    Q_OBJECT

public:
    PlotSignal();
    void set_sig_data(int channel, float *xvals, float *data, int number);
    void set_ref_data(int channel, float *xvals, float *data, int number);
    void set_visible_sig(int channel, bool state);
    void set_visible_ref(int channel, bool state);

    void zoom_in();
    void zoom_out();

    void add_channels(int number);

public slots:
    void change_color(int sysindex, QColor color);

private:
    void rezoom();

    std::vector <QwtPlotCurve *> curve_signal, curve_ref;
    QwtPlotGrid *grid_signal;

    int zoom_step;
    const int zoom_min = -5;
    const int zoom_max = 3;
};

#endif // PLOTSIGNAL_H
