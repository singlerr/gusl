#pragma once

#ifndef PATH_MAX
# define PATH_MAX 4096
#endif

#ifndef CONTENT_MAX
# define CONTENT_MAX 256
#endif

#ifndef TYPE_MAX
# define TYPE_MAX 1024
#endif

typedef enum {
  SHADER_TYPE_FILE,
  SHADER_TYPE_PACK
} ShaderType;

typedef struct _ShaderFile
{
  char name[PATH_MAX];
  char content_type[TYPE_MAX];
  char *content;
} ShaderFile;

typedef struct _ShaderPack
{
  int count;
  ShaderFile *files;
} ShaderPack;

typedef struct _ShaderSource
{
  ShaderType type;
  char path[PATH_MAX];
  ShaderPack content;
} ShaderSource;

typedef struct _ShaderFile ShaderFile;
typedef struct _ShaderPack ShaderPack;
typedef struct _ShaderSource ShaderSource;

typedef void (*ShaderLoadCallback) (ShaderSource *, void *);


int load_shader_source_async (const char *path, ShaderLoadCallback callback, void *user_data);