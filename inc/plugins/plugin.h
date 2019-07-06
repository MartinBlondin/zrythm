/*
 * Copyright (C) 2018-2019 Alexandros Theodotou <alex at zrythm dot org>
 *
 * This file is part of Zrythm
 *
 * Zrythm is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Zrythm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Zrythm.  If not, see <https://www.gnu.org/licenses/>.
 */

/**
 * @defgroup plugins Plugins
 *
 * Code related to audio Plugins (LV2).
 *
 * @{
 */

#ifndef __PLUGINS_BASE_PLUGIN_H__
#define __PLUGINS_BASE_PLUGIN_H__

#include "audio/automatable.h"
#include "audio/port.h"
#include "plugins/lv2_plugin.h"

/* FIXME allocate dynamically */
#define MAX_IN_PORTS 400000
#define MAX_OUT_PORTS 14000
#define MAX_UNKNOWN_PORTS 4000

#define DUMMY_PLUGIN "Dummy Plugin"

#define IS_PLUGIN_CATEGORY(p, c) \
  (g_strcmp0 (((Plugin *)p)->descr->category, c) == 0)
#define IS_PLUGIN_DESCR_CATEGORY(d,c) \
  (g_strcmp0 (d->category,c) == 0)

typedef struct Channel Channel;

typedef enum PluginCategory
{
  /* None specified */
  PC_NONE,
  PC_DELAY,
  PC_REVERB,
  PC_DISTORTION,
  PC_WAVESHAPER,
  PC_DYNAMICS,
  PC_AMPLIFIER,
  PC_COMPRESSOR,
  PC_ENVELOPE,
  PC_EXPANDER,
  PC_GATE,
  PC_LIMITER,
  PC_FILTER,
  PC_ALLPASS_FILTER,
  PC_BANDPASS_FILTER,
  PC_COMB_FILTER,
  PC_EQ,
  PC_MULTI_EQ,
  PC_PARA_EQ,
  PC_HIGHPASS_FILTER,
  PC_LOWPASS_FILTER,
  PC_GENERATOR,
  PC_CONSTANT,
  PC_INSTRUMENT,
  PC_OSCILLATOR,
  PC_MIDI,
  PC_MODULATOR,
  PC_CHORUS,
  PC_FLANGER,
  PC_PHASER,
  PC_SIMULATOR,
  PC_SIMULATOR_REVERB,
  PC_SPATIAL,
  PC_SPECTRAL,
  PC_PITCH,
  PC_UTILITY,
  PC_ANALYZER,
  PC_CONVERTER,
  PC_FUNCTION,
  PC_MIXER,
} PluginCategory;

typedef enum PluginProtocol
{
 PROT_LV2,
 PROT_DSSI,
 PROT_LADSPA,
 PROT_VST,
 PROT_VST3
} PluginProtocol;

typedef enum PluginArchitecture
{
  ARCH_32,
  ARCH_64
} PluginArchitecture;

/***
 * A descriptor to be implemented by all plugins
 * This will be used throughout the UI
 */
typedef struct PluginDescriptor
{
  char                 * author;
  char                 * name;
  char                 * website;
  PluginCategory   category;
  /** Lv2 plugin subcategory. */
  char                 * category_str;
  /** Number of audio input ports. */
  int              num_audio_ins;
  /** Number of MIDI input ports. */
  int              num_midi_ins;
  /** Number of audio output ports. */
  int              num_audio_outs;
  /** Number of MIDI output ports. */
  int              num_midi_outs;
  /** Number of input control (plugin param)
   * ports. */
  int              num_ctrl_ins;
  /** Number of output control (plugin param)
   * ports. */
  int              num_ctrl_outs;
  /** Number of input CV ports. */
  int              num_cv_ins;
  /** Number of output CV ports. */
  int              num_cv_outs;
  /** Architecture (32/64bit). */
  PluginArchitecture   arch;
  /** Plugin protocol (Lv2/DSSI/LADSPA/VST...). */
  PluginProtocol       protocol;
  /** Path, if not an Lv2Plugin which uses URIs. */
  char                 * path;
  /** Lv2Plugin URI. */
  char                 * uri;
} PluginDescriptor;

/**
 * The base plugin
 * Inheriting plugins must have this as a child
 */
