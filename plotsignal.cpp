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

    curve_signal.resize(NUM_SYSTEMS);
    for (int i = 0; i < NUM_SYSTEMS; i++){
        curve_signal[i] = new QwtPlotCurve();
        curve_signal[i]->setTitle( "Channel " + QString::number(i) );
        curve_signal[i]->setPen( get_color(i,0), 2 ),
        curve_signal[i]->setRenderHint( QwtPlotItem::RenderAntialiased, true );
    }

    curve_ref.resize(NUM_SYSTEMS);
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

void PlotSignal::add_channels(int number)
{
    int old_num = curve_signal.size();
    for (int i = old_num; i < (old_num + number); i++){
        curve_signal.push_back(new QwtPlotCurve());
        curve_signal[i]->setTitle( "Channel " + QString::number(i) );
        curve_signal[i]->setPen( get_color(i,0), 2 ),
        curve_signal[i]->setRenderHint( QwtPlotItem::RenderAntialiased, true );

        curve_ref.push_back(new QwtPlotCurve());
        curve_ref[i]->setTitle( "Ref " + QString::number(i) );
        curve_ref[i]->setPen( get_color(i,1), 2 ),
        curve_ref[i]->setRenderHint( QwtPlotItem::RenderAntialiased, true );

        curve_signal[i]->attach(this);
        curve_ref[i]->attach(this);
    }
}

void PlotSignal::change_color(int sysindex, QColor color)
{
    if ((sysindex >= 0) && (sysindex < NUM_SYSTEMS)){
        curve_signal[sysindex]->setPen(color, 2);
        QColor halfcolor;
        halfcolor = color;
        halfcolor.setRed((color.red() + 255) / 2);
        halfcolor.setGreen((color.green() + 255) / 2);
        halfcolor.setBlue((color.blue() + 255) / 2);
        curve_ref[sysindex]->setPen(halfcolor, 2);
    }
}

void PlotSignal::rezoom()
{
    double max = std::pow(2, zoom_step);
    setAxisScale(QwtPlot::yLeft, -max, max);
}
