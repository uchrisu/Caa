#ifndef PLOTIR_H
#define PLOTIR_H

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <vector>

class PlotIR : public QwtPlot
{
    Q_OBJECT

public:
    PlotIR();

    void set_ir_data(int channel, double *xvals, double *data, int number);
    void set_window_data(int channel, double *xvals, double *data, int number);
    void set_visible_ir(int channel, bool state);
    void set_visible_window(int channel, bool state);
    void add_channels(int number);

    void zoom_in();
    void zoom_out();

    void zoom_sample(int min, int max);

public slots:
    void change_color(int sysindex, QColor color);

private:
    void rezoom();

    std::vector<QwtPlotCurve *> curve_ir;
    std::vector<QwtPlotCurve *> curve_window_ir;
    QwtPlotGrid *grid_ir;

    int zoom_step;
    const int zoom_min = -5;
    const int zoom_max = 6;
};

#endif // PLOTIR_H
