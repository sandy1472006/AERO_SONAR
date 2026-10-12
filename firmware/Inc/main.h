/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file for AERO-SONAR Transmitter.
  *                   STM32G474RE Embedded Hardware Core.
  * @team           : Team AEROS (Team ID: 186870) - SIH 2026 PS 26058
  ******************************************************************************
  */

#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <math.h>

/* Mathematical Constants */
#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

/* Hardware Sampling & Buffer Specifications */
#define SYS_CLOCK_FREQ_HZ       170000000UL   /* 170 MHz Core PLL Clock */
#define DAC_SAMPLE_RATE_HZ      1500000UL     /* 1.5 MSPS Virtual DAC Conversion Rate */
#define DMA_BUFFER_SIZE         1024U         /* Double-buffered 1024-sample ping-pong frame */
#define DMA_HALF_BUFFER_SIZE    512U

/* Sensor Channels & Ranges */
#define NUM_EMULATED_SENSORS    8U
#define SENSOR_POLL_RATE_HZ     20U           /* 20 Hz Deterministic Adaptation Loop */
#define TELEMETRY_BAUD_RATE     115200UL      /* USART2 VCP Serial Baud */

/* Operating Waveform Modes */
typedef enum {
    WAVEFORM_LFM_CHIRP    = 0,
    WAVEFORM_HFM_DOPPLER  = 1,
    WAVEFORM_BARKER_13    = 2,
    WAVEFORM_CW_PULSE     = 3
} WaveformType_t;

/* Operating Corridors */
typedef enum {
    CORRIDOR_HIGH_RES      = 0,
    CORRIDOR_BALANCED      = 1,
    CORRIDOR_LONG_RANGE    = 2,
    CORRIDOR_POWER_SAVE    = 3,
    CORRIDOR_SAFE_FALLBACK = 4
} Corridor_t;

/* Raw & Physical Environmental Input Vector */
typedef struct {
    float depth_m;          /* 0.0 to 200.0 m */
    float temp_c;           /* 0.0 to 30.0 °C */
    float salinity_ppt;     /* 25.0 to 38.0 ppt */
    float noise_db;         /* 30.0 to 80.0 dB */
    float target_range_m;   /* 10.0 to 200.0 m */
    float speed_ms;         /* 0.0 to 3.0 m/s */
    float battery_pct;      /* 0.0 to 100.0 % */
    float turbidity_ntu;    /* 0.0 to 100.0 NTU */
} EnvironmentState_t;

/* Decision Engine Output Parameters */
typedef struct {
    Corridor_t      corridor;
    WaveformType_t  waveform;
    float           f_center_hz;
    float           f_start_hz;
    float           f_stop_hz;
    float           bandwidth_hz;
    float           duration_ms;
    float           amplitude_pct;
    bool            clamped_amplitude;
    bool            clamped_duration;
    bool            clamped_frequency;
} WaveformDecision_t;

/* Derived Acoustic & Link Telemetry */
typedef struct {
    float   sound_speed_ms;
    float   alpha_total_db_km;
    float   resolution_cm;
    float   doppler_d;
    float   usable_range_m;
    float   energy_per_ping_mj;
    bool    reaches_seabed;
} AcousticDerived_t;

/* Function Prototypes */
void SystemClock_Config(void);
void Error_Handler(void);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
