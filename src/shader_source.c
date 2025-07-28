#include "shader_source.h"
#include <adwaita.h>
#include <glib.h>
#include <glib/gi18n.h>
#include <string.h>
#include <zip.h>

struct _ShaderResolver
{
  const char type_name[1024];
  ShaderType type;
};

struct _ShaderResolver _shader_resolvers[] = {
  { .type_name = "application/zip", .type = SHADER_TYPE_PACK },
  { .type_name = "text/plain", .type = SHADER_TYPE_FILE },
  { .type_name = "text/x-matlab", .type = SHADER_TYPE_FILE },
  { .type_name = "text/x-csrc", .type = SHADER_TYPE_FILE }
};


void shader_file_new_async (GFile *file, GFileInfo *info, const char *content_type, ShaderLoadCallback callback, void *user_data);
void shader_pack_new_async (GFile *file, GFileInfo *info, ShaderLoadCallback callback, void *user_data);
void
shader_async_read_text (GObject *source_object,
                        GAsyncResult *res,
                        gpointer data);

int
load_shader_source_async (const char *path, ShaderLoadCallback callback, void *user_data)
{
  GFile *file = g_file_new_for_path (path);
  GFileInfo *file_info;
  GError *error = NULL;
  struct _ShaderResolver *loader = NULL;
  char *content_type;
  ShaderSource *shader_source;
  if (!file)
    {
      g_error ("Could not open file for %s\n", path);
      return -1;
    }

  file_info = g_file_query_info (file, "standard::*", G_FILE_QUERY_INFO_NONE, NULL, &error);
  if (error)
    {
      g_error ("Could not query file info for %s - %s\n", path, error->message);
      return -1;
    }

  if (g_file_info_has_attribute (file_info, G_FILE_ATTRIBUTE_STANDARD_CONTENT_TYPE))
    {
      content_type = (char *)g_file_info_get_content_type (file_info);
    }
  else
    {
      g_warning ("File %s does not have content type attributes. Assuming it plain text\n", path);
      content_type = "text/plain";
    }

  size_t s = sizeof (_shader_resolvers) / sizeof (struct _ShaderResolver);

  for (size_t i = 0; i < s; i++)
    {
      struct _ShaderResolver resolver = _shader_resolvers[i];
      if (strcmp (resolver.type_name, content_type) == 0)
        {
          g_debug ("Found shader resolver for %s\n", content_type);
          loader = &resolver;
          break;
        }
    }

  if (!loader)
    {
      g_error ("Could not find suitable shader loader for %s of %s\n", content_type, path);
      return -1;
    }

  shader_source = g_new (ShaderSource, 1);

  strcpy (shader_source->path, path);
  shader_source->type = loader->type;

  switch (loader->type)
    {
    case SHADER_TYPE_FILE:
      shader_file_new_async (file, file_info, content_type, callback, user_data);
      break;
    case SHADER_TYPE_PACK:
    default:
      shader_pack_new_async (file, file_info, callback, user_data);
      break;
    }
  return 0;
}

struct _AsyncData
{
  GFile *file;
  GFileInfo *info;
  ShaderSource *out;
  char *content_type;
  ShaderLoadCallback callback;
  void *callback_user_data;
};

void
shader_async_read_text (GObject *source_object,
                        GAsyncResult *res,
                        gpointer data)
{
  struct _AsyncData *params = (struct _AsyncData *)data;
  GError *err = NULL;
  GFileInputStream *ins = g_file_read_finish (params->file, res, &err);
  if (!ins)
    {
      g_error ("Could not open file for %s\n", g_file_get_path (params->file));
      goto cleanup;
    }

  if (err)
    {
      g_error ("Could not open file: %s\n", err->message);
      goto cleanup;
    }

  goffset size = g_file_info_get_size (params->info);
  char *text = (char *)malloc (sizeof (char) * size);
  gsize actual_read;
  if (!g_input_stream_read_all (G_INPUT_STREAM (ins), (void *)text, (gsize)size, &actual_read, NULL, &err))
    {
      if (err)
        {
          g_error ("Could not read contents: %s\n", err->message);
          g_free (text);
        }
      goto cleanup;
    }

  if (actual_read != (gsize)size)
    {
      g_error ("File size mismatch; expected: %lu, actual: %lu\n", size, actual_read);
      g_free (text);
      goto cleanup;
    }

  ShaderFile *single_file = g_new (ShaderFile, 1);
  single_file->content = text;
  strcpy (single_file->name, (const char *)g_file_get_basename (params->file));
  strcpy (single_file->content_type, params->content_type);

  ShaderPack *out = &params->out->content;

  out->count = 1;
  out->files = single_file;

  params->callback (params->out, params->callback_user_data);
cleanup:
  g_clear_error (&err);
}

void
shader_file_new_async (GFile *file, GFileInfo *info, const char *content_type, ShaderLoadCallback callback, void *user_data)
{
  ShaderSource *source;
  source = g_new (ShaderSource, 1);
  char *path = g_file_get_path (file);
  strcpy (source->path, path);
  source->type = SHADER_TYPE_FILE;

  struct _AsyncData *params = g_new (struct _AsyncData, 1);
  params->file = file;
  params->info = info;
  params->callback = callback;
  params->content_type = (char *)content_type;
  params->out = source;
  params->callback_user_data = user_data;
  g_file_read_async (file, G_PRIORITY_DEFAULT, NULL, shader_async_read_text, params);
}

void
shader_pack_new_async (GFile *file, GFileInfo *info, ShaderLoadCallback callback, void *user_data)
{
  g_autofree gchar *path = g_file_get_path (file);
  zip_stat_t statbuf;
  zip_file_t *fd;
  zip_t *za;
  zip_int64_t len, total_count;
  ShaderSource *out;
  ShaderFile *contents;
  char *buf;
  int count = 0;
  int err;
  if ((za = zip_open (path, ZIP_RDONLY, &err)) == NULL)
    {
      g_error ("Could not open zip file: %s\n", zip_error_strerror ((zip_error_t *)&err));
      return;
    }

  zip_stat_init (&statbuf);

  total_count = zip_get_num_entries (za, ZIP_FL_UNCHANGED);
  contents = g_new (ShaderFile, total_count);
  while (zip_stat_index (za, count, 0, &statbuf) == 0)
    {
      // skip directory
      if (strlen (statbuf.name) > 0 && statbuf.name[strlen (statbuf.name)] == '/')
        {
          count++;
          continue;
        }
      fd = zip_fopen_index (za, count, 0);
      buf = (char *)calloc (statbuf.size + 1, sizeof (char));

      if ((len = zip_fread (fd, buf, statbuf.size)) < 0)
        {
          g_free (buf);
        }
      else
        {
          ShaderFile *s = &contents[count];
          s->content = buf;
          strcpy (s->name, statbuf.name);
          gboolean uncertain;
          char *content_type = (char *)g_content_type_guess (statbuf.name, (const guchar *)buf, statbuf.size + 1, &uncertain);
          strcpy (s->content_type, uncertain ? "unknown" : content_type);
          g_free (content_type);
        }
      zip_fclose (fd);
      count++;
    }

  out = g_new (ShaderSource, 1);
  out->content.count = count;
  out->content.files = contents;
  strcpy (out->path, path);

  callback (out, user_data);
  if (zip_close (za) != 0)
    {
      g_error ("Could not close zip file\n");
    }
}