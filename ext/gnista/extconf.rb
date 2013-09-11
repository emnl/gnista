require 'mkmf'

extension_name = 'gnista'
dir_config(extension_name)

have_library('sparkey') or raise
have_header('sparkey/sparkey.h') or raise

create_makefile(extension_name)