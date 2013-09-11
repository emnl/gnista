require 'gnista/gnista'
require "gnista/version"

module Gnista
  class Logwriter
    alias_method :del, :delete
  end

  class Hash
    alias_method :size, :length
  end
end
