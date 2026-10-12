/**
  ******************************************************************************
  * @file           : ewa_decision_engine.h
  * @brief          : Environment-Energy-Waveform (EWA) decision engine & safety supervisor
  * @team           : Team AEROS (Team ID: 186870) - SIH 2026 PS 26058
  ******************************************************************************
  */

#ifndef __EWA_DECISION_ENGINE_H
#define __EWA_DECISION_ENGINE_H

#include "main.h"

/* Initialize the EWA Decision Engine */
void EWA_Init(void);

/* Main Decision Engine Function: Pure deterministic mapping from env -> decision */
WaveformDecision_t EWA_ComputeDecision(const EnvironmentState_t *env);

/* Compute secondary acoustic link derivations & telemetry */
AcousticDerived_t EWA_ComputeDerived(const EnvironmentState_t *env, const WaveformDecision_t *decision);

/* Sanitize and bound raw sensor ADC voltages into physical units */
EnvironmentState_t EWA_SanitizeInputs(const float raw_adcs[NUM_EMULATED_SENSORS]);

#endif /* __EWA_DECISION_ENGINE_H */
