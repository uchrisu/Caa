#include "channelconfigwidget.h"
#include "ui_channelconfigwidget.h"

#include "config.h"
#include "windowfunc.h"
#include <iostream>
#include <QFileDialog>
#include <QColorDialog>

ChannelConfigWidget::ChannelConfigWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChannelConfigWidget)
{
    ui->setupUi(this);

    asa = nullptr;
    sys = -1;
    plot_color = get_color(0, 0);

    for (auto &method : list_sysident_methods)
        ui->comboBox_SysIdentMethod->addItem(QString::fromStdString(method));
    ui->comboBox_SysIdentMethod->setCurrentIndex(config_stdindex_sysident_methods);
    connect(ui->comboBox_SysIdentMethod, SIGNAL(currentIndexChanged(int)), this, SLOT(sel_sysident_changed(int)));

    for (const auto& name : windowfunc::get_type_names())
        ui->comboBox_SysIdentWindow->addItem(QString::fromStdString(name));
    ui->comboBox_SysIdentWindow->setCurrentIndex(0);
    connect(ui->comboBox_SysIdentWindow, SIGNAL(currentIndexChanged(int)), this, SLOT(sel_sysident_window_changed(int)));

    connect(ui->pushButton_CalcDelay, SIGNAL(released()), this, SLOT(calc_delay_clicked()));

    ui->lineEdit_Delay->setText(QString("0"));

    ui->lineEdit_Offset->setText(QString("0"));

    connect(ui->pushButton_SetDelayOffset, SIGNAL(released()), this, SLOT(set_delay_offset_clicked()));

    for (int N : list_lengths_N)
        ui->comboBox_N->addItem(QString::number(N));
    ui->comboBox_N->setCurrentIndex(config_stdindex_N);
    connect(ui->comboBox_N, SIGNAL(currentIndexChanged(int)), this, SLOT(sel_N_changed(int)));

    for (int L : list_lengths_L)
        ui->comboBox_L->addItem(QString::number(L));
    ui->comboBox_L->setCurrentIndex(config_stdindex_L);
    connect(ui->comboBox_L, SIGNAL(currentIndexChanged(int)), this, SLOT(sel_L_changed(int)));

    for (int Nf : list_lengths_Nf)
        ui->comboBox_Nfft->addItem(QString::number(Nf));
    ui->comboBox_Nfft->setCurrentIndex(config_stdindex_Nf);
    connect(ui->comboBox_Nfft, SIGNAL(currentIndexChanged(int)), this, SLOT(sel_Nfft_changed(int)));

    ui->slider_ExpTimeSmoothing->setMinimum(0);
    ui->slider_ExpTimeSmoothing->setMaximum(100);
    ui->slider_ExpTimeSmoothing->setTickInterval(5);
    connect(ui->slider_ExpTimeSmoothing, SIGNAL(valueChanged(int)), this, SLOT(slider_expTimeSmooth_changed(int)));

    for (const auto& name : windowfunc::get_type_names())
        ui->comboBox_WindowType->addItem(QString::fromStdString(name));
    ui->comboBox_WindowType->setCurrentIndex(0);
    connect(ui->comboBox_WindowType, SIGNAL(currentIndexChanged(int)), this, SLOT(sel_window_changed(int)));

    ui->lineEdit_WindowWidth->setText(QString("0"));

    ui->lineEdit_WindowOffset->setText(QString("0"));

    connect(ui->pushButton_SetWindow, SIGNAL(released()), this, SLOT(setWindow_clicked()));

    connect(ui->pushButton_SaveIR, SIGNAL(released()), this, SLOT(saveIR_clicked()));

    connect(ui->pushButton_LoadIR, SIGNAL(released()), this, SLOT(loadIR_clicked()));

    connect(ui->pushButton_selColor, SIGNAL(released()), this, SLOT(select_color_clicked()));


}

ChannelConfigWidget::~ChannelConfigWidget()
{
    delete ui;
}

void ChannelConfigWidget::set_asa(AudioSystemAnalyzer *asa)
{
    if (asa != nullptr)
        this->asa = asa;
}

void ChannelConfigWidget::set_sysNumber(int number)
{
    sys = number;
    plot_color = get_color(number, 0);
}

