#include "wm8994_def.h"

#define DEACTIVE		0x00
#define PLAYBACK_ACTIVE		0x01
#define CAPTURE_ACTIVE		0x02
#define CALL_ACTIVE		0x04
#define FMRADIO_ACTIVE		0x08

#define PCM_STREAM_DEACTIVE	0x00
#define PCM_STREAM_PLAYBACK	0x01
#define PCM_STREAM_CAPTURE	0x02

enum playback_path      { PLAYBACK_OFF, RCV, SPK, HP, BT, SPK_HP, RING_SPK, RING_HP, RING_DUAL, EXTRA_DOCK_SPEAKER, TV_OUT, HP_3POLE = 5};
enum mic_path           { MAIN, SUB, BT_REC, MIC_OFF};
enum fmradio_path { FMR_OFF, FMR_SPK, FMR_HP, FMR_SPK_MIX, FMR_HP_MIX, FMR_DUAL_MIX};
enum fmradio_mix_path   { FMR_MIX_OFF, FMR_MIX_HP, FMR_MIX_SPK, FMR_MIX_DUAL};
enum power_state        { CODEC_OFF, CODEC_ON };
enum recognition        {REC_OFF, REC_ON};
enum state{OFF, ON};
enum ringtone_state	{RING_OFF, RING_ON};

struct wm8994_priv {
	//u16 reg_cache[WM8994_REGISTER_COUNT];
	//struct wm8993_platform_data pdata;
	struct snd_soc_codec codec;
	int master;
	int sysclk_source;
	unsigned int mclk_rate;
	unsigned int sysclk_rate;
	unsigned int fs;
	unsigned int bclk;
	int class_w_users;
	unsigned int fll_fref;
	unsigned int fll_fout;
        unsigned int codec_state;
        unsigned int stream_state;
        enum playback_path cur_path;
        enum fmradio_path fmradio_path;
        enum mic_path rec_path;
        enum fmradio_mix_path fmr_mix_path;
        enum power_state power_state;
        enum state recognition_active;          // for control gain to voice recognition. 
        enum state ringtone_active;      
};
