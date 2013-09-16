require 'gnista/gnista'
require "gnista/version"

module Gnista
  class Logwriter
    attr_reader :logpath

    def inspect
      %(#<#{self.class} #{@logpath.inspect}>)
    end

    alias_method :del, :delete
  end

  class Logreader
    attr_reader :logpath

    def inspect
      %(#<#{self.class} #{@logpath.inspect}>)
    end
  end

  class Hash
    attr_reader :hashpath, :logpath

    def inspect
      %(#<#{self.class} #{@hashpath.inspect} #{@logpath.inspect}>)
    end

    alias_method :size, :length
  end
end
