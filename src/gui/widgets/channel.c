/*
 * Copyright (C) 2018-2019 Alexandros Theodotou <alex at zrythm dot org>
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

#include <time.h>
#include <sys/time.h>

#include "actions/copy_tracks_action.h"
#include "actions/edit_tracks_action.h"
#include "actions/move_tracks_action.h"
#include "actions/undoable_action.h"
#include "actions/undo_manager.h"
#include "audio/master_track.h"
#include "audio/mixer.h"
#include "audio/track.h"
#include "gui/widgets/bot_dock_edge.h"
#include "gui/widgets/center_dock.h"
#include "gui/widgets/channel.h"
#include "gui/widgets/color_area.h"
#include "gui/widgets/editable_label.h"
#include "gui/widgets/expander_box.h"
#include "gui/widgets/meter.h"
#include "gui/widgets/channel_slot.h"
#include "gui/widgets/fader.h"
#include "gui/widgets/knob.h"
#include "gui/widgets/mixer.h"
#include "gui/widgets/pan.h"
#include "gui/widgets/route_target_selector.h"
#include "plugins/lv2_plugin.h"
#include "project.h"
#include "utils/gtk.h"
#include "utils/math.h"
#include "utils/resources.h"
#include "utils/ui.h"
#include "zrythm.h"

#include <gtk/gtk.h>

#include <glib/gi18n.h>

G_DEFINE_TYPE (ChannelWidget,
               channel_widget,
               GTK_TYPE_EVENT_BOX)

/**
 * Tick function.
 *
 * usually, the way to do that is via g_idle_add() or g_main_context_invoke()
 * the other option is to use polling and have the GTK thread check if the
 * value changed every monitor refresh
 * that would be done via gtk_widget_set_tick_functions()
 * gtk_widget_set_tick_function()
 */
gboolean
channel_widget_update_meter_reading (
  ChannelWidget * widget,
  GdkFrameClock * frame_clock,
  gpointer        user_data)
{
  double prev = widget->meter_reading_val;
  Channel * channel = widget->channel;

  if (!gtk_widget_get_mapped (
        GTK_WIDGET (widget)))
    {
      return G_SOURCE_CONTINUE;
    }

  /* TODO */
  Track * track =
    channel_get_track (channel);
  if (track->out_signal_type == TYPE_EVENT)
    {
      gtk_label_set_text (
        widget->meter_reading, "-∞");
      gtk_widget_queue_draw (
        GTK_WIDGET (widget->meter_l));
      gtk_widget_queue_draw (
        GTK_WIDGET (widget->meter_r));
      return G_SOURCE_CONTINUE;
    }

  /* calc decibels */
  channel_set_current_l_db (
    channel,
    math_calculate_rms_db (
      channel->stereo_out->l->buf,
      AUDIO_ENGINE->nframes));
  channel_set_current_r_db (
    channel,
    math_calculate_rms_db (
      channel->stereo_out->r->buf,
      AUDIO_ENGINE->nframes));

  double val =
    (channel_get_current_l_db (channel) +
      channel_get_current_r_db (channel)) / 2;
  if (math_doubles_equal (val, prev))
    return G_SOURCE_CONTINUE;
  char * string;
  if (val < -100.)
    gtk_label_set_text (widget->meter_reading, "-∞");
  else
    {
      string = g_strdup_printf ("%.1f", val);
      gtk_label_set_text (widget->meter_reading, string);
      g_free (string);
    }
  gtk_widget_queue_draw (
    GTK_WIDGET (widget->meter_l));
  gtk_widget_queue_draw (
    GTK_WIDGET (widget->meter_r));

  widget->meter_reading_val = val;

  return G_SOURCE_CONTINUE;
}

