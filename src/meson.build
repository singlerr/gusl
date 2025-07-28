src = []

revision_tag = vcs_tag(
  input: 'version.h.in',
  output: 'version.h',
)

pkg_dep += declare_dependency(sources: revision_tag)
pkg_dep += dependency('gtknodes')
pkg_dep += dependency('glslang')

resources = gnome.compile_resources(
  'resources',
  'resources/gusl.gresource.xml',
  source_dir: 'resources',
  c_name: '_gusl',
)

libsrc = [
  'gusl-log.c',
  'gusl-utils.c',
  'gusl-settings.c',
  'gusl-window.c',
  'editor-window.c',
  'shader_source.c',
]

libgusl_shared = shared_library(
  'gusl',
  [libsrc, 'library.c'],
  install: false,
  include_directories: top_inc,
  dependencies: pkg_dep,
)

libgusl_static = static_library(
  'gusl',
  libsrc,
  include_directories: top_inc,
  dependencies: pkg_dep,
)

ui_files = files(
  'resources/gtk/help-overlay.ui',
  'resources/ui/editor-window.ui',
  'resources/ui/gusl-window.ui',
  'resources/ui/startup-window.ui',
)

gtk_builder_tool = find_program('gtk4-builder-tool', required: false)
if gtk_builder_tool.found()
  preload_env = 'LD_PRELOAD=libadwaita-1.so:@0@'.format(libgusl_shared.full_path())
  foreach file : ui_files
    test(
      'Validate @0@'.format(file),
      gtk_builder_tool,
      env: [preload_env],
      args: ['validate', file],
    )
  endforeach
endif

src += ['main.c', 'gusl-application.c', revision_tag, resources]

executable(
  'gusl',
  src,
  install: true,
  win_subsystem: 'windows',
  link_with: libgusl_static,
  include_directories: top_inc,
  dependencies: pkg_dep,
)