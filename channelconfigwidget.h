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
    explicit ChannelConfigWidget(QWidget *parent = nullptr);
    ~ChannelConfigWidget();
    void set_asa(AudioSystemAnalyzer *asa);
    void set_sysNumber(int number);
    Ui::ChannelConfigWidget *ui;

private slots:
    void sel_sysident_changed(int index);
    void sel_sysident_window_changed(int index);
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

private:
    AudioSystemAnalyzer *asa;
    int sys;
};

#endif // CHANNELCONFIGWIDGET_H
