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

#include "audio/engine.h"
#include "audio/mixer.h"
#include "audio/port.h"
#include "audio/track.h"
#include "gui/widgets/editable_label.h"
#include "gui/widgets/inspector_port.h"
#include "gui/widgets/ports_expander.h"
#include "plugins/plugin.h"
#include "project.h"

#include <glib/gi18n.h>

G_DEFINE_TYPE (PortsExpanderWidget,
               ports_expander_widget,
               TWO_COL_EXPANDER_BOX_WIDGET_TYPE)

/**
 * Refreshes each field.
 */
void
ports_expander_widget_refresh (
  PortsExpanderWidget * self)
{
  /* set visibility */
  /* FIXME the port counts used don't take into
   * account invisible (notOnGUI) ports */
  if (self->owner_type == PORT_OWNER_TYPE_PLUGIN)
    {
      if (self->type == TYPE_CONTROL)
        {
          if (self->flow == FLOW_INPUT)
            {
              gtk_widget_set_visible (
                GTK_WIDGET (self),
                self->plugin &&
                  self->plugin->descr->
                    num_ctrl_ins > 0);
            }
          else if (self->flow == FLOW_OUTPUT)
            {
              gtk_widget_set_visible (
                GTK_WIDGET (self),
                self->plugin &&
                  self->plugin->descr->
                    num_ctrl_outs > 0);
            }
        }
      else if (self->type == TYPE_AUDIO)
        {
          if (self->flow == FLOW_INPUT)
            {
              gtk_widget_set_visible (
                GTK_WIDGET (self),
                self->plugin &&
                  self->plugin->descr->
                    num_audio_ins > 0);
            }
          else if (self->flow == FLOW_OUTPUT)
            {
              gtk_widget_set_visible (
                GTK_WIDGET (self),
                self->plugin &&
                  self->plugin->descr->
                    num_audio_outs > 0);
            }
        }
      else if (self->type == TYPE_EVENT)
        {
          if (self->flow == FLOW_INPUT)
            {
              gtk_widget_set_visible (
                GTK_WIDGET (self),
                self->plugin &&
                  self->plugin->descr->
                    num_midi_ins > 0);
            }
          else if (self->flow == FLOW_OUTPUT)
            {
              gtk_widget_set_visible (
                GTK_WIDGET (self),
                self->plugin &&
                  self->plugin->descr->
                    num_midi_outs > 0);
            }
        }
      else if (self->type == TYPE_CV)
        {
          if (self->flow == FLOW_INPUT)
            {
              gtk_widget_set_visible (
                GTK_WIDGET (self),
                self->plugin &&
                  self->plugin->descr->
                    num_cv_ins > 0);
            }
          else if (self->flow == FLOW_OUTPUT)
            {
              gtk_widget_set_visible (
                GTK_WIDGET (self),
                self->plugin &&
                  self->plugin->descr->
                    num_cv_outs > 0);
            }
        }
    }
  else if (self->owner_type == PORT_OWNER_TYPE_TRACK)
    {
    }
}

static void
set_icon_from_port_type (
  PortsExpanderWidget * self,
  PortType              type)
{
  if (type == TYPE_AUDIO)
    expander_box_widget_set_icon_resource (
      Z_EXPANDER_BOX_WIDGET (self),
      ICON_TYPE_ZRYTHM,
      "audio.svg");
  else if (type == TYPE_CV)
    expander_box_widget_set_icon_resource (
      Z_EXPANDER_BOX_WIDGET (self),
      ICON_TYPE_ZRYTHM,
      "cv.svg");
  else if (type == TYPE_EVENT)
    expander_box_widget_set_icon_name (
      Z_EXPANDER_BOX_WIDGET (self),
      "z-audio-midi");
  else if (type == TYPE_CONTROL)
    expander_box_widget_set_icon_name (
      Z_EXPANDER_BOX_WIDGET (self),
      "z-configure");
}

/**
 * Sets up the PortsExpanderWidget for a Plugin.
 */
