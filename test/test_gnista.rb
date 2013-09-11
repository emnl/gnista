require 'minitest/spec'
require 'minitest/autorun'
require 'fileutils'
require File.expand_path("../../lib/gnista", __FILE__)

describe Gnista do

  log_path = '/tmp/test.log'
  hash_path = '/tmp/test.hash'

  it "can create a Logwriter with only a filename" do
    @logwriter = Gnista::Logwriter.new log_path
    @logwriter.must_be_instance_of Gnista::Logwriter
    @logwriter.close
    FileUtils.rm log_path
  end

  it "can create a Logwriter with a filename and a compression number" do
    @logwriter = Gnista::Logwriter.new log_path, 4000
    @logwriter.must_be_instance_of Gnista::Logwriter
    @logwriter.close
    FileUtils.rm log_path
  end

  it "can append a Logwriter" do
    @logwriter = Gnista::Logwriter.new log_path
    @logwriter.close

    @logwriter = Gnista::Logwriter.new log_path, :append
    @logwriter.must_be_instance_of Gnista::Logwriter
    @logwriter.close
    FileUtils.rm log_path
  end

  it "can create a Logreader with a filename" do
    @logwriter = Gnista::Logwriter.new log_path
    @logwriter.close

    @logreader = Gnista::Logreader.new log_path
    @logreader.must_be_instance_of Gnista::Logreader
    @logreader.close
    FileUtils.rm log_path
  end

  it "can write and read a Hash with a log-file and a hash-file" do
    @logwriter = Gnista::Logwriter.new log_path
    @logwriter.close

    Gnista::Hash.write hash_path, log_path
    @hash = Gnista::Hash.new hash_path, log_path
    @hash.must_be_instance_of Gnista::Hash
    @hash.close
    FileUtils.rm hash_path
    FileUtils.rm log_path
  end

end
