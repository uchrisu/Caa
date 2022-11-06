#ifndef JAUDIOBUFFER_H
#define JAUDIOBUFFER_H

#include <jack/jack.h>
#include <atomic>


class JAudioBuffer
{
public:
    JAudioBuffer();
    ~JAudioBuffer();
    int start(JackProcessCallback process_helper);
    int stop();
    int process(jack_nframes_t nframes, void *arg);
    double get_fs();
    int64_t get_position();
    void get_samples(int channel, int64_t start, float *dest, int num);

private:
    int num_channels;
    int buffer_size;
    bool started;
    jack_port_t **input_ports;
    jack_client_t *client;
    double sample_rate;

    float **buffer;

    std::atomic<int64_t> position;
};

#endif // JAUDIOBUFFER_H
