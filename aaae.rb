#!/usr/bin/env ruby -w

#
# apple ascii art encoder.
#

require 'optparse'


# 
# read stdin
# --trim -> trim trailing whitespace
# --dle -> compress > 2 spaces into a dle
# --indent=# -> indent it by # spaces
# --width=# -> console width (default = 80)
# --center  -> center block assuming width
# --target=x -> target language (c, orca, mpw, etc)

#
# todo -- expand tabs? (--ts=8)
# todo -- left/right/center slign block?
#


# 
# output char name[] = { 0x01, 0x02, ... };
#
def output_bytes(name, data)
	tmp = data.join('')
	index = 0

	if name
		puts "char #{name}[] = {"
	else
		puts "{"
	end
	tmp.each_byte {|b|
		print "    " if index == 0

		printf("%02x, ", b)

		index = index + 1
		if index == 16
			print "\n"
			index = 0
		end
	}
	print "\n" if index > 0
	puts "};"
end

# 
# output char name[] = { "bleh\r","bleh\r" };
#
def output_strings(name, data)

	map = {
		0x0d => "\\r",
		0x0a => "\\n",
	}

	if name
		puts "char #{name}[] = "
	end

	data.each {|x|

		# escape \
		x = x.gsub(/\\/, '\\')

		# escape "
		x = x.gsub(/"/, '\"')

		# escape chars...
		# unfortunately, \x doesn't have a length limit so
		# it can't be used.  octal sucks but it's limited 
		# to 3 characters.
		x = x.gsub(/([\x00-\x1f\x7f])/) {|m|
			c = m.getbyte(0)
			next map[c] if map[c]
			sprintf("\\%03o", c)
		}
		printf("    \"%s\"\n", x)
	}
	puts ";"

end


data = []
options = {
	:trim => true,
	:dle => false,
	:indent => 0,
	:width => 80,
	:center => false,
	:dry_run => false,
	:name => nil,
	:format => :string
}

OptionParser.new do |opts|
	opts.banner = "Usage: aaae.rb [options] [input file]"

	opts.on("--[no-]trim", "Trim trailing white space from input.") do |x|
		options[:trim] = x
	end

	opts.on("--[no-]dle", "Use DLE to compress spaces.") do |x|
		options[:dle] = x
	end

	opts.on("--width N", Integer, "Set screen width. Default = 80.") do |x|
		options[:width] = x
	end

	opts.on("--indent N", Integer, "Indent N spaces.") do |x|
		options[:indent] = x
	end


	opts.on("--name N", String, "Set variable name.") do |x|
		if x.match(/^[A-Za-z_][A-Za-z0-9_]*$/)
			options[:name] = x
		else
			$stderr.puts "Invalid name -- #{x}"
		end
	end

	opts.on("-f", "--format FORMAT", [:string, :bytes], "Set output format (string, bytes)") do |x|
		options[:format] = x
	end

	opts.on("--[no-]center", "Center output.") do |x|
		options[:center] = x
	end

	opts.on("--dry-run", "Simulated output.") do
		options[:dry_run] = true
	end

	opts.on_tail("-h", "--help", "Show this message") do
		puts opts
		exit
	end

	opts.on_tail("--version", "Show version") do
		puts "aaae.rb 0.1"
		exit
	end

end.parse!


ARGF.each_line {|x|
	x.chomp!
	x.rstrip! if options[:trim]
	data.push(x)
}

# option to strip leading space?
if (options[:trim])

end

# get the width of the block.
bw = data.reduce(0) {|akku, x|
	next x.length if x.length > akku
	next akku 
}


if options[:indent] > 0
	space = ' ' * options[:indent]
	data = data.map {|x| space + x }
end

if options[:center]

	indent = (options[:width] - bw) / 2
	space = ' ' * indent
	data = data.map {|x| space + x }
end

# todo -- check if any lines > screen width...

if options[:dry_run]
	width = options[:width]

    print "+", "-" * width, "+\n" 
	#print "|", " " * width, "|\n"
	data.each {|x|

		printf("|%-*s|\n", width, x)
	}
	#print "|", " " * width, "|\n"
    print "+", "-" * width, "+\n" 

	exit 0
end

# dle!

if options[:dle]

	data = data.map {|x|
		x.gsub(/( {3,})/) {|m|
			size = m.length
			next "\x10" + (size + 32).chr
		}
	}
end


# now add r/n
data = data.map {|x| x + "\r" }

# now output
# should have flag for text vs bytes, asm, etc.

case options[:format]
when :string
	output_strings(options[:name], data)
when :bytes	
	output_bytes(options[:name], data)
end
