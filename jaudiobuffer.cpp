#include "jaudiobuffer.h"

#include <iostream>
#include <cstring>
#include <sstream>
#include <iomanip>

#include "config.h"


#define BUFFER_AUDIO_LENGTH 300000


JAudioBuffer::JAudioBuffer()
{
    position = 0;
    started = false;
    num_channels = 2*NUM_SYSTEMS;
    buffer_size = BUFFER_AUDIO_LENGTH;
    input_ports = new jack_port_t*[num_channels];
    buffer = new float*[num_channels];
    for (int i = 0; i < num_channels; i++){
        buffer[i] = new float[buffer_size];
        for (int j = 0; j < buffer_size; j++){
            buffer[i][j] = 0;
        }
    }
}

JAudioBuffer::~JAudioBuffer()
{
    stop();
    delete input_ports;
    for (int i = 0; i < num_channels; i++){
        delete buffer[i];
    }
    delete buffer;
}

int JAudioBuffer::start(JackProcessCallback process_helper)
{
    const char *client_name = "caa";
    const char *server_name = NULL;
    jack_options_t options = JackNullOption;
    jack_status_t status;

    client = jack_client_open (client_name, options, &status, server_name);
    if (client == NULL) {
        std::cout << "ERROR: jack_client_open failed with status: " << status << std::endl;
        if (status & JackServerFailed) {
            std::cout << "Unable to connect to JACK server" << std::endl;
        }
        return (1);
    }

    std::cout << "Jack client name is: " << jack_get_client_name(client) << std::endl;

    jack_set_process_callback (client, process_helper, 0);

    this->sample_rate = jack_get_sample_rate (client);
    std::cout << "Running at sampling Rate " << this->sample_rate << "Hz" << std::endl;

    for(int i = 0; i < num_channels; i++){
        std::ostringstream tmpstream;

        if (i%2 == 0)
            tmpstream << std::setw(1) << std::setfill('0') << i/2 << "_ref";
        else
            tmpstream << std::setw(1) << std::setfill('0') << i/2 << "_meas" ;

        input_ports[i] = jack_port_register (client, tmpstream.str().c_str(), JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
        if (input_ports[i] == NULL) {
            std::cout << "Could not create Jack ports" << std::endl;
            return (1);
        }
    }

    if (jack_activate (client)) {
        std::cout << "Could not start Jack client" << std::endl;
        return (1);
    }

    started = true;
    return 0;
}

int JAudioBuffer::stop()
{
    if (started) {
        jack_deactivate(client);
        for(int i = 0; i < num_channels; i++){
            jack_port_unregister(client, input_ports[i]);
        }
        jack_client_close(client);
        started = false;
    }

    return 0;
}

int JAudioBuffer::process(jack_nframes_t nframes, void *arg)
{
    (void) arg;
    jack_default_audio_sample_t *samples;
    int64_t pos_out = position;

    for (int i = 0; i < num_channels; i++){

        samples = (jack_default_audio_sample_t *) jack_port_get_buffer (input_ports[i], nframes);
        if (samples == NULL)
            continue;
        int num = nframes;

        int written = 0;
        pos_out = position;

        //calc_buffer_usage();

        while (written < num){
            int64_t next_buffer_border = (pos_out / buffer_size)*buffer_size + buffer_size; // end of next ring loop (exclusive)
            if ((next_buffer_border - pos_out) >= (num-written)){
                std::memcpy(&buffer[i][pos_out % buffer_size], &samples[written], sizeof(jack_default_audio_sample_t) * (num-written));
                pos_out += (num-written);
                written = num;
            }
            else {
                std::memcpy(&buffer[i][pos_out % buffer_size], &samples[written], sizeof(jack_default_audio_sample_t) * (next_buffer_border - pos_out));
                written += next_buffer_border - pos_out;
                pos_out = next_buffer_border;

            }
        }
    }

    position = pos_out;

    return 0;
}

double JAudioBuffer::get_fs()
{
    return this->sample_rate;
}

int64_t JAudioBuffer::get_position()
{
    return position;
}

void JAudioBuffer::get_samples(int channel, int64_t start, float *dest, int num)
{
    int64_t pos_out = start;
    int read = 0;

    while (read < num){
        int64_t next_buffer_border = (pos_out / buffer_size)*buffer_size + buffer_size; // end of next ring loop (exclusive)
        if ((next_buffer_border - pos_out) >= (num-read)){
            std::memcpy(&dest[read], &buffer[channel][pos_out % buffer_size], sizeof(jack_default_audio_sample_t) * (num-read));
            pos_out += (num-read);
            read = num;
        }
        else {
            std::memcpy(&dest[read], &buffer[channel][pos_out % buffer_size], sizeof(jack_default_audio_sample_t) * (next_buffer_border - pos_out));
            read += next_buffer_border - pos_out;
            pos_out = next_buffer_border;

        }
    }
}