void
ports_expander_widget_setup_plugin (
  PortsExpanderWidget * self,
  PortFlow      flow,
  PortType      type,
  Plugin *      pl)
{
  self->plugin = pl;
  self->flow = flow;
  self->type = type;
  self->owner_type = PORT_OWNER_TYPE_PLUGIN;

  /* set name and icon */
  char fullstr[200];

  if (type == TYPE_CONTROL &&
      flow == FLOW_INPUT)
    strcpy (fullstr, _("Controls"));
  else if (type == TYPE_CONTROL &&
      flow == FLOW_OUTPUT)
    strcpy (fullstr, _("Control Outs"));
  else if (type == TYPE_AUDIO &&
           flow == FLOW_INPUT)
    strcpy (fullstr, _("Audio Ins"));
  else if (type == TYPE_AUDIO &&
      flow == FLOW_OUTPUT)
    strcpy (fullstr, _("Audio Outs"));
  else if (type == TYPE_EVENT &&
      flow == FLOW_INPUT)
    strcpy (fullstr, _("MIDI Ins"));
  else if (type == TYPE_EVENT &&
      flow == FLOW_OUTPUT)
    strcpy (fullstr, _("MIDI Outs"));
  else if (type == TYPE_CV &&
      flow == FLOW_INPUT)
    strcpy (fullstr, _("CV Ins"));
  else if (type == TYPE_CV &&
      flow == FLOW_OUTPUT)
    strcpy (fullstr, _("CV Outs"));

  expander_box_widget_set_label (
    Z_EXPANDER_BOX_WIDGET (self),
    fullstr);

  set_icon_from_port_type (self, type);

  /* readd ports */
  two_col_expander_box_widget_remove_children (
    Z_TWO_COL_EXPANDER_BOX_WIDGET (self));

  /* set scrollbar options */
  two_col_expander_box_widget_set_scroll_policy (
    Z_TWO_COL_EXPANDER_BOX_WIDGET (self),
    GTK_POLICY_NEVER,
    GTK_POLICY_AUTOMATIC);
  two_col_expander_box_widget_set_min_max_size (
    Z_TWO_COL_EXPANDER_BOX_WIDGET (self),
    -1, -1, -1, 120);

  TwoColExpanderBoxWidgetPrivate * prv =
    two_col_expander_box_widget_get_private (
      Z_TWO_COL_EXPANDER_BOX_WIDGET (self));
  gtk_widget_set_vexpand_set (
    GTK_WIDGET (prv->content), 1);

#define ADD_SINGLE(x) \
  ip = \
    inspector_port_widget_new (x); \
  two_col_expander_box_widget_add_single ( \
    Z_TWO_COL_EXPANDER_BOX_WIDGET (self), \
    GTK_WIDGET (ip))

  InspectorPortWidget * ip;
  Port * port;
  PortIdentifier * pi;
  if (pl && type == TYPE_CONTROL &&
      flow == FLOW_INPUT)
    {
      for (int i = 0; i < pl->num_in_ports; i++)
        {
          port = pl->in_ports[i];
          pi = &port->id;
          if (pi->type != TYPE_CONTROL ||
              pi->flags & PORT_FLAG_NOT_ON_GUI)
            continue;

          ADD_SINGLE (port);
        }
    }
  else if (pl && type == TYPE_CONTROL &&
           flow == FLOW_OUTPUT)
    {
      for (int i = 0; i < pl->num_out_ports; i++)
        {
          port = pl->out_ports[i];
          pi = &port->id;
          if (pi->type != TYPE_CONTROL ||
              pi->flags & PORT_FLAG_NOT_ON_GUI)
            continue;

          ADD_SINGLE (port);
        }
    }
  else if (pl && type == TYPE_CV &&
           flow == FLOW_INPUT)
    {
      for (int i = 0; i < pl->num_in_ports; i++)
        {
          port = pl->in_ports[i];
          pi = &port->id;
          if (pi->type != TYPE_CV ||
              pi->flags & PORT_FLAG_NOT_ON_GUI)
            continue;

          ADD_SINGLE (port);
        }
    }
  else if (pl && type == TYPE_CV &&
           flow == FLOW_OUTPUT)
    {
      for (int i = 0; i < pl->num_out_ports; i++)
        {
          port = pl->out_ports[i];
          pi = &port->id;
          if (pi->type != TYPE_CV ||
              pi->flags & PORT_FLAG_NOT_ON_GUI)
            continue;

          ADD_SINGLE (port);
        }
    }
  else if (pl && type == TYPE_AUDIO &&
           flow == FLOW_INPUT)
    {
      for (int i = 0; i < pl->num_in_ports; i++)
        {
          port = pl->in_ports[i];
          pi = &port->id;
          if (pi->type != TYPE_AUDIO ||
              pi->flags & PORT_FLAG_NOT_ON_GUI)
            continue;

          ADD_SINGLE (port);
        }
    }
  else if (pl && type == TYPE_AUDIO &&
           flow == FLOW_OUTPUT)
    {
      for (int i = 0; i < pl->num_out_ports; i++)
        {
          port = pl->out_ports[i];
          pi = &port->id;
          if (pi->type != TYPE_AUDIO ||
              pi->flags & PORT_FLAG_NOT_ON_GUI)
            continue;

          ADD_SINGLE (port);
        }
    }
  else if (pl && type == TYPE_EVENT &&
           flow == FLOW_INPUT)
    {
      for (int i = 0; i < pl->num_in_ports; i++)
        {
          port = pl->in_ports[i];
          pi = &port->id;
          if (pi->type != TYPE_EVENT ||
              pi->flags & PORT_FLAG_NOT_ON_GUI)
            continue;

          ADD_SINGLE (port);
        }
    }
  else if (pl && type == TYPE_EVENT &&
           flow == FLOW_OUTPUT)
    {
      for (int i = 0; i < pl->num_out_ports; i++)
        {
          port = pl->out_ports[i];
          pi = &port->id;
          if (pi->type != TYPE_EVENT ||
              pi->flags & PORT_FLAG_NOT_ON_GUI)
            continue;

          ADD_SINGLE (port);
        }
    }
#undef ADD_SINGLE

  ports_expander_widget_refresh (self);
}

