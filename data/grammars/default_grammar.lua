-- Default grammar for an unknown language
-- parses given text into lines
-- DON'T REMOVE from /grammars directory !!!
require "lpeg"
-- important fields for Analyzer class (order is not relevant)
extensions = {""}			-- language file extensions, e.g. "lua"
language = "Text"		-- short description/name of this language
full_grammar = "grammar"	-- name of complete grammars
other_grammars = {}		-- names of available partial grammars
paired = {}				-- list of paired elements (terminal or nonterminal) e.g. {"begin", "end", "(", ")" } 
selectable = {"general_text", "line", "word", "unknown"}	-- list of nonterminal elements that could be selected and moved by user
multi_text = {}	-- list of nonterminal elements able/allowed to contain more lines of text (in their child terminals)
floating = {}			-- list of floating elements
multiline_support = "false"		-- natural support for multiline comments in language
line_tokens = {}	-- start & end tokens for line comment
multiline_tokens = {}		-- start & end tokens for multiline comment; if language does not support multiline comments, define custom tokens

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
grammar = P{"general_text",
general_text = Ct(Cc("general_text") *
	N'nl'^0 *
	N'line'^0 *
	N'unknown'^-1),
line = N'word'^1 * N'nl'^1,
word = T(V'char'^1),
char = P(1) - S(" \t\r\n"),

unknown = Ct(C(P(1)^1)), 			-- anything
whites = Ct(C(S(" \t")^1)),			-- spaces and tabs
nl = S(" \t")^0 * Ct(C(P"\r"^-1*P"\n")),	-- single newline, preceding spaces are ignored

-- NOTE: "unknown", "whites", and "nl" are reserved names recognised by Analyzer class
-- do not use for other than descibed purposes
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

-- test("../../input/text.txt", grammar)