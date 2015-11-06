                                                  /* Shree Krishnaya Namaha */

/**
 * Contains the configuration defines
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#define SAMPLE_RATE           (44100)

#define PA_SAMPLE_TYPE        paFloat32
#define FRAMES_PER_BUFFER      (128)

typedef float SAMPLE;

#define MAX_ADC_VAL          (1023)           /**< Maximum value of the ADC */

#define MAX_ACTUATOR_VAL      MAX_ADC_VAL     /**< The maximum value for the selected actuator */

#endif  /* __CONFIG_H */
