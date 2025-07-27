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
#include <time.h>

#include "editor-window.h"
#include "glsl_parser.h"
#include "gusl-log.h"
#include "gusl-utils.h"
#include "shader_source.h"

struct _EditorWindow
{
  AdwApplicationWindow parent_instance;
  GtkWidget *menu_button;
  GtkWidget *log_revealer;
  GtkWidget *log_text_view;
};

typedef enum {
  LOG_INFO = 0,
  LOG_ERROR = 1,
  LOG_DEBUG = 2,
  LOG_WARN = 3,

  LEVEL_SIZE
} LogLevel;

static char *level_str[] = { "INFO", "ERROR", "DEBUG", "WARN" };

#define log_info(view, ...)  log_to_view (view, LOG_INFO, __VA_ARGS__)
#define log_err(view, ...)   log_to_view (view, LOG_ERROR, __VA_ARGS__)
#define log_debug(view, ...) log_to_view (view, LOG_DEBUG, __VA_ARGS__)
#define log_warn(view, ...)  log_to_view (view, LOG_WARN, __VA_ARGS__)

G_DEFINE_TYPE (EditorWindow, editor_window, ADW_TYPE_APPLICATION_WINDOW)

void log_to_view (GtkTextView *view, LogLevel level, const char *format, ...);
void toggle_log_panel (GtkButton *button, EditorWindow *win, gpointer user_data);
void editor_setup (gchar *path, gpointer user_data);
void error_cb (const char *str, int lineno, int first_col, int last_col, void *user_data);
void on_shader_load (ShaderSource *src, void *user_data);

void
editor_setup (gchar *path, void *user_data)
{
  EditorWindow *win = EDITOR_WINDOW (user_data);
  int ret;
  if ((ret = load_shader_source_async ((const char *)path, on_shader_load, user_data)) < 0)
    {
      log_err (GTK_TEXT_VIEW (win->log_text_view), "Could not load shader file.\n");
      return;
    }
}

void
toggle_log_panel (GtkButton *button, EditorWindow *win, gpointer user_data)
{
  GtkRevealer *revealer = GTK_REVEALER (win->log_revealer);
  gtk_revealer_set_reveal_child (revealer, !gtk_revealer_get_child_revealed (revealer));
}

void
on_shader_load (ShaderSource *src, void *user_data)
{
  GtkWindow *win = GTK_WINDOW (user_data);
  g_debug ("count: %d, path: %s\n", src->content.count, src->path);
  for (int i = 0; i < src->content.count; i++)
    {
      struct glsl_parse_context parse_ctx;
      ShaderFile content = src->content.files[i];
      glsl_parse_context_init (&parse_ctx);
      glsl_parse_set_error_cb (&parse_ctx, error_cb, (void *)win);

      if (glsl_parse_string (&parse_ctx, content.content))
        {
        }
      else
        {
          g_error ("Could not parse shader: %s\n", content.name);
        }
    }
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
  gtk_widget_class_bind_template_callback_full (widget_class, "toggle_log_panel", G_CALLBACK (toggle_log_panel));
  gtk_widget_class_bind_template_child (widget_class, EditorWindow, log_revealer);
  gtk_widget_class_bind_template_child (widget_class, EditorWindow, log_text_view);
}

static void
editor_window_init (EditorWindow *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));
}

GtkWidget *
editor_window_new (GtkApplication *application, gpointer user_data)
{
  g_assert (GTK_IS_APPLICATION (application));

  gpointer win = g_object_new (EDITOR_TYPE_WINDOW,
                               "application",
                               application,
                               NULL);
  editor_setup ((gchar *)user_data, win);

  return win;
}

void
error_cb (const char *str, int lineno, int first_col, int last_col, void *user_data)
{
  GtkWindow *win = GTK_WINDOW (user_data);
  EditorWindow *editor_win = EDITOR_WINDOW (win);
  g_debug ("GLSL parse error line %d(%d-%d): %s\n", lineno, first_col, last_col, str);
  log_err (GTK_TEXT_VIEW (editor_win->log_text_view), "GLSL parse error line %d(%d-%d): %s\n", lineno, first_col, last_col, str);
}

void
log_to_view (GtkTextView *view, LogLevel level, const char *format, ...)
{
  GtkTextBuffer *tb;
  time_t t;
  GtkTextIter start_it, end_it;
  va_list argptr;
  va_start (argptr, format);
  GString *buf = g_string_new (NULL);

  time (&t);
  if (level >= LOG_INFO && level < LEVEL_SIZE)
    {
      g_string_printf (buf, "%s - [%s] ", ctime (&t), level_str[level]);
    }
  else
    {
      g_error ("Could not determine log level str - out of range\n");
      g_string_printf (buf, "%s ", ctime (&t));
    }


  g_string_append_vprintf (buf, format, argptr);
  va_end (argptr);

  tb = gtk_text_view_get_buffer (view);
  gtk_text_buffer_get_bounds (tb, &start_it, &end_it);
  gtk_text_buffer_insert (tb, &end_it, buf->str, buf->len);

  g_string_free (buf, TRUE);
}