
test_files = files(
    'curl_wrapper_test.cpp'
)

test_exe = executable('curl_wrapper_test', test_files, include_directories : incdir, dependencies : [catch_lib, curl_lib])
test('curl_wrapper_test', test_exe)