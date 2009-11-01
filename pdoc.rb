require 'rubygems'
require 'rake'
require '../pdoc/lib/pdoc'

rm_rf "docs"
begin
  files = Dir.glob("src/*.jsdoc")
  files << {
    :output => "docs",
    :templates => "../prototype-pdoc-template/html",
    :syntax_highlighter => :none,
    :index_page => 'README.markdown'
  }
  PDoc::Runner.new(*files).run
rescue PDoc::ParseError => e
  print e.message
  #print "\n"
  #print e.failures
  print e.file
  print "\n"

rescue ArgumentError => e
  print "ArgError\n"
end

