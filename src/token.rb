require_relative "consts"

module KilateC
  class Token
    attr_reader :type, :value

    def initialize(type, value)
      @type = type
      @value = value
    end

    def to_s
      "#{type}: #{value.inspect}"
    end
  end
end