static void
on_drag_data_received (
  GtkWidget        *widget,
  GdkDragContext   *context,
  gint              x,
  gint              y,
  GtkSelectionData *data,
  guint             info,
  guint             time,
  ChannelWidget * self)
{
  g_message ("drag data received");
  Track * this =
    channel_get_track (self->channel);

  /* determine if moving or copying */
  GdkDragAction action =
    gdk_drag_context_get_selected_action (
      context);

  /*tracklist_selections_print (*/
    /*TRACKLIST_SELECTIONS);*/

  int w =
    gtk_widget_get_allocated_width (widget);

  /* determine position to move to */
  int pos;
  if (x < w / 2)
    {
      if (this->pos <=
          MW_MIXER->start_drag_track->pos)
        pos = this->pos;
      else
        {
          Track * prev =
            tracklist_get_prev_visible_track (
              TRACKLIST, this);
          pos =
            prev ? prev->pos : this->pos;
        }
    }
  else
    {
      if (this->pos >=
          MW_MIXER->start_drag_track->pos)
        pos = this->pos;
      else
        {
          Track * next =
            tracklist_get_next_visible_track (
              TRACKLIST, this);
          pos =
            next ? next->pos : this->pos;
        }
    }

  UndoableAction * ua = NULL;
  if (action == GDK_ACTION_COPY)
    {
      ua =
        copy_tracks_action_new (
          TRACKLIST_SELECTIONS, pos);
    }
  else if (action == GDK_ACTION_MOVE)
    {
      ua =
        move_tracks_action_new (
          TRACKLIST_SELECTIONS, pos);
    }

  g_warn_if_fail (ua);

  undo_manager_perform (
    UNDO_MANAGER, ua);
}

static void
on_drag_data_get (
  GtkWidget        *widget,
  GdkDragContext   *context,
  GtkSelectionData *data,
  guint             info,
  guint             time,
  ChannelWidget * self)
{
  g_message ("drag data get");

  /* Not really needed since the selections are
   * used. just send master */
  gtk_selection_data_set (
    data,
    gdk_atom_intern_static_string (
      TARGET_ENTRY_TRACK),
    32,
    (const guchar *) &P_MASTER_TRACK,
    sizeof (P_MASTER_TRACK));
}

/**
 * For drag n drop.
 */
static void
on_dnd_drag_begin (
  GtkWidget      *widget,
  GdkDragContext *context,
  ChannelWidget * self)
{
  Track * track =
    channel_get_track (self->channel);
  self->selected_in_dnd = 1;
  MW_MIXER->start_drag_track = track;

  if (self->n_press == 1)
    {
      int ctrl = 0, selected = 0;

      ctrl = self->ctrl_held_at_start;

      if (tracklist_selections_contains_track (
            TRACKLIST_SELECTIONS,
            track))
        selected = 1;

      /* no control & not selected */
      if (!ctrl && !selected)
        {
          tracklist_selections_select_single (
            TRACKLIST_SELECTIONS,
            track);
        }
      else if (!ctrl && selected)
        { }
      else if (ctrl && !selected)
        tracklist_selections_add_track (
          TRACKLIST_SELECTIONS, track, 1);
    }
}

/**
 * Highlights/unhighlights the channels
 * appropriately.
 */
static void
do_highlight (
  ChannelWidget * self,
  gint x,
  gint y)
{
  /* if we are closer to the start of selection than
   * the end */
  int w =
    gtk_widget_get_allocated_width (
      GTK_WIDGET (self));
  if (x < w / 2)
    {
      /* highlight left */
      gtk_drag_highlight (
        GTK_WIDGET (
          self->highlight_left_box));
      gtk_widget_set_size_request (
        GTK_WIDGET (
          self->highlight_left_box),
        2, -1);

      /* unhilight right */
      gtk_drag_unhighlight (
        GTK_WIDGET (
          self->highlight_right_box));
      gtk_widget_set_size_request (
        GTK_WIDGET (
          self->highlight_right_box),
        -1, -1);
    }
  else
    {
      /* highlight right */
      gtk_drag_highlight (
        GTK_WIDGET (
          self->highlight_right_box));
      gtk_widget_set_size_request (
        GTK_WIDGET (
          self->highlight_right_box),
        2, -1);

      /* unhilight left */
      gtk_drag_unhighlight (
        GTK_WIDGET (
          self->highlight_left_box));
      gtk_widget_set_size_request (
        GTK_WIDGET (
          self->highlight_left_box),
        -1, -1);
    }
}

static void
on_drag_motion (
  GtkWidget *widget,
  GdkDragContext *context,
  gint x,
  gint y,
  guint time,
  ChannelWidget * self)
{
  GdkModifierType mask;
  z_gtk_widget_get_mask (
    widget, &mask);
  if (mask & GDK_CONTROL_MASK)
    gdk_drag_status (context, GDK_ACTION_COPY, time);
  else
    gdk_drag_status (context, GDK_ACTION_MOVE, time);

  do_highlight (self, x, y);
}

