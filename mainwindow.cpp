#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>
#include <qwt_scale_engine.h>
#include "config.h"
#include "windowfunc.h"

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

    channels_layouts = new QVBoxLayout*[NUM_SYSTEMS];

    for (int i = 0; i < NUM_SYSTEMS; i++){
        channels_layouts[i] = new QVBoxLayout;
        channels_tabs[i]->setLayout(channels_layouts[i]);
    }

    channels_check_show = new QCheckBox*[NUM_SYSTEMS];

    for (int i = 0; i < NUM_SYSTEMS; i++){
        channels_check_show[i] = new QCheckBox;
        channels_check_show[i]->setCheckState(Qt::Unchecked);
        channels_check_show[i]->setText(QString("Show"));
        channels_layouts[i]->addWidget(channels_check_show[i]);
    }
    channels_check_show[0]->setCheckState(Qt::Checked);

    channels_sysident_method = new QLabel*[NUM_SYSTEMS];
    for (int i = 0; i < NUM_SYSTEMS; i++){
        channels_sysident_method[i] = new QLabel;
        channels_sysident_method[i]->setText(QString("Systen Ident. Method:"));
        channels_layouts[i]->addWidget(channels_sysident_method[i]);
    }

    channels_sel_sysident_method = new QComboBox*[NUM_SYSTEMS];
    for (int i = 0; i < NUM_SYSTEMS; i++){
        channels_sel_sysident_method[i] = new QComboBox;
        for (auto &method : list_sysident_methods)
            channels_sel_sysident_method[i]->addItem(QString::fromStdString(method));
        channels_sel_sysident_method[i]->setCurrentIndex(config_stdindex_sysident_methods);
        connect(channels_sel_sysident_method[i], SIGNAL(currentIndexChanged(int)), this, SLOT(sel_sysident_changed(int)));
        channels_layouts[i]->addWidget(channels_sel_sysident_method[i]);
    }

    channels_sel_sysident_window = new QComboBox*[NUM_SYSTEMS];
    for (int i = 0; i < NUM_SYSTEMS; i++){
        channels_sel_sysident_window[i] = new QComboBox;
        for (const auto& name : windowfunc::get_type_names())
            channels_sel_sysident_window[i]->addItem(QString::fromStdString(name));
        channels_sel_sysident_window[i]->setCurrentIndex(0);
        connect(channels_sel_sysident_window[i], SIGNAL(currentIndexChanged(int)), this, SLOT(sel_sysident_window_changed(int)));
        channels_layouts[i]->addWidget(channels_sel_sysident_window[i]);
    }

    /*
    channels_sel_Nfft = new QComboBox*[NUM_SYSTEMS];
    for (int i = 0; i < NUM_SYSTEMS; i++){
        channels_sel_Nfft[i] = new QComboBox;
        for (int Nf : list_lengths_Nf)
            channels_sel_Nfft[i]->addItem(QString::number(Nf));
        channels_sel_Nfft[i]->setCurrentIndex(config_stdindex_Nf);
        connect(channels_sel_Nfft[i], SIGNAL(currentIndexChanged(int)), this, SLOT(sel_Nfft_changed(int)));
        channels_layouts[i]->addWidget(channels_sel_Nfft[i]);
    }*/


    channels_line1 = new QFrame*[NUM_SYSTEMS];

    for (int i = 0; i < NUM_SYSTEMS; i++){
        channels_line1[i] = new QFrame;
        channels_line1[i]->setFrameShape(QFrame::HLine);
        channels_line1[i]->setFrameShadow(QFrame::Sunken);
        channels_layouts[i]->addWidget(channels_line1[i]);
    }

    channels_button_calcdelay = new QPushButton*[NUM_SYSTEMS];

    for (int i = 0; i < NUM_SYSTEMS; i++){
        channels_button_calcdelay[i] = new QPushButton;
        channels_button_calcdelay[i]->setText(QString("Calc Delay"));
        connect(channels_button_calcdelay[i], SIGNAL(released()), &channels_calcdelay_mapper, SLOT(map()));
        channels_calcdelay_mapper.setMapping(channels_button_calcdelay[i], i);
        channels_layouts[i]->addWidget(channels_button_calcdelay[i]);
    }
    connect(&channels_calcdelay_mapper, SIGNAL(mapped(int)), this, SLOT(calcDelay_clicked(int)));

    channels_label_delay = new QLabel*[NUM_SYSTEMS];

    for (int i = 0; i < NUM_SYSTEMS; i++){
        channels_label_delay[i] = new QLabel;
        channels_label_delay[i]->setText(QString("Delay (Samples):"));
        channels_layouts[i]->addWidget(channels_label_delay[i]);
    }

    channels_edit_delay = new QLineEdit*[NUM_SYSTEMS];

    for (int i = 0; i < NUM_SYSTEMS; i++){
        channels_edit_delay[i] = new QLineEdit;
        channels_edit_delay[i]->setText(QString("0"));
        channels_layouts[i]->addWidget(channels_edit_delay[i]);
    }

    channels_label_offset = new QLabel*[NUM_SYSTEMS];

    for (int i = 0; i < NUM_SYSTEMS; i++){
        channels_label_offset[i] = new QLabel;
        channels_label_offset[i]->setText(QString("Offset (Samples):"));
        channels_layouts[i]->addWidget(channels_label_offset[i]);
    }

    channels_edit_offset = new QLineEdit*[NUM_SYSTEMS];

    for (int i = 0; i < NUM_SYSTEMS; i++){
        channels_edit_offset[i] = new QLineEdit;
        channels_edit_offset[i]->setText(QString("0"));
        channels_layouts[i]->addWidget(channels_edit_offset[i]);
    }

    channels_button_setdelay = new QPushButton*[NUM_SYSTEMS];

    for (int i = 0; i < NUM_SYSTEMS; i++){
        channels_button_setdelay[i] = new QPushButton;
        channels_button_setdelay[i]->setText(QString("Set Delay+Offset"));
        connect(channels_button_setdelay[i], SIGNAL(released()), &channels_setdelay_mapper, SLOT(map()));
        channels_setdelay_mapper.setMapping(channels_button_setdelay[i], i);
        channels_layouts[i]->addWidget(channels_button_setdelay[i]);
    }
    connect(&channels_setdelay_mapper, SIGNAL(mapped(int)), this, SLOT(on_setDelay_clicked(int)));

    channels_line2 = new QFrame*[NUM_SYSTEMS];

    for (int i = 0; i < NUM_SYSTEMS; i++){
        channels_line2[i] = new QFrame;
        channels_line2[i]->setFrameShape(QFrame::HLine);
        channels_line2[i]->setFrameShadow(QFrame::Sunken);
        channels_layouts[i]->addWidget(channels_line2[i]);
    }

    channels_selLabel_N = new QLabel*[NUM_SYSTEMS];
    for (int i = 0; i < NUM_SYSTEMS; i++){
        channels_selLabel_N[i] = new QLabel;
        channels_selLabel_N[i]->setText(QString("System Length:"));
        channels_layouts[i]->addWidget(channels_selLabel_N[i]);
    }

    channels_sel_N = new QComboBox*[NUM_SYSTEMS];
    for (int i = 0; i < NUM_SYSTEMS; i++){
        channels_sel_N[i] = new QComboBox;
        for (int N : list_lengths_N)
            channels_sel_N[i]->addItem(QString::number(N));
        channels_sel_N[i]->setCurrentIndex(config_stdindex_N);
        connect(channels_sel_N[i], SIGNAL(currentIndexChanged(int)), this, SLOT(sel_N_changed(int)));
        channels_layouts[i]->addWidget(channels_sel_N[i]);
    }

    channels_selLabel_L = new QLabel*[NUM_SYSTEMS];
    for (int i = 0; i < NUM_SYSTEMS; i++){
        channels_selLabel_L[i] = new QLabel;
        channels_selLabel_L[i]->setText(QString("Identification Length:"));
        channels_layouts[i]->addWidget(channels_selLabel_L[i]);
    }

    channels_sel_L = new QComboBox*[NUM_SYSTEMS];
    for (int i = 0; i < NUM_SYSTEMS; i++){
        channels_sel_L[i] = new QComboBox;
        for (int L : list_lengths_L)
            channels_sel_L[i]->addItem(QString::number(L));
        channels_sel_L[i]->setCurrentIndex(config_stdindex_L);
        connect(channels_sel_L[i], SIGNAL(currentIndexChanged(int)), this, SLOT(sel_L_changed(int)));
        channels_layouts[i]->addWidget(channels_sel_L[i]);
    }

    channels_selLabel_Nfft = new QLabel*[NUM_SYSTEMS];
    for (int i = 0; i < NUM_SYSTEMS; i++){
        channels_selLabel_Nfft[i] = new QLabel;
        channels_selLabel_Nfft[i]->setText(QString("Frequency Length:"));
        channels_layouts[i]->addWidget(channels_selLabel_Nfft[i]);
    }

    channels_sel_Nfft = new QComboBox*[NUM_SYSTEMS];
    for (int i = 0; i < NUM_SYSTEMS; i++){
        channels_sel_Nfft[i] = new QComboBox;
        for (int Nf : list_lengths_Nf)
            channels_sel_Nfft[i]->addItem(QString::number(Nf));
        channels_sel_Nfft[i]->setCurrentIndex(config_stdindex_Nf);
        connect(channels_sel_Nfft[i], SIGNAL(currentIndexChanged(int)), this, SLOT(sel_Nfft_changed(int)));
        channels_layouts[i]->addWidget(channels_sel_Nfft[i]);
    }

    channels_line3 = new QFrame*[NUM_SYSTEMS];

    for (int i = 0; i < NUM_SYSTEMS; i++){
        channels_line3[i] = new QFrame;
        channels_line3[i]->setFrameShape(QFrame::HLine);
        channels_line3[i]->setFrameShadow(QFrame::Sunken);
        channels_layouts[i]->addWidget(channels_line3[i]);
    }

    channels_label_expTimeSmooth = new QLabel*[NUM_SYSTEMS];
    for (int i = 0; i < NUM_SYSTEMS; i++){
        channels_label_expTimeSmooth[i] = new QLabel;
        channels_label_expTimeSmooth[i]->setText(QString("Exp. Time Smooting:"));
        channels_layouts[i]->addWidget(channels_label_expTimeSmooth[i]);
    }

    channels_slider_expTimeSmooth = new QSlider*[NUM_SYSTEMS];
    for (int i = 0; i < NUM_SYSTEMS; i++){
        channels_slider_expTimeSmooth[i] = new QSlider;
        channels_slider_expTimeSmooth[i]->setOrientation(Qt::Horizontal);
        channels_slider_expTimeSmooth[i]->setMinimum(0);
        channels_slider_expTimeSmooth[i]->setMaximum(100);
        channels_slider_expTimeSmooth[i]->setTickInterval(5);
        connect(channels_slider_expTimeSmooth[i], SIGNAL(valueChanged(int)), this, SLOT(slider_expTimeSmooth_changed(int)));
        channels_layouts[i]->addWidget(channels_slider_expTimeSmooth[i]);
    }

    channels_line4 = new QFrame*[NUM_SYSTEMS];

    for (int i = 0; i < NUM_SYSTEMS; i++){
        channels_line4[i] = new QFrame;
        channels_line4[i]->setFrameShape(QFrame::HLine);
        channels_line4[i]->setFrameShadow(QFrame::Sunken);
        channels_layouts[i]->addWidget(channels_line4[i]);
    }


    channels_sel_window = new QComboBox*[NUM_SYSTEMS];
    for (int i = 0; i < NUM_SYSTEMS; i++){
        channels_sel_window[i] = new QComboBox;
        for (const auto& name : windowfunc::get_type_names())
            channels_sel_window[i]->addItem(QString::fromStdString(name));
        channels_sel_window[i]->setCurrentIndex(0);
        connect(channels_sel_window[i], SIGNAL(currentIndexChanged(int)), this, SLOT(sel_window_changed(int)));
        channels_layouts[i]->addWidget(channels_sel_window[i]);
    }

    channels_Label_WindowLength = new QLabel*[NUM_SYSTEMS];
    for (int i = 0; i < NUM_SYSTEMS; i++){
        channels_Label_WindowLength[i] = new QLabel;
        channels_Label_WindowLength[i]->setText(QString("Window Width:"));
        channels_layouts[i]->addWidget(channels_Label_WindowLength[i]);
    }

    channels_edit_WindowLength = new QLineEdit*[NUM_SYSTEMS];
    for (int i = 0; i < NUM_SYSTEMS; i++){
        channels_edit_WindowLength[i] = new QLineEdit;
        channels_edit_WindowLength[i]->setText(QString("0"));
        //channels_edit_WindowLength[i]->
        channels_layouts[i]->addWidget(channels_edit_WindowLength[i]);
    }

    channels_Label_WindowOffset = new QLabel*[NUM_SYSTEMS];
    for (int i = 0; i < NUM_SYSTEMS; i++){
        channels_Label_WindowOffset[i] = new QLabel;
        channels_Label_WindowOffset[i]->setText(QString("Window Offset:"));
        channels_layouts[i]->addWidget(channels_Label_WindowOffset[i]);
    }

    channels_edit_WindowOffset = new QLineEdit*[NUM_SYSTEMS];
    for (int i = 0; i < NUM_SYSTEMS; i++){
        channels_edit_WindowOffset[i] = new QLineEdit;
        channels_edit_WindowOffset[i]->setText(QString("0"));
        channels_layouts[i]->addWidget(channels_edit_WindowOffset[i]);
    }

    channels_button_WindowSet = new QPushButton*[NUM_SYSTEMS];
    for (int i = 0; i < NUM_SYSTEMS; i++){
        channels_button_WindowSet[i] = new QPushButton;
        channels_button_WindowSet[i]->setText(QString("Set Window Width/Offset"));
        connect(channels_button_WindowSet[i], SIGNAL(released()), this, SLOT(setWindow_clicked()));
        channels_layouts[i]->addWidget(channels_button_WindowSet[i]);
    }




    channels_spacer1 = new QSpacerItem*[NUM_SYSTEMS];

    for (int i = 0; i < NUM_SYSTEMS; i++){
        channels_spacer1[i] = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
        channels_layouts[i]->addItem(channels_spacer1[i]);
    }

    channels_check_update = new QCheckBox*[NUM_SYSTEMS];

    for (int i = 0; i < NUM_SYSTEMS; i++){
        channels_check_update[i] = new QCheckBox;
        channels_check_update[i]->setCheckState(Qt::Unchecked);
        channels_check_update[i]->setText(QString("Update"));
        channels_layouts[i]->addWidget(channels_check_update[i]);
    }
    channels_check_update[0]->setCheckState(Qt::Checked);



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
      /*  delete channels_tabs[i];
        //delete channels_layouts[i];
        delete channels_check_show[i];
        delete channels_sysident_method[i];
        delete channels_sel_sysident_method[i];
        delete channels_sel_sysident_window[i];
        delete channels_line1[i];
        delete channels_button_calcdelay[i];
        delete channels_label_delay[i];
        delete channels_edit_delay[i];
        delete channels_label_offset[i];
        delete channels_edit_offset[i];
        delete channels_button_setdelay[i];
        delete channels_line2[i];
        delete channels_selLabel_N[i];
        delete channels_sel_N[i];
        delete channels_selLabel_L[i];
        delete channels_sel_L[i];
        delete channels_selLabel_Nfft[i];
        delete channels_sel_Nfft[i];
        delete channels_line3[i];
        delete channels_label_expTimeSmooth[i];
        delete channels_slider_expTimeSmooth[i];
        delete channels_line4[i];
        delete channels_sel_window[i];
        delete channels_Label_WindowLength[i];
        delete channels_edit_WindowLength[i];
        delete channels_Label_WindowOffset[i];
        delete channels_edit_WindowOffset[i];
        delete channels_button_WindowSet[i];
        delete channels_spacer1[i];
        delete channels_check_update[i];*/
    }

    delete channels_tabs;
    delete channels_layouts;
    delete channels_check_show;
    delete channels_sysident_method;
    delete channels_sel_sysident_method;
    delete channels_sel_sysident_window;
    delete channels_line1;
    delete channels_button_calcdelay;
    delete channels_label_delay;
    delete channels_edit_delay;
    delete channels_label_offset;
    delete channels_edit_offset;
    delete channels_button_setdelay;
    delete channels_line2;
    delete channels_selLabel_N;
    delete channels_sel_N;
    delete channels_selLabel_L;
    delete channels_sel_L;
    delete channels_selLabel_Nfft;
    delete channels_sel_Nfft;
    delete channels_line3;
    delete channels_label_expTimeSmooth;
    delete channels_slider_expTimeSmooth;
    delete channels_line4;
    delete channels_sel_window;
    delete channels_Label_WindowLength;
    delete channels_edit_WindowLength;
    delete channels_Label_WindowOffset;
    delete channels_edit_WindowOffset;
    delete channels_button_WindowSet;
    delete channels_spacer1;
    delete channels_check_update;


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
        asa[i]->set_filterlength(list_lengths_N[channels_sel_N[i]->currentIndex()]);
        asa[i]->set_analyze_length(list_lengths_L[channels_sel_L[i]->currentIndex()]);
        asa[i]->set_freq_length(list_lengths_Nf[channels_sel_Nfft[i]->currentIndex()]);
    }
}



