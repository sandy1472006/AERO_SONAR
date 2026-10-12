/**
  ******************************************************************************
  * @file           : cordic_waveform_synth.h
  * @brief          : Hardware CORDIC coprocessor accelerated waveform generator
  * @team           : Team AEROS (Team ID: 186870) - SIH 2026 PS 26058
  ******************************************************************************
  */

#ifndef __CORDIC_WAVEFORM_SYNTH_H
#define __CORDIC_WAVEFORM_SYNTH_H

#include "main.h"

/* Initialize STM32G4 CORDIC hardware accelerator and DMA channels */
void Waveform_Init(void);

/* Configure real-time chirp generator parameters from EWA decision */
void Waveform_Configure(const WaveformDecision_t *decision);

/* Fill a DMA ping-pong buffer chunk (512 samples) with synthesized waveform */
void Waveform_FillBuffer(uint16_t *buf, uint32_t num_samples, bool apply_window);

/* Start DAC streaming */
void Waveform_StartStream(void);

/* Stop DAC streaming */
void Waveform_StopStream(void);

#endif /* __CORDIC_WAVEFORM_SYNTH_H */