static void
on_drag_leave (GtkWidget      *widget,
               GdkDragContext *context,
               guint           time,
               ChannelWidget * self)
{
  g_message ("on_drag_leave");

  /*do_highlight (self);*/
  gtk_drag_unhighlight (
    GTK_WIDGET (self->highlight_left_box));
  gtk_widget_set_size_request (
    GTK_WIDGET (self->highlight_left_box),
    -1, -1);
  gtk_drag_unhighlight (
    GTK_WIDGET (self->highlight_right_box));
  gtk_widget_set_size_request (
    GTK_WIDGET (self->highlight_right_box),
    -1, -1);
}
/*static void*/
/*phase_invert_button_clicked (ChannelWidget * self,*/
                             /*GtkButton     * button)*/
/*{*/

/*}*/

/**
 * Callback when somewhere in the channel is
 * pressed.
 *
 * Only responsible for setting the tracklist
 * selection and should not do anything else.
 */
static void
on_whole_channel_press (
  GtkGestureMultiPress *gesture,
  gint                  n_press,
  gdouble               x,
  gdouble               y,
  ChannelWidget * self)
{
  self->n_press = n_press;

  GdkModifierType state_mask =
    ui_get_state_mask (
      GTK_GESTURE (gesture));
  self->ctrl_held_at_start =
    state_mask & GDK_CONTROL_MASK;
}

void
channel_widget_redraw_fader (
  ChannelWidget * self)
{
  g_return_if_fail (self->fader);
  gtk_widget_queue_draw (
    GTK_WIDGET (self->fader));
}

static void
on_drag_begin (GtkGestureDrag *gesture,
               gdouble         start_x,
               gdouble         start_y,
               ChannelWidget * self)
{
  self->selected_in_dnd = 0;
  self->dragged = 0;
}

static void
on_drag_update (GtkGestureDrag * gesture,
               gdouble         offset_x,
               gdouble         offset_y,
               ChannelWidget * self)
{
  self->dragged = 1;
}

static gboolean
on_btn_release (
  GtkWidget *      widget,
  GdkEventButton * event,
  ChannelWidget *  self)
{
  if (self->dragged || self->selected_in_dnd)
    return FALSE;

  Track * track =
    channel_get_track (self->channel);
  if (self->n_press == 1)
    {
      gint64 cur_time = g_get_monotonic_time ();
      if (cur_time - self->last_plugin_press >
            1000)
        {
          PROJECT->last_selection =
            SELECTION_TYPE_TRACK;
        }

      int ctrl = 0, selected = 0;

      if (event->state & GDK_CONTROL_MASK)
        ctrl = 1;

      if (tracklist_selections_contains_track (
            TRACKLIST_SELECTIONS,
            track))
        selected = 1;

      /* no control & not selected */
      if (!ctrl && !selected)
        {
          tracklist_selections_select_single (
            TRACKLIST_SELECTIONS,
            track);
        }
      else if (!ctrl && selected)
        {
        }
      else if (ctrl && !selected)
        {
          tracklist_selections_add_track (
            TRACKLIST_SELECTIONS, track, 1);
        }
      else if (ctrl && selected && !self->dragged)
        {
          tracklist_selections_remove_track (
            TRACKLIST_SELECTIONS, track, 1);
        }
    }

  return FALSE;
}

static void
on_record_toggled (
  GtkToggleButton * btn,
  ChannelWidget *   self)
{
  Track * track =
    channel_get_track (self->channel);
  track_set_recording (
    track,
    gtk_toggle_button_get_active (btn), 1);
}

static void
on_solo_toggled (
  GtkToggleButton * btn,
  ChannelWidget *   self)
{
  Track * track =
    channel_get_track (self->channel);
  track_set_soloed (
    track,
    gtk_toggle_button_get_active (btn), 1);
}

static void
on_mute_toggled (GtkToggleButton * btn,
                 ChannelWidget * self)
{
  Track * track =
    channel_get_track (self->channel);
  track_set_muted (
    track,
    gtk_toggle_button_get_active (btn),
    1, 1);
}

/*static void*/
/*on_listen_toggled (GtkToggleButton * btn,*/
                   /*ChannelWidget *   self)*/
