require_relative "consts"
require_relative "token"

module KilateC
  module Regex
    NUMBER = /\d/
    IDENTIFIER = /[a-zA-Z0-9_]/
  end

  class Lexer
    def initialize(input)
      @input = input
      @pos = 0
    end

    def current_char
      (@pos < @input.size) ? @input[@pos] : nil
    end

    def advance
      @pos += 1
    end

    def skip_whitespace
      advance while current_char =~ /\s/
    end

    def number
      num = ""
      while current_char =~ Regex::NUMBER
        num << current_char
        advance
      end
      Token.new(:NUMBER, num.to_i)
    end

    def identifier
      id = ""
      while current_char =~ Regex::IDENTIFIER
        id << current_char
        advance
      end
      if KEYWORDS.include?(id)
        Token.new(:KEYWORD, id)
      else
        Token.new(:IDENTIFIER, id)
      end
    end

    def string
      str = ""
      while current_char && current_char != '"'
        if current_char == "\\"
          advance
          str << case current_char
          when "n" then "\n"
          when "t" then "\t"
          when '"' then '"'
          when "\\" then "\\"
          else current_char
          end
        else
          str << current_char
        end
        advance
      end
      advance
      Token.new(:STRING, str)
    end

    def next_token
      skip_whitespace

      char = current_char
      return nil if char.nil?

      if Regex::NUMBER.match?(char)
        number
      elsif Regex::IDENTIFIER.match?(char)
        identifier
      else
        advance
        case char
        when '"' then string
        when ":" then Token.new(:COLON, ":")
        when "," then Token.new(:COMMA, ",")
        when "[" then Token.new(:LBRACKET, "[")
        when "]" then Token.new(:RBRACKET, "]")
        when "(" then Token.new(:LPAREN, "(")
        when ")" then Token.new(:RPAREN, ")")
        when "{" then Token.new(:LBRACE, "{")
        when "}" then Token.new(:RBRACE, "}")
        when "=" then Token.new(:ASSIGN, "=")
        else Token.new(:UNKNOWN, char)
        end
      end
    end

    def lex
      tokens = []
      while (t = next_token)
        tokens << t
      end
      tokens << Token.new(:EOF, "<<EOF>>")
      tokens
    end
  end
end
