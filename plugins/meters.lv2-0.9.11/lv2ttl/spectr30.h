// generated by lv2ttl2c from
// http://gareus.org/oss/lv2/meters#spectr30mono

extern const LV2_Descriptor* lv2_descriptor(uint32_t index);
extern const LV2UI_Descriptor* lv2ui_spectr30(uint32_t index);

static const RtkLv2Description _plugin_spectr30 = {
	&lv2_descriptor,
	&lv2ui_spectr30
	, 13 // uint32_t dsp_descriptor_id
	, 0 // uint32_t gui_descriptor_id
	, "1/3 Octave Spectrum Analyzer" // const char *plugin_human_id
	, (const struct LV2Port[66])
	{
		{ "band25", CONTROL_OUT, -100.000000, -100.000000, 6.000000, "25Hz"},
		{ "band31", CONTROL_OUT, nan, -100.000000, 6.000000, "31.5Hz"},
		{ "band40", CONTROL_OUT, nan, -100.000000, 6.000000, "40Hz"},
		{ "band50", CONTROL_OUT, nan, -100.000000, 6.000000, "50Hz"},
		{ "band63", CONTROL_OUT, nan, -100.000000, 6.000000, "63Hz"},
		{ "band80", CONTROL_OUT, nan, -100.000000, 6.000000, "80Hz"},
		{ "band100", CONTROL_OUT, nan, -100.000000, 6.000000, "100Hz"},
		{ "band125", CONTROL_OUT, nan, -100.000000, 6.000000, "125Hz"},
		{ "band160", CONTROL_OUT, nan, -100.000000, 6.000000, "160Hz"},
		{ "band200", CONTROL_OUT, nan, -100.000000, 6.000000, "200Hz"},
		{ "band250", CONTROL_OUT, nan, -100.000000, 6.000000, "250Hz"},
		{ "band315", CONTROL_OUT, nan, -100.000000, 6.000000, "315Hz"},
		{ "band400", CONTROL_OUT, nan, -100.000000, 6.000000, "400Hz"},
		{ "band500", CONTROL_OUT, nan, -100.000000, 6.000000, "500Hz"},
		{ "band630", CONTROL_OUT, nan, -100.000000, 6.000000, "630Hz"},
		{ "band800", CONTROL_OUT, nan, -100.000000, 6.000000, "800Hz"},
		{ "band1000", CONTROL_OUT, nan, -100.000000, 6.000000, "1kHz"},
		{ "band1250", CONTROL_OUT, nan, -100.000000, 6.000000, "1.25kHz"},
		{ "band1600", CONTROL_OUT, nan, -100.000000, 6.000000, "1.6kHz"},
		{ "band2000", CONTROL_OUT, nan, -100.000000, 6.000000, "2kHz"},
		{ "band2500", CONTROL_OUT, nan, -100.000000, 6.000000, "2.5Hz"},
		{ "band3150", CONTROL_OUT, nan, -100.000000, 6.000000, "3.15Hz"},
		{ "band4000", CONTROL_OUT, nan, -100.000000, 6.000000, "4Hz"},
		{ "band5000", CONTROL_OUT, nan, -100.000000, 6.000000, "5kHz"},
		{ "band6300", CONTROL_OUT, nan, -100.000000, 6.000000, "6.3kHz"},
		{ "band8000", CONTROL_OUT, nan, -100.000000, 6.000000, "8Hz"},
		{ "band10000", CONTROL_OUT, nan, -100.000000, 6.000000, "10Hz"},
		{ "band12500", CONTROL_OUT, nan, -100.000000, 6.000000, "12.5Hz"},
		{ "band16000", CONTROL_OUT, nan, -100.000000, 6.000000, "16kHz"},
		{ "band20000", CONTROL_OUT, nan, -100.000000, 6.000000, "20kHz"},
		{ "max25", CONTROL_OUT, nan, -100.000000, 6.000000, "Peak 25Hz"},
		{ "max31", CONTROL_OUT, nan, -100.000000, 6.000000, "Peak 31.5Hz"},
		{ "max40", CONTROL_OUT, nan, -100.000000, 6.000000, "Peak 40Hz"},
		{ "max50", CONTROL_OUT, nan, -100.000000, 6.000000, "Peak 50Hz"},
		{ "max63", CONTROL_OUT, nan, -100.000000, 6.000000, "Peak 63Hz"},
		{ "max80", CONTROL_OUT, nan, -100.000000, 6.000000, "Peak 80Hz"},
		{ "max100", CONTROL_OUT, nan, -100.000000, 6.000000, "Peak 100Hz"},
		{ "max125", CONTROL_OUT, nan, -100.000000, 6.000000, "Peak 125Hz"},
		{ "max160", CONTROL_OUT, nan, -100.000000, 6.000000, "Peak 160Hz"},
		{ "max200", CONTROL_OUT, nan, -100.000000, 6.000000, "Peak 200Hz"},
		{ "max250", CONTROL_OUT, nan, -100.000000, 6.000000, "Peak 250Hz"},
		{ "max315", CONTROL_OUT, nan, -100.000000, 6.000000, "Peak 315Hz"},
		{ "max400", CONTROL_OUT, nan, -100.000000, 6.000000, "Peak 400Hz"},
		{ "max500", CONTROL_OUT, nan, -100.000000, 6.000000, "Peak 500Hz"},
		{ "max630", CONTROL_OUT, nan, -100.000000, 6.000000, "Peak 630Hz"},
		{ "max800", CONTROL_OUT, nan, -100.000000, 6.000000, "Peak 800Hz"},
		{ "max1000", CONTROL_OUT, nan, -100.000000, 6.000000, "Peak 1kHz"},
		{ "max1250", CONTROL_OUT, nan, -100.000000, 6.000000, "Peak 1.25kHz"},
		{ "max1600", CONTROL_OUT, nan, -100.000000, 6.000000, "Peak 1.6kHz"},
		{ "max2000", CONTROL_OUT, nan, -100.000000, 6.000000, "Peak 2kHz"},
		{ "max2500", CONTROL_OUT, nan, -100.000000, 6.000000, "Peak 2.5Hz"},
		{ "max3150", CONTROL_OUT, nan, -100.000000, 6.000000, "Peak 3.15Hz"},
		{ "max4000", CONTROL_OUT, nan, -100.000000, 6.000000, "Peak 4Hz"},
		{ "max5000", CONTROL_OUT, nan, -100.000000, 6.000000, "Peak 5kHz"},
		{ "max6300", CONTROL_OUT, nan, -100.000000, 6.000000, "Peak 6.3kHz"},
		{ "max8000", CONTROL_OUT, nan, -100.000000, 6.000000, "Peak 8Hz"},
		{ "max10000", CONTROL_OUT, nan, -100.000000, 6.000000, "Peak 10Hz"},
		{ "max12500", CONTROL_OUT, nan, -100.000000, 6.000000, "Peak 12.5Hz"},
		{ "max16000", CONTROL_OUT, nan, -100.000000, 6.000000, "Peak 16kHz"},
		{ "max20000", CONTROL_OUT, nan, -100.000000, 6.000000, "Peak 20kHz"},
		{ "UIspeed", CONTROL_IN, 1.000000, 0.020000, 15.000000, "UI speed"},
		{ "UIreset", CONTROL_IN, -4.000000, -4.000000, 4.000000, "UI reset peak"},
		{ "UIgain", CONTROL_IN, 0.000000, -12.000000, 32.000000, "UI gain"},
		{ "UImiscstate", CONTROL_IN, 1.000000, 0.000000, 256.000000, "UI miscstate"},
		{ "in", AUDIO_IN, nan, nan, nan, "In"},
		{ "out", AUDIO_OUT, nan, nan, nan, "Out"},
	}
	, 66 // uint32_t nports_total
	, 1 // uint32_t nports_audio_in
	, 1 // uint32_t nports_audio_out
	, 0 // uint32_t nports_midi_in
	, 0 // uint32_t nports_midi_out
	, 0 // uint32_t nports_atom_in
	, 0 // uint32_t nports_atom_out
	, 64 // uint32_t nports_ctrl
	, 4 // uint32_t nports_ctrl_in
	, 60 // uint32_t nports_ctrl_out
	, 8192 // uint32_t min_atom_bufsiz
	, false // bool send_time_info
	, UINT32_MAX // uint32_t latency_ctrl_port
};

#ifdef X42_PLUGIN_STRUCT
#undef X42_PLUGIN_STRUCT
#endif
#define X42_PLUGIN_STRUCT _plugin_spectr30