/*{*/

/*}*/

/*static void*/
/*on_e_activate (GtkButton *     btn,*/
               /*ChannelWidget * self)*/
/*{*/

/*}*/

static void
refresh_color (ChannelWidget * self)
{
  Track * track =
    channel_get_track (self->channel);
  color_area_widget_set_color (
    self->color, &track->color);
}

#if 0
static void
setup_phase_panel (ChannelWidget * self)
{
  self->phase_knob =
    knob_widget_new_simple (
      channel_get_phase,
      channel_set_phase,
      self->channel,
      0, 180, 20, 0.0f);
  gtk_box_pack_end (self->phase_controls,
                       GTK_WIDGET (self->phase_knob),
                       0, 1, 0);
  char * str =
    g_strdup_printf (
      "%.1f",
      (double) self->channel->fader.phase);
  gtk_label_set_text (
    self->phase_reading, str);
  g_free (str);
}
#endif

static void
setup_meter (ChannelWidget * self)
{
  MeterType type = METER_TYPE_DB;
  Track * track =
    channel_get_track (self->channel);
  switch (track->out_signal_type)
    {
    case TYPE_EVENT:
      type = METER_TYPE_MIDI;
      meter_widget_setup (
        self->meter_l, channel_get_current_l_db,
        self->channel, type, 14);
      gtk_widget_set_margin_start (
        GTK_WIDGET (self->meter_l), 5);
      gtk_widget_set_margin_end (
        GTK_WIDGET (self->meter_l), 5);
      gtk_widget_set_visible (
        GTK_WIDGET (self->meter_r), 0);
      break;
    case TYPE_AUDIO:
      type = METER_TYPE_DB;
      meter_widget_setup (
        self->meter_l, channel_get_current_l_db,
        self->channel, type, 12);
      meter_widget_setup (
        self->meter_r, channel_get_current_r_db,
        self->channel, type, 12);
      break;
    default:
      break;
    }
}

/**
 * Updates the inserts.
 */
void
channel_widget_update_inserts (ChannelWidget * self)
{
  for (int i = 0; i < STRIP_SIZE; i++)
    gtk_widget_queue_draw (
      GTK_WIDGET (self->inserts[i]));
}


/**
 * Sets up the inserts.
 *
 * First removes the add button, then creates each slot.
 */
static void
setup_inserts (ChannelWidget * self)
{
  /*gtk_container_remove (GTK_CONTAINER (self->inserts_box),*/
                        /*GTK_WIDGET (self->add_slot));*/
  /*Channel * channel = self->channel;*/
  for (int i = 0; i < STRIP_SIZE; i++)
    {
      self->insert_boxes[i] =
        GTK_BOX (gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0));

      self->inserts[i] = channel_slot_widget_new (i, self);
      /* FIXME set to channel widget width */
      /*gtk_widget_set_size_request (GTK_WIDGET (self->insert_boxes[i]),*/
                                   /*20, 20);*/
      gtk_box_pack_start (
        self->insert_boxes[i],
        GTK_WIDGET (self->inserts[i]),
        1, 1, 0);
      gtk_box_pack_start (
        self->inserts_box,
        GTK_WIDGET (self->insert_boxes[i]),
        0, 1, 0);
      gtk_widget_show_all (
        GTK_WIDGET (self->insert_boxes[i]));
    }
}

static void
setup_channel_icon (ChannelWidget * self)
{
  Track * track =
    channel_get_track (self->channel);
  switch (track->type)
    {
    case TRACK_TYPE_INSTRUMENT:
    case TRACK_TYPE_MIDI:
      resources_set_image_icon (
        self->icon, ICON_TYPE_ZRYTHM,
        "instrument.svg");
      break;
    case TRACK_TYPE_AUDIO:
      resources_set_image_icon (
        self->icon, ICON_TYPE_ZRYTHM,
        "audio.svg");
      break;
    case TRACK_TYPE_AUDIO_BUS:
    case TRACK_TYPE_AUDIO_GROUP:
    case TRACK_TYPE_MASTER:
      resources_set_image_icon (
        self->icon, ICON_TYPE_ZRYTHM,
        "bus.svg");
      break;
    default:
      break;
    }
}

static void
refresh_output (ChannelWidget * self)
{
  route_target_selector_widget_refresh (
    self->output, self->channel);
}

