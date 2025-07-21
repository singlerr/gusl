#pragma once

#include <adwaita.h>

#include "gusl-settings.h"

G_BEGIN_DECLS

#define EDITOR_TYPE_WINDOW (editor_window_get_type ())

G_DECLARE_FINAL_TYPE (EditorWindow, editor_window, EDITOR, WINDOW, AdwApplicationWindow)

GtkWidget *editor_window_new (GtkApplication *application, gpointer *user_data);

G_END_DECLS
