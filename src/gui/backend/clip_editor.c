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
 * Piano roll backend.
 *
 * This is meant to be serialized along with each project.
 */

#include <stdlib.h>

#include "audio/channel.h"
#include "gui/backend/clip_editor.h"
#include "audio/track.h"
#include "project.h"

/**
 * Inits the ClipEditor after a Project is loaded.
 */
void
clip_editor_init_loaded (
  ClipEditor * self)
{
  self->region =
    region_find_by_name (self->region_name);
  piano_roll_init_loaded (&self->piano_roll);
}

/**
 * Sets the region and refreshes the widgets.
 *
 * To be called only from GTK threads.
 */
void
clip_editor_set_region (
  ClipEditor * self,
  Region *     region)
{
  if (self->region && self->region->type ==
        REGION_TYPE_MIDI)
    channel_reattach_midi_editor_manual_press_port (
      track_get_channel (self->region->lane->track),
      0);
  if (region->type == REGION_TYPE_MIDI)
    channel_reattach_midi_editor_manual_press_port (
      track_get_channel (region->lane->track),
      1);
  self->region = region;

  if (self->region_name)
    {
      g_free (self->region_name);
      self->region_name = NULL;
    }
  self->region_name = g_strdup (region->name);

  self->region_changed = 1;

  EVENTS_PUSH (ET_CLIP_EDITOR_REGION_CHANGED,
               NULL);
}

/**
 * Returns the Region that widgets are expected
 * to use.
 */
Region *
clip_editor_get_region_for_widgets (
  ClipEditor * self)
{
  return self->region_cache;
}

void
clip_editor_init (ClipEditor * self)
{
  self->region_name = NULL;
  piano_roll_init (&self->piano_roll);
  audio_clip_editor_init (&self->audio_clip_editor);
  chord_editor_init (&self->chord_editor);
  automation_editor_init (&self->automation_editor);
}