typedef struct Plugin
{
  /**
   * Pointer back to plugin in its original format.
   */
  Lv2Plugin *          lv2;

  /** Descriptor. */
  PluginDescriptor *   descr;

  /** Ports coming in as input. */
  PortIdentifier      in_port_ids[MAX_IN_PORTS];
  Port *              in_ports[MAX_IN_PORTS]; ///< cache
  int                  num_in_ports;    ///< counter

  /** Outgoing ports. */
  PortIdentifier      out_port_ids[MAX_OUT_PORTS];
  Port *              out_ports[MAX_OUT_PORTS];
  int                 num_out_ports;    ///< counter

  /** Ports with unknown direction (not used). */
  PortIdentifier      unknown_port_ids[MAX_UNKNOWN_PORTS];
  Port *              unknown_ports[MAX_UNKNOWN_PORTS];
  int                 num_unknown_ports;

  /** The Channel this plugin belongs to. */
  Track              * track;
  int                  track_pos;

  /**
   * The slot this plugin is at in its channel.
   */
  int                  slot;

  /** Enabled or not. */
  int                  enabled;

  /** Plugin automatables. */
  Automatable **       automatables;
  int                  num_automatables;

  /** Whether plugin UI is opened or not. */
  int                  visible;

  /**
   * UI has been instantiated or not.
   *
   * When instantiating a plugin UI, if it takes
   * too long there is a UI buffer overflow because
   * UI updates are sent in lv2_plugin_process.
   *
   * This should be set to 0 until the plugin UI
   * has finished instantiating, and if this is 0
   * then no UI updates should be sent to the
   * plugin.
   */
  int                  ui_instantiated;

  /** Plugin is in deletion. */
  int                  deleting;
} Plugin;

static const cyaml_strval_t
plugin_protocol_strings[] =
{
	{ "LV2",          PROT_LV2    },
	{ "DSSI",         PROT_DSSI   },
	{ "LADSPA",       PROT_LADSPA },
	{ "VST",          PROT_VST    },
	{ "VST3",         PROT_VST3   },
};

static const cyaml_strval_t
plugin_architecture_strings[] =
{
	{ "32-bit",       ARCH_32     },
	{ "64-bit",       ARCH_64     },
};

static const cyaml_schema_field_t
descriptor_fields_schema[] =
{
  CYAML_FIELD_STRING_PTR (
    "author", CYAML_FLAG_POINTER,
    PluginDescriptor, author,
   	0, CYAML_UNLIMITED),
  CYAML_FIELD_STRING_PTR (
    "name", CYAML_FLAG_POINTER,
    PluginDescriptor, name,
   	0, CYAML_UNLIMITED),
  CYAML_FIELD_STRING_PTR (
    "website", CYAML_FLAG_POINTER | CYAML_FLAG_OPTIONAL,
    PluginDescriptor, website,
   	0, CYAML_UNLIMITED),
  CYAML_FIELD_STRING_PTR (
    "category_str", CYAML_FLAG_POINTER,
    PluginDescriptor, category_str,
   	0, CYAML_UNLIMITED),
	CYAML_FIELD_INT (
    "num_audio_ins", CYAML_FLAG_DEFAULT,
	  PluginDescriptor, num_audio_ins),
	CYAML_FIELD_INT (
    "num_audio_outs", CYAML_FLAG_DEFAULT,
	  PluginDescriptor, num_audio_outs),
	CYAML_FIELD_INT (
    "num_midi_ins", CYAML_FLAG_DEFAULT,
	  PluginDescriptor, num_midi_ins),
	CYAML_FIELD_INT (
    "num_midi_outs", CYAML_FLAG_DEFAULT,
	  PluginDescriptor, num_midi_outs),
	CYAML_FIELD_INT (
    "num_ctrl_ins", CYAML_FLAG_DEFAULT,
	  PluginDescriptor, num_ctrl_ins),
	CYAML_FIELD_INT (
    "num_ctrl_outs", CYAML_FLAG_DEFAULT,
	  PluginDescriptor, num_audio_outs),
  CYAML_FIELD_ENUM (
    "arch", CYAML_FLAG_DEFAULT,
    PluginDescriptor, arch, plugin_architecture_strings,
    CYAML_ARRAY_LEN (plugin_architecture_strings)),
  CYAML_FIELD_ENUM (
    "protocol", CYAML_FLAG_DEFAULT,
    PluginDescriptor, arch, plugin_protocol_strings,
    CYAML_ARRAY_LEN (plugin_protocol_strings)),
  CYAML_FIELD_STRING_PTR (
    "path", CYAML_FLAG_POINTER | CYAML_FLAG_OPTIONAL,
    PluginDescriptor, path,
   	0, CYAML_UNLIMITED),
  CYAML_FIELD_STRING_PTR (
    "uri", CYAML_FLAG_POINTER,
    PluginDescriptor, uri,
   	0, CYAML_UNLIMITED),

	CYAML_FIELD_END
};

