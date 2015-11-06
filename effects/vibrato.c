                                                  /* Shree Krishnaya Namaha */
/** @file vibrato.c
    @brief Implements vibrato effects
	@author Navin Bhaskar
*/


/*
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 
 * * Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above
 *   copyright notice, this list of conditions and the following disclaimer
 *   in the documentation and/or other materials provided with the
 *   distribution.
 * * Neither the name of the  nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */

#include <math.h>
#include <string.h>
#include "../config.h"
#include "../effects.h"

#define DELAY_SCALE 10
#define MAX_DEPTH  (DELAY_SCALE*FRAMES_PER_BUFFER)
#define BUFFER_SIZE (MAX_DEPTH*2)

static float delayBuffer[BUFFER_SIZE];      /**< Delay buffer for left and right channel */
static int rdPtr;                           /**< The pointer from where data is to be read */
static int delayPtr;                        /**< The amount of delay */

static int squareCnt = 100; 

void initVibrato(void)
{
    memset(delayBuffer, 0, sizeof(delayBuffer));
    rdPtr = delayPtr = 0;
}

void inline fillDelayBuffer(float *in, int len)
{
    int i;
    for (i=0; i<len; i++)
    {
        /* Copy the data to our delay buffers */
        delayBuffer[rdPtr] = in[i];
        rdPtr = (rdPtr+1)%BUFFER_SIZE;
    }
}

/**
 * Implements the vibrato effect
 */

static uint16_t sinFreqs[] = {5,   10,  15,  20,  25,  30,  35,  40,  45,  50,  55};
static uint16_t sawMaxs[]  = {50, 100, 150, 200, 250, 300, 350, 400, 450, 500, 550};
static uint16_t sqMax[] =    {10,  20,  30,  40,  50,  60,  65,  70,  80,  85,  90};

extern uint16_t gActuatorValIdx;

uint16_t vibratoGetParamLen(void)
{
    return sizeof(sinFreqs)/sizeof(sinFreqs[0]);
}

void sinVibrato(float *in, float *out, int len)
{
    float modulationFreq;
    int i;
    float omega;
    
    int delay;
    float modFreq;

    if (gActuatorValIdx < vibratoGetParamLen())
    {
        modFreq = sinFreqs[gActuatorValIdx];
    }
    else
    {
        modFreq = sinFreqs[0];
    }
    
    modulationFreq = (float)(modFreq)/SAMPLE_RATE;
    omega = M_PI*2*modulationFreq;
    
    fillDelayBuffer(in, len);
        
    for (i=0; i<len; i++)
    {
        float mod = sin(omega*i);
        delayPtr = (delayPtr + (int)(mod*100)) % BUFFER_SIZE;
        *out++ = delayBuffer[delayPtr];
        delayPtr = (delayPtr+1) % BUFFER_SIZE;
    }
}

void sawToothVibrato(float *in, float *out, int len)
{
    static int sawOsc;
    int i;
    uint16_t maxVal;

    if (gActuatorValIdx < vibratoGetParamLen())
    {
        maxVal = sawMaxs[gActuatorValIdx];
    }
    else
    {
        maxVal = sawMaxs[0];
    }
    
    if (sawOsc < maxVal)
    {
        sawOsc++;
    }
    else if (sawOsc == maxVal)
    {
        sawOsc = 0;
    } 
    fillDelayBuffer(in, len);
    
    for (i=0; i<len; i++)
    {
        delayPtr = (delayPtr + sawOsc) % BUFFER_SIZE;
        *out++ = delayBuffer[delayPtr];
        delayPtr = (delayPtr+1) % BUFFER_SIZE;
    }
}    

void squareVibrato(float *in, float *out, int len)
{
    static int counter;
    int i;
    
    fillDelayBuffer(in, len);
    
    if (gActuatorValIdx < vibratoGetParamLen())
    {
        counter = sqMax[gActuatorValIdx];
    }
    else
    {
        counter = sqMax[0];
    }

    if (counter <= squareCnt)
    {
        for (i=0; i<len; i++)
        {
            *out++ = *in++;
        }
    }
    else
    {
        for (i=0; i<len; i++)
        {
            delayPtr = (delayPtr + i) % BUFFER_SIZE;
            *out++ = delayBuffer[delayPtr];
        }
        if (counter >= 2*squareCnt) counter = 0;
    }
    counter++;
}
            
             
    
    
    
    
