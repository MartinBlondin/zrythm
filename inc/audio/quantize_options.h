/*
 * Copyright (C) 2019 Alexandros Theodotou <alex at zrythm dot org>
 *
 * This file is part of Zrythm
 *
 * Zrythm is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Zrythm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with Zrythm.  If not, see <https://www.gnu.org/licenses/>.
 */

/**
 * \file
 *
 * Quantize options.
 */

#ifndef __AUDIO_QUANTIZE_OPTIONS_H__
#define __AUDIO_QUANTIZE_OPTIONS_H__

#include "audio/position.h"
#include "audio/snap_grid.h"

/**
 * @addtogroup audio
 *
 * @{
 */

#define QUANTIZE_OPTIONS_IS_EDITOR(qo) \
  (&PROJECT->quantize_opts_editor == qo)
#define QUANTIZE_OPTIONS_IS_TIMELINE(qo) \
  (&PROJECT->quantize_opts_timeline == qo)
#define QUANTIZE_OPTIONS_TIMELINE \
  (&PROJECT->quantize_opts_timeline)
#define QUANTIZE_OPTIONS_EDITOR \
  (&PROJECT->quantize_opts_editor)

#define MAX_SNAP_POINTS 120096

typedef struct QuantizeOptions
{
  /** See SnapGrid. */
  NoteLength       note_length;

  /** See SnapGrid. */
  NoteType         note_type;

  /** Percentage to apply quantize (0-100). */
  float            amount;

  /** Adjust start position or not (only applies to
   * objects with length. */
  int              adj_start;

  /** Adjust end position or not (only applies to
   * objects with length. */
  int              adj_end;

  /** Swing amount (0-100). */
  float            swing;

  /** Number of ticks for randomization. */
  unsigned int     rand_ticks;

  /**
   * Quantize points.
   *
   * These only take into account note_length,
   * note_type and swing. They don't take into
   * account the amount % or randomization ticks.
   *
   * Not to be serialized.
   */
  Position         q_points[MAX_SNAP_POINTS];
  int              num_q_points;
} QuantizeOptions;

static const cyaml_schema_field_t
  quantize_options_fields_schema[] =
{
  CYAML_FIELD_ENUM (
    "note_length", CYAML_FLAG_DEFAULT,
    QuantizeOptions, note_length, note_length_strings,
    CYAML_ARRAY_LEN (note_length_strings)),
  CYAML_FIELD_ENUM (
    "note_type", CYAML_FLAG_DEFAULT,
    QuantizeOptions, note_type, note_type_strings,
    CYAML_ARRAY_LEN (note_type_strings)),
  CYAML_FIELD_FLOAT (
    "amount", CYAML_FLAG_DEFAULT,
    QuantizeOptions, amount),
  CYAML_FIELD_INT (
    "adj_start", CYAML_FLAG_DEFAULT,
    QuantizeOptions, adj_start),
  CYAML_FIELD_INT (
    "adj_end", CYAML_FLAG_DEFAULT,
    QuantizeOptions, adj_end),
  CYAML_FIELD_FLOAT (
    "swing", CYAML_FLAG_DEFAULT,
    QuantizeOptions, swing),
  CYAML_FIELD_FLOAT (
    "rand_ticks", CYAML_FLAG_DEFAULT,
    QuantizeOptions, rand_ticks),

	CYAML_FIELD_END
};

static const cyaml_schema_value_t
quantize_options_schema = {
	CYAML_VALUE_MAPPING (
    CYAML_FLAG_POINTER,
	  QuantizeOptions, quantize_options_fields_schema),
};

void
quantize_options_init (
  QuantizeOptions * self,
  NoteLength        note_length);

/**
 * Updates snap points.
 */
void
quantize_options_update_quantize_points (
  QuantizeOptions * self);

float
quantize_options_get_swing (
  QuantizeOptions * self);

float
quantize_options_get_amount (
  QuantizeOptions * self);

float
quantize_options_get_randomization (
  QuantizeOptions * self);

void
quantize_options_set_swing (
  QuantizeOptions * self,
  float             swing);

void
quantize_options_set_amount (
  QuantizeOptions * self,
  float             amount);

void
quantize_options_set_randomization (
  QuantizeOptions * self,
  float             randomization);

/**
 * Returns the grid intensity as a human-readable string.
 *
 * Must be free'd.
 */
char *
quantize_options_stringize (
  NoteLength note_length,
  NoteType   note_type);

/**
 * Clones the QuantizeOptions.
 */
QuantizeOptions *
quantize_options_clone (
  const QuantizeOptions * src);

/**
 * Quantizes the given Position using the given
 * QuantizeOptions.
 *
 * This assumes that the start/end check has been
 * done already and it ignores the adjust_start and
 * adjust_end options.
 *
 * @return The amount of ticks moved (negative for
 *   backwards).
 */
int
quantize_options_quantize_position (
  QuantizeOptions * self,
  Position *              pos);

/**
 * Free's the QuantizeOptions.
 */
void
quantize_options_free (
  QuantizeOptions * self);

/**
 * @}
 */

#endif
