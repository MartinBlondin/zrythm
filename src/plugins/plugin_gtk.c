/*
 * Copyright (C) 2020 Alexandros Theodotou <alex at zrythm dot org>
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
 *
 * This file incorporates work covered by the following copyright and
 * permission notice:
 *
  Copyright 2007-2016 David Robillard <http://drobilla.net>

  Permission to use, copy, modify, and/or distribute this software for any
  purpose with or without fee is hereby granted, provided that the above
  copyright notice and this permission notice appear in all copies.

  THIS SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "config.h"

#include "audio/engine.h"
#include "audio/track.h"
#include "gui/backend/events.h"
#include "gui/widgets/main_window.h"
#include "plugins/lv2/lv2_gtk.h"
#include "plugins/lv2/lv2_state.h"
#include "plugins/plugin.h"
#include "plugins/plugin_gtk.h"
#include "plugins/vst/vst_x11.h"
#include "settings/settings.h"
#include "project.h"
#include "zrythm.h"

#include <glib/gi18n.h>

static void
on_quit_activate (
  GtkWidget* widget, gpointer data)
{
  GtkWidget* window = (GtkWidget*)data;
  gtk_widget_destroy (window);
}

static void
on_save_activate (
  GtkWidget* widget,
  Plugin * plugin)
{
  switch (plugin->descr->protocol)
    {
    case PROT_LV2:
      g_return_if_fail (plugin->lv2);
      lv2_gtk_on_save_activate (plugin->lv2);
      break;
    case PROT_VST:
      break;
    default:
      break;
    }
  GtkWidget* dialog = gtk_file_chooser_dialog_new(
    _("Save State"),
    (GtkWindow*)plugin->window,
    GTK_FILE_CHOOSER_ACTION_CREATE_FOLDER,
    _("_Cancel"), GTK_RESPONSE_CANCEL,
    _("_Save"), GTK_RESPONSE_ACCEPT,
    NULL);

  if (gtk_dialog_run (
        GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
    {
      char* path =
        gtk_file_chooser_get_filename (
          GTK_FILE_CHOOSER(dialog));
      char* base =
        g_build_filename (path, "/", NULL);

      switch (plugin->descr->protocol)
        {
        case PROT_LV2:
          lv2_state_save (plugin->lv2, base);
          break;
        case PROT_VST:
          break;
        default:
          break;
        }
      g_free(path);
      g_free(base);
    }

  gtk_widget_destroy(dialog);
}

void
plugin_gtk_set_window_title (
  Plugin *    plugin,
  GtkWindow * window)
{
  g_return_if_fail (plugin && plugin->descr);
  Track * track =
    plugin_get_track (plugin);
  const char* track_name = track->name;
  const char* plugin_name = plugin->descr->name;
  g_return_if_fail (
    track_name && plugin_name && window);

  char title[500];
  sprintf (
    title,
    "%s (%s)",
    track_name, plugin_name);

  switch (plugin->descr->protocol)
    {
    case PROT_LV2:
      if (plugin->lv2->preset)
        {
          Lv2Plugin * lv2 = plugin->lv2;
          const char* preset_label =
            lilv_state_get_label (lv2->preset);
          g_return_if_fail (preset_label);
          char preset_part[500];
          sprintf (
            preset_part, " - %s",
            preset_label);
          strcat (
            title, preset_part);
        }
      break;
    case PROT_VST:
      /* TODO */
      break;
    default:
      break;
    }

  gtk_window_set_title (window, title);
}

void
plugin_gtk_on_preset_activate (
  GtkWidget* widget,
  PluginGtkPresetRecord * record)
{
  g_return_if_fail (record && record->plugin);
  Plugin * plugin = record->plugin;

  if (GTK_CHECK_MENU_ITEM (widget) !=
        plugin->active_preset_item)
    {
      switch (plugin->descr->protocol)
        {
        case PROT_LV2:
          g_return_if_fail (plugin->lv2);
          lv2_state_apply_preset (
            plugin->lv2, (LilvNode *) record->preset);
          break;
        default:
          break;
        }
      if (plugin->active_preset_item)
        {
          gtk_check_menu_item_set_active (
            plugin->active_preset_item,
            FALSE);
        }

      plugin->active_preset_item =
        GTK_CHECK_MENU_ITEM (widget);
      gtk_check_menu_item_set_active (
        plugin->active_preset_item, TRUE);
      plugin_gtk_set_window_title (
        plugin, plugin->window);
    }
}

