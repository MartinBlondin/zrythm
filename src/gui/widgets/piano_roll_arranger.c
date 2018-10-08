/*
 * gui/widgets/piano_roll_arranger.c - The piano_roll_arranger containing regions
 *
 * Copyright (C) 2018 Alexandros Theodotou
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

#include "zrythm_app.h"
#include "project.h"
#include "settings_manager.h"
#include "gui/widgets/region.h"
#include "audio/channel.h"
#include "audio/mixer.h"
#include "audio/track.h"
#include "audio/transport.h"
#include "gui/widgets/main_window.h"
#include "gui/widgets/midi_editor.h"
#include "gui/widgets/ruler.h"
#include "gui/widgets/piano_roll_arranger.h"
/*#include "gui/widgets/piano_roll_arranger_bg.h"*/
#include "gui/widgets/piano_roll_labels.h"
#include "gui/widgets/tracks.h"
#include "utils/ui.h"

#include <gtk/gtk.h>

G_DEFINE_TYPE (PianoRollArrangerWidget, piano_roll_arranger_widget, GTK_TYPE_OVERLAY)

#define MW_RULER MAIN_WINDOW->ruler


/**
 * Gets called to set the position/size of each overlayed widget.
 */
static gboolean
get_child_position (GtkOverlay   *overlay,
               GtkWidget    *widget,
               GdkRectangle *allocation,
               gpointer      user_data)
{
  /*if (IS_MIDI_NOTE_WIDGET (widget))*/
    /*{*/
      /*MidiNoteWidget * mnw = MIDI_NOTE_WIDGET (widget);*/

      /*gint wx, wy;*/
      /*gtk_widget_translate_coordinates(*/
                /*GTK_WIDGET (mnw->note->track->widget),*/
                /*GTK_WIDGET (overlay),*/
                /*0,*/
                /*0,*/
                /*&wx,*/
                /*&wy);*/

      /*allocation->x =*/
        /*(rw->region->start_pos.bars - 1) * MW_RULER->px_per_bar +*/
        /*(rw->region->start_pos.beats - 1) * MW_RULER->px_per_beat +*/
        /*(rw->region->start_pos.quarter_beats - 1) * MW_RULER->px_per_quarter_beat +*/
        /*rw->region->start_pos.ticks * MW_RULER->px_per_tick +*/
        /*SPACE_BEFORE_START;*/
      /*allocation->y = wy;*/
      /*allocation->width =*/
        /*((rw->region->end_pos.bars - 1) * MW_RULER->px_per_bar +*/
        /*(rw->region->end_pos.beats - 1) * MW_RULER->px_per_beat +*/
        /*(rw->region->end_pos.quarter_beats - 1) * MW_RULER->px_per_quarter_beat +*/
        /*rw->region->end_pos.ticks * MW_RULER->px_per_tick +*/
        /*SPACE_BEFORE_START) - allocation->x;*/
      /*gint w, h;*/
      /*gtk_widget_get_size_request (GTK_WIDGET (rw->region->track->widget),*/
                             /*&w,*/
                             /*&h);*/
      /*allocation->height = gtk_widget_get_allocated_height (GTK_WIDGET (rw->region->track->widget));*/
    /*}*/

  return TRUE;
}

