#pragma once

#ifndef PATH_MAX
# define PATH_MAX 4096
#endif

struct _ShaderFile;
struct _ShaderPack;
struct _ShaderSource;

typedef struct _ShaderFile ShaderFile;
typedef struct _ShaderPack ShaderPack;
typedef struct _ShaderSource ShaderSource;

typedef enum {
  SHADER_TYPE_FILE,
  SHADER_TYPE_PACK
} ShaderType;

ShaderSource *shader_source_new (const char *path);