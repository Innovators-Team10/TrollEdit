-- Default grammar for an unknown language
-- parses given text into lines
-- DON'T REMOVE from /grammars directory !!!

-- important constants for Analyzer class
extension = ""
full_grammar = "grammar"
other_grammars = {}
paired = {}

require 'lpeg'

--patterns
local P, V = lpeg.P, lpeg.V
--captures
local C, Ct, Cc = lpeg.C, lpeg.Ct, lpeg.Cc

-- ***  GRAMMAR  ****
grammar = P{"text",
text = Ct(Cc("text") * Ct(V'line')^1 * Ct(C(P(1)^0))),
line = C((P(1)-P"\n")^0 * P"\n"),
}
-- ***  END OF GRAMMAR  ****

-- *** TESTING FUNCTIONS ****
function test(filename, grammar, print_input)
	local fh = assert(io.open(filename))
	local input = fh:read"*a"
	fh:close()
	if (print_input == 1) then
		print(input)
	end
	print_table(0, lpeg.match(grammar,input))
end

function print_table(index, t)
  for i,v in ipairs(t) do
    if type(v)=="table" then
      print_table(index+1, v)
    else
      io.write("("..index..")")
      for i=1,index do io.write("   ") end
      io.write(" "..v.."\n")
    end
  end
end

--*******************************************************************
-- TESTING - this script cannot be used by Analyzer.cpp when this line is uncommented !!!

-- test("../qrc_input.cpp", grammar)