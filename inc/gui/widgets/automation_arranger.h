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
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

/**
 * \file
 *
 * Automation arranger API.
 */

#ifndef __GUI_WIDGETS_AUTOMATION_ARRANGER_H__
#define __GUI_WIDGETS_AUTOMATION_ARRANGER_H__

#include "audio/position.h"
#include "gui/backend/tool.h"
#include "gui/backend/automation_selections.h"
#include "gui/widgets/arranger.h"
#include "gui/widgets/main_window.h"

#include <gtk/gtk.h>

typedef struct AutomationPoint AutomationPoint;
typedef struct AutomationCurve AutomationCurve;
typedef struct _AutomationPointWidget
  AutomationPointWidget;
typedef struct _AutomationCurveWidget
  AutomationCurveWidget;
typedef struct SnapGrid SnapGrid;
typedef struct AutomationTrack AutomationTrack;
typedef struct _RegionWidget RegionWidget;

/**
 * @addtogroup widgets
 *
 * @{
 */

#define MW_AUTOMATION_ARRANGER \
  MW_AUTOMATION_EDITOR_SPACE->arranger

/**
 * Create an AutomationPointat the given Position
 * in the given Track's AutomationTrack.
 *
 * @param pos The pre-snapped position.
 */
void
automation_arranger_widget_create_ap (
  ArrangerWidget * self,
  const Position *           pos,
  const double               start_y,
  ZRegion *                   region);

/**
 * Change curviness of selected curves.
 */
void
automation_arranger_widget_resize_curves (
  ArrangerWidget * self,
  double                     offset_y);

/**
 * @}
 */

#endif
