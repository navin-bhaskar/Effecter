                                /* Shree Krishnaya Namaha */
                                
#ifndef __EFFECTS_H
#define __EFFECTS_H

#include <inttypes.h>
/** Function pointer type for calling the selected effect */
typedef void(*effect_function)(float *, float *, int);

/** The init function for the effects */
typedef void(*effect_init)(void);

/** Effects params */
typedef uint16_t(*effect_params)(void);

/** Typedef that aggregates the effects structure */
typedef struct effect
{
    effect_function effectCb;
    effect_init effectInit;
    effect_params effectParams;
    const char * effectName;
    uint8_t rgb[3];
}effect;

/* Forward declerations */
/** Does amplitude modulation */
void ampMod(float * in, float *out, int len);
/** Does over drive */
void overDrive(float *in, float *out, int len);
/** Does the reverb effect */
void reverb(float *in, float *out, int len);
/** The ring modulator effect */
void ringMod(float * in, float *out, int len);
/** The sine vibrato */
void sinVibrato(float *in, float *out, int len);
/** Second over drive */
void overDrive2(float *in, float *out, int len);
/** Sawtooth vibrato */
void sawToothVibrato(float *in, float *out, int len);
/** Square wave vibrato */
void squareVibrato(float *in, float *out, int len);
/** Fuzz box effect */
void fuzzBox(float *in, float *out, int len);

/** Init the vibrato */
void initVibrato(void);
/** Init reverb */
void initReverb(void);

/** Len functions */
uint16_t ringModGetParamLen(void);
uint16_t ovrDrive2GetParamLen(void);
uint16_t ampMaxVal(void);
uint16_t reverbGetParamLen(void);
uint16_t vibratoGetParamLen(void);

#endif /* __EFFECTS_H */                                
