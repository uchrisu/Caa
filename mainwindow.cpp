#include "mainwindow.h"
#include "ui_channelconfigwidget.h"
#include "ui_mainwindow.h"

#include <iostream>
#include <qwt_scale_engine.h>
#include "config.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    setWindowTitle(QString("Caa - ") + QString(APP_VERSION) );

    jabuffer = nullptr;
    asa = nullptr;

    ui->setupUi(this);

    statusBar()->addPermanentWidget(&status_label);

    plot_signal = new PlotSignal;

    plot_ir = new PlotIR();

    plot_freqResp = new PlotFreqResp();

    ui->verticalLayout_signal->addWidget(plot_signal);
    ui->verticalLayout_signal->setDirection(QBoxLayout::BottomToTop);

    ui->verticalLayout_ir->addWidget(plot_ir);
    ui->verticalLayout_ir->setDirection(QBoxLayout::BottomToTop);

    ui->verticalLayout_freqResp->addWidget(plot_freqResp);
    ui->verticalLayout_freqResp->setDirection(QBoxLayout::BottomToTop);


    // Channel Tabs

    channels_tabs = new QWidget*[NUM_SYSTEMS];

    for (int i = 0; i < NUM_SYSTEMS; i++){
        channels_tabs[i] = new QWidget;
        ui->tab_channels->addTab(channels_tabs[i], QString::number(i));
    }

    channel_configs = new ChannelConfigWidget*[NUM_SYSTEMS];
    for (int i = 0; i < NUM_SYSTEMS; i++){
        channel_configs[i] = new ChannelConfigWidget(this);
        channel_configs[i]->set_sysNumber(i);
    }

    channels_layouts = new QVBoxLayout*[NUM_SYSTEMS];

    for (int i = 0; i < NUM_SYSTEMS; i++){
        channels_layouts[i] = new QVBoxLayout;
        channels_tabs[i]->setLayout(channels_layouts[i]);
        channels_layouts[i]->addWidget(channel_configs[i]);
    }

    // ---

    ui->comboBox_freqSmoothing->addItem("no smooting");
    for (int sm : list_smoothing_per_oct){
        ui->comboBox_freqSmoothing->addItem(QString("1/")+QString::number(sm)+QString(" oct."));
    }
    connect(ui->comboBox_freqSmoothing, SIGNAL(currentIndexChanged(int)), this, SLOT(sel_freSmooting_changed(int)));


    // Buttons etc.

    connect(ui->pushButton_zoomSignalIn, &QPushButton::clicked, plot_signal, &PlotSignal::zoom_in);
    connect(ui->pushButton_zoomSignalOut, &QPushButton::clicked, plot_signal, &PlotSignal::zoom_out);
    connect(ui->pushButton_zoomIRIn, &QPushButton::clicked, plot_ir, &PlotIR::zoom_in);
    connect(ui->pushButton_zoomIROut, &QPushButton::clicked, plot_ir, &PlotIR::zoom_out);
    connect(ui->pushButton_zoomFreqLeftIn, &QPushButton::clicked, plot_freqResp, &PlotFreqResp::zoom_in_left);
    connect(ui->pushButton_zoomFreqLeftOut, &QPushButton::clicked, plot_freqResp, &PlotFreqResp::zoom_out_left);
    connect(ui->pushButton_zoomFreqRightIn, &QPushButton::clicked, plot_freqResp, &PlotFreqResp::zoom_in_right);
    connect(ui->pushButton_zoomFreqRightOut, &QPushButton::clicked, plot_freqResp, &PlotFreqResp::zoom_out_right);


    // Timer

    updatetimer = new QTimer(this);
    connect(updatetimer, SIGNAL(timeout()), this, SLOT(update_timer_event()));
    updatetimer->start(100);
}

MainWindow::~MainWindow()
{
    updatetimer->stop();
    delete updatetimer;
    delete ui;
    delete plot_signal;
    delete plot_ir;
    delete plot_freqResp;

    delete channels_tabs;
    delete channels_layouts;

}

