require "llvm/core"
require "llvm/execution_engine"
require "llvm/transforms/scalar"
require "llvm/transforms/ipo"

require_relative "ast"

module KilateC
  class Codegen
    attr_reader :mod

    def initialize(module_name)
      @mod = LLVM::Module.new(module_name)
    end

    def gen(nodes)
      nodes.each { |node| generate_node(node) }
    end

    def generate_node(node)
      case node
      when Ast::FunctionNode
        generate_function(node)
      else
        # i should handle other tupes somday
      end
    end

    def kilate_type_to_llvm(k_type, for_param: false)
      case k_type
        when "i32"  then LLVM::Int
        when "bool" then LLVM::Int
        when "string" then LLVM::Int8.pointer
        else
          if for_param
            raise "Invalid parameter type: #{k_type}"
          else
            LLVM.Void
          end
      end
    end

    def generate_function(function_node)
      return_type = kilate_type_to_llvm(function_node.return_type)

      param_types = function_node.params.map { |param| kilate_type_to_llvm(param.type) }
      func_type = LLVM.Function(param_types, return_type, varargs: false)
      func = @mod.functions.add(function_node.name, func_type)

      function_node.params.each_with_index do |param_node, index|
        func.params[index].name = param_node.name
      end

      entry_block = func.basic_blocks.append("entry")
      builder = LLVM::Builder.new
      builder.position_at_end(entry_block)

      if return_type == LLVM.Void
        builder.ret_void
      elsif return_type == LLVM::Int
        builder.ret(LLVM::Int(0))
      elsif return_type == LLVM::Int
        builder.ret(LLVM::Int(0))   # bool
      else
        raise "Unsupported return type #{return_type}"
      end

      func
    end

    def save_ir(filename)
      File.write(filename, @mod.to_s)
    end

    def save_bitcode(filename)
      @mod.write_bitcode(filename)
    end
  end
end