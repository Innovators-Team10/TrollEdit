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
local P, V, S = lpeg.P, lpeg.V, lpeg.S
--captures
local C, Ct, Cc = lpeg.C, lpeg.Ct, lpeg.Cc


-- nonterminal, general node
function N(arg)
return Ct(
	Cc(arg) *
	V(arg)
	)
end
-- terminal, text node
function T(arg)
return
	N'whites'^-1 *
	Ct(C(arg))
end

-- ***  GRAMMAR  ****
grammar = P{"text",
text = Ct(Cc("text") *
	N'line'^0 *
	N'unknown'^-1),
line = N'word'^1 * V'nl'^1,
word = T(V'char'^1),

unknown = Ct(C(P(1)^1)), -- anything

whites = Ct(C(S(" \t")^1)),
nl = N'whites'^-1 * Ct(C(P"\r"^-1*P"\n")),
char = P(1) - S(" \t\r\n")

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

-- test("../../input/in.c", grammar)