/*static RegionWidget **/
/*get_hit_region (double x, double y)*/
/*{*/
  /*GList *children, *iter;*/

  /*[> go through each overlay child <]*/
  /*children = gtk_container_get_children(GTK_CONTAINER(MAIN_WINDOW->piano_roll_arranger));*/
  /*for(iter = children; iter != NULL; iter = g_list_next(iter))*/
    /*{*/
      /*GtkWidget * widget = GTK_WIDGET (iter->data);*/

      /*[> if region <]*/
      /*if (IS_REGION_WIDGET (widget))*/
        /*{*/
          /*GtkAllocation allocation;*/
          /*gtk_widget_get_allocation (widget,*/
                                     /*&allocation);*/

          /*gint wx, wy;*/
          /*gtk_widget_translate_coordinates(*/
                    /*GTK_WIDGET (widget),*/
                    /*GTK_WIDGET (MAIN_WINDOW->piano_roll_arranger),*/
                    /*0,*/
                    /*0,*/
                    /*&wx,*/
                    /*&wy);*/

          /*[> if hit <]*/
          /*if (x >= wx &&*/
              /*x <= (wx + allocation.width) &&*/
              /*y >= wy &&*/
              /*y <= (wy + allocation.height))*/
            /*{*/
              /*return REGION_WIDGET (widget);*/
            /*}*/
        /*}*/
    /*}*/
  /*return NULL;*/
/*}*/


static gboolean
multipress_pressed (GtkGestureMultiPress *gesture,
               gint                  n_press,
               gdouble               x,
               gdouble               y,
               gpointer              user_data)
{
  /*PianoRollArrangerWidget * self = (PianoRollArrangerWidget *) user_data;*/
  /*gtk_widget_grab_focus (GTK_WIDGET (self));*/

  /*[> open MIDI editor if double clicked on a region <]*/
  /*RegionWidget * region_widget = get_hit_region (x, y);*/
  /*if (region_widget && n_press == 2)*/
    /*{*/
      /*midi_editor_set_region (region_widget->region);*/
    /*}*/
}

static void
drag_begin (GtkGestureDrag * gesture,
               gdouble         start_x,
               gdouble         start_y,
               gpointer        user_data)
{
  /*PianoRollArrangerWidget * self = (PianoRollArrangerWidget *) user_data;*/
  /*self->start_x = start_x;*/

  /*RegionWidget * region_widget = get_hit_region (start_x, start_y);*/
  /*if (region_widget)*/
    /*{*/
      /*self->region = region_widget->region;*/

      /*switch (region_widget->hover_state)*/
        /*{*/
        /*case REGION_HOVER_STATE_NONE:*/
          /*g_warning ("hitting region but region hover state is none, should be fixed");*/
          /*break;*/
        /*case REGION_HOVER_STATE_EDGE_L:*/
          /*self->action = PIANO_ROLL_ARRANGER_WIDGET_ACTION_RESIZING_REGION_L;*/
          /*break;*/
        /*case REGION_HOVER_STATE_EDGE_R:*/
          /*self->action = PIANO_ROLL_ARRANGER_WIDGET_ACTION_RESIZING_REGION_R;*/
          /*break;*/
        /*case REGION_HOVER_STATE_MIDDLE:*/
          /*self->action = PIANO_ROLL_ARRANGER_WIDGET_ACTION_MOVING_REGION;*/
          /*ui_set_cursor (GTK_WIDGET (region_widget), "grabbing");*/
          /*break;*/
        /*}*/
    /*}*/
}