void
plugin_gtk_on_preset_destroy (
  PluginGtkPresetRecord * record,
  GClosure* closure)
{
  switch (record->plugin->descr->protocol)
    {
    case PROT_LV2:
      lilv_node_free ((LilvNode *) record->preset);
      break;
    default:
      break;
    }
  free (record);
}

PluginGtkPresetMenu*
plugin_gtk_preset_menu_new (
  const char* label)
{
  PluginGtkPresetMenu* menu =
    (PluginGtkPresetMenu*) calloc (1, sizeof(PluginGtkPresetMenu));

  menu->label = g_strdup(label);
  menu->item =
    GTK_MENU_ITEM (
      gtk_menu_item_new_with_label(menu->label));
  menu->menu = GTK_MENU (gtk_menu_new());
  menu->banks = NULL;

  return menu;
}

static void
preset_menu_free (
  PluginGtkPresetMenu* menu)
{
  if (menu->banks)
    {
      for (GSequenceIter* i =
             g_sequence_get_begin_iter(menu->banks);
           !g_sequence_iter_is_end(i);
           i = g_sequence_iter_next(i))
        {
          PluginGtkPresetMenu* bank_menu =
            (PluginGtkPresetMenu*)g_sequence_get(i);
          preset_menu_free (bank_menu);
        }
      g_sequence_free(menu->banks);
    }

  free(menu->label);
  free(menu);
}

gint
plugin_gtk_menu_cmp (
  gconstpointer a, gconstpointer b, gpointer data)
{
  return strcmp(((PluginGtkPresetMenu*)a)->label,
                ((PluginGtkPresetMenu*)b)->label);
}

static void
finish_menu (PluginGtkPresetMenu* menu)
{
  for (GSequenceIter* i =
         g_sequence_get_begin_iter(menu->banks);
       !g_sequence_iter_is_end(i);
       i = g_sequence_iter_next(i))
    {
      PluginGtkPresetMenu* bank_menu =
        (PluginGtkPresetMenu*)g_sequence_get(i);
      gtk_menu_shell_append (
        GTK_MENU_SHELL(menu->menu),
        GTK_WIDGET(bank_menu->item));
    }

  g_sequence_free(menu->banks);
}

void
plugin_gtk_rebuild_preset_menu (
  Plugin* plugin,
  GtkContainer* pset_menu)
{
  // Clear current menu
  plugin->active_preset_item = NULL;
  for (GList* items =
         g_list_nth (
           gtk_container_get_children (pset_menu),
           3);
       items;
       items = items->next)
    {
      gtk_container_remove (
        pset_menu, GTK_WIDGET(items->data));
    }

  // Load presets and build new menu
  PluginGtkPresetMenu menu =
    {
      NULL, NULL, GTK_MENU(pset_menu),
      g_sequence_new (
        (GDestroyNotify)preset_menu_free),
    };
  switch (plugin->descr->protocol)
    {
    case PROT_LV2:
      lv2_state_load_presets (
        plugin->lv2, lv2_gtk_add_preset_to_menu,
        &menu);
      break;
    default:
      break;
    }
  finish_menu (&menu);
  gtk_widget_show_all (GTK_WIDGET(pset_menu));
}

static void
on_save_preset_activate (
  GtkWidget * widget,
  Plugin *    plugin)
{
  switch (plugin->descr->protocol)
    {
    case PROT_LV2:
      lv2_gtk_on_save_preset_activate (
        widget, plugin->lv2);
      break;
    default:
      break;
    }
}

