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
/*
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

#include "plugins/lv2_plugin.h"
#include "plugins/lv2/lv2_gtk.h"
#include "plugins/lv2/lv2_ui.h"
#include "plugins/plugin.h"
#include "plugins/plugin_manager.h"
#include "zrythm.h"

#include <lv2/lv2plug.in/ns/ext/instance-access/instance-access.h>

static const bool dump = false;

/**
 * Returns if the UI of the plugin is resizable.
 */
int
lv2_ui_is_resizable (
  Lv2Plugin* plugin)
{
  if (!plugin->ui)
    return 0;

  const LilvNode * s =
    lilv_ui_get_uri (plugin->ui);
  LilvNode * p =
    lilv_new_uri (
      LILV_WORLD, LV2_CORE__optionalFeature);
  LilvNode * fs =
    lilv_new_uri (
      LILV_WORLD, LV2_UI__fixedSize);
  LilvNode * nrs =
    lilv_new_uri (
      LILV_WORLD, LV2_UI__noUserResize);

  LilvNodes * fs_matches =
    lilv_world_find_nodes (LILV_WORLD, s, p, fs);
  LilvNodes * nrs_matches =
    lilv_world_find_nodes (LILV_WORLD, s, p, nrs);

  lilv_nodes_free (nrs_matches);
  lilv_nodes_free (fs_matches);
  lilv_node_free (nrs);
  lilv_node_free (fs);
  lilv_node_free (p);

  return !fs_matches && !nrs_matches;
}

/**
 * Suil callback to get the index of the Lv2Port
 * corresponding to the given symbol.
 */
static uint32_t
get_port_index (
  SuilController controller,
  const char* symbol)
{
	Lv2Plugin* const  plugin = (Lv2Plugin*)controller;
	Lv2Port* port =
    lv2_port_get_by_symbol (
      plugin, symbol);

	return
    port ?
    port->index :
    LV2UI_INVALID_PORT_INDEX;
}

/**
 * Write events from the plugin's UI to the plugin.
 */
void
lv2_ui_write_events_from_ui_to_plugin (
  Lv2Plugin *    plugin,
  uint32_t       port_index,
  uint32_t       buffer_size,
  uint32_t       protocol, ///< format
  const void*    buffer)
{
  if (protocol != 0 &&
      protocol != PM_URIDS.atom_eventTransfer)
    {
      g_warning (
        "UI write with unsupported protocol %d (%s)",
        protocol,
        lv2_urid_unmap_uri (plugin, protocol));
      return;
    }

  if ((int) port_index >= plugin->num_ports)
    {
      g_warning (
        "UI write to out of range port index %d",
        port_index);
      return;
    }

  if (dump &&
      protocol == PM_URIDS.atom_eventTransfer)
    {
      const LV2_Atom* atom =
        (const LV2_Atom*)buffer;
      char * str  =
        sratom_to_turtle (
          plugin->sratom,
          &plugin->unmap,
          "plugin:",
          NULL, NULL,
          atom->type,
          atom->size,
          LV2_ATOM_BODY_CONST(atom));
      g_message (
        "## UI => Plugin (%u bytes) ##\n%s",
        atom->size, str);
      free(str);
    }

  char buf[sizeof(Lv2ControlChange) + buffer_size];
  Lv2ControlChange* ev = (Lv2ControlChange*)buf;
  ev->index    = port_index;
  ev->protocol = protocol;
  ev->size     = buffer_size;
  memcpy (ev->body, buffer, buffer_size);
  zix_ring_write (
    plugin->ui_to_plugin_events, buf,
    (uint32_t) sizeof(buf));
}

/**
 * Instantiates the plugin UI.
 */
