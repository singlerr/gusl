/* gusl-log.h
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

#include <glib.h>

#ifndef GUSL_LOG_LEVEL_TRACE
# define GUSL_LOG_LEVEL_TRACE ((GLogLevelFlags)(1 << G_LOG_LEVEL_USER_SHIFT))
# define GUSL_LOG_DETAILED ((GLogLevelFlags)(1 << (G_LOG_LEVEL_USER_SHIFT + 1)))
#endif

#define GUSL_TRACE_MSG(fmt, ...)                         \
  gusl_log (G_LOG_DOMAIN, GUSL_LOG_LEVEL_TRACE,           \
           NULL, __FILE__, G_STRINGIFY (__LINE__),      \
           G_STRFUNC, fmt, ##__VA_ARGS__)
#define GUSL_TRACE(value, fmt, ...)                      \
  gusl_log (G_LOG_DOMAIN,                                \
           GUSL_LOG_LEVEL_TRACE,                         \
           value, __FILE__, G_STRINGIFY (__LINE__),     \
           G_STRFUNC, fmt, ##__VA_ARGS__)
#define GUSL_TRACE_DETAILED(value, fmt, ...)             \
  gusl_log (G_LOG_DOMAIN,                                \
           GUSL_LOG_LEVEL_TRACE | GUSL_LOG_DETAILED,      \
           value, __FILE__, G_STRINGIFY (__LINE__),     \
           G_STRFUNC, fmt, ##__VA_ARGS__)
#define GUSL_DEBUG_MSG(fmt, ...)                         \
  gusl_log (G_LOG_DOMAIN,                                \
           G_LOG_LEVEL_DEBUG | GUSL_LOG_DETAILED,        \
           NULL, __FILE__, G_STRINGIFY (__LINE__),      \
           G_STRFUNC, fmt, ##__VA_ARGS__)
#define GUSL_DEBUG(value, fmt, ...)                      \
  gusl_log (G_LOG_DOMAIN,                                \
           G_LOG_LEVEL_DEBUG | GUSL_LOG_DETAILED,        \
           value, __FILE__, G_STRINGIFY (__LINE__),     \
           G_STRFUNC, fmt, ##__VA_ARGS__)
#define GUSL_WARNING(value, fmt, ...)                    \
  gusl_log (G_LOG_DOMAIN,                                \
           G_LOG_LEVEL_WARNING | GUSL_LOG_DETAILED,      \
           value, __FILE__, G_STRINGIFY (__LINE__),     \
           G_STRFUNC, fmt, ##__VA_ARGS__)

void         gusl_log_init               (void);
void         gusl_log_increase_verbosity (void);
int          gusl_log_get_verbosity      (void);
void         gusl_log_to_file            (const char     *file_path,
                                         gboolean        append);
const char  *gusl_log_bool_str           (gboolean        value,
                                         gboolean        use_success);
void         gusl_log                    (const char     *domain,
                                         GLogLevelFlags  log_level,
                                         const char     *value,
                                         const char     *file,
                                         const char     *line,
                                         const char     *func,
                                         const char     *message_format,
                                         ...) G_GNUC_PRINTF (7, 8);
void         gusl_log_anonymize_value    (GString        *str,
                                         const char     *value);
