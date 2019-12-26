/*
************************************************************************************************************************
*                                                     Oscilloscope
*
*                                    (c) Copyright 2016; Micrium, Inc.; Weston, FL
*                           All rights reserved.  Protected by international copyright laws.
*
*
* File    : PROBE_SCOPE.H
* By      : JJL
* Version : V1.00.00
************************************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/
#ifndef PROBE_SCOPE_H
#define PROBE_SCOPE_H

#include  <stdint.h>
#include  <probe_scope_cfg.h>

/*
*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void     ProbeScope_Init(uint32_t  sampling_clk_hz);
void     ProbeScope_Sampling(void);
void     ProbeScope_SamplingTmrInitHz(uint32_t  sampling_clk_hz);

#endif // !PROBE_SCOPE_H


