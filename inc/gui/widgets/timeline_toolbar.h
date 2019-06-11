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

#ifndef __GUI_WIDGETS_TIMELINE_TOOLBAR_H__
#define __GUI_WIDGETS_TIMELINE_TOOLBAR_H__

#include <gtk/gtk.h>

#define TIMELINE_TOOLBAR_WIDGET_TYPE \
  (timeline_toolbar_widget_get_type ())
G_DECLARE_FINAL_TYPE (
  TimelineToolbarWidget,
  timeline_toolbar_widget,
  Z, TIMELINE_TOOLBAR_WIDGET,
  GtkToolbar)

/**
 * \file
 */

#define MW_TIMELINE_TOOLBAR \
  MW_CENTER_DOCK->timeline_toolbar

typedef struct _ToolboxWidget ToolboxWidget;
typedef struct _QuantizeMbWidget QuantizeMbWidget;
typedef struct _SnapGridWidget SnapGridWidget;

/**
 * The Timeline toolbar in the top.
 */
typedef struct _TimelineToolbarWidget
{
  GtkToolbar       parent_instance;
  SnapGridWidget * snap_grid_timeline;
} TimelineToolbarWidget;

void
timeline_toolbar_widget_setup (
  TimelineToolbarWidget * self);

#endif
