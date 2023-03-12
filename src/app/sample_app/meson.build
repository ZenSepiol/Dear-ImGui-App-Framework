subdir('sample_module')

app_files = files(
    'main.cpp',
)

executable('sample_app', app_files + gui_files, include_directories : incdir, dependencies : [glfw_lib, gl] )