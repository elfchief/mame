// license:BSD-3-Clause
// copyright-holders:Mirko Buffoni, James Wallace
#pragma once

#ifndef __OKIM6376_H__
#define __OKIM6376_H__

/* an interface for the OKIM6376 and similar chips (CPU interface only) */

/* struct describing a single playing ADPCM voice */
struct ADPCMVoice
{
	uint8_t playing;          /* 1 if we are actively playing */

	uint32_t base_offset;     /* pointer to the base memory location */
	uint32_t sample;          /* current sample number */
	uint32_t count;           /* total samples to play */

	uint32_t volume;          /* output volume */
	int32_t signal;
	int32_t step;
};

class okim6376_device : public device_t,
									public device_sound_interface
{
public:
	okim6376_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
	~okim6376_device() {}

	DECLARE_WRITE8_MEMBER( write );

	DECLARE_WRITE_LINE_MEMBER( st_w );
	DECLARE_WRITE_LINE_MEMBER( ch2_w );

	DECLARE_READ_LINE_MEMBER( busy_r );
	DECLARE_READ_LINE_MEMBER( nar_r );

	void set_frequency(int frequency);


protected:
	// device-level overrides
	virtual void device_config_complete() override;
	virtual void device_start() override;
	virtual void device_reset() override;

	// sound stream update overrides
	virtual void sound_stream_update(sound_stream &stream, stream_sample_t **inputs, stream_sample_t **outputs, int samples) override;

private:
	// internal state
	required_region_ptr<uint8_t> m_region_base;     /* pointer to the base of the region */

	#define OKIM6376_VOICES     2
	struct ADPCMVoice m_voice[OKIM6376_VOICES];
	int32_t m_command[OKIM6376_VOICES];
	int32_t m_latch;            /* Command data is held before transferring to either channel */
	uint8_t m_stage[OKIM6376_VOICES];/* If a sample is playing, flag that we have a command staged */
	sound_stream *m_stream;   /* which stream are we playing on? */
	uint32_t m_master_clock;    /* master clock frequency */
	uint8_t m_divisor;          /* can be 8,10,16, and is read out of ROM data */
	uint8_t m_channel;
	uint8_t m_nar;              /* Next Address Ready */
	uint8_t m_nartimer;
	uint8_t m_busy;
	uint8_t m_ch2;              /* 2CH pin - enables Channel 2 operation */
	uint8_t m_st;               /* STart */
	uint8_t m_st_pulses;        /* Keep track of attenuation */
	uint8_t m_ch2_update;       /* Pulse shape */
	uint8_t m_st_update;

	void oki_process(int channel, int command);
	void generate_adpcm(struct ADPCMVoice *voice, int16_t *buffer, int samples,int channel);
	void postload();
	void okim6376_state_save_register();
	void adpcm_state_save_register(struct ADPCMVoice *voice, int index);
};

extern const device_type OKIM6376;

#endif /* __OKIM6376_H__ */