void
lv2_ui_instantiate (
  Lv2Plugin *  plugin,
  const char * native_ui_type,
  void*        parent)
{
  plugin->ui_host =
    suil_host_new (
      (SuilPortWriteFunc)
      lv2_ui_write_events_from_ui_to_plugin,
      get_port_index, NULL, NULL);
  plugin->extuiptr = NULL;

  const char* bundle_uri =
    lilv_node_as_uri (
      lilv_ui_get_bundle_uri ( plugin->ui));
  const char* binary_uri =
    lilv_node_as_uri (
      lilv_ui_get_binary_uri (plugin->ui));
  char* bundle_path =
    lilv_file_uri_parse (bundle_uri, NULL);
  char* binary_path =
    lilv_file_uri_parse (binary_uri, NULL);

  const LV2_Feature data_feature = {
          LV2_DATA_ACCESS_URI, &plugin->ext_data
  };
  const LV2_Feature idle_feature = {
          LV2_UI__idleInterface, NULL
  };

  if (plugin->externalui)
    {
      const LV2_Feature external_lv_feature = {
        LV2_EXTERNAL_UI_DEPRECATED_URI, parent
      };
      const LV2_Feature external_kx_feature = {
        LV2_EXTERNAL_UI__Host, parent
      };
      const LV2_Feature instance_feature = {
        LV2_INSTANCE_ACCESS_URI,
        lilv_instance_get_handle(plugin->instance)
      };
      const LV2_Feature* ui_features[] = {
        &plugin->map_feature, &plugin->unmap_feature,
        &instance_feature,
        &data_feature,
        &idle_feature,
        &plugin->log_feature,
        &external_lv_feature,
        &external_kx_feature,
        &plugin->options_feature,
        NULL
      };

      plugin->ui_instance = suil_instance_new(
        plugin->ui_host,
        plugin,
        native_ui_type,
        lilv_node_as_uri(lilv_plugin_get_uri(plugin->lilv_plugin)),
        lilv_node_as_uri(lilv_ui_get_uri(plugin->ui)),
        lilv_node_as_uri(plugin->ui_type),
        bundle_path,
        binary_path,
        ui_features);

      if (plugin->ui_instance)
        {
          plugin->extuiptr =
            suil_instance_get_widget((SuilInstance*)plugin->ui_instance);
      }
      else
        {
          plugin->externalui = false;
        }
    }
  else
    {

      const LV2_Feature parent_feature = {
              LV2_UI__parent, parent
      };
      const LV2_Feature instance_feature = {
              LV2_INSTANCE_ACCESS_URI, lilv_instance_get_handle(plugin->instance)
      };
      const LV2_Feature* ui_features[] = {
              &plugin->map_feature, &plugin->unmap_feature,
              &instance_feature,
              &data_feature,
              &idle_feature,
              &plugin->log_feature,
              &parent_feature,
              &plugin->options_feature,
              NULL
      };

      plugin->ui_instance = suil_instance_new(
              plugin->ui_host,
              plugin,
              native_ui_type,
              lilv_node_as_uri(lilv_plugin_get_uri(plugin->lilv_plugin)),
              lilv_node_as_uri(lilv_ui_get_uri(plugin->ui)),
              lilv_node_as_uri(plugin->ui_type),
              bundle_path,
              binary_path,
              ui_features);
    }

  lilv_free(binary_path);
  lilv_free(bundle_path);

  if (!plugin->ui_instance)
    {
      g_warning ("Failed to get UI instance for %s",
                 plugin->plugin->descr->name);
    }
}

/**
 * Inits the LV2 plugin UI.
 */
void
lv2_ui_init (
  Lv2Plugin* plugin)
{
  // Set initial control port values
  for (int i = 0; i < plugin->num_ports; ++i)
    {
      if (plugin->ports[i].port->identifier.type ==
          TYPE_CONTROL)
        {
          lv2_gtk_ui_port_event (
            plugin, (uint32_t) i,
            sizeof(float), 0,
            &plugin->ports[i].control);
        }
    }

  if (plugin->control_in != -1)
    {
      // Send patch:Get message for initial parameters/etc
      LV2_Atom_Forge forge = plugin->forge;
      LV2_Atom_Forge_Frame frame;
      uint8_t              buf[1024];
      lv2_atom_forge_set_buffer (
        &forge, buf, sizeof(buf));
      lv2_atom_forge_object (
        &forge, &frame, 0, PM_URIDS.patch_Get);

      const LV2_Atom* atom =
        lv2_atom_forge_deref(&forge, frame.ref);
      lv2_ui_write_events_from_ui_to_plugin (
        plugin,
        (uint32_t) plugin->control_in,
        lv2_atom_total_size (atom),
        PM_URIDS.atom_eventTransfer,
        atom);
      lv2_atom_forge_pop(&forge, &frame);
    }
}
