/* gusl-settings.c
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

#define G_LOG_DOMAIN "gusl-settings"

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "gusl-settings.h"
#include "gusl-log.h"

/**
 * SECTION: gusl-settings
 * @title: GuslSettings
 * @short_description: The Application settings
 * @include: "gusl-settings.h"
 *
 * A class that handles application specific settings, and
 * to store them to disk.
 */

struct _GuslSettings
{
  GObject    parent_instance;

  GSettings *app_settings;
  gboolean   first_run;
};

G_DEFINE_TYPE (GuslSettings, gusl_settings, G_TYPE_OBJECT)


static void
gusl_settings_dispose (GObject *object)
{
  GuslSettings *self = (GuslSettings *)object;

  GUSL_TRACE_MSG ("disposing settings");

  g_settings_set_string (self->app_settings, "version", PACKAGE_VERSION);
  g_settings_apply (self->app_settings);

  G_OBJECT_CLASS (gusl_settings_parent_class)->dispose (object);
}

static void
gusl_settings_class_init (GuslSettingsClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->dispose = gusl_settings_dispose;
}

static void
gusl_settings_init (GuslSettings *self)
{
  g_autofree char *version = NULL;

  self->app_settings = g_settings_new (PACKAGE_ID);
  version = g_settings_get_string (self->app_settings, "version");

  if (!g_str_equal (version, PACKAGE_VERSION))
    self->first_run = TRUE;

  g_settings_delay (self->app_settings);
}

/**
 * gusl_settings_new:
 *
 * Create a new #GuslSettings
 *
 * Returns: (transfer full): A #GuslSettings.
 * Free with g_object_unref().
 */
GuslSettings *
gusl_settings_new (void)
{
  return g_object_new (GUSL_TYPE_SETTINGS, NULL);
}

/**
 * gusl_settings_save:
 * @self: A #GuslSettings
 *
 * Save modified settings to disk.  By default,
 * the modified settings are saved to disk only
 * when #GuslSettings is disposed.  Use this
 * to force save to disk.
 */
void
gusl_settings_save (GuslSettings *self)
{
  g_return_if_fail (GUSL_IS_SETTINGS (self));

  g_settings_apply (self->app_settings);
}

/**
 * gusl_settings_get_is_first_run:
 * @self: A #GuslSettings
 *
 * Get if the application has ever launched after install
 * or update.
 *
 * Returns: %TRUE for the first launch of application after
 * install or update.  %FALSE otherwise.
 */
gboolean
gusl_settings_get_is_first_run (GuslSettings *self)
{
  g_return_val_if_fail (GUSL_IS_SETTINGS (self), FALSE);

  return self->first_run;
}

/**
 * gusl_settings_get_window_maximized:
 * @self: A #GuslSettings
 *
 * Get the window maximized state as saved in @self.
 *
 * Returns: %TRUE if maximized.  %FALSE otherwise.
 */
gboolean
gusl_settings_get_window_maximized (GuslSettings *self)
{
  g_return_val_if_fail (GUSL_IS_SETTINGS (self), FALSE);

  return g_settings_get_boolean (self->app_settings, "window-maximized");
}

/**
 * gusl_settings_set_window_maximized:
 * @self: A #GuslSettings
 * @maximized: The window state to save
 *
 * Set the window maximized state in @self.
 */
void
gusl_settings_set_window_maximized (GuslSettings *self,
                                   gboolean     maximized)
{
  g_return_if_fail (GUSL_IS_SETTINGS (self));

  g_settings_set_boolean (self->app_settings, "window-maximized", !!maximized);
}

/**
 * gusl_settings_get_window_geometry:
 * @self: A #GuslSettings
 * @geometry: (out): A #GdkRectangle
 *
 * Get the window geometry as saved in @self.
 */
void
gusl_settings_get_window_geometry (GuslSettings  *self,
                                  GdkRectangle *geometry)
{
  g_return_if_fail (GUSL_IS_SETTINGS (self));
  g_return_if_fail (geometry != NULL);

  g_settings_get (self->app_settings, "window-size", "(ii)",
                  &geometry->width, &geometry->height);
  geometry->x = geometry->y = -1;
}

/**
 * gusl_settings_set_window_geometry:
 * @self: A #GuslSettings
 * @geometry: A #GdkRectangle
 *
 * Set the window geometry in @self.
 */
void
gusl_settings_set_window_geometry (GuslSettings  *self,
                                  GdkRectangle *geometry)
{
  g_return_if_fail (GUSL_IS_SETTINGS (self));
  g_return_if_fail (geometry != NULL);

  g_settings_set (self->app_settings, "window-size", "(ii)",
                  geometry->width, geometry->height);
}
