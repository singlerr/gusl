/* gusl-window.c
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

#define G_LOG_DOMAIN "gusl-window"

#ifdef HAVE_CONFIG_H
# include "config.h"
# include "version.h"
#endif

#include <glib/gi18n.h>

#include "gusl-window.h"
#include "gusl-log.h"

struct _GuslWindow
{
  AdwApplicationWindow  parent_instance;

  GuslSettings          *settings;

  GtkWidget            *menu_button;
};

G_DEFINE_TYPE (GuslWindow, gusl_window, ADW_TYPE_APPLICATION_WINDOW)


enum {
  PROP_0,
  PROP_SETTINGS,
  N_PROPS
};

static GParamSpec *properties[N_PROPS];

static void
gusl_window_show_about (GuslWindow *self)
{
  GtkWidget *about;
  const char *developers[] = {
    "singlerr https://www.sadiqpk.org",
    NULL
  };

  g_assert (GUSL_IS_WINDOW (self));

  about = g_object_new (ADW_TYPE_ABOUT_WINDOW,
                        "transient-for", self,
                        "application-name", _("gusl"),
                        "application-icon", PACKAGE_ID,
                        "developer-name", "singlerr",
                        "copyright", _("Copyright © 2024 singlerr"),
                        "license-type", GTK_LICENSE_GPL_3_0,
                        "version", PACKAGE_VERSION,
                        "website", "https://www.sadiqpk.org/projects/gusl.html",
                        "developers", developers,
                        "translator-credits", _("translator-credits"),
                        NULL);

  gtk_window_present (GTK_WINDOW (about));
}

static void
gusl_window_unmap (GtkWidget *widget)
{
  GuslWindow *self = (GuslWindow *)widget;
  GtkWindow *window = (GtkWindow *)widget;
  GdkRectangle geometry;
  gboolean is_maximized;

  is_maximized = gtk_window_is_maximized (window);
  gusl_settings_set_window_maximized (self->settings, is_maximized);

  if (!is_maximized)
    {
      g_object_get (self, "default-width", &geometry.width, NULL);
      g_object_get (self, "default-height", &geometry.height, NULL);
      gusl_settings_set_window_geometry (self->settings, &geometry);
    }

  GTK_WIDGET_CLASS (gusl_window_parent_class)->unmap (widget);
}

static void
gusl_window_set_property (GObject      *object,
                         guint         prop_id,
                         const GValue *value,
                         GParamSpec   *pspec)
{
  GuslWindow *self = (GuslWindow *)object;

  switch (prop_id)
    {
    case PROP_SETTINGS:
      self->settings = g_value_dup_object (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gusl_window_constructed (GObject *object)
{
  GuslWindow *self = (GuslWindow *)object;
  GtkWindow *window = (GtkWindow *)object;
  GdkRectangle geometry;

  gusl_settings_get_window_geometry (self->settings, &geometry);
  gtk_window_set_default_size (window, geometry.width, geometry.height);

  if (gusl_settings_get_window_maximized (self->settings))
    gtk_window_maximize (window);

  G_OBJECT_CLASS (gusl_window_parent_class)->constructed (object);
}

static void
gusl_window_finalize (GObject *object)
{
  GuslWindow *self = (GuslWindow *)object;

  GUSL_TRACE_MSG ("finalizing window");

  g_object_unref (self->settings);

  G_OBJECT_CLASS (gusl_window_parent_class)->finalize (object);
}

static void
gusl_window_class_init (GuslWindowClass *klass)
{
  GObjectClass   *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->set_property = gusl_window_set_property;
  object_class->constructed  = gusl_window_constructed;
  object_class->finalize     = gusl_window_finalize;

  widget_class->unmap = gusl_window_unmap;

  /**
   * GuslWindow:settings:
   *
   * The Application Settings
   */
  properties[PROP_SETTINGS] =
    g_param_spec_object ("settings",
                         "Settings",
                         "The Application Settings",
                         GUSL_TYPE_SETTINGS,
                         G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS);

  g_object_class_install_properties (object_class, N_PROPS, properties);

  gtk_widget_class_set_template_from_resource (widget_class,
                                               "/io/github/singlerr/gusl/"
                                               "ui/gusl-window.ui");

  gtk_widget_class_bind_template_child (widget_class, GuslWindow, menu_button);

  gtk_widget_class_install_action (widget_class, "win.about", NULL,
                                   (GtkWidgetActionActivateFunc)gusl_window_show_about);
}

static void
gusl_window_init (GuslWindow *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));
}

GtkWidget *
gusl_window_new (GtkApplication *application,
                GuslSettings    *settings)
{
  g_assert (GTK_IS_APPLICATION (application));
  g_assert (GUSL_IS_SETTINGS (settings));

  return g_object_new (GUSL_TYPE_WINDOW,
                       "application", application,
                       "settings", settings,
                       NULL);
}
