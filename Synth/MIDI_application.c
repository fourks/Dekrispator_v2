/*
 * MIDI_application.c
 *
 *  Created on: 6 d�c. 2014
 *      Author: Xavier Halgand
 */


/* Includes ------------------------------------------------------------------*/

//#include "main.h"
#include "MIDI_application.h"




/* Private define ------------------------------------------------------------*/

#define RX_BUFF_SIZE   64  /* Max Received data 64 bytes */


uint8_t MIDI_RX_Buffer[RX_BUFF_SIZE]; // MIDI reception buffer

/* Private function prototypes -----------------------------------------------*/
void ProcessReceivedMidiDatas(void);


/*-----------------------------------------------------------------------------*/
/**
 * @brief  Main routine for MIDI application, looped in main.c
 * @param  None
 * @retval none
 */
void MIDI_Application(void)
{
	if(Appli_state == APPLICATION_READY)
	{
		USBH_MIDI_Receive(&hUSBHost, MIDI_RX_Buffer, RX_BUFF_SIZE); // just once at the beginning, start the first reception
		Appli_state = APPLICATION_RUNNING;
	}
	if(Appli_state == APPLICATION_RUNNING)
	{
			//....pffff......grrrrr......
	}

	if(Appli_state == APPLICATION_DISCONNECT)
	{
		Appli_state = APPLICATION_IDLE;
		USBH_MIDI_Stop(&hUSBHost);
	}

}

/*-----------------------------------------------------------------------------*/
void MagicFX(uint8_t val) /* random effects parameters */
{
	if (val == MIDI_MAX)
	{
		Delay_switch(MIDI_MAXi);
		Delay_time_set(MIDIrandVal());
		DelayWet_set(MIDIrandVal());
		DelayFeedback_set(MIDIrandVal());

		Chorus_switch(MIDI_MAXi);
		ChorusRate_set(MIDIrandVal());
		ChorusSecondRate_set(MIDIrandVal());
		ChorusDelay_set(MIDIrandVal());
		ChorusSweep_set(MIDIrandVal());
		ChorusFeedback_set(MIDIrandVal());
		ChorusMode_switch(MIDIrandVal());
		ChorusFDBsign_switch(MIDIrandVal());

		Phaser_switch(MIDI_MAXi);
		Phaser_Rate_set(MIDIrandVal());
		Phaser_Feedback_set(MIDIrandVal());
		Phaser_Wet_set(MIDIrandVal());
	}
}
/*-----------------------------------------------------------------------------*/
void MagicPatch(uint8_t val) /* random sound parameters */
{
	if (val == MIDI_MAX)
	{
		seq_tempo_set(MIDIrandVal());
		seq_freqMax_set(MIDIrandVal());
		seq_scale_set(MIDIrandVal());
		seq_switchMovingSeq(MIDIrandVal());
		seq_switchMute(MIDIrandVal());
		seq_gateTime_set(MIDIrandVal());
		autoSound_set(rand()%3);
		Sound_set(MIDIrandVal());
		uint8_t snd = soundNumber_get();

		if (snd == FM2)
		{
			//STM_EVAL_LEDOn(LED_Blue);
			FM_OP1_freq_set(MIDIrandVal());
			FM_OP1_modInd_set(MIDIrandVal());
			FM_OP2_freq_set(MIDIrandVal());
			FM_OP2_modInd_set(MIDIrandVal());
			FM_OP3_freq_set(MIDIrandVal());
			FM_OP3_modInd_set(MIDIrandVal());
			FM_OP4_freq_set(MIDIrandVal());
			FM_OP4_modInd_set(MIDIrandVal());
		}
		else if (snd == DRIFTERS)
		{
			//STM_EVAL_LEDOn(LED_Green);
			Drifter_amp_set(MIDIrandVal());
			Drifter_minFreq_set(MIDIrandVal());
			Drifter_maxFreq_set(MIDIrandVal());
			Drifter_centralFreq_set(MIDIrandVal());
		}

		Filter1Freq_set(MIDIrandVal());
		Filter1Res_set(MIDIrandVal());
		Filter1Drive_set(MIDIrandVal());
		Filter1Type_set(MIDIrandVal());
		Filt1LFO_amp_set(MIDIrandVal());
		Filt1LFO_freq_set(MIDIrandVal());

		Filter2Freq_set(MIDIrandVal());
		Filter2Res_set(MIDIrandVal());
		Filter2Drive_set(MIDIrandVal());
		Filter2Type_set(MIDIrandVal());
		Filt2LFO_amp_set(MIDIrandVal());
		Filt2LFO_freq_set(MIDIrandVal());

		Filter_Random_switch(MIDIrandVal());

		AttTime_set((uint8_t)lrintf(frand_a_b(0 , MIDI_MAX/10)));
		DecTime_set(MIDIrandVal());
		SustLevel_set(MIDIrandVal());
		RelTime_set(MIDIrandVal());

		VibratoAmp_set(MIDIrandVal());
		VibratoFreq_set(MIDIrandVal());

		AmpLFO_amp_set(MIDIrandVal());
		AmpLFO_freq_set(MIDIrandVal());

	}
}