static void
drag_update (GtkGestureDrag * gesture,
               gdouble         offset_x,
               gdouble         offset_y,
               gpointer        user_data)
{
  PianoRollArrangerWidget * self = PIANO_ROLL_ARRANGER_WIDGET (user_data);

  /*if (self->action == PIANO_ROLL_ARRANGER_WIDGET_ACTION_RESIZING_REGION_L)*/
    /*{*/
      /*Position pos;*/
      /*ruler_widget_px_to_pos (&pos,*/
                 /*(self->start_x + offset_x) - SPACE_BEFORE_START);*/
      /*region_set_start_pos (self->region,*/
                            /*&pos);*/
      /*gtk_widget_queue_allocate (GTK_WIDGET (self));*/
    /*}*/
  /*else if (self->action == PIANO_ROLL_ARRANGER_WIDGET_ACTION_RESIZING_REGION_R)*/
    /*{*/
      /*Position pos;*/
      /*ruler_widget_px_to_pos (&pos,*/
                 /*(self->start_x + offset_x) - SPACE_BEFORE_START);*/
      /*region_set_end_pos (self->region,*/
                            /*&pos);*/
      /*gtk_widget_queue_allocate (GTK_WIDGET (self));*/
    /*}*/
  /*else if (self->action == PIANO_ROLL_ARRANGER_WIDGET_ACTION_MOVING_REGION)*/
    /*{*/
      /*Position pos;*/

      /*[> move region start & end on x-axis <]*/
      /*int start_px = ruler_widget_pos_to_px (&self->region->start_pos);*/
      /*ruler_widget_px_to_pos (&pos,*/
                              /*start_px + (offset_x - self->last_offset_x));*/
      /*region_set_start_pos (self->region,*/
                            /*&pos);*/
      /*start_px = ruler_widget_pos_to_px (&self->region->end_pos);*/
      /*ruler_widget_px_to_pos (&pos,*/
                              /*start_px + (offset_x - self->last_offset_x));*/
      /*region_set_end_pos (self->region,*/
                          /*&pos);*/

      /*gtk_widget_queue_allocate (GTK_WIDGET (self));*/
    /*}*/
  /*self->last_offset_x = offset_x;*/
}

static void
drag_end (GtkGestureDrag *gesture,
               gdouble         offset_x,
               gdouble         offset_y,
               gpointer        user_data)
{
  PianoRollArrangerWidget * self = (PianoRollArrangerWidget *) user_data;
  /*self->start_x = 0;*/
  /*self->last_offset_x = 0;*/
  /*self->action = PIANO_ROLL_ARRANGER_WIDGET_ACTION_NONE;*/
  /*ui_set_cursor (GTK_WIDGET (self->note->widget), "default");*/
  /*self->region = NULL;*/
}


PianoRollArrangerWidget *
piano_roll_arranger_widget_new ()
{
  g_message ("Creating piano_roll_arranger...");
  PianoRollArrangerWidget * self = g_object_new (PIANO_ROLL_ARRANGER_WIDGET_TYPE, NULL);

  /*self->bg = piano_roll_arranger_bg_widget_new ();*/

  /*gtk_container_add (GTK_CONTAINER (self),*/
                     /*GTK_WIDGET (self->bg));*/

  /* set the size */
  gtk_widget_set_size_request (
    GTK_WIDGET (self),
    MAIN_WINDOW->ruler->total_px,
    MAIN_WINDOW->piano_roll_labels->total_px);

  /* make it able to notify */
  gtk_widget_add_events (GTK_WIDGET (self), GDK_ALL_EVENTS_MASK);
  gtk_widget_set_can_focus (GTK_WIDGET (self),
                           1);
  gtk_widget_set_focus_on_click (GTK_WIDGET (self),
                                 1);

  g_signal_connect (G_OBJECT (self), "get-child-position",
                    G_CALLBACK (get_child_position), NULL);
  g_signal_connect (G_OBJECT(self->drag), "drag-begin",
                    G_CALLBACK (drag_begin),  self);
  g_signal_connect (G_OBJECT(self->drag), "drag-update",
                    G_CALLBACK (drag_update),  self);
  g_signal_connect (G_OBJECT(self->drag), "drag-end",
                    G_CALLBACK (drag_end),  self);
  g_signal_connect (G_OBJECT (self->multipress), "pressed",
                    G_CALLBACK (multipress_pressed), self);


  return self;
}

static void
piano_roll_arranger_widget_class_init (PianoRollArrangerWidgetClass * klass)
{
}

static void
piano_roll_arranger_widget_init (PianoRollArrangerWidget *self )
{
  self->drag = GTK_GESTURE_DRAG (
                gtk_gesture_drag_new (GTK_WIDGET (self)));
  self->multipress = GTK_GESTURE_MULTI_PRESS (
                gtk_gesture_multi_press_new (GTK_WIDGET (self)));
}

