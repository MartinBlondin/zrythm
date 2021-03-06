/*
 * gui/widgets/top_bar.h - Toptom bar
 *
 * Copyright (C) 2019 Alexandros Theodotou
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

#ifndef __GUI_WIDGETS_TOP_BAR_H__
#define __GUI_WIDGETS_TOP_BAR_H__

#include <gtk/gtk.h>

#define TOP_BAR_WIDGET_TYPE \
  (top_bar_widget_get_type ())
G_DECLARE_FINAL_TYPE (TopBarWidget,
                      top_bar_widget,
                      Z,
                      TOP_BAR_WIDGET,
                      GtkBox)

#define TOP_BAR MW->top_bar

typedef struct _DigitalMeterWidget
  DigitalMeterWidget;
typedef struct _TransportControlsWidget
  TransportControlsWidget;
typedef struct _CpuWidget CpuWidget;
typedef struct _MidiActivityBarWidget
  MidiActivityBarWidget;
typedef struct _LiveWaveformWidget
  LiveWaveformWidget;

typedef struct _TopBarWidget
{
  GtkBox                    parent_instance;
  GtkToolbar *              top_bar_left;
  GtkBox *                  digital_meters;
  DigitalMeterWidget *      digital_bpm;

  /**
   * Overlay for showing things on top of the
   * playhead positionmeter.
   */
  GtkOverlay *              playhead_overlay;

  /**
   * The playhead digital meter.
   */
  DigitalMeterWidget *      digital_transport;

  /** Jack timebase master image. */
  GtkWidget *               master_img;

  /** Jack client master image. */
  GtkWidget *               client_img;

  /**
   * Menuitems in context menu of digital transport.
   */
  GtkCheckMenuItem *        timebase_master_check;
  GtkCheckMenuItem *        transport_client_check;
  GtkCheckMenuItem *        no_jack_transport_check;

  DigitalMeterWidget *      digital_timesig;
  TransportControlsWidget * transport_controls;
  LiveWaveformWidget *      live_waveform;
  MidiActivityBarWidget *   midi_activity;
  CpuWidget *               cpu_load;
} TopBarWidget;

void
top_bar_widget_refresh (TopBarWidget * self);

#endif
