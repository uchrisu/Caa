#ifndef CHANNELCONFIGWIDGET_H
#define CHANNELCONFIGWIDGET_H

#include "qobjectdefs.h"
#include <QWidget>
#include "audiosystemanalyzer.h"

namespace Ui {
class ChannelConfigWidget;
}

class ChannelConfigWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ChannelConfigWidget(int number, QWidget *parent = nullptr);
    ~ChannelConfigWidget();
    void set_asa(AudioSystemAnalyzer *asa);
    Ui::ChannelConfigWidget *ui;
    void add_channels(int number);

private slots:
    void sel_type_changed(int index);
    void sel_sysident_changed(int index);
    void sel_sysident_window_changed(int index);
    void slider_sysidentMu_changed(int position);
    void sel_combine_chA_changed(int index);
    void sel_combine_chB_changed(int index);
    void calc_delay_clicked();
    void set_delay_offset_clicked();
    void sel_N_changed(int index);
    void sel_L_changed(int index);
    void sel_Nfft_changed(int index);
    void slider_expTimeSmooth_changed(int position);
    void sel_window_changed(int index);
    void setWindow_clicked();
    void saveIR_clicked();
    void loadIR_clicked();
    void select_color_clicked();

signals:
    void colorChanged(int sysindex, QColor color);

private:
    AudioSystemAnalyzer *asa;
    int sys;
    int type;
    QColor plot_color;
};

#endif // CHANNELCONFIGWIDGET_H
