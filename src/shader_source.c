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
  char *contents[CONTENT_MAX];
};

struct _ShaderSource
{
  ShaderType type;
  char path[PATH_MAX];

  union _source {
    ShaderFile *file;
    ShaderPack *pack;
  } source;
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

ShaderFile *shader_file_new (GFile *file);
ShaderPack *shader_pack_new (GFile *file);

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

  shader_source = (ShaderSource *)g_malloc (sizeof (ShaderSource));

  strcpy (shader_source->path, path);
  shader_source->type = loader->type;

  switch (loader->type)
    {
    case SHADER_TYPE_FILE:
      shader_source->source.file = shader_file_new (file);
      break;
    case SHADER_TYPE_PACK:
      shader_source->source.pack = shader_pack_new (file);
      break;
    default:
      break;
    }

  g_object_unref (file);
  g_object_unref (file_info);

  return shader_source;
}

ShaderFile *
shader_file_new (GFile *file)
{
  return NULL;
}

ShaderPack *
shader_pack_new (GFile *file)
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