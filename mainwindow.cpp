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
    //asa = nullptr;

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

    channels_tabs.resize(NUM_SYSTEMS + 1);

    for (int i = 0; i < (NUM_SYSTEMS + 1); i++){
        channels_tabs[i] = new QWidget;
        if (i == NUM_SYSTEMS)
            ui->tab_channels->addTab(channels_tabs[i], "+");
        else
            ui->tab_channels->addTab(channels_tabs[i], QString::number(i+1));
    }

    connect(ui->tab_channels, SIGNAL(currentChanged(int)), this, SLOT(ch_tab_changes(int)));

    channel_configs.resize(NUM_SYSTEMS);

    for (int i = 0; i < NUM_SYSTEMS; i++){
        channel_configs[i] = new ChannelConfigWidget(this);
        channel_configs[i]->set_sysNumber(i);
    }

    channels_layouts.resize(NUM_SYSTEMS);

    for (int i = 0; i < NUM_SYSTEMS; i++){
        channels_layouts[i] = new QVBoxLayout;
        channels_tabs[i]->setLayout(channels_layouts[i]);
        channels_layouts[i]->addWidget(channel_configs[i]);
        connect(channel_configs[i], SIGNAL(colorChanged(int,QColor)), plot_ir, SLOT(change_color(int,QColor)));
        connect(channel_configs[i], SIGNAL(colorChanged(int,QColor)), plot_signal, SLOT(change_color(int,QColor)));
        connect(channel_configs[i], SIGNAL(colorChanged(int,QColor)), plot_freqResp, SLOT(change_color(int,QColor)));
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

    for (int i = 0; i < NUM_SYSTEMS; i++){
        delete asa[i];
    }

}

void MainWindow::set_buffer(JAudioBuffer *jabuffer)
{
    this->jabuffer = jabuffer;
}

void MainWindow::init_analyzer()
{
    asa.resize(NUM_SYSTEMS);

    for (int i = 0; i < NUM_SYSTEMS; i++){
        asa[i] = new AudioSystemAnalyzer(jabuffer, i);
        asa[i]->set_filterlength(list_lengths_N[channel_configs[i]->ui->comboBox_N->currentIndex()]);
        asa[i]->set_analyze_length(list_lengths_L[channel_configs[i]->ui->comboBox_L->currentIndex()]);
        asa[i]->set_freq_length(list_lengths_Nf[channel_configs[i]->ui->comboBox_Nfft->currentIndex()]);
        channel_configs[i]->set_asa(asa[i]);
    }
}



void MainWindow::update_timer_event()
{
    if (static_cast<long>(asa.size()) < NUM_SYSTEMS)
        return;


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

void MainWindow::ch_tab_changes(int index)
{
    if (index == NUM_SYSTEMS) {
        jabuffer->add_channels(2);
        asa.push_back(new AudioSystemAnalyzer(jabuffer, NUM_SYSTEMS));
        channels_tabs.push_back(new QWidget);
        ui->tab_channels->addTab(channels_tabs[NUM_SYSTEMS + 1], "+");
        ui->tab_channels->setTabText(NUM_SYSTEMS, QString::number(NUM_SYSTEMS + 1));
        channels_layouts.push_back(new QVBoxLayout);
        channels_tabs[NUM_SYSTEMS]->setLayout(channels_layouts[NUM_SYSTEMS]);
        channel_configs.push_back(new ChannelConfigWidget(this));
        channel_configs[NUM_SYSTEMS]->set_sysNumber(NUM_SYSTEMS);
        channels_layouts[NUM_SYSTEMS]->addWidget(channel_configs[NUM_SYSTEMS]);
        asa[NUM_SYSTEMS]->set_filterlength(list_lengths_N[channel_configs[NUM_SYSTEMS]->ui->comboBox_N->currentIndex()]);
        asa[NUM_SYSTEMS]->set_analyze_length(list_lengths_L[channel_configs[NUM_SYSTEMS]->ui->comboBox_L->currentIndex()]);
        asa[NUM_SYSTEMS]->set_freq_length(list_lengths_Nf[channel_configs[NUM_SYSTEMS]->ui->comboBox_Nfft->currentIndex()]);
        channel_configs[NUM_SYSTEMS]->set_asa(asa[NUM_SYSTEMS]);

        plot_signal->add_channels(1);
        plot_ir->add_channels(1);
        plot_freqResp->add_channels(1);

        connect(channel_configs[NUM_SYSTEMS], SIGNAL(colorChanged(int,QColor)), plot_ir, SLOT(change_color(int,QColor)));
        connect(channel_configs[NUM_SYSTEMS], SIGNAL(colorChanged(int,QColor)), plot_signal, SLOT(change_color(int,QColor)));
        connect(channel_configs[NUM_SYSTEMS], SIGNAL(colorChanged(int,QColor)), plot_freqResp, SLOT(change_color(int,QColor)));

        NUM_SYSTEMS++;

    }

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
    if (static_cast<long>(asa.size()) < NUM_SYSTEMS)
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

