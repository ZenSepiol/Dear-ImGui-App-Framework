
test_files = files(
    'sample_module.cpp',
    'sample_module_test.cpp'
)

test_exe = executable('sample_module_test', test_files, include_directories : incdir, dependencies : [catch_lib])
test('sample_module_test', test_exe)