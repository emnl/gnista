require 'mkmf'

extension_name = 'gnista'
dir_config(extension_name)

have_library('sparkey') or raise "Sparkey is missing"
have_header('sparkey/sparkey.h') or raise "sparkey.h could not be found"

create_makefile(extension_name)
