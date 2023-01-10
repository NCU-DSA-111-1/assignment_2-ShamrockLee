project('myshogi', 'c',
default_options: [
  'buildtype=debugoptimized',
  'c_std=gnu11',
  'warning_level=3',
])

prog_gnushogi_found = find_program('gnushogi')
path_gnushogi = prog_gnushogi_found.full_path()

str_gnushogi_command = path_gnushogi

inc_popen3 = include_directories('bourgeous_popen3_2011')
inc_optparse = include_directories('skeeto_optparse')

cfg_h = configuration_data()
cfg_h.set('GNUSHOGI_COMMAND', str_gnushogi_command)

configure_file(output: 'config.h', configuration: cfg_h)
inc_config = include_directories('.')

add_global_arguments([
  '-Werror=implicit-function-declaration'
],
language: 'c')

subdir('include')
subdir('src')

