/* library.c
 *
 * Copyright 2024 singlerr <singlerr@naver.com>
 *
 * Author(s):
 *   singlerr <singlerr@naver.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later OR CC0-1.0
 */

#include <adwaita.h>

#include "gusl-window.h"

void __attribute__((constructor)) ensure_types (void);

void
__attribute__((constructor)) ensure_types (void)
{
  gtk_init ();
  adw_init ();

  /* fixme: Why do gtk warn with the following code? */
  /* g_type_ensure (GUSL_TYPE_WINDOW); */
}
