require 'minitest/spec'
require 'minitest/autorun'
require 'fileutils'
require File.expand_path("../../lib/gnista", __FILE__)

describe Gnista do

  log_path = '/tmp/test.log'
  hash_path = '/tmp/test.hash'

  before do
    @logwriter = Gnista::Logwriter.new log_path
    @logreader = Gnista::Logreader.new log_path
  end

  after do
    @logwriter.close
    @logreader.close
    FileUtils.rm log_path
  end

  it "can use put command" do
    res = @logwriter.put "key1", "value1"
    res.must_be_nil
  end

  it "can use delete command" do
    res = @logwriter.delete "key10"
    res.must_be_nil
  end

  it "can use flush command" do
    @logwriter.flush.must_be_nil
  end

  it "can iterate each log entry" do
    @logwriter.put "key1", "value1"
    @logwriter.put "key2", "value2"

    a = true
    @logreader.each do |key,value|
      if a
        key.must_equal "key1"
        value.must_equal "value1"
        a = false
      else
        key.must_equal "key2"
        value.must_equal "value2"
      end
    end

  end

  it "can read hash max key/value length" do
    @logwriter.put "abc", "a"
    @logwriter.put "a", "abcd"
    @logwriter.flush
    Gnista::Hash.write hash_path, log_path

    hash = Gnista::Hash.new hash_path, log_path

    hash.maxkeylen.must_equal 3
    hash.maxvaluelen.must_equal 4
    FileUtils.rm hash_path
  end

  it "can iterate each hash entry" do
    @logwriter.put "key1", "value1"
    @logwriter.put "key2", "value2"
    @logwriter.flush
    Gnista::Hash.write hash_path, log_path

    hash = Gnista::Hash.new hash_path, log_path

    a = true
    hash.each do |key,value|
      if a
        key.must_equal "key1"
        value.must_equal "value1"
        a = false
      else
        key.must_equal "key2"
        value.must_equal "value2"
      end
    end

    hash.close
    FileUtils.rm hash_path
  end

  it "can get from hash" do
    @logwriter.put "key1", "value1"
    @logwriter.put "key2", "value2"
    @logwriter.delete "key2"
    @logwriter.flush
    Gnista::Hash.write hash_path, log_path

    hash = Gnista::Hash.new hash_path, log_path
    ret = hash.get "key1"
    ret.must_equal "value1"

    ret = hash.get "key2"
    ret.must_be_nil
    FileUtils.rm hash_path
  end

  it "can get the hash length" do
    @logwriter.put "key1", "value1"
    @logwriter.put "key2", "value2"
    @logwriter.put "key3", "value3"
    @logwriter.flush
    Gnista::Hash.write hash_path, log_path

    hash = Gnista::Hash.new hash_path, log_path
    hash.length.must_equal 3
    FileUtils.rm hash_path
  end

  it "can get the hash collisions count" do
    # This is hard to test
  end

  it "can be open or closed" do
    @logwriter.open?.must_equal true
    @logreader.open?.must_equal true
    Gnista::Hash.write hash_path, log_path
    hash = Gnista::Hash.new hash_path, log_path
    hash.open?.must_equal true
    hash.close
    hash.open?.must_equal false
    FileUtils.rm hash_path
  end

end
