/* gusl-settings.h
 *
 * Copyright 2024 singlerr <singlerr@naver.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author(s):
 *   singlerr <singlerr@naver.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GUSL_TYPE_SETTINGS (gusl_settings_get_type ())

G_DECLARE_FINAL_TYPE (GuslSettings, gusl_settings, GUSL, SETTINGS, GObject)

GuslSettings *gusl_settings_new                  (void);
void         gusl_settings_save                 (GuslSettings *self);

gboolean     gusl_settings_get_is_first_run     (GuslSettings *self);
gboolean     gusl_settings_get_window_maximized (GuslSettings *self);
void         gusl_settings_set_window_maximized (GuslSettings *self,
                                                gboolean     maximized);
void         gusl_settings_get_window_geometry  (GuslSettings  *self,
                                                GdkRectangle *geometry);
void         gusl_settings_set_window_geometry  (GuslSettings  *self,
                                                GdkRectangle *geometry);
G_END_DECLS