static void
on_delete_preset_activate (
  GtkWidget * widget,
  Plugin *    plugin)
{
  switch (plugin->descr->protocol)
    {
    case PROT_LV2:
      lv2_gtk_on_delete_preset_activate (
        widget, plugin->lv2);
      break;
    default:
      break;
    }
}

GtkWidget*
plugin_gtk_new_label (
  const char* text,
  int title,
  float xalign,
  float yalign)
{
  GtkWidget*  label = gtk_label_new(NULL);
  const char* fmt   = title ? "<span font_weight=\"bold\">%s</span>" : "%s:";
  gchar * str = g_markup_printf_escaped(fmt, text);
  gtk_label_set_markup (GTK_LABEL(label), str);
  g_free(str);
  gtk_label_set_xalign (
    GTK_LABEL (label), xalign);
  gtk_label_set_yalign (
    GTK_LABEL (label), yalign);
  return label;
}

void
plugin_gtk_add_control_row (
  GtkWidget*  table,
  int         row,
  const char* name,
  PluginGtkController* controller)
{
  GtkWidget* label =
    plugin_gtk_new_label (name, false, 1.0, 0.5);
  gtk_grid_attach (
    GTK_GRID (table), label,
    0, row, 1, 1);
  int control_left_attach = 1;
  if (controller->spin)
    {
      control_left_attach = 2;
      gtk_grid_attach (
        GTK_GRID (table),
        GTK_WIDGET (controller->spin),
        1, row, 1, 1);
    }
  gtk_grid_attach (
    GTK_GRID (table), controller->control,
    control_left_attach, row, 3 - control_left_attach, 1);
}

static void
build_menu (
  Plugin* plugin,
  GtkWidget* window,
  GtkWidget* vbox)
{
  GtkWidget* menu_bar  = gtk_menu_bar_new();
  GtkWidget* file      = gtk_menu_item_new_with_mnemonic("_File");
  GtkWidget* file_menu = gtk_menu_new();

  GtkAccelGroup* ag = gtk_accel_group_new();
  gtk_window_add_accel_group(GTK_WINDOW(window), ag);

  GtkWidget* save =
    gtk_menu_item_new_with_mnemonic ("_Save");
  GtkWidget* quit =
    gtk_menu_item_new_with_mnemonic ("_Quit");

  gtk_menu_item_set_submenu(GTK_MENU_ITEM(file), file_menu);
  gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), save);
  gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), quit);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), file);

  GtkWidget* pset_item   = gtk_menu_item_new_with_mnemonic("_Presets");
  GtkWidget* pset_menu   = gtk_menu_new();
  GtkWidget* save_preset = gtk_menu_item_new_with_mnemonic(
          "_Save Preset...");
  GtkWidget* delete_preset = gtk_menu_item_new_with_mnemonic(
          "_Delete Current Preset...");
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(pset_item), pset_menu);
  gtk_menu_shell_append(GTK_MENU_SHELL(pset_menu), save_preset);
  gtk_menu_shell_append(GTK_MENU_SHELL(pset_menu), delete_preset);
  gtk_menu_shell_append(GTK_MENU_SHELL(pset_menu),
                        gtk_separator_menu_item_new());
  gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), pset_item);

  PluginGtkPresetMenu menu = {
    NULL, NULL, GTK_MENU (pset_menu),
    g_sequence_new (
      (GDestroyNotify) preset_menu_free)
  };
  switch (plugin->descr->protocol)
    {
    case PROT_LV2:
      g_return_if_fail (plugin->lv2);
      lv2_state_load_presets (
        plugin->lv2, lv2_gtk_add_preset_to_menu,
        &menu);
      break;
    default:
      break;
    }
  finish_menu (&menu);

  /* connect signals */
  g_signal_connect (
    G_OBJECT(quit), "activate",
    G_CALLBACK(on_quit_activate), window);
  g_signal_connect (
    G_OBJECT(save), "activate",
    G_CALLBACK(on_save_activate), plugin);
  g_signal_connect (
    G_OBJECT(save_preset), "activate",
    G_CALLBACK(on_save_preset_activate), plugin);
  g_signal_connect (
    G_OBJECT(delete_preset), "activate",
    G_CALLBACK(on_delete_preset_activate), plugin);

  gtk_box_pack_start (
    GTK_BOX (vbox), menu_bar, FALSE, FALSE, 0);
}

