task default: %w[test]

task :test do
  sh "bundle install"
  sh "ruby src/kilatec.rb test/main.klt -o test/main"
end
