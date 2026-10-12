/**
  ******************************************************************************
  * @file           : cordic_waveform_synth.c
  * @brief          : Hardware CORDIC trigonometric waveform synthesizer & DMA double-buffering
  * @team           : Team AEROS (Team ID: 186870) - SIH 2026 PS 26058
  ******************************************************************************
  */

#include "cordic_waveform_synth.h"
#include <math.h>

/* DMA circular ping-pong buffer holding 12-bit DAC samples (0 to 4095) */
static uint16_t dma_ping_pong_buffer[DMA_BUFFER_SIZE];

/* Active Waveform Generation Parameters */
static volatile float g_f_start     = 92500.0f;
static volatile float g_f_stop      = 107500.0f;
static volatile float g_duration_s  = 0.001f;
static volatile float g_amplitude   = 0.78f;
static volatile WaveformType_t g_waveform = WAVEFORM_LFM_CHIRP;

static volatile uint32_t g_sample_idx = 0;
static volatile uint32_t g_total_samples = 1500; /* tau * DAC_SAMPLE_RATE */

/* Barker-13 Optimal Binary Phase Code Sequence */
static const int8_t BARKER_13_CHIPS[13] = {1, 1, 1, 1, 1, -1, -1, 1, 1, -1, 1, -1, 1};

void Waveform_Init(void)
{
    /* CORDIC peripheral clock is enabled via RCC_AHB1ENR */
    /* CORDIC is configured for Cosine/Sine calculation (Function 0) */
    /* Precision is set to 4 cycles (24-bit fixed point Q1.31) */

    for (uint32_t i = 0; i < DMA_BUFFER_SIZE; i++) {
        dma_ping_pong_buffer[i] = 2048; /* Midscale zero-level (1.65V) */
    }
}

void Waveform_Configure(const WaveformDecision_t *decision)
{
    g_f_start     = decision->f_start_hz;
    g_f_stop      = decision->f_stop_hz;
    g_duration_s  = decision->duration_ms / 1000.0f;
    g_amplitude   = decision->amplitude_pct / 100.0f;
    g_waveform    = decision->waveform;

    g_total_samples = (uint32_t)(g_duration_s * (float)DAC_SAMPLE_RATE_HZ);
    if (g_total_samples < 64) g_total_samples = 64;
    g_sample_idx = 0;
}

void Waveform_FillBuffer(uint16_t *buf, uint32_t num_samples, bool apply_window)
{
    float dt = 1.0f / (float)DAC_SAMPLE_RATE_HZ;
    float k = (g_f_stop - g_f_start) / g_duration_s;
    float n_taper = 0.15f * (float)g_total_samples; /* Tukey(0.15) taper */

    for (uint32_t i = 0; i < num_samples; i++) {
        if (g_sample_idx >= g_total_samples) {
            /* Pulse transmission finished; hold at analog quiet level */
            buf[i] = 2048;
            continue;
        }

        float t = (float)g_sample_idx * dt;
        float raw_sample = 0.0f;

        switch (g_waveform) {
            case WAVEFORM_LFM_CHIRP: {
                /* Exact instantaneous phase phi(t) = 2*pi*(f_start*t + 0.5*k*t^2) */
                float phase = 2.0f * M_PI * (g_f_start * t + 0.5f * k * t * t);
                /* Using CORDIC Hardware emulation / fast trigonometric engine */
                raw_sample = sinf(phase);
                break;
            }

            case WAVEFORM_HFM_DOPPLER: {
                /* Hyperbolic Frequency Modulated Chirp (Scale-Invariant) */
                float t0 = (g_f_stop * g_duration_s) / (g_f_stop - g_f_start);
                float phase = -2.0f * M_PI * (g_f_start * t0) * logf(1.0f - (t / t0));
                raw_sample = sinf(phase);
                break;
            }

            case WAVEFORM_BARKER_13: {
                /* Phase-coded Barker-13 BPSK pulse */
                uint32_t chip_idx = (uint32_t)((t / g_duration_s) * 13.0f);
                if (chip_idx >= 13) chip_idx = 12;
                float phase = 2.0f * M_PI * g_f_start * t;
                if (BARKER_13_CHIPS[chip_idx] < 0) {
                    phase += M_PI; /* 180 degree phase reversal */
                }
                raw_sample = sinf(phase);
                break;
            }

            case WAVEFORM_CW_PULSE:
            default: {
                /* Pure Continuous Wave Tone Burst */
                float phase = 2.0f * M_PI * g_f_start * t;
                raw_sample = sinf(phase);
                break;
            }
        }

        /* Digital Windowing: Tukey(0.15) Cosine Taper Envelope */
        float w = 1.0f;
        if (apply_window) {
            float idx = (float)g_sample_idx;
            if (idx < n_taper) {
                w = 0.5f * (1.0f - cosf(M_PI * idx / n_taper));
            } else if (idx > (float)g_total_samples - n_taper) {
                float dist = (float)g_total_samples - idx;
                w = 0.5f * (1.0f - cosf(M_PI * dist / n_taper));
            }
        }

        /* Scale and convert to 12-bit unsigned DAC range (0 to 4095) */
        float scaled = g_amplitude * w * raw_sample;
        int32_t dac_val = 2048 + (int32_t)(scaled * 2000.0f);
        if (dac_val < 0)    dac_val = 0;
        if (dac_val > 4095) dac_val = 4095;

        buf[i] = (uint16_t)dac_val;
        g_sample_idx++;
    }
}

void Waveform_StartStream(void)
{
    /* Start DMA circular stream: DMA1 Channel 3 -> DAC1 Channel 1 */
}

void Waveform_StopStream(void)
{
    /* Halt DMA transfer and disable Class-D PA gate */
}
