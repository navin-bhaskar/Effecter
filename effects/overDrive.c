                                                  /* Shree Krishnaya Namaha */
/** @file overDrive.c
    @brief Implements overdrive effects
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

#define abs(x) (x < 0?(x*-1.0):x)

static const float threshold = (1/3.0);

static inline float calcValue(float val)
{
    float retVal;
    
    if (abs(val) < threshold)
    {
        retVal = val*2;
    }
    else if (abs(val) >= threshold && abs(val) < 2*threshold)
    {
        if (val > 0)
        {
            retVal = (3-((2 - val*3)*(2 - val*3)))/3.0;
        }
        else if (val < 0)
        {
            retVal = (3-((2 - abs(val)*3)*(2 - val*3)))/3.0;
        }
    }
    else 
    {
        if (val > 0) retVal = 1;
        if (val < 0) retVal = -1;
    }
    
    return retVal;
}

static int drives[] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 99};
extern uint16_t gActuatorValIdx;

uint16_t ovrDrive2GetParamLen(void)
{
     return sizeof(drives)/sizeof(drives[0]);
}

static inline float ovr2(float val)
{
    int drive;
    float k;
    if (gActuatorValIdx < ovrDrive2GetParamLen())
    {
        drive = drives[gActuatorValIdx];
    }
    else
    {
        drive = drives[0];
    }
    
    float a = sin(((drive+1)/101)*(M_PI/2));
    float retVal;
    k = 2.0*a/(1-a);
    retVal = ((1+k)*val)/(1+k*abs(val));
    return retVal;
} 
            
    
void overDrive(float *in, float *out, int len)
{
    int i;

    for (i=0; i<len; i++)
    {
        *out++ = calcValue(*in++);
        *out++ = calcValue(*in++);
    }
}

void overDrive2(float *in, float *out, int len)
{
    int i;

    for (i=0; i<len; i++)
    {
        *out++ = ovr2(*in++);
        *out++ = ovr2(*in++);
    }
}   
