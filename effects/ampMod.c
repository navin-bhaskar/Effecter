                                /* Shree Krishnaya Namaha */
                                
/** @file ampMod.c
    @brief Implements amplitude modulation effect
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
#include "../effects.h"

static float alphas[] = {0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8};
static float Fcs[] =    {30,  50,  80,  90,  100, 200, 300, 400};

extern uint16_t gActuatorValIdx;

uint16_t ampMaxVal(void)
{
    return sizeof(alphas)/sizeof(alphas[0]);
}

void ampMod(float * in, float *out, int len)
{
    int i;
    float Fc = 20;
    float alpha;
    float carrier;
    if (gActuatorValIdx < ampMaxVal())
    {
        Fc = Fcs[gActuatorValIdx];
        alpha = alphas[gActuatorValIdx];
    }
    else
    {
        Fc = Fcs[0];
        alpha = alphas[0];
    }
    float omega = (2*M_PI*(Fc/44100.0));

    for (i=0; i<len; i++)
    {
        carrier = ( 1 + alpha*sin(omega*i));
        *out++ = (*in++)*carrier;
        *out++ = (*in++)*carrier;
    }
}

    
    