static void
refresh_name (ChannelWidget * self)
{
  Track * track =
    channel_get_track (self->channel);
  g_warn_if_fail (track->name);
  gtk_label_set_text (
    GTK_LABEL (self->name->label), track->name);
}

/**
 * Sets up the icon and name box for drag & move.
 */
/*static void*/
/*setup_drag_move (*/
  /*ChannelWidget * self)*/
/*{*/

/*}*/

static void
setup_pan (ChannelWidget * self)
{
  self->pan =
    pan_widget_new (
      channel_get_pan, channel_set_pan,
      self->channel, 12);
  gtk_box_pack_start (
    self->pan_box, GTK_WIDGET (self->pan),
    Z_GTK_NO_EXPAND, Z_GTK_FILL, 0);
}

void
channel_widget_refresh_buttons (
  ChannelWidget * self)
{
  channel_widget_block_all_signal_handlers (
    self);
  Track * track =
    channel_get_track (self->channel);
  gtk_toggle_button_set_active (
    self->record, track->recording);
  gtk_toggle_button_set_active (
    self->solo, track->solo);
  gtk_toggle_button_set_active (
    self->mute, track_get_muted (track));
  channel_widget_unblock_all_signal_handlers (
    self);
}

/**
 * Updates everything on the widget.
 *
 * It is reduntant but keeps code organized. Should fix if it causes lags.
 */
void
channel_widget_refresh (ChannelWidget * self)
{
  refresh_name (self);
  refresh_output (self);
  channel_widget_update_meter_reading (
    self, NULL, NULL);
  channel_widget_refresh_buttons (self);
  refresh_color (self);

  Track * track =
    channel_get_track (self->channel);
  if (track_is_selected (track))
    {
      /* set selected or not */
      gtk_widget_set_state_flags (
        GTK_WIDGET (self),
        GTK_STATE_FLAG_SELECTED, 0);
    }
  else
    {
      gtk_widget_unset_state_flags (
        GTK_WIDGET (self),
        GTK_STATE_FLAG_SELECTED);
    }
}

/**
 * Blocks all signal handlers.
 */
void
channel_widget_block_all_signal_handlers (
  ChannelWidget * self)
{
  g_signal_handler_block (
    self->solo,
    self->solo_toggled_handler_id);
  g_signal_handler_block (
    self->mute,
    self->mute_toggled_handler_id);
}

/**
 * Unblocks all signal handlers.
 */
void
channel_widget_unblock_all_signal_handlers (
  ChannelWidget * self)
{
  g_signal_handler_unblock (
    self->solo,
    self->solo_toggled_handler_id);
  g_signal_handler_unblock (
    self->mute,
    self->mute_toggled_handler_id);
}

static void
on_destroy (
  ChannelWidget * self)
{
  self->channel->widget = NULL;

  g_object_unref (self);
}

ChannelWidget *
channel_widget_new (Channel * channel)
{
  ChannelWidget * self =
    g_object_new (CHANNEL_WIDGET_TYPE, NULL);
  self->channel = channel;

  /*setup_phase_panel (self);*/
  /*setup_pan (self);*/
  setup_inserts (self);
  fader_widget_setup (
    self->fader,
    &self->channel->fader,
    40);
  setup_meter (self);
  setup_pan (self);
  setup_channel_icon (self);
  Track * track =
    channel_get_track (self->channel);
  editable_label_widget_setup (
    self->name, track,
    (EditableLabelWidgetTextGetter) track_get_name,
    (EditableLabelWidgetTextSetter) track_set_name);
  route_target_selector_widget_setup (
    self->output, self->channel);
  channel_widget_refresh (self);

  gtk_widget_add_tick_callback (
    GTK_WIDGET (self),
    (GtkTickCallback)
      channel_widget_update_meter_reading,
    NULL,
    NULL);

  g_signal_connect (
    self, "destroy",
    G_CALLBACK (on_destroy), NULL);

  g_object_ref (self);

  return self;
}

