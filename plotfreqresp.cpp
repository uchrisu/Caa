#include "plotfreqresp.h"

#include "config.h"
#include "qwt_scale_engine.h"
#include <cmath>

PlotFreqResp::PlotFreqResp()
    : QwtPlot()
{
    zoom_step_left = 0;
    zoom_step_right = 2;

    setTitle( "Frequency Response" );
    setCanvasBackground( Qt::white );

    setAxisVisible( QwtAxis::YRight );

    grid_freqResp = new QwtPlotGrid;
    grid_freqResp->enableXMin( true );
    grid_freqResp->enableYMin( true );
    grid_freqResp->setMajorPen( Qt::gray, 0, Qt::SolidLine );
    grid_freqResp->setMinorPen( Qt::gray, 0, Qt::DotLine );
    grid_freqResp->setAxes(QwtPlot::xBottom, QwtPlot::yLeft);
    grid_freqResp->attach(this);

    curve_magn.resize(NUM_SYSTEMS);
    for (int i = 0; i < NUM_SYSTEMS; i++){
        curve_magn[i] = new QwtPlotCurve();
        curve_magn[i]->setTitle( "Amplitude" );
        curve_magn[i]->setPen( get_color(i,0), 2 );
        curve_magn[i]->setYAxis( QwtAxis::YLeft );
        curve_magn[i]->setRenderHint( QwtPlotItem::RenderAntialiased, true );
    }

    curve_phase.resize(NUM_SYSTEMS);
    for (int i = 0; i < NUM_SYSTEMS; i++){
        curve_phase[i] = new QwtPlotCurve();
        curve_phase[i]->setTitle( "Phase" );
        curve_phase[i]->setPen( get_color(i,1), 2 );
        curve_phase[i]->setYAxis( QwtAxis::YRight );
        curve_phase[i]->setRenderHint( QwtPlotItem::RenderAntialiased, true );
    }

    curve_groupdelay.resize(NUM_SYSTEMS);
    for (int i = 0; i < NUM_SYSTEMS; i++){
        curve_groupdelay[i] = new QwtPlotCurve();
        curve_groupdelay[i]->setTitle( "Group Delay" );
        curve_groupdelay[i]->setPen( get_color(i,1), 2 );
        curve_groupdelay[i]->setYAxis( QwtAxis::YRight );
        curve_groupdelay[i]->setRenderHint( QwtPlotItem::RenderAntialiased, true );
    }

    curve_mscohere.resize(NUM_SYSTEMS);
    for (int i = 0; i < NUM_SYSTEMS; i++){
        curve_mscohere[i] = new QwtPlotCurve();
        curve_mscohere[i]->setTitle( "Coherence" );
        curve_mscohere[i]->setPen( get_color(i,1), 2 );
        curve_mscohere[i]->setYAxis( QwtAxis::YRight );
        curve_mscohere[i]->setRenderHint( QwtPlotItem::RenderAntialiased, true );
    }


    for (int i = 0; i < NUM_SYSTEMS; i++){
        curve_magn[i]->attach(this);
        curve_phase[i]->attach(this);
        curve_groupdelay[i]->attach(this);
        curve_mscohere[i]->attach(this);
    }

    // Axes

    setAxisScaleEngine(QwtPlot::xBottom, new QwtLogScaleEngine);
    setAxisScale(QwtPlot::xBottom, 20, 20000);
    setAxisScale(QwtPlot::yLeft, -40, 40);
    setAxisScale(QwtPlot::yRight, -M_PI, M_PI);
    setAxisMaxMinor(QwtPlot::xBottom, 9);
    setAxisTitle(QwtPlot::xBottom, "Frequency [Hz]");
    setAxisTitle(QwtPlot::yLeft, "Amplitude [dB]");
    setAxisTitle(QwtPlot::yRight, "Phase");

}

void PlotFreqResp::set_magn_data(int channel, double *xvals, double *data, int number)
{
    if (channel >= NUM_SYSTEMS)
        return;
    if (channel < 0)
        return;

    if (xvals == nullptr)
        curve_magn[channel]->setSamples(data, number);
    else
        curve_magn[channel]->setSamples(xvals, data, number);
}

void PlotFreqResp::set_phase_data(int channel, double *xvals, double *data, int number)
{
    if (channel >= NUM_SYSTEMS)
        return;
    if (channel < 0)
        return;

    if (xvals == nullptr)
        curve_phase[channel]->setSamples(data, number);
    else
        curve_phase[channel]->setSamples(xvals, data, number);
}

void PlotFreqResp::set_groupdelay_data(int channel, double *xvals, double *data, int number)
{
    if (channel >= NUM_SYSTEMS)
        return;
    if (channel < 0)
        return;

    if (xvals == nullptr)
        curve_groupdelay[channel]->setSamples(data, number);
    else
        curve_groupdelay[channel]->setSamples(xvals, data, number);
}

