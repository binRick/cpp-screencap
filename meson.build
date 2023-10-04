project('screencap', 'cpp',
  version : '0.1',
  default_options : ['warning_level=1', 'cpp_std=c++14'])

cmake = import('cmake')
sc = cmake.subproject('screen_capture_lite')

jansson_project = subproject('jansson')

message('Screen Capture Lite CMake targets:\n - ' + '\n - '.join(sc.target_list()))

sc_dep = sc.dependency('screen_capture_lite_shared')
jansson_dep = jansson_project.get_variable('jansson_dep')


deps = [
	sc_dep,
	jansson_dep,
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
