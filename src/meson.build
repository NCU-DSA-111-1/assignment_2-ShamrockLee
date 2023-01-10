lib_errprinter = library('errprinter', 'errprinter.c',
include_directories: [
  inc_myshogi,
],
)

lib_translate_board = library('translate_board', 'translate_board.c',
include_directories: [
  inc_myshogi,
],
)

exe_testexec_translate_board = executable('translate-board', 'translate_board.c',
include_directories: [
  inc_myshogi,
],
c_args: [
  '-DTESTEXEC_TRANSLATE_BOARD',
],
)

executable('myshogi', [
  'myshogi.c',
],
include_directories: [
  inc_config,
  inc_myshogi,
  inc_optparse,
  inc_popen3,
],
link_with: [
  lib_errprinter,
  lib_translate_board,
],
)
