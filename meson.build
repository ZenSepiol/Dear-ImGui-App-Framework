project('App Framework with Dear ImGui', 'cpp')

incdir = include_directories('/imgui/backends', '/implot', '/imgui', 'src/lib')

glfw_lib = subproject('glfw').get_variable('glfw_dep')
gl = dependency('gl')
catch_lib = subproject('catch2').get_variable('catch2_with_main_dep')

gui_files = files(
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

subdir('src/app/clock')
subdir('src/app/long_task')
subdir('src/app/multithreading')
subdir('src/app/roombot')
subdir('src/app/sample_app')
subdir('src/app/stock_market_app')
subdir('src/app/tetris')
subdir('src/app/thread_pool')
subdir('src/app/tictactoe')

subdir('src/lib/thread_pool')