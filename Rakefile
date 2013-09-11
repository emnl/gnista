require "bundler/gem_tasks"

require 'rake/testtask'
Rake::TestTask.new(:test) do |test|
  test.libs << 'ext' << 'test'
  test.pattern = 'test/**/test_*'
  test.verbose = false
end

task :make do |t|
  sh "cd ext/gnista && ruby extconf.rb && make"
end

task :clean do |t|
  sh "rm ext/gnista/*.bundle"
  sh "rm ext/gnista/Makefile"
  sh "rm ext/gnista/*.log"
  sh "rm ext/gnista/*.o"
end