void MainWindow::update_timer_event()
{
    //std::cout << "Timer" << std::endl;

    for (int i = 0; i < NUM_SYSTEMS; i++)
    {
        if (!channels_check_show[i]->isChecked()){
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

        if (!channels_check_show[number]->isChecked()){
            continue;
        }

        if (!channels_check_update[number]->isChecked()){
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
            for (int i = 0; i < Nf; i++){
                fr_freq[i] = i * jabuffer->get_fs() / Nf;
            }
            asa[number]->get_freq_resp_db(fr, Nf);
            asa[number]->get_phase(phase, Nf);
            asa[number]->get_groupdelay(groupdelay, Nf);
            asa[number]->get_mscohere(mscohere, Nf);
            if (asa[number]->get_freq_smooting() == 0) {
                plot_freqResp->set_magn_data(number, &fr_freq[1], &fr[1], round(Nf/2) -1);
                plot_freqResp->set_phase_data(number, &fr_freq[1], &phase[1], round(Nf/2) -1);
                plot_freqResp->set_groupdelay_data(number, &fr_freq[1], &groupdelay[1], round(Nf/2) -1);
                plot_freqResp->set_mscohere_data(number, &fr_freq[1], &mscohere[1], round(Nf/2) -1);
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

        if (!asa[number]->is_in_calculation())
            asa[number]->calc_all(position, false);
    }

    for (int i = 0; i < NUM_SYSTEMS; i++)
    {
        if (channels_check_show[i]->isChecked()){
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


void MainWindow::on_setDelay_clicked(int sysnum)
{
    int delay;
    bool ok;
    delay = channels_edit_delay[sysnum]->text().toInt(&ok);
    if (ok) {
        std::cout << "Setting channel " << sysnum << " delay: " << delay << std::endl;
        asa[sysnum]->set_delay(delay);
    }

    int offset;
    offset = channels_edit_offset[sysnum]->text().toInt(&ok);
    if (ok) {
        std::cout << "Setting channel " << sysnum << " offset: " << offset << std::endl;
        asa[sysnum]->set_offset(offset);
    }
}

void MainWindow::calcDelay_clicked(int sysnum)
{
    int delay = asa[sysnum]->identify_delay(jabuffer->get_position());
    channels_edit_delay[sysnum]->setText(QString::number(delay));
}

void MainWindow::sel_N_changed(int index)
{
    int sys = -1;
    for (int i = 0; i < NUM_SYSTEMS; i++)
        if (QObject::sender() == channels_sel_N[i])
            sys = i;
    if (sys >= 0){
        std::cout << "System " << sys << ": Setting N = " << list_lengths_N[index] << std::endl;
        asa[sys]->set_filterlength(list_lengths_N[index]);
    }
}

void MainWindow::sel_L_changed(int index)
{
    int sys = -1;
    for (int i = 0; i < NUM_SYSTEMS; i++)
        if (QObject::sender() == channels_sel_L[i])
            sys = i;
    if (sys >= 0){
        std::cout << "System " << sys << ": Setting L = " << list_lengths_L[index] << std::endl;
        asa[sys]->set_analyze_length(list_lengths_L[index]);
    }
}

void MainWindow::sel_Nfft_changed(int index)
{
    int sys = -1;
    for (int i = 0; i < NUM_SYSTEMS; i++){
        std::cout << QObject::sender() << ", " << channels_sel_Nfft[i] << std::endl;
        if (QObject::sender() == channels_sel_Nfft[i])
            sys = i;
    }
    if (sys >= 0){
        std::cout << "System " << sys << ": Setting Nfft = " << list_lengths_Nf[index] << std::endl;
        asa[sys]->set_freq_length(list_lengths_Nf[index]);
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

void MainWindow::sel_sysident_changed(int index)
{
    int sys = -1;
    for (int i = 0; i < NUM_SYSTEMS; i++){
        std::cout << QObject::sender() << ", " << channels_sel_sysident_method[i] << std::endl;
        if (QObject::sender() == channels_sel_sysident_method[i])
            sys = i;
    }
    if (sys >= 0){
        std::cout << "System " << sys << ": Setting Identification Method = " << list_sysident_methods[index] << std::endl;
        asa[sys]->set_sysident_method(index);
    }
}

void MainWindow::sel_sysident_window_changed(int index)
{
    int sys = -1;
    for (int i = 0; i < NUM_SYSTEMS; i++){
        if (QObject::sender() == channels_sel_sysident_window[i])
            sys = i;
    }
    if (sys >= 0){
        std::cout << "System " << sys << ": Syident-Window new Value: " << windowfunc::get_type_name(index) << std::endl;
        asa[sys]->set_sysident_window_type(index);
    }
}

void MainWindow::slider_expTimeSmooth_changed(int value)
{
    int sys = -1;
    for (int i = 0; i < NUM_SYSTEMS; i++){
        if (QObject::sender() == channels_slider_expTimeSmooth[i])
            sys = i;
    }
    if (sys >= 0){
        std::cout << "System " << sys << ": Exp. Time Smooting new Value: " << value << std::endl;
        asa[sys]->set_expTimeSmoothFactor(static_cast<double>(value)/100.0);
    }
}


void MainWindow::sel_window_changed(int index)
{
    int sys = -1;
    for (int i = 0; i < NUM_SYSTEMS; i++){
        if (QObject::sender() == channels_sel_window[i])
            sys = i;
    }
    if (sys >= 0){
        std::cout << "System " << sys << ": Window new Value: " << windowfunc::get_type_name(index) << std::endl;
        asa[sys]->set_window_type(index);
    }
}

void MainWindow::setWindow_clicked()
{
    int sys = -1;
    for (int i = 0; i < NUM_SYSTEMS; i++){
        if (QObject::sender() == channels_button_WindowSet[i])
            sys = i;
    }
    if (sys >= 0){
        int len, offset;
        bool okay;
        len = channels_edit_WindowLength[sys]->text().toInt(&okay);
        if (!okay)
            return;
        offset = channels_edit_WindowOffset[sys]->text().toInt(&okay);
        if (!okay)
            return;
        std::cout << "System " << sys << ": Window Length: " << len << ", Window Offset: " << offset << std::endl;
        asa[sys]->set_window_length(len);
        asa[sys]->set_window_offset(offset);
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
        if (channels_check_update[i]->isChecked())
            text.append(QString::number(asa[i]->get_calc_time_full()).rightJustified(4) + QString(" ms"));
        else
            text.append("-");
        if (i != (NUM_SYSTEMS - 1))
            text.append(" / ");
    }
    text.append("  |  Identification time: ");
    for (int i = 0; i < NUM_SYSTEMS; i++){
        if (channels_check_update[i]->isChecked())
            text.append(QString::number(asa[i]->get_calc_time_identify()).rightJustified(4) + QString(" ms"));
        else
            text.append("-");
        if (i != (NUM_SYSTEMS - 1))
            text.append(" / ");
    }
    status_label.setText(text);
}

