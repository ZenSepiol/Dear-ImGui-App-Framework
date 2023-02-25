project('App Framework with Dear ImGui', 'cpp')

incdir = include_directories('/imgui/backends', '/implot', '/imgui')

glfw_lib = subproject('glfw').get_variable('glfw_dep')
gl = dependency('gl')
catch_lib = subproject('catch2').get_variable('catch2_dep')
curl_lib = dependency('libcurl')

subdir('src/sample_module')
subdir('src/curl_wrapper')
subdir('src/spot_data')

app_files = files(
    'src/main.cpp',
    '/imgui/imgui.cpp',
    '/imgui/imgui_draw.cpp',
    '/imgui/imgui_tables.cpp',
    '/imgui/imgui_demo.cpp',
    '/imgui/imgui_widgets.cpp',
    '/imgui/backends/imgui_impl_glfw.cpp',
    '/imgui/backends/imgui_impl_opengl3.cpp',
    '/implot/implot.cpp',
    '/implot/implot_items.cpp'
)

executable('app', app_files, include_directories : incdir, dependencies : [glfw_lib, gl, curl_lib] )