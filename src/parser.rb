require_relative "ast"
require_relative "lexer"

module KilateC
  class Parser
    def initialize(tokens)
      @tokens = tokens
      @pos = 0
      @nodes = []
      @functions = {}
    end

    def current_token
      @tokens[@pos]
    end

    def next_token
      @tokens[@pos + 1]
    end

    def advance
      @pos += 1
      current_token
    end

    def expect(type)
      if current_token && current_token.type == type
        t = current_token
        advance
        t
      else
        raise "Expected #{type}, got #{current_token&.type}"
      end
    end

    def parse
      until current_token.nil? || current_token.type == :EOF
        case current_token.type
        when :KEYWORD
          @nodes << parse_pub_or_work
        when :IDENTIFIER
          @nodes << parse_statement
        else
          advance
        end
      end
      @nodes
    end

    def parse_pub_or_work
      if current_token.value == "pub"
        advance
        if current_token.value == "work"
          parse_function(["pub"])
        else
          raise "Unexpected token after pub: #{current_token.inspect}"
        end
      elsif current_token.value == "work"
        parse_function([])
      else
        raise "Unexpected keyword: #{current_token.value}"
      end
    end

    def parse_function(modifiers)
      expect(:KEYWORD) # work
      name = expect(:IDENTIFIER).value

      expect(:LPAREN)
      params = parse_params
      expect(:RPAREN)

      return_type = nil
      if current_token&.type == :COLON
        advance
        return_type = expect(:IDENTIFIER).value
      end

      expect(:LBRACE)
      body = []
      until current_token.nil? || current_token.type == :RBRACE
        body << parse_statement
      end
      expect(:RBRACE)

      fn = Ast::FunctionNode.new(name, modifiers, params, body, return_type)
      @functions[name] = fn
      fn
    end

    def parse_params
      params = []
      return params if current_token&.type == :RPAREN

      loop do
        type = expect(:IDENTIFIER).value
        is_array = false
        if current_token&.type == :LBRACKET
          advance
          expect(:RBRACKET)
          is_array = true
        end

        expect(:COLON)
        name = expect(:IDENTIFIER).value

        params << Ast::ParamNode.new(type, name, is_array)

        break if current_token.type == :RPAREN
        expect(:COMMA)
      end
      params
    end

    def parse_statement
      if func = @functions[current_token.value]
        parse_call(func)
      else
        advance
      end
    end

    def parse_call(func)
      expect(:IDENTIFIER)
      expect(:LPAREN)
      expect(:RPAREN)
      Ast::CallNode.new(func, [])
    end
  end
end