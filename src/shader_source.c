#include "shader_source.h"
#include <adwaita.h>
#include <glib.h>
#include <glib/gi18n.h>
#include <string.h>
#include <zip.h>

#ifndef CONTENT_MAX
# define CONTENT_MAX 256
#endif

#ifndef TYPE_MAX
# define TYPE_MAX 1024
#endif

struct _ShaderFile
{
  char content_type[TYPE_MAX];
  char *content;
};

struct _ShaderPack
{
  int count;
  ShaderFile *files;
};

struct _ShaderSource
{
  ShaderType type;
  char path[PATH_MAX];
  ShaderPack content;
};

struct _ShaderResolver
{
  const char type_name[1024];
  ShaderType type;
};

struct _ShaderResolver _shader_resolvers[] = {
  { .type_name = "application/zip", .type = SHADER_TYPE_PACK },
  { .type_name = "text/plain", .type = SHADER_TYPE_FILE },
  { .type_name = "text/x-matlab", .type = SHADER_TYPE_FILE }
};

void shader_file_new (GFile *file, GFileInfo *info, ShaderFile *out);
void shader_pack_new (GFile *file, GFileInfo *info, ShaderPack *out);

ShaderSource *
shader_source_new (const char *path)
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
      return NULL;
    }

  file_info = g_file_query_info (file, "standard::*", G_FILE_QUERY_INFO_NONE, NULL, &error);
  if (error)
    {
      g_error ("Could not query file info for %s - %s\n", path, error->message);
      return NULL;
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
      return NULL;
    }

  shader_source = g_new (ShaderSource, 1);

  strcpy (shader_source->path, path);
  shader_source->type = loader->type;
  // TODO: for single shader file, create psuedo shader pack and its content is only that file, for archive file, create shader pack
  return shader_source;
}

struct _AsyncData
{
  GFile *file;
  GFileInfo *info;
  char **contents;
};

void
shader_async_read_text (GObject *source_object,
                        GAsyncResult *res,
                        gpointer data)
{
  struct _AsyncData *params = (struct _AsyncData *)data;
  GError *err = NULL;
  GFileInputStream *ins = g_file_read_finish (params->file, res, err);
  if (!ins)
    {
      g_error ("Could not read contents for %s\n", g_file_get_path (params->file));
      goto cleanup;
    }

  g_file_input_stream_query_info ();
  g_input_stream_read_all (ins, );
cleanup:
  g_object_unref (params->file);
  g_object_unref (params);
  g_clear_error (err);
}

void
shader_file_new (GFile *file, GFileInfo *info, ShaderFile *out)
{
  struct _AsyncData *params = g_new (struct _AsyncData, 1);
  params->file = file;
  params->contents = &out->content;
  g_file_read_async (file, G_PRIORITY_DEFAULT, NULL, G_CALLBACK (shader_async_read_text), params);
  return NULL;
}

void
shader_pack_new (GFile *file, ShaderPack *out)
{
  g_autofree gchar *path = g_file_get_path (file);
  int err;
  zip_source_t *src;
  zip_t *za;
  if ((za = zip_open (path, ZIP_RDONLY, &err)) == NULL)
    {
      g_error ("Could not open zip file ");
      return NULL;
    }

  return NULL;
}