/*-----------------------------------------------------------------------------*/
void ProcessReceivedMidiDatas(void)
{
	uint16_t numberOfPackets;
	uint8_t *ptr = MIDI_RX_Buffer;
	midi_package_t pack;

	numberOfPackets = USBH_MIDI_GetLastReceivedDataSize(&hUSBHost) / 4; //each USB midi package is 4 bytes long
	if (numberOfPackets != 0) // seems useless...
	{
		BSP_LED_Toggle(LED_Blue);

		while(numberOfPackets--)
		{
			pack.cin_cable = *ptr ; ptr++ ;
			pack.evnt0 = *ptr ; ptr++ ;
			pack.evnt1 = *ptr ; ptr++ ;
			pack.evnt2 = *ptr ; ptr++ ;

			if ((pack.evnt0 & 0xF0) == 0xB0) /* If the midi message is a Control Change... */
			{
				uint8_t val = pack.evnt2;

				switch(pack.evnt1) // CC number
				{
				case 3 	: 	seq_tempo_set(val); 		break ;	// tempo
				case 13 : 	Volume_set(val); 			break;	// master volume
				case 31 :	SynthOut_switch(val); 		break;  // toggle synth output
				case 4  : 	seq_freqMax_set(val);		break;	// max frequency

				case 67 :	DemoMode_toggle(val);		break;
				//case 76 :	DemoMode_freeze(val);		break;

				case 39 :	MagicPatch(val);			break;	// random settings except effects
				case 38 :	MagicFX(val);				break;	// random settings for effects
				case 82 :	MagicPatch(val);			break;	//
				case 81 :	MagicFX(val);				break;	//

				case 5 :	seq_scale_set(val);			break; 	// scale
				case 6 :	Sound_set(val);				break;	// waveform
				case 76 :	RandSound1(val);			break;
				case 77 :	RandSound2(val);			break;

				case 8 :	Filter1Freq_set(val);		break;	//
				case 9 :	Filter1Res_set(val);		break;	//
				case 53 :	Filter1Freq_set(val);		break;	//
				case 54 :	Filter1Res_set(val);		break;	//
				case 12 :	Filter1Drive_set(val);		break;	//
				case 55 :	Filter1Drive_set(val);		break;	//
				case 56 :	Filter1Type_set(val);		break;	//

				case 23 :	Delay_switch(val);			break;	// Delay ON/OFF
				case 14 :	Delay_time_set(val);		break;	// Delay time
				case 15 :	DelayFeedback_set(val);		break;	// Delay feedback
				case 2 :	DelayWet_set(val);			break;	// Delay wet signal amplitude
				case 40 :	Delay_time_dec(val);		break;
				case 41 :	Delay_time_inc(val);		break;

				case 16 :	VibratoAmp_set(val);		break;	// Vibrato amplitude
				case 17 :	VibratoFreq_set(val);		break;	// Vibrato frequency

				case 27 :	Filter_Random_switch(val);	break;	// Filter ON/OFF
				case 63 :	SynthOut_amp_set(val);		break;	// Distorsion

				case 28 :	Chorus_switch(val);			break;	// Chorus ON/OFF
				case 37 :	Chorus_reset(val);			break;
				case 18 :	ChorusRate_set(val);		break;	// Chorus rate
				case 22 :	ChorusSecondRate_set(val);	break;	// Chorus relative rate for LFO right
				case 19 :	ChorusDelay_set(val);		break;	// Chorus delay
				case 20 :	ChorusSweep_set(val);		break;	// Chorus sweep
				case 21 :	ChorusFeedback_set(val);	break;	// Chorus feedback
				case 29 :	ChorusMode_switch(val);		break;	// Chorus mode
				case 30 :	ChorusFDBsign_switch(val);	break;	// Chorus fdb sign

				case 24 :	Phaser_switch(val);			break;
				case 102 :	Phaser_Rate_set(val);		break;
				case 103 :	Phaser_Feedback_set(val);	break;
				case 89 :	Phaser_Wet_set(val);		break;

				case 26 :	seq_switchMovingSeq(val);	break;	// toggle sequence mode
				//case 24 :	seq_switchGlide(val);		break; 	// toggle glissando
				case 25 :	seq_switchMute(val);		break;	// toggle muted notes

				case 33 :	seq_transp(-2, val);		break;	// transpose 1 tone down
				case 34 :	seq_transp(2, val);			break;	// transpose 1 tone up
				case 35 :	seq_transp(-7, val);		break;	//
				case 36 :	seq_transp( 7, val);		break;	//

				case 42 :	seq_gateTime_set(val);		break;	//

				case 43 :	AttTime_set(val);			break;	//
				case 50 :	DecTime_set(val);			break;	//
				case 51 :	SustLevel_set(val);			break;	//
				case 52 :	RelTime_set(val);			break;	//

				case 65 :	Filt1LFO_amp_set(val);		break;	//
				case 66 :	Filt1LFO_freq_set(val);		break;	//

				case 57 :	AmpLFO_amp_set(val);		break;	//
				case 58 :	AmpLFO_freq_set(val);		break;	//

				case 90 :	Filter2Freq_set(val);		break;	//
				case 91 :	Filter2Res_set(val);		break;	//
				case 92 :	Filter2Drive_set(val);		break;	//
				case 93 :	Filter2Type_set(val);		break;	//

				case 105 :	Filt2LFO_amp_set(val);		break;	//
				case 106 :	Filt2LFO_freq_set(val);		break;	//

				case 85 : 	FM_OP1_freq_set(val);		break;
				case 94 :	FM_OP1_modInd_set(val);		break;

				case 86 : 	FM_OP2_freq_set(val);		break;
				case 95 :	FM_OP2_modInd_set(val);		break;

				case 108 : 	FM_OP2_freqMul_inc(val);	break;
				case 117 :	FM_OP2_freqMul_dec(val);	break;

				case 87 : 	FM_OP3_freq_set(val);		break;
				case 96 :	FM_OP3_modInd_set(val);		break;

				case 109 : 	FM_OP3_freqMul_inc(val);	break;
				case 118 :	FM_OP3_freqMul_dec(val);	break;

				case 88 : 	FM_OP4_freq_set(val);		break;
				case 97 :	FM_OP4_modInd_set(val);		break;

				case 110 : 	FM_OP4_freqMul_inc(val);	break;
				case 119 :	FM_OP4_freqMul_dec(val);	break;

				case 59 : 	Drifter_amp_set(val);		break;
				case 60 : 	Drifter_minFreq_set(val);	break;
				case 61 : 	Drifter_maxFreq_set(val);	break;
				case 62 : 	Drifter_centralFreq_set(val);	break;
				}
			}
		}
	}
}

/*-----------------------------------------------------------------------------*/
/**
 * @brief  MIDI data receive callback.
 * @param  phost: Host handle
 * @retval None
 */
void USBH_MIDI_ReceiveCallback(USBH_HandleTypeDef *phost)
{
	ProcessReceivedMidiDatas();

	USBH_MIDI_Receive(&hUSBHost, MIDI_RX_Buffer, RX_BUFF_SIZE); // start a new reception
}
/*-----------------------------------------------------------------------------*/