require 'mkmf'

extension_name = 'gnista'
dir_config(extension_name)

$CFLAGS << ' -std=c99'
$INCFLAGS << ' -I$(srcdir)/sparkey/src'
$INCFLAGS << ' -I$(srcdir)/snappy'
$VPATH ||= []
$VPATH << '$(srcdir)/sparkey/src'
$VPATH << '$(srcdir)/snappy'
gnista_srcs = %w[
  gnista.c
]

sparkey_srcs = %w[
  buf.c
  hashalgorithms.c
  hashiter.c
  hashwriter.c
  logreader.c
  returncodes.c
  util.c
  endiantools.c
  hashheader.c
  hashreader.c
  logheader.c
  logwriter.c
  MurmurHash3.c
]

snappy_srcs = %w[
  snappy.cc
  snappy-c.cc
  snappy-sinksource.cc
]

$srcs = gnista_srcs + sparkey_srcs + snappy_srcs

# Create macro definitions used by Snappy sources.
%w[
  byteswap.h
  stddef.h
  stdint.h
  sys/endian.h
  sys/mman.h
  sys/resource.h
  sys/time.h
  sys/uio.h
  unistd.h
  windows.h
].each { |header| have_header header }

if have_func 'mmap', 'sys/mman.h'
  $defs << '-DHAVE_FUNC_MMAP'
end
if have_func 'sysconf', 'unistd.h'
  $defs << '-DHAVE_FUNC_SYSCONF'
end
if try_compile('int main(void) { return __builtin_expect(0, 1); }')
  $defs << '-DHAVE_BUILTIN_EXPECT'
end
if try_compile('int main(void) { return __builtin_ctzll(0); }')
  $defs << '-DHAVE_BUILTIN_CTZ'
end

create_header
create_makefile(extension_name)
