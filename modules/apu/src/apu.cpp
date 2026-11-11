#include "apu.h"

#include <iostream>
#include <iomanip>
#include <string>

#include "miniaudio.h"

#define DEVICE_FORMAT       ma_format_f32
#define DEVICE_CHANNELS     2
#define DEVICE_SAMPLE_RATE  48000

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    ma_waveform_read_pcm_frames((ma_waveform*)pDevice->pUserData, pOutput, frameCount, NULL);

    (void)pInput;   /* Unused. */
}

NES::APU::APU()
{

}

void NES::APU::test()
{
    ma_waveform sineWave;
    ma_device_config deviceConfig;
    ma_device device;
    ma_waveform_config sineWaveConfig;

    deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format = DEVICE_FORMAT;
    deviceConfig.playback.channels = DEVICE_CHANNELS;
    deviceConfig.sampleRate = DEVICE_SAMPLE_RATE;
    deviceConfig.dataCallback = data_callback;
    deviceConfig.pUserData = &sineWave;

    if (ma_device_init(NULL, &deviceConfig, &device) != MA_SUCCESS) {
        std::cout << "Failed to open playback device." << std::endl;
        return;
    }

    std::cout << "DEvice Name: " << device.playback.name << std::endl;
    

    sineWaveConfig = ma_waveform_config_init(device.playback.format, device.playback.channels, device.sampleRate, ma_waveform_type_sine, 0.2, 220);
    ma_waveform_init(&sineWaveConfig, &sineWave);

    if (ma_device_start(&device) != MA_SUCCESS) {
        std::cout << "Failed to start playback device." << std::endl;
        ma_device_uninit(&device);
        return;
    }

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(main_loop__em, 0, 1);
#else
    std::cout << "Press Enter to Quit";
    std::string iobuf;
    std::getline(std::cin, iobuf);
#endif

    ma_device_uninit(&device);
    ma_waveform_uninit(&sineWave);  /* Uninitialize the waveform after the device so we don't pull it from under the device while it's being reference in the data callback. */
}