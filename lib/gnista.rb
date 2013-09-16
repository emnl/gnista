require 'gnista/gnista'
require "gnista/version"

module Gnista
  class Logwriter
    attr_reader :logpath

    def inspect
      %(#<#{self.class} #{@logpath.inspect}>)
    end

    alias_method :del, :delete
    alias_method :[]=, :put
    alias_method :close!, :close
  end

  class Logreader
    include Enumerable

    attr_reader :logpath

    def inspect
      %(#<#{self.class} #{@logpath.inspect}>)
    end

    alias_method :close!, :close
  end

  class Hash
    include Enumerable

    attr_reader :hashpath, :logpath

    def keys
      map {|key,value| key }
    end

    def values
      map {|key,value| value }
    end

    def has_key?(key)
      return self[key] != nil
    end

    def empty?
      return length == 0
    end

    def inspect
      %(#<#{self.class} #{@hashpath.inspect} #{@logpath.inspect}>)
    end

    alias_method :size, :length
    alias_method :[], :get
    alias_method :include?, :has_key?
    alias_method :member?, :has_key?
    alias_method :contains?, :has_key?
    alias_method :exists?, :has_key?
    alias_method :close!, :close
  end
end
