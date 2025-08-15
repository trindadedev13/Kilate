require "fileutils"

require_relative "lexer"
require_relative "parser"
require_relative "codegen"

if ARGV.length < 1
  puts "Please provided an file to be compiled."
  exit 1
end

file = ARGV[0]
if !File.exist?(file)
  puts "Provided file #{file} not exists."
  exit 1
end
file_content = File.read(file)

pos = 1
out_file = "a.out"
ARGV[1..].each do |arg|
  case arg
  when "-o"
    pos += 1
    out_file = ARGV[pos]
    puts "Invalid output file." if !out_file
  end
end

def run(cmd)
  system(cmd) or abort("Failed to run #{cmd}")
end

lexer = KilateC::Lexer.new(file_content)
tokens = lexer.lex
# puts
# puts "Tokens:"
# tokens.each do |t|
  # puts t
# end

parser = KilateC::Parser.new(tokens)
nodes = parser.parse
# puts
# puts "Nodes:"
# nodes.each do |n|
  # puts n
# end

codegen = KilateC::Codegen.new("main")
codegen.gen(nodes)

out_bc = "#{out_file}.bc"
codegen.save_bitcode(out_bc)

out_obj = "#{out_file}.obj"
run "llc #{out_bc} -filetype=obj -o #{out_obj}"

run "clang #{out_obj} -o #{out_file}"

FileUtils.rm(out_bc)
FileUtils.rm(out_obj)