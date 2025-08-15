module KilateC
  module Ast
    class Node
      def fmt_with_indent(indent = 0)
        "#{'  ' * indent}#{self.class.name.split('::').last}"
      end

      def to_s
        fmt_with_indent
      end
    end

    class LiteralNode < Node
      attr_reader :value

      def initialize(value)
        @value = value
      end

      def fmt_with_indent(indent = 0)
        "#{'  ' * indent}Literal: #{value.inspect}"
      end
    end

    class VarNode < Node
      attr_reader :name, :value

      def initialize(name, value)
        @name = name
        @value = value
      end

      def fmt_with_indent(indent = 0)
        pad = '  ' * indent
        val_str =
          if value.is_a?(Array)
            value.map { |v| v.respond_to?(:fmt_with_indent) ? v.fmt_with_indent(indent + 2) : "#{'  ' * (indent + 2)}#{v.inspect}" }.join("\n")
          elsif value.respond_to?(:fmt_with_indent)
            value.fmt_with_indent(indent + 1)
          else
            value.inspect
          end
        "#{pad}Var: #{name}\n#{val_str}"
      end
    end

    class CallNode < Node
      attr_reader :func, :params

      def initialize(func, params)
        @func = func
        @params = params
      end

      def fmt_with_indent(indent = 0)
        pad = '  ' * indent
        str = "#{pad}Call:"
        str += "\n" + func.fmt_with_indent(indent + 1) if func
        params.each do |p|
          str += "\n" + p.fmt_with_indent(indent + 1)
        end
        str
      end
    end

    class ParamNode < Node
      attr_reader :type, :name, :is_array, :value

      def initialize(type, name, is_array, value = nil)
        @type = type
        @name = name
        @is_array = is_array
        @value = value
      end

      def fmt_with_indent(indent = 0)
        pad = '  ' * indent
        array_suffix = is_array ? '[]' : ''
        val_str = value.nil? ? 'no value' : (value.respond_to?(:fmt_with_indent) ? "\n" + value.fmt_with_indent(indent + 1) : value.inspect)
        "#{pad}Param: #{type} #{name}#{array_suffix}#{val_str.is_a?(String) && val_str.start_with?("\n") ? val_str : " (#{val_str})"}"
      end
    end

    class FunctionNode < Node
      attr_reader :name, :modifiers, :params, :body, :return_type

      def initialize(name, modifiers, params, body, return_type)
        @name = name
        @modifiers = modifiers
        @params = params
        @body = body
        @return_type = return_type
      end

      def fmt_with_indent(indent = 0)
        pad = '  ' * indent
        str = "#{pad}Function: #{name}"
        str += "\n#{pad}  Modifiers: #{modifiers.join(', ')}"
        str += "\n#{pad}  Return: #{return_type}"
        str += "\n#{pad}  Params:"
        params.each { |p| str += "\n" + p.fmt_with_indent(indent + 2) }
        str += "\n#{pad}  Body:"
        body.each { |b| str += "\n" + b.fmt_with_indent(indent + 2) }
        str
      end
    end
  end
end