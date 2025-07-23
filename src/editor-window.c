/* editor-window.c
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

#define G_LOG_DOMAIN "editor-window"

#ifdef HAVE_CONFIG_H
# include "config.h"
# include "version.h"
#endif

#include <glib/gi18n.h>

#include "editor-window.h"
#include "gusl-log.h"

struct _EditorWindow
{
  AdwApplicationWindow parent_instance;
  GtkWidget *menu_button;
};

G_DEFINE_TYPE (EditorWindow, editor_window, ADW_TYPE_APPLICATION_WINDOW)


void editor_setup (gchar *path);

void
editor_setup (gchar *path)
{
  GFile *file = g_file_new_for_path (path);
}

static void
editor_window_constructed (GObject *object)
{
  EditorWindow *self = (EditorWindow *)object;
  GtkWindow *window = (GtkWindow *)object;

  G_OBJECT_CLASS (editor_window_parent_class)->constructed (object);
}

static void
editor_window_finalize (GObject *object)
{
  GUSL_TRACE_MSG ("finalizing window");

  G_OBJECT_CLASS (editor_window_parent_class)->finalize (object);
}

static void
editor_window_class_init (EditorWindowClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);
  object_class->constructed = editor_window_constructed;
  object_class->finalize = editor_window_finalize;

  gtk_widget_class_set_template_from_resource (widget_class,
                                               "/io/github/singlerr/gusl/"
                                               "ui/editor-window.ui");
}

static void
editor_window_init (EditorWindow *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));
}

GtkWidget *
editor_window_new (GtkApplication *application, gpointer *user_data)
{
  g_assert (GTK_IS_APPLICATION (application));
  return g_object_new (EDITOR_TYPE_WINDOW,
                       "application",
                       application,
                       NULL);
}
