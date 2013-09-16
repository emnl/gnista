require 'minitest/spec'
require 'minitest/autorun'
require 'fileutils'
require File.expand_path("../../lib/gnista", __FILE__)

describe Gnista do

  log_path = '/tmp/test.log'
  hash_path = '/tmp/test.hash'

  before do
    @logwriter = Gnista::Logwriter.new log_path
  end

  after do
    @logwriter.close
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
    @logwriter.delete "key2"
    @logwriter.flush

    lograder = Gnista::Logreader.new log_path

    a = true
    lograder.each do |key,value,type|
      if a
        key.must_equal "key1"
        value.must_equal "value1"
        type.must_equal :put
        a = false
      else
        key.must_equal "key2"
        value.must_be_nil
        type.must_equal :delete
      end
    end

    a.must_equal false

    lograder.close
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

    a.must_equal false

    hash.close
    FileUtils.rm hash_path
  end

  it "can get from hash" do
    @logwriter.put "key1", "value1"
    @logwriter.put "key2", "value2"
    @logwriter.put "key3", "value3"
    @logwriter.delete "key2"
    @logwriter.flush
    Gnista::Hash.write hash_path, log_path

    hash = Gnista::Hash.new hash_path, log_path
    ret = hash.get "key1"
    ret.must_equal "value1"

    ret = hash.get "key2"
    ret.must_be_nil

    ret = hash["key3"]
    ret.must_equal "value3"
    FileUtils.rm hash_path
  end

  it "can get the hash length" do
    @logwriter.put "key1", "value1"
    @logwriter["key2"] = "value2"
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

    logreader = Gnista::Logreader.new log_path
    logreader.open?.must_equal true
    logreader.close
    logreader.open?.must_equal false

    Gnista::Hash.write hash_path, log_path
    hash = Gnista::Hash.new hash_path, log_path
    hash.open?.must_equal true
    hash.close
    hash.open?.must_equal false
    FileUtils.rm hash_path
  end

  it "can inspect logwriter, logreader and hash" do
    logreader = Gnista::Logreader.new log_path
    Gnista::Hash.write hash_path, log_path
    hash = Gnista::Hash.new hash_path, log_path

    @logwriter.inspect.must_equal %(#<#{@logwriter.class} #{log_path.inspect}>)
    logreader.inspect.must_equal %(#<#{logreader.class} #{log_path.inspect}>)
    hash.inspect.must_equal %(#<#{hash.class} #{hash_path.inspect} #{log_path.inspect}>)

    hash.close
    logreader.close
    FileUtils.rm hash_path
  end

  it "can get the logpath and the hashpath from logwriter, logreader and hash" do
    logreader = Gnista::Logreader.new log_path
    Gnista::Hash.write hash_path, log_path
    hash = Gnista::Hash.new hash_path, log_path

    @logwriter.logpath.must_equal log_path
    logreader.logpath.must_equal log_path
    hash.hashpath.must_equal hash_path
    hash.logpath.must_equal log_path

    hash.close
    logreader.close
    FileUtils.rm hash_path
  end

  it "can use empty? on hash" do
    Gnista::Hash.write hash_path, log_path
    hash = Gnista::Hash.new hash_path, log_path
    hash.empty?.must_equal true
    hash.close

    @logwriter["key"] = "value"
    @logwriter.flush
    Gnista::Hash.write hash_path, log_path
    hash = Gnista::Hash.new hash_path, log_path
    hash.empty?.must_equal false
    hash.close
    FileUtils.rm hash_path
  end

  it "can read logreader max key/value length" do
    @logwriter.put "abc", "a"
    @logwriter.put "a", "abcd"
    @logwriter.flush

    logreader = Gnista::Logreader.new log_path

    logreader.maxkeylen.must_equal 3
    logreader.maxvaluelen.must_equal 4
  end

  it "can decide if hash has_key?" do
    @logwriter["key"] = "value"
    @logwriter.flush

    Gnista::Hash.write hash_path, log_path
    hash = Gnista::Hash.new hash_path, log_path

    hash.has_key?("key").must_equal true
    hash.include?("key1").must_equal false

    hash.close
    FileUtils.rm hash_path
  end

  it "extract keys and values from hash" do
    @logwriter["key1"] = "value1"
    @logwriter["key2"] = "value2"
    @logwriter.flush

    Gnista::Hash.write hash_path, log_path
    hash = Gnista::Hash.new hash_path, log_path

    hash.keys.must_equal ["key1", "key2"]
    hash.values.must_equal ["value1", "value2"]

    hash.close
    FileUtils.rm hash_path
  end

end
