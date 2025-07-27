/* gusl-utils.h
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


#include <adwaita.h>
#include <glib.h>

G_BEGIN_DECLS

extern void
_adw_destroy_dialog (AdwAlertDialog *dialog,
                     GAsyncResult *result);

#define GUSL_IS_MAIN_THREAD() (g_thread_self () == gusl_utils_get_main_thread ())
#define ADW_DESTROY_DIALOG    (G_CALLBACK (_adw_destroy_dialog))

GThread *gusl_utils_get_main_thread (void);
AdwAlertDialog *adw_alert_dialog_new_yes_or_no (const char *title, const char *body, GCallback yes_callback, GCallback no_callback);
AdwAlertDialog *adw_alert_dialog_new_ok (const char *title, const char *body, GCallback ok_callback);
AdwMessageDialog *adw_message_dialog_new_ok (GtkWindow *parent, const char *title, const char *body, GCallback ok_callback);

G_END_DECLS