static void
channel_widget_class_init (
  ChannelWidgetClass * _klass)
{
  GtkWidgetClass * klass =
    GTK_WIDGET_CLASS (_klass);
  resources_set_class_template (klass,
                                "channel.ui");
  gtk_widget_class_set_css_name (klass,
                                 "channel");

  gtk_widget_class_bind_template_child (
    klass,
    ChannelWidget,
    color);
  gtk_widget_class_bind_template_child (
    klass,
    ChannelWidget,
    output);
  gtk_widget_class_bind_template_child (
    klass,
    ChannelWidget,
    grid);
  gtk_widget_class_bind_template_child (
    klass,
    ChannelWidget,
    icon_and_name_event_box);
  gtk_widget_class_bind_template_child (
    klass,
    ChannelWidget,
    name);
#if 0
  gtk_widget_class_bind_template_child (
    klass,
    ChannelWidget,
    phase_invert);
  gtk_widget_class_bind_template_child (
    klass,
    ChannelWidget,
    phase_reading);
  gtk_widget_class_bind_template_child (
    klass,
    ChannelWidget,
    phase_controls);
#endif
  gtk_widget_class_bind_template_child (
    klass,
    ChannelWidget,
    inserts_expander);
  gtk_widget_class_bind_template_child (
    klass,
    ChannelWidget,
    e);
  gtk_widget_class_bind_template_child (
    klass,
    ChannelWidget,
    solo);
  gtk_widget_class_bind_template_child (
    klass,
    ChannelWidget,
    listen);
  gtk_widget_class_bind_template_child (
    klass,
    ChannelWidget,
    mute);
  gtk_widget_class_bind_template_child (
    klass,
    ChannelWidget,
    record);
  gtk_widget_class_bind_template_child (
    klass,
    ChannelWidget,
    fader);
  gtk_widget_class_bind_template_child (
    klass,
    ChannelWidget,
    meter_area);
  gtk_widget_class_bind_template_child (
    klass,
    ChannelWidget,
    meter_l);
  gtk_widget_class_bind_template_child (
    klass,
    ChannelWidget,
    meter_r);
  gtk_widget_class_bind_template_child (
    klass,
    ChannelWidget,
    meter_reading);
  gtk_widget_class_bind_template_child (
    klass,
    ChannelWidget,
    icon);
  gtk_widget_class_bind_template_child (
    klass,
    ChannelWidget,
    pan_box);
  gtk_widget_class_bind_template_child (
    klass,
    ChannelWidget,
    highlight_left_box);
  gtk_widget_class_bind_template_child (
    klass,
    ChannelWidget,
    highlight_right_box);
}