void PlotFreqResp::set_mscohere_data(int channel, double *xvals, double *data, int number)
{
    if (channel >= NUM_SYSTEMS)
        return;
    if (channel < 0)
        return;

    if (xvals == nullptr)
        curve_mscohere[channel]->setSamples(data, number);
    else
        curve_mscohere[channel]->setSamples(xvals, data, number);
}

void PlotFreqResp::set_visible_magn(int channel, bool state)
{
    if (channel >= NUM_SYSTEMS)
        return;
    if (channel < 0)
        return;

    if (state)
        curve_magn[channel]->show();
    else
        curve_magn[channel]->hide();
}

void PlotFreqResp::set_visible_phase(int channel, bool state)
{
    if (channel >= NUM_SYSTEMS)
        return;
    if (channel < 0)
        return;

    if (state)
        curve_phase[channel]->show();
    else
        curve_phase[channel]->hide();
}

void PlotFreqResp::set_visible_groupdelay(int channel, bool state)
{
    if (channel >= NUM_SYSTEMS)
        return;
    if (channel < 0)
        return;

    if (state)
        curve_groupdelay[channel]->show();
    else
        curve_groupdelay[channel]->hide();
}

void PlotFreqResp::set_visible_mscohere(int channel, bool state)
{
    if (channel >= NUM_SYSTEMS)
        return;
    if (channel < 0)
        return;

    if (state)
        curve_mscohere[channel]->show();
    else
        curve_mscohere[channel]->hide();
}

void PlotFreqResp::zoom_in_left()
{
    if (zoom_step_left > zoom_min_left)
        zoom_step_left--;
    rezoom();
}

void PlotFreqResp::zoom_out_left()
{
    if (zoom_step_left < zoom_max_left)
        zoom_step_left++;
    rezoom();
}

void PlotFreqResp::zoom_in_right()
{
    if (zoom_step_right > zoom_min_right)
        zoom_step_right--;
    rezoom();
}

void PlotFreqResp::zoom_out_right()
{
    if (zoom_step_right < zoom_max_right)
        zoom_step_right++;
    rezoom();
}

void PlotFreqResp::zoom_freq(int min, int max)
{
    setAxisScale(QwtPlot::xBottom, min, max);
}

void PlotFreqResp::add_channels(int number)
{
    int old_num = curve_magn.size();
    for (int i = old_num; i < (old_num + number); i++){
        curve_magn.push_back(new QwtPlotCurve());
        curve_magn[i]->setTitle( "Amplitude" );
        curve_magn[i]->setPen( get_color(i,0), 2 );
        curve_magn[i]->setYAxis( QwtAxis::YLeft );
        curve_magn[i]->setRenderHint( QwtPlotItem::RenderAntialiased, true );

        curve_phase.push_back(new QwtPlotCurve());
        curve_phase[i]->setTitle( "Phase" );
        curve_phase[i]->setPen( get_color(i,1), 2 );
        curve_phase[i]->setYAxis( QwtAxis::YRight );
        curve_phase[i]->setRenderHint( QwtPlotItem::RenderAntialiased, true );

        curve_groupdelay.push_back(new QwtPlotCurve());
        curve_groupdelay[i]->setTitle( "Group Delay" );
        curve_groupdelay[i]->setPen( get_color(i,1), 2 );
        curve_groupdelay[i]->setYAxis( QwtAxis::YRight );
        curve_groupdelay[i]->setRenderHint( QwtPlotItem::RenderAntialiased, true );

        curve_mscohere.push_back(new QwtPlotCurve());
        curve_mscohere[i]->setTitle( "Coherence" );
        curve_mscohere[i]->setPen( get_color(i,1), 2 );
        curve_mscohere[i]->setYAxis( QwtAxis::YRight );
        curve_mscohere[i]->setRenderHint( QwtPlotItem::RenderAntialiased, true );

        curve_magn[i]->attach(this);
        curve_phase[i]->attach(this);
        curve_groupdelay[i]->attach(this);
        curve_mscohere[i]->attach(this);
    }
}

void PlotFreqResp::change_color(int sysindex, QColor color)
{
    if ((sysindex >= 0) && (sysindex < NUM_SYSTEMS)){
        curve_magn[sysindex]->setPen(color, 2);
        QColor halfcolor;
        halfcolor = color;
        halfcolor.setRed((color.red() + 255) / 2);
        halfcolor.setGreen((color.green() + 255) / 2);
        halfcolor.setBlue((color.blue() + 255) / 2);
        curve_phase[sysindex]->setPen(halfcolor, 2);
        curve_groupdelay[sysindex]->setPen(halfcolor, 2);
        curve_mscohere[sysindex]->setPen(halfcolor, 2);
    }
}

void PlotFreqResp::rezoom()
{
    double max = 40 * std::pow(2, zoom_step_left);
    setAxisScale(QwtPlot::yLeft, -max, max);

    max = std::pow(2, zoom_step_right);
    setAxisScale(QwtPlot::yRight, -max, max);
}
