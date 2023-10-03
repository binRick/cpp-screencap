project('screencap', 'cpp',
  version : '0.1',
  default_options : ['warning_level=1', 'cpp_std=c++14'])

cmake = import('cmake')
sc = cmake.subproject('screen_capture_lite')
message('CMake targets:\n - ' + '\n - '.join(sc.target_list()))
sc_lib = sc.dependency('screen_capture_lite_shared')

deps = [
	sc_lib,
]

inc = [include_directories(
	'include',
)]

executable('screencap',
           [
		'screencap.cpp',
	   ],
           install : false,
	   include_directories: inc,
	   dependencies: deps,
)