/**
 * Called when the plugin window is destroyed.
 */
static void
on_window_destroy (
  GtkWidget * widget,
  Plugin *    plugin)
{
  plugin->window = NULL;
  g_return_if_fail (IS_PLUGIN (plugin));
  g_message (
    "destroying window for %s",
    plugin->descr->name);

  switch (plugin->descr->protocol)
    {
    case PROT_LV2:
      g_return_if_fail (plugin->lv2);
      lv2_gtk_on_window_destroy (plugin->lv2);
      break;
    case PROT_VST:
      g_return_if_fail (plugin->vst);
#ifdef HAVE_X11
      vst_x11_destroy_editor (plugin->vst);
#endif
      break;
    default:
      break;
    }

#ifdef _WOE32
  gtk_window_close (
    plugin->black_window);
#endif

  if (plugin->descr->protocol == PROT_LV2)
    {
      Lv2Plugin * lv2 = plugin->lv2;

      /* reinit widget in plugin ports and controls */
      int num_ctrls = lv2->controls.n_controls;
      int i;
      for (i = 0; i < num_ctrls; i++)
        {
          Lv2Control * control =
            lv2->controls.controls[i];

          control->widget = NULL;
        }

      for (i = 0; i < lv2->num_ports; i++)
        {
          Lv2Port * port = &lv2->ports[i];
          port->widget = NULL;
        }

      suil_instance_free (lv2->ui_instance);
      lv2->ui_instance = NULL;
    }
}

static gboolean
on_delete_event (
  GtkWidget *widget,
  GdkEvent  *event,
  Plugin * plugin)
{
  plugin->visible = 0;
  plugin->window = NULL;
  EVENTS_PUSH (
    ET_PLUGIN_VISIBILITY_CHANGED, plugin);

  return FALSE;
}

#ifdef _WOE32
static void
on_plugin_window_configure (
  GtkWidget    *widget,
  GdkEventConfigure *    event,
  Plugin *      plugin)
{
  /*
  gtk_widget_set_size_request (
    GTK_WIDGET (plugin->black_window),
    event->width, event->height);
    */
  int width, height;
  gtk_window_get_size (
    plugin->window, &width, &height);
  gtk_widget_set_size_request (
    GTK_WIDGET (plugin->black_window),
    width + 12, height + 32);

  int root_x, root_y;
  gtk_window_get_position (
    plugin->window, &root_x, &root_y);
  gtk_window_move (
    plugin->black_window, root_x, root_y);

  GdkWindow * plugin_win =
    gtk_widget_get_window (
      GTK_WIDGET (plugin->window));
  GdkWindow * black_win =
    gtk_widget_get_window (
      GTK_WIDGET (plugin->black_window));
  gdk_window_restack (
    black_win, plugin_win, 0);
  gdk_window_show_unraised (
    gtk_widget_get_window (
      GTK_WIDGET (plugin->black_window)));
}

static int
on_black_window_da_draw (
  GtkWidget    *widget,
  cairo_t *     cr,
  Plugin *      plugin)
{
  int width =
    gtk_widget_get_allocated_width (widget);
  int height =
    gtk_widget_get_allocated_height (widget);

  cairo_set_source_rgba (cr, 0, 0, 0, 1);
  cairo_rectangle (cr, 0, 0, width, height);
  cairo_fill (cr);

  return FALSE;
}

