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

    void on_setDelay_clicked(int sysnum);
    void calcDelay_clicked(int sysnum);

    void sel_N_changed(int index);
    void sel_L_changed(int index);
    void sel_Nfft_changed(int index);

    void sel_freSmooting_changed(int index);
    void sel_sysident_changed(int index);
    void sel_sysident_window_changed(int index);
    void slider_expTimeSmooth_changed(int value);

    void sel_window_changed(int index);
    void setWindow_clicked();

private:
    void update_statusbar();

    Ui::MainWindow *ui;

    QLabel status_label;

    PlotSignal *plot_signal;
    PlotIR *plot_ir;
    PlotFreqResp *plot_freqResp;

    QWidget **channels_tabs;
    QVBoxLayout **channels_layouts;
    QCheckBox **channels_check_show;
    QLabel **channels_sysident_method;
    QComboBox **channels_sel_sysident_method;
    QComboBox **channels_sel_sysident_window;
    QFrame **channels_line1;
    QPushButton **channels_button_calcdelay;
    QSignalMapper channels_calcdelay_mapper;
    QLabel **channels_label_delay;
    QLineEdit **channels_edit_delay;
    QLabel **channels_label_offset;
    QLineEdit **channels_edit_offset;
    QPushButton **channels_button_setdelay;
    QSignalMapper channels_setdelay_mapper;
    QFrame **channels_line2;
    QLabel **channels_selLabel_N;
    QComboBox **channels_sel_N;
    QLabel **channels_selLabel_L;
    QComboBox **channels_sel_L;
    QLabel **channels_selLabel_Nfft;
    QComboBox **channels_sel_Nfft;
    QFrame **channels_line3;
    QLabel **channels_label_expTimeSmooth;
    QSlider **channels_slider_expTimeSmooth;
    QFrame **channels_line4;
    QComboBox **channels_sel_window;
    QLabel **channels_Label_WindowLength;
    QLineEdit **channels_edit_WindowLength;
    QLabel **channels_Label_WindowOffset;
    QLineEdit **channels_edit_WindowOffset;
    QPushButton **channels_button_WindowSet;

    QSpacerItem **channels_spacer1;
    QCheckBox **channels_check_update;



    QTimer *updatetimer;

    JAudioBuffer *jabuffer;
    AudioSystemAnalyzer **asa;
};
#endif // MAINWINDOW_H