static const cyaml_schema_field_t
plugin_fields_schema[] =
{
  CYAML_FIELD_MAPPING_PTR (
    "descr", CYAML_FLAG_POINTER,
    Plugin, descr,
    descriptor_fields_schema),
  CYAML_FIELD_MAPPING_PTR (
    "lv2", CYAML_FLAG_POINTER,
    Plugin, lv2,
    lv2_plugin_fields_schema),
  CYAML_FIELD_SEQUENCE_COUNT (
    "in_port_ids", CYAML_FLAG_DEFAULT,
    Plugin, in_port_ids, num_in_ports,
    &port_identifier_schema_default, 0, CYAML_UNLIMITED),
  CYAML_FIELD_SEQUENCE_COUNT (
    "out_port_ids", CYAML_FLAG_DEFAULT,
    Plugin, out_port_ids, num_out_ports,
    &port_identifier_schema_default, 0, CYAML_UNLIMITED),
  CYAML_FIELD_SEQUENCE_COUNT (
    "unknown_port_ids", CYAML_FLAG_DEFAULT,
    Plugin, unknown_port_ids, num_unknown_ports,
    &port_identifier_schema_default, 0, CYAML_UNLIMITED),
  CYAML_FIELD_INT (
    "enabled", CYAML_FLAG_DEFAULT,
    Plugin, enabled),
	CYAML_FIELD_INT (
    "visible", CYAML_FLAG_DEFAULT,
    Plugin, visible),

	CYAML_FIELD_END
};

static const cyaml_schema_value_t
plugin_schema =
{
	CYAML_VALUE_MAPPING (
    CYAML_FLAG_POINTER | CYAML_FLAG_OPTIONAL,
	  Plugin, plugin_fields_schema),
};

void
plugin_init_loaded (Plugin * plgn);

/**
 * Adds an Automatable to the Plugin.
 */
void
plugin_add_automatable (
  Plugin * pl,
  Automatable * a);

/**
 * Creates/initializes a plugin and its internal
 * plugin (LV2, etc.)
 * using the given descriptor.
 */
Plugin *
plugin_new_from_descr (
  PluginDescriptor * descr);

/**
 * Clones the plugin descriptor.
 */
void
plugin_clone_descr (
  PluginDescriptor * src,
  PluginDescriptor * dest);

/**
 * Clones the given plugin.
 */
Plugin *
plugin_clone (
  Plugin * pl);

/**
 * Returns if the Plugin is an instrument or not.
 */
int
plugin_descriptor_is_instrument (
  PluginDescriptor * descr);

/**
 * Returns if the Plugin is an effect or not.
 */
int
plugin_descriptor_is_effect (
  PluginDescriptor * descr);

/**
 * Returns if the Plugin is a modulator or not.
 */
int
plugin_descriptor_is_modulator (
  PluginDescriptor * descr);

/**
 * Returns if the Plugin is a midi modifier or not.
 */
int
plugin_descriptor_is_midi_modifier (
  PluginDescriptor * descr);

/**
 * Returns the PluginCategory matching the given
 * string.
 */
PluginCategory
plugin_descriptor_string_to_category (
  const char * str);

/**
 * Moves the Plugin's automation from one Channel
 * to another.
 */
void
plugin_move_automation (
  Plugin *  pl,
  Channel * prev_ch,
  Channel * ch);

/**
 * Generates automatables for the plugin.
 *
 *
 * Plugin must be instantiated already.
 */
void
plugin_generate_automatables (Plugin * plugin);

/**
 * Loads the plugin from its state file.
 */
//void
//plugin_load (Plugin * plugin);

/**
 * Instantiates the plugin (e.g. when adding to a
 * channel)
 */
int
plugin_instantiate (Plugin * plugin);

/**
 * Sets the track and track_pos on the plugin.
 */
void
plugin_set_track (
  Plugin * pl,
  Track * tr);

/**
 * Process plugin
 */
void
plugin_process (Plugin * plugin);

/**
 * Process show ui
 */
void
plugin_open_ui (Plugin *plugin);

/**
 * Returns if Plugin exists in MixerSelections.
 */
int
plugin_is_selected (
  Plugin * pl);

/**
 * Process hide ui
 */
void
plugin_close_ui (Plugin *plugin);
/**
 * (re)Generates automatables for the plugin.
 */
void
plugin_update_automatables (Plugin * plugin);

/**
 * To be called immediately when a channel or plugin
 * is deleted.
 *
 * A call to plugin_free can be made at any point
 * later just to free the resources.
 */
void
plugin_disconnect (Plugin * plugin);

/**
 * Frees given plugin, breaks all its port connections, and frees its ports
 * and other internal pointers
 */
void
plugin_free (Plugin *plugin);

/**
 * @}
 */

#endif