void MainWindow::set_buffer(JAudioBuffer *jabuffer)
{
    this->jabuffer = jabuffer;
}

void MainWindow::set_analyzer(AudioSystemAnalyzer **asa)
{
    this->asa = asa;

    //

    for (int i = 0; i < NUM_SYSTEMS; i++){
        asa[i]->set_filterlength(list_lengths_N[channel_configs[i]->ui->comboBox_N->currentIndex()]);
        asa[i]->set_analyze_length(list_lengths_L[channel_configs[i]->ui->comboBox_L->currentIndex()]);
        asa[i]->set_freq_length(list_lengths_Nf[channel_configs[i]->ui->comboBox_Nfft->currentIndex()]);
        channel_configs[i]->set_asa(asa[i]);
    }
}



void MainWindow::update_timer_event()
{
    //std::cout << "Timer" << std::endl;

    for (int i = 0; i < NUM_SYSTEMS; i++)
    {
        if (!channel_configs[i]->ui->checkBox_show->isChecked()){
            plot_signal->set_visible_sig(i, false);
            plot_signal->set_visible_ref(i, false);
            plot_ir->set_visible_ir(i, false);
            plot_ir->set_visible_window(i, false);
            plot_freqResp->set_visible_magn(i, false);
            plot_freqResp->set_visible_phase(i, false);
            plot_freqResp->set_visible_groupdelay(i, false);
            plot_freqResp->set_visible_mscohere(i, false);
        }
        if (!ui->checkBox_showFreqAmp->isChecked())
            plot_freqResp->set_visible_magn(i, false);
        if (!ui->checkBox_showFreqPhase->isChecked())
            plot_freqResp->set_visible_phase(i, false);
        if (!ui->checkBox_showGroupdelay->isChecked())
            plot_freqResp->set_visible_groupdelay(i, false);
        if (!ui->checkBox_showMSCohere->isChecked())
            plot_freqResp->set_visible_mscohere(i, false);
        if (!ui->checkBox_showMeas->isChecked())
            plot_signal->set_visible_sig(i, false);
        if (!ui->checkBox_showRef->isChecked())
            plot_signal->set_visible_ref(i, false);
        if (!ui->checkBox_showWindowIR->isChecked())
            plot_ir->set_visible_window(i, false);
    }

    if (jabuffer == nullptr)
        return;
    float data[1000];
    float xaxis[1000];

    for (int i = 0; i < 1000; i++){
        xaxis[i] = i;
    }

    int64_t position = jabuffer->get_position();

    //std::cout << "Position: " << position << std::endl;

    for (int number = 0; number < NUM_SYSTEMS; number++){
        jabuffer->get_samples(number*2+1, position - 1000, data, 1000);
        plot_signal->set_ref_data(number, xaxis, data, 1000);

        jabuffer->get_samples(number*2, position - 1000, data, 1000);
        plot_signal->set_sig_data(number, xaxis, data, 1000);
    }

    plot_signal->replot();


    for (int number = 0; number < NUM_SYSTEMS; number++){

        if (!channel_configs[number]->ui->checkBox_show->isChecked()){
            continue;
        }

        if (asa[number]->is_calculation_done()){
            int N = asa[number]->result_N();
            //int L = asa[number]->result_L();
            int Nf = asa[number]->result_Nf();

            //std::cout << "result N: " << N << ", Nf: " << Nf << std::endl;

            double ir[N];
            asa[number]->get_impulse_response(ir, N);
            for (int i = 0; i < N; i++)
                if (std::isnan(ir[i]))
                    std::cout << "NAN3" << std::endl;
            plot_ir->set_ir_data(number, nullptr, ir, N);
            double window[N];
            asa[number]->get_window_vals(window, N);
            plot_ir->set_window_data(number, nullptr, window, N);
            plot_ir->replot();


            double fr[Nf];
            double phase[Nf];
            double groupdelay[Nf];
            double mscohere[Nf];
            double fr_freq[Nf];
            asa[number]->get_freq(fr_freq, Nf);
            asa[number]->get_freq_resp_db(fr, Nf);
            asa[number]->get_phase(phase, Nf);
            asa[number]->get_groupdelay(groupdelay, Nf);
            asa[number]->get_mscohere(mscohere, Nf);
            if (asa[number]->get_freq_smooting() == 0) {
                plot_freqResp->set_magn_data(number, &fr_freq[1], &fr[1], Nf - 1);
                plot_freqResp->set_phase_data(number, &fr_freq[1], &phase[1], Nf - 1);
                plot_freqResp->set_groupdelay_data(number, &fr_freq[1], &groupdelay[1], Nf - 1);
                plot_freqResp->set_mscohere_data(number, &fr_freq[1], &mscohere[1], Nf - 1);
            }
            else {
                std::vector<double> tmp_data;
                std::vector<double> tmp_freq;
                tmp_freq = asa[number]->get_smfreq();
                int len = tmp_freq.size();
                tmp_data = asa[number]->get_smfreq_resp_db();
                plot_freqResp->set_magn_data(number, tmp_freq.data(), tmp_data.data(), len);
                tmp_data = asa[number]->get_smphase();
                plot_freqResp->set_phase_data(number, tmp_freq.data(), tmp_data.data(), len);
                tmp_data = asa[number]->get_smgroupdelay();
                plot_freqResp->set_groupdelay_data(number, tmp_freq.data(), tmp_data.data(), len);
                tmp_data = asa[number]->get_smmscohere();
                plot_freqResp->set_mscohere_data(number, tmp_freq.data(), tmp_data.data(), len);
            }
            plot_freqResp->replot();

        }

        if (!channel_configs[number]->ui->checkBox_Update->isChecked()){
            continue;
        }

        if (!asa[number]->is_in_calculation())
            asa[number]->calc_all(position, false);
    }

    for (int i = 0; i < NUM_SYSTEMS; i++)
    {
        if (channel_configs[i]->ui->checkBox_show->isChecked()){
            if (ui->checkBox_showMeas->isChecked())
                plot_signal->set_visible_sig(i, true);
            if (ui->checkBox_showRef->isChecked())
                plot_signal->set_visible_ref(i, true);
            plot_ir->set_visible_ir(i, true);
            if (ui->checkBox_showWindowIR->isChecked())
                plot_ir->set_visible_window(i, true);
            if (ui->checkBox_showFreqAmp->isChecked())
                plot_freqResp->set_visible_magn(i, true);
            if (ui->checkBox_showFreqPhase->isChecked())
                plot_freqResp->set_visible_phase(i, true);
            if (ui->checkBox_showGroupdelay->isChecked())
                plot_freqResp->set_visible_groupdelay(i, true);
            if (ui->checkBox_showMSCohere->isChecked())
                plot_freqResp->set_visible_mscohere(i, true);
        }
    }

    update_statusbar();

}



void MainWindow::sel_freSmooting_changed(int index)
{
    for (int i = 0; i < NUM_SYSTEMS; i++){
        if (index == 0)
            asa[i]->set_freq_smooting(0);
        else
            asa[i]->set_freq_smooting(list_smoothing_per_oct[index-1]);
    }
}


void MainWindow::update_statusbar()
{
    if (asa == nullptr)
        return;
    QString text;
    text.clear();
    text.append("Calculation time: ");
    for (int i = 0; i < NUM_SYSTEMS; i++){
        if (channel_configs[i]->ui->checkBox_Update->isChecked())
            text.append(QString::number(asa[i]->get_calc_time_full()).rightJustified(4) + QString(" ms"));
        else
            text.append("-");
        if (i != (NUM_SYSTEMS - 1))
            text.append(" / ");
    }
    text.append("  |  Identification time: ");
    for (int i = 0; i < NUM_SYSTEMS; i++){
        if (channel_configs[i]->ui->checkBox_Update->isChecked())
            text.append(QString::number(asa[i]->get_calc_time_identify()).rightJustified(4) + QString(" ms"));
        else
            text.append("-");
        if (i != (NUM_SYSTEMS - 1))
            text.append(" / ");
    }
    status_label.setText(text);
}

