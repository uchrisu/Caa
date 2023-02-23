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
    void init_analyzer();

private slots:
    void update_timer_event();
    void ch_tab_changes(int index);

    void sel_freSmooting_changed(int index);

    void spin_freqMin_changed(int val);
    void spin_freqMax_changed(int val);

    void spin_irMinX_changed(int val);
    void spin_irMaxX_changed(int val);

private:
    void update_statusbar();

    Ui::MainWindow *ui;

    QLabel status_label;

    PlotSignal *plot_signal;
    PlotIR *plot_ir;
    PlotFreqResp *plot_freqResp;

    std::vector<QWidget *> channels_tabs;
    std::vector<QVBoxLayout *> channels_layouts;
    std::vector<ChannelConfigWidget *> channel_configs;

    QTimer *updatetimer;

    JAudioBuffer *jabuffer;

    std::vector<AudioSystemAnalyzer *> asa;
};
#endif // MAINWINDOW_H
