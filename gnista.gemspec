# coding: utf-8
lib = File.expand_path('../lib', __FILE__)
$LOAD_PATH.unshift(lib) unless $LOAD_PATH.include?(lib)
require 'gnista/version'

Gem::Specification.new do |spec|
  spec.name          = "gnista"
  spec.version       = Gnista::VERSION
  spec.authors       = ["Emanuel Andersson"]
  spec.email         = ["manusdude@gmail.com"]
  spec.description   = %q{Gnista is a wrapper for the database/hashstore Sparkey written for Ruby. Gnista packs all the native features of Sparkey into a shiny red package, easy to use.}
  spec.summary       = %q{Ruby wrapper for Spotify's Sparkey}
  spec.homepage      = "http://github.com/emnl/gnista"
  spec.license       = "MIT"

  spec.files         = %w[README.md LICENSE.txt HISTORY.md Rakefile
                          ext/gnista/extconf.rb ext/gnista/gnista.c
                          test/test_gnista.rb test/test_commands.rb
                          lib/gnista.rb lib/gnista/version.rb]

  spec.test_files    = ['test/test_gnista.rb', 'test/test_commands.rb']
  spec.require_paths = ["ext", "lib"]
  spec.extensions << 'ext/gnista/extconf.rb'

  spec.add_development_dependency "bundler", "~> 1.3"
  spec.add_development_dependency "rake"
end
