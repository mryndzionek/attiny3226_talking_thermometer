#ifndef __SPEECH_SYNTH__
#define __SPEECH_SYNTH__

#define LPC_NUM (29)
#define LPC_ORDER (8)
#define LPC_FRAME_LEN (220)
#define LPC_SAMPLE_RATE (3800)
#define LPC_DEEMPHASIS_FACTOR (0x00364)

typedef enum {
    lpc_name__talking__digital__thermometer_starting = 0,
    lpc_name_temperature = 1,
    lpc_name_minus = 2,
    lpc_name_point = 3,
    lpc_name_out_of_range = 4,
    lpc_name_zero = 5,
    lpc_name_one = 6,
    lpc_name_two = 7,
    lpc_name_three = 8,
    lpc_name_four = 9,
    lpc_name_five = 10,
    lpc_name_six = 11,
    lpc_name_seven = 12,
    lpc_name_eight = 13,
    lpc_name_nine = 14,
    lpc_name_ten = 15,
    lpc_name_eleven = 16,
    lpc_name_twelve = 17,
    lpc_name_thirteen = 18,
    lpc_name_fourteen = 19,
    lpc_name_fifteen = 20,
    lpc_name_sixteen = 21,
    lpc_name_seventeen = 22,
    lpc_name_eighteen = 23,
    lpc_name_nineteen = 24,
    lpc_name_twenty = 25,
    lpc_name_thirty = 26,
    lpc_name_forty = 27,
    lpc_name_fifty = 28,
    lpc_name_max,
} lpc_name_e;

// TODO PB3 is only supported for now
#define SPEECH_SYNTH_PWM_PIN PIN3_bm

typedef struct _speech_synth_t speech_synth_t;

speech_synth_t *speech_synth_init(void);
void speech_synth_say(speech_synth_t *self, lpc_name_e phrase_id);
void speech_synth_tone(speech_synth_t *self);

#endif // __SPEECH_SYNTH__