void ChannelConfigWidget::sel_sysident_changed(int index)
{
    if (asa != nullptr){
        std::cout << "System " << sys << ": Setting Identification Method = " << list_sysident_methods[index] << std::endl;
        asa->set_sysident_method(index);
    }
}

void ChannelConfigWidget::sel_sysident_window_changed(int index)
{
    if (asa != nullptr){
        std::cout << "System " << sys << ": Syident-Window new Value: " << windowfunc::get_type_name(index) << std::endl;
        asa->set_sysident_window_type(index);
    }
}

void ChannelConfigWidget::calc_delay_clicked()
{
    if (asa != nullptr){
        int delay = asa->identify_delay();
        ui->lineEdit_Delay->setText(QString::number(delay));
    }
}

void ChannelConfigWidget::set_delay_offset_clicked()
{
    if (asa != nullptr){
        int delay;
        bool ok;
        delay = ui->lineEdit_Delay->text().toInt(&ok);
        if (ok) {
            std::cout << "Setting channel " << sys << " delay: " << delay << std::endl;
            asa->set_delay(delay);
        }

        int offset;
        offset = ui->lineEdit_Offset->text().toInt(&ok);
        if (ok) {
            std::cout << "Setting channel " << sys << " offset: " << offset << std::endl;
            asa->set_offset(offset);
        }
    }
}

void ChannelConfigWidget::sel_N_changed(int index)
{
    if (asa != nullptr){
        std::cout << "System " << sys << ": Setting N = " << list_lengths_N[index] << std::endl;
        asa->set_filterlength(list_lengths_N[index]);
    }
}

void ChannelConfigWidget::sel_L_changed(int index)
{
    if (asa != nullptr){
        std::cout << "System " << sys << ": Setting L = " << list_lengths_L[index] << std::endl;
        asa->set_analyze_length(list_lengths_L[index]);
    }
}

void ChannelConfigWidget::sel_Nfft_changed(int index)
{
    if (asa != nullptr){
        std::cout << "System " << sys << ": Setting Nfft = " << list_lengths_Nf[index] << std::endl;
        asa->set_freq_length(list_lengths_Nf[index]);
    }
}

void ChannelConfigWidget::slider_expTimeSmooth_changed(int position)
{
    if (asa != nullptr){
        std::cout << "System " << sys << ": Exp. Time Smooting new Value: " << position << std::endl;
        asa->set_expTimeSmoothFactor(static_cast<double>(position)/100.0);
    }
}

void ChannelConfigWidget::sel_window_changed(int index)
{
    if (asa != nullptr){
        std::cout << "System " << sys << ": Window new Value: " << windowfunc::get_type_name(index) << std::endl;
        asa->set_window_type(index);
    }
}

void ChannelConfigWidget::setWindow_clicked()
{
    if (asa != nullptr){
        int len, offset;
        bool okay;
        len = ui->lineEdit_WindowWidth->text().toInt(&okay);
        if (!okay)
            return;
        offset = ui->lineEdit_WindowOffset->text().toInt(&okay);
        if (!okay)
            return;
        std::cout << "System " << sys << ": Window Length: " << len << ", Window Offset: " << offset << std::endl;
        asa->set_window_length(len);
        asa->set_window_offset(offset);
    }
}

void ChannelConfigWidget::saveIR_clicked()
{
    if (asa != nullptr){
        QString filename = QFileDialog::getSaveFileName(this, "Save Impulse Response", nullptr, "Impulse Response (*.wav)");
        if (!filename.isEmpty()){
            if (!filename.endsWith(".wav"))
                filename.append(".wav");
            asa->save_impulse_response(filename.toLocal8Bit().data());
        }
    }
}

void ChannelConfigWidget::loadIR_clicked()
{
    if (asa != nullptr){
        QString filename = QFileDialog::getOpenFileName(this, "Load Impulse Response", nullptr, "Impulse Response (*.wav)");
        if (!filename.isEmpty())
            asa->load_impulse_response(filename.toLocal8Bit().data());
    }
}

void ChannelConfigWidget::select_color_clicked()
{
    plot_color = QColorDialog::getColor(plot_color, this );
    emit colorChanged(sys, plot_color);
}




