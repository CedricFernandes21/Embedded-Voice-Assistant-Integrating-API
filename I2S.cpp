#include "I2S.h"

// ------------------------------------------------------------------------------------------------
// I2S CONFIGURATION
// ------------------------------------------------------------------------------------------------
// Sample rate for the microphone
#define SAMPLE_RATE (16000)

// Pins for the I2S interface based on your circuit diagram
#define PIN_I2S_BCLK 27 // SCK
#define PIN_I2S_LRC  22 // WS
#define PIN_I2S_DIN  34 // SD

// The DOUT pin is not used for a microphone, so it is not defined here.
// ------------------------------------------------------------------------------------------------

/**
 * @brief Constructor for the I2S class.
 * This initializes the I2S driver based on the specified microphone type.
 */
I2S::I2S(MicType micType) {
    // This configuration is for the INMP441, ADMP441, or ICS43434 microphones.
    if (micType == ADMP441 || micType == ICS43434) {
        // The I2S peripheral reads 32-bit samples from these microphones.
        BITS_PER_SAMPLE = I2S_BITS_PER_SAMPLE_32BIT;

        // I2S driver configuration
        i2s_config_t i2s_config = {
            .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
            .sample_rate = SAMPLE_RATE,
            .bits_per_sample = BITS_PER_SAMPLE,
            // Since L/R is grounded, the mic outputs on the left channel.
            .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
            .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
            .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
            .dma_buf_count = 8,
            .dma_buf_len = 256,
            .use_apll = false
        };

        // Install the I2S driver.
        i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);

        // Define the physical pins for the I2S interface.
        i2s_pin_config_t pin_config;
        pin_config.bck_io_num = PIN_I2S_BCLK;
        pin_config.ws_io_num = PIN_I2S_LRC;
        pin_config.data_out_num = I2S_PIN_NO_CHANGE; // We are not using DOUT.
        pin_config.data_in_num = PIN_I2S_DIN;

        // Set the pins for the I2S driver.
        i2s_set_pin(I2S_NUM_0, &pin_config);
    }
    // This part of the code is for other hardware (M5GO, M5Stack) and is not used
    // in your setup, but is kept for library compatibility.
    else if (micType == M5GO || micType == M5STACKFIRE) {
        BITS_PER_SAMPLE = I2S_BITS_PER_SAMPLE_16BIT;
        i2s_config_t i2s_config = {
            .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN | I2S_MODE_ADC_BUILT_IN),
            .sample_rate = SAMPLE_RATE,
            .bits_per_sample = BITS_PER_SAMPLE,
            .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
            .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S_MSB),
            .intr_alloc_flags = 0,
            .dma_buf_count = 2,
            .dma_buf_len = 1024
        };
        i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
        i2s_set_adc_mode(ADC_UNIT_1, ADC1_CHANNEL_6);
        i2s_set_clk(I2S_NUM_0, SAMPLE_RATE, BITS_PER_SAMPLE, I2S_CHANNEL_STEREO);
        i2s_adc_enable(I2S_NUM_0);
    }
}

/**
 * @brief Reads a block of audio data from the I2S microphone.
 * @param data Pointer to the buffer to store the read data.
 * @param numData Number of bytes to read.
 * @return The number of bytes successfully read.
 */
int I2S::Read(char* data, int numData) {
    size_t bytes_read = 0;
    // Use the newer i2s_read function for better error handling.
    i2s_read(I2S_NUM_0, (char *)data, numData, &bytes_read, portMAX_DELAY);
    return bytes_read;
}

/**
 * @brief Gets the configured bits per sample for the I2S interface.
 * @return The bits per sample (16 or 32).
 */
int I2S::GetBitPerSample() {
    return (int)BITS_PER_SAMPLE;
}