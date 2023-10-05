project('screencap', 'cpp',
  version : '0.1',
  default_options : ['warning_level=1', 'cpp_std=c++17'])

cmake = import('cmake')
sc = cmake.subproject('screen_capture_lite')
#argparse_project = cmake.subproject('argparse')

jansson_project = subproject('jansson')
argparse_project = subproject('argparse')

message('Screen Capture Lite CMake targets:\n - ' + '\n - '.join(sc.target_list()))

sc_dep = sc.dependency('screen_capture_lite_shared')
#argparse_dep = argparse_project.dependency('argparse')
jansson_dep = jansson_project.get_variable('jansson_dep')
argparse_dep = argparse_project.get_variable('argparse_dep')


deps = [
	sc_dep,
	jansson_dep,
	argparse_dep,
]

inc = [include_directories(
	'include',
	'subprojects',
)]



executable('screencap',
           [
		'screencap.cpp',
	   ],
           install : false,
	   include_directories: inc,
	   dependencies: deps,
)
