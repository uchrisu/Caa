#include "mainwindow.h"
#include <iostream>

#include <QApplication>

#include "jaudiobuffer.h"
#include "audiosystemanalyzer.h"

#include "config.h"

JAudioBuffer *jabuffer;
AudioSystemAnalyzer **asa;


int jack_process_helper (jack_nframes_t nframes, void *arg)
{
    return jabuffer->process(nframes, arg);
}

int main(int argc, char *argv[])
{
    jabuffer = new JAudioBuffer();
    if (jabuffer->start(&jack_process_helper) != 0){
        std::cerr << "No Audio" << std::endl;
        exit(-1);
    }

    asa = new AudioSystemAnalyzer*[NUM_SYSTEMS];

    for (int i = 0; i < NUM_SYSTEMS; i++)
        asa[i] = new AudioSystemAnalyzer(jabuffer, i);

    QApplication a(argc, argv);
    MainWindow w;
    w.set_buffer(jabuffer);
    w.set_analyzer(asa);
    w.show();

    int result = a.exec();

    delete asa;
    jabuffer->stop();
    delete jabuffer;


    return result;
}
