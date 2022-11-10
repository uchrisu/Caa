#include "plotsignal.h"

#include "config.h"
#include <cmath>

PlotSignal::PlotSignal()
    : QwtPlot()
{  
    zoom_step = 0;
    setTitle( "Signal Amplitude" );
    setCanvasBackground( Qt::white );

    grid_signal = new QwtPlotGrid;
    grid_signal->enableXMin( true );
    grid_signal->enableYMin( true );
    grid_signal->setMajorPen( Qt::gray, 0, Qt::SolidLine );
    grid_signal->setMinorPen( Qt::gray, 0, Qt::DotLine );
    grid_signal->setAxes(QwtPlot::xBottom, QwtPlot::yLeft);
    grid_signal->attach(this);

    curve_signal = new QwtPlotCurve *[NUM_SYSTEMS];
    for (int i = 0; i < NUM_SYSTEMS; i++){
        curve_signal[i] = new QwtPlotCurve();
        curve_signal[i]->setTitle( "Channel " + QString::number(i) );
        curve_signal[i]->setPen( get_color(i,0), 2 ),
        curve_signal[i]->setRenderHint( QwtPlotItem::RenderAntialiased, true );
    }

    curve_ref = new QwtPlotCurve *[NUM_SYSTEMS];
    for (int i = 0; i < NUM_SYSTEMS; i++){
        curve_ref[i] = new QwtPlotCurve();
        curve_ref[i]->setTitle( "Ref " + QString::number(i) );
        curve_ref[i]->setPen( get_color(i,1), 2 ),
        curve_ref[i]->setRenderHint( QwtPlotItem::RenderAntialiased, true );
    }

    for (int i = 0; i < NUM_SYSTEMS; i++){
        curve_signal[i]->attach(this);
        curve_ref[i]->attach(this);
    }

    setAxisTitle(QwtPlot::xBottom, "Sample");
    rezoom();
}

void PlotSignal::set_sig_data(int channel, float *xvals, float *data, int number)
{
    if (channel >= NUM_SYSTEMS)
        return;
    if (channel < 0)
        return;

    curve_signal[channel]->setSamples(xvals, data, number);
}

void PlotSignal::set_ref_data(int channel, float *xvals, float *data, int number)
{
    if (channel >= NUM_SYSTEMS)
        return;
    if (channel < 0)
        return;

    curve_ref[channel]->setSamples(xvals, data, number);
}

void PlotSignal::set_visible_sig(int channel, bool state)
{
    if (channel >= NUM_SYSTEMS)
        return;
    if (channel < 0)
        return;

    if (state)
        curve_signal[channel]->show();
    else
        curve_signal[channel]->hide();
}

void PlotSignal::set_visible_ref(int channel, bool state)
{
    if (channel >= NUM_SYSTEMS)
        return;
    if (channel < 0)
        return;

    if (state)
        curve_ref[channel]->show();
    else
        curve_ref[channel]->hide();
}

void PlotSignal::zoom_in()
{
    if (zoom_step > zoom_min)
        zoom_step--;
    rezoom();
}

void PlotSignal::zoom_out()
{
    if (zoom_step < zoom_max)
        zoom_step++;
    rezoom();
}

void PlotSignal::rezoom()
{
    double max = std::pow(2, zoom_step);
    setAxisScale(QwtPlot::yLeft, -max, max);
}