int
on_plugin_window_state_change (
  GtkWidget * widget,
  GdkEventWindowState *  event,
  Plugin * plugin)
{
  g_message ("window state");
  if (event->new_window_state & GDK_WINDOW_STATE_FOCUSED)
    {
      if (plugin->
            black_window_shown_manually)
        {
          plugin->
            black_window_shown_manually = 0;
          return FALSE;
        }
      GdkWindow * plugin_win =
        gtk_widget_get_window (
          GTK_WIDGET (plugin->window));
      GdkWindow * black_win =
        gtk_widget_get_window (
          GTK_WIDGET (plugin->black_window));
      g_message ("FOCUSED");

      gdk_window_show (
        gtk_widget_get_window (
          GTK_WIDGET (plugin->black_window)));
      gdk_window_restack (
        black_win, plugin_win, 0);
      gdk_window_show (
        gtk_widget_get_window (
          GTK_WIDGET (plugin->window)));
      plugin->black_window_shown_manually = 1;
    }
  return FALSE;
}
#endif

/**
 * Creates a new GtkWindow that will be used to
 * either wrap plugin UIs or create generic UIs in.
 */
void
plugin_gtk_create_window (
  Plugin * plugin)
{
  /* create window */
  plugin->window =
    GTK_WINDOW (
      gtk_window_new (GTK_WINDOW_TOPLEVEL));
  plugin_gtk_set_window_title (
    plugin, plugin->window);
  gtk_window_set_icon_name (
    plugin->window, "zrythm");
  gtk_window_set_role (
    plugin->window, "plugin_ui");

#ifdef _WOE32
  /* add black window in the background and make it
   * follow the plugin one */
  plugin->black_window =
    GTK_WINDOW (
      gtk_window_new (GTK_WINDOW_TOPLEVEL));
  gtk_window_set_decorated (
    plugin->black_window, 0);
  GtkWidget * da =
    gtk_drawing_area_new ();
  gtk_widget_set_visible (da, 1);
  gtk_widget_set_hexpand (da, 1);
  gtk_widget_set_vexpand (da, 1);
  gtk_container_add (
    GTK_CONTAINER (plugin->black_window),
    da);
  gtk_window_present (plugin->black_window);
  g_signal_connect (
    da, "draw",
    G_CALLBACK (on_black_window_da_draw),
    plugin);
  g_signal_connect (
    plugin->window, "configure-event",
    G_CALLBACK (on_plugin_window_configure),
    plugin);
  g_signal_connect (
    plugin->window, "window-state-event",
    G_CALLBACK (on_plugin_window_state_change),
    plugin);
#endif

  if (g_settings_get_int (
        S_PREFERENCES, "plugin-uis-stay-on-top"))
    {
      gtk_window_set_transient_for (
        plugin->window,
        GTK_WINDOW (MAIN_WINDOW));
    }

  /* add vbox for stacking elements */
  plugin->vbox =
    GTK_BOX (
      gtk_box_new (GTK_ORIENTATION_VERTICAL, 0));
  gtk_container_add (
    GTK_CONTAINER (plugin->window),
    GTK_WIDGET (plugin->vbox));

  /* add menu bar */
  build_menu (
    plugin, GTK_WIDGET (plugin->window),
    GTK_WIDGET (plugin->vbox));

  /* Create/show alignment to contain UI (whether
   * custom or generic) */
  plugin->ev_box =
    GTK_EVENT_BOX (
      gtk_event_box_new ());
  gtk_box_pack_start (
    plugin->vbox, GTK_WIDGET (plugin->ev_box),
    TRUE, TRUE, 0);
  gtk_widget_show_all (GTK_WIDGET (plugin->vbox));

  /* connect signals */
  g_signal_connect (
    plugin->window, "destroy",
    G_CALLBACK (on_window_destroy), plugin);
  plugin->delete_event_id =
    g_signal_connect (
      G_OBJECT (plugin->window), "delete-event",
      G_CALLBACK (on_delete_event), plugin);
}

/**
 * Closes the window of the plugin.
 */
void
plugin_gtk_close_window (
  Plugin * plugin)
{
  gtk_widget_set_sensitive (
    GTK_WIDGET (plugin->window), 0);
  gtk_window_close (
    GTK_WINDOW (plugin->window));
}
