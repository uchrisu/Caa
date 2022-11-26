#include "mainwindow.h"
#include <iostream>

#include <QApplication>

#include "jaudiobuffer.h"


JAudioBuffer *jabuffer;

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

    QApplication a(argc, argv);
    MainWindow w;
    w.set_buffer(jabuffer);
    w.init_analyzer();
    w.show();

    int result = a.exec();

    jabuffer->stop();
    delete jabuffer;


    return result;
}
