                                        /* Shree Krishnaya Namha */

/** @file effecter.cc
    @brief The 'main' file for the effecter application
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


#include <stdio.h>
#include <math.h>
#include <signal.h>
#include "portaudio.h"
#include "config.h"
#include "effects.h"
#include "mraa.h"
#include "upm/jhd1313m1.h"

//*************************** global vars **********************************//
uint16_t gActuatorValIdx;        /**< Global var that holds the current actuator index */


//************************ static global vars ******************************//
static int gCounter;          /**< Used to detect the button press */
static bool gbKeepGoing;      /**< Flag to indicate whether to continue with the application or not */
static int gCurrentEffect;    /**< The index to the current effect within the effects array */
static uint16_t gMaxEffects;  /**< The total number of effects that are in operation */
//*************************** The effects array *****************************//

static effect effects[] = 
{
    {NULL,    NULL, NULL, "Plain", {0, 0, 255} },
    {ringMod, NULL, ringModGetParamLen, "Ring mod", {40, 10, 200} },
    {ampMod,  NULL, ampMaxVal, "Amplitude mod", {10, 10, 10} },
    {overDrive, NULL, NULL, "Overdrive", {50, 50, 100} },
    {overDrive2, NULL, ovrDrive2GetParamLen, "Overdrive 2", {40, 10, 50} },
    {reverb,    initReverb, reverbGetParamLen, "Reverb", {55, 22, 11} },
    {sinVibrato, initVibrato, vibratoGetParamLen, "Sin Vibrato", {76, 44, 12} },
    {sawToothVibrato, initVibrato, vibratoGetParamLen, "Sawtooth Vibrato", {11, 10, 4} },
    {squareVibrato, initVibrato,  vibratoGetParamLen, "Square vibrato", {10, 20, 30} },
    {fuzzBox, NULL, NULL, "Fuzz box", {101, 201, 22} }
};

/**
 * The button interrupt handler 
 *
 */

void btnInterrupt(void * param)
{
    gCounter++;
}

/**
 * Signal handler, called whenever ctrl+c is pressed to exit the tuner
 */
void signalHandler(int signum)
{
    gbKeepGoing = false;
}


/* 
 * This routine will be called by the PortAudio engine when audio is needed.
 * It may be called at interrupt level on some machines so don't do anything
 * that could mess up the system like calling malloc() or free().
 */

static int audioCallback(
    const void *inpBuffer, 
    void *outBuffer,
    unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags,
    void *userData 
    )
{
    SAMPLE * out      = (SAMPLE*) outBuffer;
    SAMPLE * in = (SAMPLE*) inpBuffer;
    int i;

    /* Unused vars */
    (void) timeInfo; 
    (void) statusFlags;
    (void) userData;

    if (inpBuffer == NULL)
    {
        for (i=0; i<framesPerBuffer; i++)
        {
            *out++ = 0;        /* Left channel */
            *out++ = 0;        /* Right channel */
        }
    }
    else
    {
        if (gCurrentEffect > 0 && gCurrentEffect < gMaxEffects)
        {
            effects[gCurrentEffect].effectCb(in, out, framesPerBuffer);
        }
        else
        {
            for (i=0; i<framesPerBuffer; i++)
            {
                *out++ = *in++;   /* Left channel */
                *out++ = *in++;   /* Right channel */
            }
        }
    }

    return paContinue;
}

int main(void)
{
    PaStreamParameters inpParams, outParams;
    PaStream * stream;
    PaError err;
    int     btnStatus;
    mraa_gpio_context   btn;
    upm::Jhd1313m1      *lcd;
    mraa_aio_context    aio;
    uint16_t  actVal, len;
    
    
    mraa_init();
  
    btn = mraa_gpio_init(6);
    aio = mraa_aio_init(0);

    if (btn == NULL) 
    {
        return 1;
    }
    /* Set up the LCD */
    lcd = new upm::Jhd1313m1(0, 0x3E, 0x62);
  
    mraa_gpio_dir(btn, MRAA_GPIO_IN);
    mraa_gpio_isr(btn, MRAA_GPIO_EDGE_RISING, &btnInterrupt, NULL);
    
    gbKeepGoing = true;
    /* register the signal handler */
    signal(SIGINT, signalHandler);
    
    gMaxEffects = sizeof(effects)/sizeof(effects[0]);

    err = Pa_Initialize();
    
    if (err != paNoError) goto done;

    /* Open the input device and set the params */
    inpParams.device = Pa_GetDefaultInputDevice();
    
    if (inpParams.device == paNoDevice)
    {
        fprintf(stderr, "Could not get the input device\n");
        goto done;
    }

    inpParams.channelCount = 2;       /* stereo input */
    inpParams.sampleFormat = PA_SAMPLE_TYPE;
    inpParams.suggestedLatency = Pa_GetDeviceInfo( inpParams.device )->defaultLowInputLatency;
    inpParams.hostApiSpecificStreamInfo = NULL;

    outParams.device = Pa_GetDefaultOutputDevice(); /* default output device */
    if (outParams.device == paNoDevice) 
    {
      fprintf(stderr,"Could not get the output device\n");
      goto done;
    }
    outParams.channelCount = 2;       /* stereo output */
    outParams.sampleFormat = PA_SAMPLE_TYPE;
    outParams.suggestedLatency = Pa_GetDeviceInfo( outParams.device )->defaultLowOutputLatency;
    outParams.hostApiSpecificStreamInfo = NULL;

    err = Pa_OpenStream(
              &stream,
              &inpParams,
              &outParams,
              SAMPLE_RATE,
              FRAMES_PER_BUFFER,
              0, /* paClipOff, */  /* we won't output out of range samples so don't bother clipping them */
              audioCallback,
              NULL );
    if( err != paNoError ) goto done;
    
    err = Pa_StartStream( stream );
    if( err != paNoError ) goto done;
    
    lcd->setCursor(0, 0);
    lcd->write("                ");
    lcd->setCursor(0, 0);
    lcd->write(effects[gCurrentEffect].effectName);
    lcd->setColor(effects[gCurrentEffect].rgb[0], 
        effects[gCurrentEffect].rgb[1],
        effects[gCurrentEffect].rgb[2]);
    
    while(1)
    {
        if (gbKeepGoing == false) break;
        
        if (gCounter > 0 ) 
        {
           
            //mraa_gpio_isr_exit(btn);
            sleep(1);         // debounce
            
            gCurrentEffect = (gCurrentEffect + 1) % gMaxEffects;
            lcd->setCursor(0, 0);
            lcd->write("                ");
            lcd->setCursor(0, 0);
            lcd->write(effects[gCurrentEffect].effectName);
            lcd->setColor(effects[gCurrentEffect].rgb[0], 
                    effects[gCurrentEffect].rgb[1],
                    effects[gCurrentEffect].rgb[2]);
            gCounter = 0;
            
        }

        
        if (effects[gCurrentEffect].effectParams != NULL)
        {
            /* Keep updating the ADC value */
            actVal = mraa_aio_read(aio);
            len = effects[gCurrentEffect].effectParams();
            gActuatorValIdx = MAX_ACTUATOR_VAL/len;
            gActuatorValIdx = actVal/gActuatorValIdx;
            fflush(stdout);
        }
            
        Pa_Sleep(100);
    }
    err = Pa_CloseStream( stream );

    if( err != paNoError ) goto done;

done:
    Pa_Terminate();
    lcd->clear();
    fprintf( stderr, "An error occured while using the portaudio stream\n" );
    fprintf( stderr, "Error number: %d\n", err );
    fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
    return 0;
}

