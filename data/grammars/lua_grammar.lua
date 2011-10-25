-- important fields for Analyzer class
require "lpeg"

extensions = {"lua"}
language = "Lua"
full_grammar = "lua_grammar"
other_grammars = {block = "lua_grammar"}
paired = {"{", "}", "(", ")"}
selectable = {"block", "chunk", "stat",  "laststat", "function_name", 
"namelist", "varlist", "function_call", "expression", "unknown", }
multi_text = {"line_comment", "multiline_comment"}
floating = {"line_comment", "multiline_comment"}

-- does language support multiline comments?
multiline_support = "false"

-- start & end tokens for comments; if language does not support multiline comments, define custom tokens
line_tokens = {"--", ""}
multiline_tokens = {"--[*[", "]*]"}

local lpeg = require "lpeg";

local locale = lpeg.locale();

local P, S, V = lpeg.P, lpeg.S, lpeg.V;

local C, Cb, Cc, Cg, Cs, Cmt, Ct =
    lpeg.C, lpeg.Cb, lpeg.Cc, lpeg.Cg, lpeg.Cs, lpeg.Cmt, lpeg.Ct;

local shebang = P "#" * (P(1) - P "\n")^0 * P "\n";

local function K (k) -- keyword
  return 	
	Ct(
	Cc("keyword") *
	T(k))  * -(locale.alnum + P "_");
end

function N(arg)	-- nonterminal
return Ct(
	Cc(arg) *
	V(arg)
	)
end

function T(arg) -- terminal
return
	Ct(C(arg))
end

