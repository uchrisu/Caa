#include "plotir.h"

#include "config.h"
#include <cmath>

PlotIR::PlotIR()
    : QwtPlot()
{
    zoom_step = 0;

    setTitle( "Impulse Response" );
    setCanvasBackground( Qt::white );

    grid_ir = new QwtPlotGrid;
    grid_ir->enableXMin( true );
    grid_ir->enableYMin( true );
    grid_ir->setMajorPen( Qt::gray, 0, Qt::SolidLine );
    grid_ir->setMinorPen( Qt::gray, 0, Qt::DotLine );
    grid_ir->setAxes(QwtPlot::xBottom, QwtPlot::yLeft);
    grid_ir->attach(this);

    curve_ir.resize(NUM_SYSTEMS);
    for (int i = 0; i < NUM_SYSTEMS; i++){
        curve_ir[i] = new QwtPlotCurve();
        curve_ir[i]->setTitle( QString("Ch. ") + QString::number(i+1)  );
        curve_ir[i]->setPen( get_color(i,0), 2 ),
        curve_ir[i]->setRenderHint( QwtPlotItem::RenderAntialiased, true );
    }

    curve_window_ir.resize(NUM_SYSTEMS);
    for (int i = 0; i < NUM_SYSTEMS; i++){
        curve_window_ir[i] = new QwtPlotCurve();
        curve_window_ir[i]->setTitle( "Window" );
        curve_window_ir[i]->setPen( get_color(i,1), 2 ),
        curve_window_ir[i]->setRenderHint( QwtPlotItem::RenderAntialiased, true );
    }

    setAxisTitle(QwtPlot::xBottom, "Sample");

    for (int i = 0; i < NUM_SYSTEMS; i++){
        curve_ir[i]->attach(this);
        curve_window_ir[i]->attach(this);
    }

    rezoom();

}

void PlotIR::set_ir_data(int channel, double *xvals, double *data, int number)
{
    if (channel >= NUM_SYSTEMS)
        return;
    if (channel < 0)
        return;

    if (xvals == nullptr)
        curve_ir[channel]->setSamples(data, number);
    else
        curve_ir[channel]->setSamples(xvals, data, number);
}

void PlotIR::set_window_data(int channel, double *xvals, double *data, int number)
{
    if (channel >= NUM_SYSTEMS)
        return;
    if (channel < 0)
        return;

    if (xvals == nullptr)
        curve_window_ir[channel]->setSamples(data, number);
    else
        curve_window_ir[channel]->setSamples(xvals, data, number);
}

void PlotIR::set_visible_ir(int channel, bool state)
{
    if (channel >= NUM_SYSTEMS)
        return;
    if (channel < 0)
        return;

    if (state)
        curve_ir[channel]->show();
    else
        curve_ir[channel]->hide();
}

void PlotIR::set_visible_window(int channel, bool state)
{
    if (channel >= NUM_SYSTEMS)
        return;
    if (channel < 0)
        return;

    if (state)
        curve_window_ir[channel]->show();
    else
        curve_window_ir[channel]->hide();
}

void PlotIR::add_channels(int number)
{
    int old_num = curve_ir.size();
    for (int i = old_num; i < (old_num + number); i++){
        curve_ir.push_back(new QwtPlotCurve());
        curve_ir[i]->setTitle( QString("Ch. ") + QString::number(i+1)  );
        curve_ir[i]->setPen( get_color(i,0), 2 ),
        curve_ir[i]->setRenderHint( QwtPlotItem::RenderAntialiased, true );
        curve_ir[i]->attach(this);

        curve_window_ir.push_back(new QwtPlotCurve());
        curve_window_ir[i]->setTitle( "Window" );
        curve_window_ir[i]->setPen( get_color(i,1), 2 ),
        curve_window_ir[i]->setRenderHint( QwtPlotItem::RenderAntialiased, true );
        curve_window_ir[i]->attach(this);
    }

}

void PlotIR::zoom_in()
{
    if (zoom_step > zoom_min)
        zoom_step--;
    rezoom();
}

void PlotIR::zoom_out()
{
    if (zoom_step < zoom_max)
        zoom_step++;
    rezoom();
}

void PlotIR::zoom_sample(int min, int max)
{
    setAxisScale(QwtPlot::xBottom, min, max);
}

void PlotIR::change_color(int sysindex, QColor color)
{
    if ((sysindex >= 0) && (sysindex < NUM_SYSTEMS)){
        curve_ir[sysindex]->setPen(color, 2);
        QColor halfcolor;
        halfcolor = color;
        halfcolor.setRed((color.red() + 255) / 2);
        halfcolor.setGreen((color.green() + 255) / 2);
        halfcolor.setBlue((color.blue() + 255) / 2);
        curve_window_ir[sysindex]->setPen(halfcolor, 2);
    }
}

void PlotIR::rezoom()
{
    double max = std::pow(2, zoom_step);
    setAxisScale(QwtPlot::yLeft, -max, max);
}