static void
channel_widget_init (ChannelWidget * self)
{
  g_type_ensure (ROUTE_TARGET_SELECTOR_WIDGET_TYPE);
  g_type_ensure (FADER_WIDGET_TYPE);
  g_type_ensure (METER_WIDGET_TYPE);
  g_type_ensure (COLOR_AREA_WIDGET_TYPE);

  gtk_widget_init_template (GTK_WIDGET (self));

  self->last_midi_trigger_time = 0;

  /* set font sizes */
  GtkStyleContext * context =
    gtk_widget_get_style_context (
      GTK_WIDGET (self->name->label));
  gtk_style_context_add_class (
    context, "channel_label");
  gtk_label_set_max_width_chars (
    self->name->label, 10);
  gtk_label_set_max_width_chars (
    self->output->label, 9);

  /* setup inserts */
  GtkWidget * inserts_scroll =
    gtk_scrolled_window_new (
      NULL, NULL);
  gtk_widget_set_vexpand (inserts_scroll, 1);
  gtk_widget_set_visible (inserts_scroll, 1);
  gtk_scrolled_window_set_shadow_type (
    GTK_SCROLLED_WINDOW (inserts_scroll),
    GTK_SHADOW_ETCHED_IN);
  gtk_widget_set_size_request (
    inserts_scroll, -1, 124);

  GtkWidget * viewport =
    gtk_viewport_new (NULL, NULL);
  gtk_widget_set_visible (viewport, 1);
  gtk_container_add (
    GTK_CONTAINER (inserts_scroll),
    GTK_WIDGET (viewport));

  self->inserts_box =
    GTK_BOX (
      gtk_box_new (GTK_ORIENTATION_VERTICAL, 0));
  gtk_widget_set_visible (
    GTK_WIDGET (self->inserts_box), 1);
  gtk_container_add (
    GTK_CONTAINER (viewport),
    GTK_WIDGET (self->inserts_box));

  expander_box_widget_add_content (
    self->inserts_expander, inserts_scroll);
  expander_box_widget_set_label (
    self->inserts_expander, _("Inserts"));

  char * entry_track = g_strdup (TARGET_ENTRY_TRACK);
  GtkTargetEntry entries[] = {
    {
      entry_track, GTK_TARGET_SAME_APP,
      symap_map (ZSYMAP, TARGET_ENTRY_TRACK),
    },
  };

  /* set as drag source for track */
  gtk_drag_source_set (
    GTK_WIDGET (self->icon_and_name_event_box),
    GDK_BUTTON1_MASK,
    entries, G_N_ELEMENTS (entries),
    GDK_ACTION_MOVE | GDK_ACTION_COPY);

  /* set as drag dest for channel (the channel will
   * be moved based on which half it was dropped in,
   * left or right) */
  gtk_drag_dest_set (
    GTK_WIDGET (self),
    GTK_DEST_DEFAULT_MOTION |
      GTK_DEST_DEFAULT_DROP,
    entries, G_N_ELEMENTS (entries),
    GDK_ACTION_MOVE | GDK_ACTION_COPY);
  g_free (entry_track);

  self->mp =
    GTK_GESTURE_MULTI_PRESS (
      gtk_gesture_multi_press_new (
        GTK_WIDGET (self)));
  self->drag =
    GTK_GESTURE_DRAG (
      gtk_gesture_drag_new (
        GTK_WIDGET (
          self)));

  gtk_widget_set_hexpand (
    GTK_WIDGET (self), 0);

  z_gtk_container_destroy_all_children (
    GTK_CONTAINER (self->record));
  z_gtk_button_set_icon_name (
    GTK_BUTTON (self->record),
    "z-media-record");
  context =
    gtk_widget_get_style_context (
      GTK_WIDGET (self->record));
  gtk_style_context_add_class (
    context, "record-button");
  context =
    gtk_widget_get_style_context (
      GTK_WIDGET (self->solo));
  gtk_style_context_add_class (
    context, "solo-button");

  self->solo_toggled_handler_id =
    g_signal_connect (
      G_OBJECT (self->solo), "toggled",
      G_CALLBACK (on_solo_toggled), self);
  self->mute_toggled_handler_id =
    g_signal_connect (
      G_OBJECT (self->mute), "toggled",
      G_CALLBACK (on_mute_toggled), self);
  self->record_toggled_handler_id =
    g_signal_connect (
      G_OBJECT (self->record), "toggled",
      G_CALLBACK (on_record_toggled), self);
  g_signal_connect (
    G_OBJECT (self->mp), "pressed",
    G_CALLBACK (on_whole_channel_press), self);
  g_signal_connect (
    G_OBJECT (self->drag), "drag-begin",
    G_CALLBACK (on_drag_begin), self);
  g_signal_connect (
    G_OBJECT (self->drag), "drag-update",
    G_CALLBACK (on_drag_update), self);
  /*g_signal_connect (*/
    /*G_OBJECT (self->drag), "drag-end",*/
    /*G_CALLBACK (on_drag_end), self);*/
  g_signal_connect_after (
    GTK_WIDGET (self->icon_and_name_event_box),
    "drag-begin",
    G_CALLBACK(on_dnd_drag_begin), self);
  /*g_signal_connect (*/
    /*GTK_WIDGET (self->icon_and_name_event_box),*/
    /*"drag-end",*/
    /*G_CALLBACK(on_dnd_drag_end), self);*/
  g_signal_connect (
    GTK_WIDGET (self), "drag-data-received",
    G_CALLBACK(on_drag_data_received), self);
  g_signal_connect (
    GTK_WIDGET (self->icon_and_name_event_box),
    "drag-data-get",
    G_CALLBACK (on_drag_data_get), self);
  g_signal_connect (
    GTK_WIDGET (self), "drag-motion",
    G_CALLBACK (on_drag_motion), self);
  g_signal_connect (
    GTK_WIDGET (self), "drag-leave",
    G_CALLBACK (on_drag_leave), self);
  /*g_signal_connect (*/
    /*GTK_WIDGET (self), "drag-failed",*/
    /*G_CALLBACK (on_drag_failed), self);*/
  g_signal_connect (
    GTK_WIDGET (self), "button-release-event",
    G_CALLBACK (on_btn_release), self);
}
