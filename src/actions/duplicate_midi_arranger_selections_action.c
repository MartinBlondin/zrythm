/*
 * Copyright (C) 2019 Alexandros Theodotou <alex at zrythm dot org>
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

#include "audio/track.h"
#include "gui/backend/midi_arranger_selections.h"
#include "gui/widgets/center_dock.h"
#include "gui/widgets/midi_arranger.h"
#include "project.h"
#include "actions/duplicate_midi_arranger_selections_action.h"

/**
 * Note: chord addresses are to be copied.
 */
UndoableAction *
duplicate_midi_arranger_selections_action_new (
  long ticks,
  int  delta)
{
  DuplicateMidiArrangerSelectionsAction * self =
    calloc (1, sizeof (
                 DuplicateMidiArrangerSelectionsAction));
  UndoableAction * ua = (UndoableAction *) self;
  ua->type =
    UNDOABLE_ACTION_TYPE_DUPLICATE_MIDI_NOTES;
  self->mas = midi_arranger_selections_clone ();
  self->ticks = ticks;
  self->delta = delta;
  return ua;
}

void
duplicate_midi_arranger_selections_action_do (
  DuplicateMidiArrangerSelectionsAction * self)
{
  MidiNote * mn;
	for (int i = 0; i < self->mas->num_midi_notes; i++)
    {
      mn = self->mas->midi_notes[i];
      midi_region_add_midi_note (
        mn->midi_region,
        mn);
      midi_note_shift (
        mn, self->ticks, self->delta);
    }
  EVENTS_PUSH (ET_MIDI_ARRANGER_SELECTIONS_CHANGED,
               NULL);
}

void
duplicate_midi_arranger_selections_action_undo (
  DuplicateMidiArrangerSelectionsAction * self)
{
  MidiNote * mn;
  for (int i = 0; i < self->mas->num_midi_notes; i++)
    {
      mn = self->mas->midi_notes[i];
      midi_region_remove_midi_note (
        mn->midi_region,
        mn);
    }
  EVENTS_PUSH (ET_MIDI_ARRANGER_SELECTIONS_CHANGED,
               NULL);
}

void
duplicate_midi_arranger_selections_action_free (
  DuplicateMidiArrangerSelectionsAction * self)
{
  midi_arranger_selections_free (self->mas);

  free (self);
}
