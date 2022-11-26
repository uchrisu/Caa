#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QPushButton>
#include <QSignalMapper>
#include <QTimer>
#include <QVBoxLayout>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include "jaudiobuffer.h"
#include "audiosystemanalyzer.h"
#include "plotfreqresp.h"
#include "plotir.h"
#include "plotsignal.h"
#include "qcombobox.h"
#include "channelconfigwidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void set_buffer(JAudioBuffer *jabuffer);
    void set_analyzer(AudioSystemAnalyzer **asa);

private slots:
    void update_timer_event();

    void sel_freSmooting_changed(int index);

private:
    void update_statusbar();

    Ui::MainWindow *ui;

    QLabel status_label;

    PlotSignal *plot_signal;
    PlotIR *plot_ir;
    PlotFreqResp *plot_freqResp;

    QWidget **channels_tabs;
    QVBoxLayout **channels_layouts;

    ChannelConfigWidget **channel_configs;


    QTimer *updatetimer;

    JAudioBuffer *jabuffer;
    AudioSystemAnalyzer **asa;
};
#endif // MAINWINDOW_H