/**
 * Sets up the PortsExpanderWidget for Track ports.
 *
 * @param type The type of ports to include.
 */
void
ports_expander_widget_setup_track (
  PortsExpanderWidget *      self,
  Track *                    tr,
  PortsExpanderTrackPortType type)
{
  self->track = tr;
  self->owner_type = PORT_OWNER_TYPE_TRACK;
  /*PortType in_type =*/
    /*self->track->in_signal_type;*/
  PortType out_type =
    self->track->out_signal_type;

  switch (type)
    {
    case PE_TRACK_PORT_TYPE_SENDS:
      expander_box_widget_set_label (
        Z_EXPANDER_BOX_WIDGET (self),
        _("Sends"));
      self->flow = FLOW_OUTPUT;
      self->type = out_type;
      break;
    case PE_TRACK_PORT_TYPE_STEREO_IN:
      expander_box_widget_set_label (
        Z_EXPANDER_BOX_WIDGET (self),
        _("Stereo In"));
      self->flow = FLOW_INPUT;
      self->owner_type = PORT_OWNER_TYPE_TRACK;
      break;
    case PE_TRACK_PORT_TYPE_MIDI_IN:
      expander_box_widget_set_label (
        Z_EXPANDER_BOX_WIDGET (self),
        _("MIDI In"));
      self->owner_type = PORT_OWNER_TYPE_TRACK;
      self->flow = FLOW_INPUT;
      self->type = TYPE_EVENT;
      break;
    case PE_TRACK_PORT_TYPE_MIDI_OUT:
      expander_box_widget_set_label (
        Z_EXPANDER_BOX_WIDGET (self),
        _("MIDI Out"));
      self->owner_type = PORT_OWNER_TYPE_TRACK;
      self->flow = FLOW_OUTPUT;
      self->type = TYPE_EVENT;
      break;
    }

  set_icon_from_port_type (self, self->type);

  /* readd ports */
  two_col_expander_box_widget_remove_children (
    Z_TWO_COL_EXPANDER_BOX_WIDGET (self));

  TwoColExpanderBoxWidgetPrivate * prv =
    two_col_expander_box_widget_get_private (
      Z_TWO_COL_EXPANDER_BOX_WIDGET (self));
  gtk_widget_set_vexpand_set (
    GTK_WIDGET (prv->content), 1);

#define ADD_SINGLE(x) \
  ip = \
    inspector_port_widget_new (x); \
  two_col_expander_box_widget_add_single ( \
    Z_TWO_COL_EXPANDER_BOX_WIDGET (self), \
    GTK_WIDGET (ip))

  InspectorPortWidget * ip;
  switch (type)
    {
    case PE_TRACK_PORT_TYPE_SENDS:
      if (out_type == TYPE_AUDIO)
        {
          ADD_SINGLE (
            tr->channel->prefader.stereo_out->l);
          ADD_SINGLE (
            tr->channel->prefader.stereo_out->r);
          ADD_SINGLE (
            tr->channel->fader.stereo_out->l);
          ADD_SINGLE (
            tr->channel->fader.stereo_out->r);
        }
      else if (out_type == TYPE_EVENT)
        {
          ADD_SINGLE (
            tr->channel->prefader.midi_out);
          ADD_SINGLE (
            tr->channel->fader.midi_out);
        }
      break;
      break;
    case PE_TRACK_PORT_TYPE_STEREO_IN:
      ADD_SINGLE (
        tr->processor.stereo_in->l);
      ADD_SINGLE (
        tr->processor.stereo_in->r);
      break;
    case PE_TRACK_PORT_TYPE_MIDI_IN:
      ADD_SINGLE (
        tr->processor.midi_in);
      break;
    case PE_TRACK_PORT_TYPE_MIDI_OUT:
      ADD_SINGLE (
        tr->channel->midi_out);
      break;
    }

#undef ADD_SINGLE
}

static void
ports_expander_widget_class_init (
  PortsExpanderWidgetClass * klass)
{
}

static void
ports_expander_widget_init (
  PortsExpanderWidget * self)
{
  /*two_col_expander_box_widget_add_single (*/
    /*Z_TWO_COL_EXPANDER_BOX_WIDGET (self),*/
    /*GTK_WIDGET (self->name));*/
}