-- ***  GRAMMAR  ****
local grammar = {"S", -- dummy symbol
-- ENTRY POINTS
  lua_block =  
	Ct(Cc("lua_program") *
	(shebang)^-1 * V "space" * 
	N "chunk"^-1 * V "space" * 
	N'unknown'^0 
	* -1);

  -- keywords

  keywords = K "and" + K "break" + K "do" + K "else" + K "elseif" +
             K "end" + K "false" + K "for" + K "function" + K "if" +
             K "in" + K "local" + K "nil" + K "not" + K "or" + K "repeat" +
             K "return" + K "then" + K "true" + K "until" + K "while";

  -- longstrings

--   longstring = P{ -- from Roberto Ierusalimschy's lpeg examples
--     V "open" * C((P(1) - V "closeeq")^0) *
--         V "close" / function (o, s) return s end;

--     open = "[" * Cg((P "=")^0, "init") * P "[" * (P "\n")^-1;
--     close = "]" * C((P "=")^0) * "]";
--     closeeq = Cmt(V "close" * Cb "init", function (s, i, a, b) return a == b end)
--   };
longstring = T"[[" *
	(Cc(" ") * N"nl")^0 *
	N"longline"^0*
	T"]]";
longline =  T((1-P"]]"-P"\n")^1) * N"nl"^0;
	
  -- comments & whitewhite

  space = (N'whites' + N'multiline_comment' + N'line_comment' + N'nl'^1)^0;
  
  line_comment = T "--" * V "longstring" +
            T(P"--" * (1 -S"\r\n")^0)  * N'nl' ;
			
  multiline_comment = T(P"--[*[" * (1 - P"]*]")^0 * P"]*]"),
  
  whites = T((locale.space - S"\n\r")^1);
  
  nl = T(P"\r"^-1 * P"\n");
  
  unknown =  T((1 - S"\n\r")^1)* N'nl'^0,	-- anything divided to lines
--   unknown_word =  N'whites'^-1 * T((1 - S" \t\r\n")^1),

  -- Types and Comments

  identifier = T((locale.alpha + P "_") * (locale.alnum + P "_")^0 - N "keywords");
  number_constant = T((P "-")^-1 * V "space" * P "0x" * locale.xdigit^1 *
               -(locale.alnum + P "_") +
           (P "-")^-1 * V "space" * locale.digit^1 *
               (P "." * locale.digit^1)^-1 * (S "eE" * (P "-")^-1 *
                   locale.digit^1)^-1 * -(locale.alnum + P "_") +
           (P "-")^-1 * V "space" * P "." * locale.digit^1 *
               (S "eE" * (P "-")^-1 * locale.digit^1)^-1 *
               -(locale.alnum + P "_"));
  string_constant = T( "\"" * (P "\\" * P(1) + (1 - P "\""))^0 * P "\"" +
           P "'" * (P "\\" * P(1) + (1 - P "'"))^0 * P "'");

  -- Lua Complete Syntax

  chunk = 
	(V "space" * N "laststat" * (V "space" * T ";")^-1) +  -- laststat only
	(V "space" * N "stat" * (V "space" * T ";")^-1)^1 *	-- stats
	(V "space" * N "laststat" * (V "space" * T ";")^-1)^-1;

  block = V"chunk";

  stat = K "do" * V "space" * N "block" * V "space" * K "end" +
         K "while" * V "space" * N "expression" * V "space" * K "do" * V "space" *
             N "block" * V "space" * K "end" +
         K "repeat" * V "space" * N "block" * V "space" * K "until" *
             V "space" * N "expression" +
         K "if" * V "space" * N "expression" * V "space" * K "then" *
             V "space" * N "block" * V "space" *
             (K "elseif" * V "space" * N "expression" * V "space" * K "then" *
              V "space" * N "block" * V "space"
             )^0 *
             (K "else" * V "space" * N "block" * V "space")^-1 * K "end" +
         K "for" * V "space" * N "identifier" * V "space" * T "=" * V "space" *
             N "expression" * V "space" * T "," * V "space" * N "expression" *
             (V "space" * T "," * V "space" * N "expression")^-1 * V "space" *
             K "do" * V "space" * N "block" * V "space" * K "end" +
         K "for" * V "space" * N "namelist" * V "space" * K "in" * V "space" *
             N "explist" * V "space" * K "do" * V "space" * N "block" *
             V "space" * K "end" +
         K "function" * V "space" * N "function_name" * V "space" *  V "function_body" +
         K "local" * V "space" * K "function" * V "space" * N "identifier" *
             V "space" * V "function_body" +
         K "local" * V "space" * N "namelist" *
             (V "space" * T "=" * V "space" * N "explist")^-1 +
         N "varlist" * V "space" * T "=" * V "space" * N "explist" +
         N "function_call";

  laststat = K "return" * (V "space" * N "explist")^-1 + K "break";

  function_name = N "identifier" * (V "space" * T "." * V "space" * N "identifier")^0 *
      (V "space" * T ":" * V "space" * N "identifier")^-1;

  namelist = N "identifier" * (V "space" * T "," * V "space" * N "identifier")^0;

  varlist = N "var" * (V "space" * T "," * V "space" * N "var")^0;

  -- Let's come up with a syntax that does not use left recursion
  -- (only listing changes to Lua 5.1 extended BNF syntax)
  -- value ::= nil | false | true | number_constant | string_constant | '...' | function |
  --           table_constructor | function_call | var | '(' expression ')'
  -- expression ::= unop expression | value [binop expression]
  -- prefix ::= '(' expression ')' | identifier
  -- index ::= '[' expression ']' | '.' identifier
  -- call ::= args | ':' identifier args
  -- suffix ::= call | index
  -- var ::= prefix {suffix} index | identifier
  -- function_call ::= prefix {suffix} call

  -- Something that represents a value (or many values)
  value = K "nil" +
          K "false" +
          K "true" +
          N "number_constant" +
          N "string_constant" +
	  N "longstring" +
          T "..." +
          N "function" +
          N "table_constructor" +
          N "function_call" +
          N "var" +
          T "(" * V "space" * N "expression" * V "space" * T ")";

  -- An expression operates on values to produce a new value or is a value
  expression = N "unop" * V "space" * N "expression" +
        N "value" * (V "space" * N "binop" * V "space" * N "expression")^-1;

  -- Index and Call
  index = T "[" * V "space" * N "expression" * V "space" * T "]" +
          T "." * V "space" * N "identifier";
  call = N "args" +
         T ":" * V "space" * N "identifier" * V "space" * N "args";

  -- A Prefix is a the leftmost side of a var(iable) or function_call
  prefix = T "(" * V "space" * N "expression" * V "space" * T ")" +
           N "identifier";
  -- A Suffix is a Call or Index
  suffix = N "call" +  N "index";

  var = N "prefix" * (V "space" * N "suffix" * #(V "space" * N "suffix"))^0 *
            V "space" * N "index" +
        N "identifier";
  function_call = N "prefix" *
                     (V "space" * N "suffix" * #(V "space" * N "suffix"))^0 *
                 V "space" * N "call";

  explist = N "expression" * (V "space" * T "," * V "space" * N "expression")^0;

  args = T "(" * V "space" * (N "explist" * V "space")^-1 * T ")" +
         N "table_constructor" +
         N "string_constant";

  ["function"] = K "function" * V "space" *  V"function_body";

  function_body = T "(" * V "space" * (N "parlist" * V "space")^-1 * T ")" *
                 V "space" *  N "block" * V "space" * K "end";

  parlist = N "namelist" * (V "space" * T "," * V "space" * T "...")^-1 +
            T "...";

  table_constructor = T "{" * V "space" * (N "field_list" * V "space")^-1 * T "}";

  field_list = N "field" * (V "space" * N "fieldsep" * V "space" * N "field")^0
                  * (V "space" * N "fieldsep")^-1;

  field = T "[" * V "space" * N "expression" * V "space" * T "]" * V "space" * T "=" *
              V "space" * N "expression" +
          N "identifier" * V "space" * T "=" * V "space" * N "expression" +
          N "expression";

  fieldsep = T "," +
             T ";";

  binop = K "and" + -- match longest token sequences first
          K "or" +
          T ".." +
          T "<=" +
          T ">=" +
          T "==" +
          T "~=" +
          T "+" +
          T "-" +
          T "*" +
          T "/" +
          T "^" +
          T "%" +
          T "<" +
          T ">";

  unop = T "-" +
         T "#" +
         K "not";
};
-- ***  END OF GRAMMAR  ****

-- *** POSSIBLE GRAMMARS (ENTRY POINTS) ****

grammar[1] = "lua_block"
lua_grammar = P(grammar)

--*******************************************************************
-- TESTING - this script cannot be used by Analyzer.cpp when these lines are uncommented !!!

-- dofile('default_grammar.lua')
-- test("snippets.lua", lua_grammar)
