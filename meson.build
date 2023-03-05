project('App Framework with Dear ImGui', 'cpp')

incdir = include_directories('/imgui/backends', '/implot', '/imgui', 'src/lib')

glfw_lib = subproject('glfw').get_variable('glfw_dep')
gl = dependency('gl')
catch_lib = subproject('catch2').get_variable('catch2_dep')

subdir('src/app/sample_app')
subdir('src/app/stock_market_app')