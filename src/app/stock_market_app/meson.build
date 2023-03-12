curl_lib = dependency('libcurl')

subdir('curl_wrapper')
subdir('spot_data')

app_files = files(
    'main.cpp',
)

executable('stock_market_app', app_files + gui_files, include_directories : incdir, dependencies : [glfw_lib, gl, curl_lib] )