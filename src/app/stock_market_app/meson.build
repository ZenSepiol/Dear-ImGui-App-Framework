curl_lib = dependency('libcurl')

subdir('curl_wrapper')
subdir('spot_data')

app_files = files(
    'main.cpp',
    '../../../imgui/imgui.cpp',
    '../../../imgui/imgui_draw.cpp',
    '../../../imgui/imgui_tables.cpp',
    '../../../imgui/imgui_demo.cpp',
    '../../../imgui/imgui_widgets.cpp',
    '../../../imgui/backends/imgui_impl_glfw.cpp',
    '../../../imgui/backends/imgui_impl_opengl3.cpp',
    '../../../implot/implot.cpp',
    '../../../implot/implot_items.cpp'
)

executable('stock_market_app', app_files, include_directories : incdir, dependencies : [glfw_lib, gl, curl_lib] )