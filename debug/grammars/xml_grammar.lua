-- Simple XML grammar
--
-- TODO:



-- important constants for Analyzer class
extension = "xml"
full_grammar = "document"
other_grammars = {
	element="in_element", 
}
paired = {"<", ">", "el_start", "el_end"}
multi_line = {"unknown"}
multi_block = {"document", "element"}

require 'lpeg'

--patterns
local P, R, S, V = lpeg.P, lpeg.R, lpeg.S, lpeg.V
--captures
local C, Ct, Cc = lpeg.C, lpeg.Ct, lpeg.Cc

-- nonterminal, general node
function N(arg)
return Ct(
	Cc(arg) *
	V(arg)
	)
end

-- nonterminal, ignored in tree
function NI(arg)
return
	V(arg)
end

-- terminal, text node
function T(arg)
return
	N'whites'^-1 *
	TP(arg) *
	N'nl'^0
end

-- terminal, plain node without comments or whitespaces
function TP(arg)
return
	Ct(C(arg))
end

-- ***  GRAMMAR  ****
local grammar = {"S", 

-- ENTRY POINTS
document =  
	Ct(
	Cc("document") *
	N'nl'^0 *
	N'header'^-1 * N'element' *
	N'unknown'^-1 *-1),
	
in_element =  
	Ct(
	N'whites'^-1 *
	N'nl'^0 *
	(N'element')^0 *
	N'unknown'^-1 *-1),
	
-- NONTERMINALS
header = T"<?xml" * T"version=" * N'version_number' * T"?>",
version_number = NI'number' * T'.' * NI'number',
element = 
	N'el_empty' +
	N'el_start' * (N'word'^1 + N'element'^0) * N'el_end',
el_start = T"<" * NI'name' * T">",
el_end = T"</" * NI'name' * T">",
el_empty = T"<" * NI'name' * T"/>",
	
-- TERMINALS
name = T((NI'letter' + S("_:")) * NI'name_char'^0),
word = T(NI'char'^1),
number = T(NI'digit'^1),

unknown = TP(P(1)^1), -- anything
	
-- LITERALS
whites = TP(S(" \t")^1),	-- spaces and tabs
nl = S(" \t")^0 * TP(P"\r"^-1*P"\n"), -- single newline, preceding spaces are ignored

letter = R("az", "AZ"),
digit = R("09"),
char = P(1) - S("<>/"),
name_char = NI'letter' + NI'digit' + S(".-_:"),
}
-- ***  END OF GRAMMAR  ****

-- *** POSSIBLE GRAMMARS (ENTRY POINTS) ****
grammar[1] = "document"
document = P(grammar)
grammar[1] = "in_element"
in_element = P(grammar)

--*******************************************************************
-- TESTING - this script cannot be used by Analyzer.cpp when these lines are uncommented !!!

-- dofile('default_grammar.lua')
-- test("../../input/input.xml", document)
