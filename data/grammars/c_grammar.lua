-- C grammar
require "lpeg"

-- TODO:
-- enumeration constant?

-- BUGS:
-- (j)++ crashes

-- important fields for Analyzer class
-- for clarifications see 'default_grammar.lua'
extensions = {"c", "h"}
language = "C"
full_grammar = "program"
other_grammars = {
	block="in_block", 
	translation_unit="top_element"
}
paired = {"{", "}", "(", ")", "[", "]"}
selectable = {
	"preprocessor", "preprocessors",
	"funct_definition", "declaration", 
	"initializer", "block", "funct_parameter", "expression",  
	"program", "header_file", "cast", 
	"unknown","unknown_word", "if_statement", "while_statement", 
	"for_statement", "switch_statement", "simple_statement"
	}	
multi_text = {"ine_comment", "multiline_comment", "doc_comment",}
floating = {"doc_comment", "multiline_comment", "line_comment"}

-- does language support multiline comments?
multiline_support = "true"

-- start & end tokens for comments; if language does not support multiline comments, define custom tokens
line_tokens = {"//", ""}
multiline_tokens = {"/*", "*/"}

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
	(NI'comment' + N'nl'^1)^0
end

-- terminal, keyword text node
function TK(arg)
return
	Ct(
	Cc("keyword") *
	T(arg))
end

-- terminal, plain node without comment or any whitespaces
function TP(arg)
return
	Ct(C(arg))
end

-- ***  GRAMMAR  ****
local grammar = {"S", 

-- ENTRY POINTS
program =  
	Ct(Cc("program") *
	(NI'comment' + N'nl'^1)^0 *
	N'translation_unit'^0 *
	N'unknown'^0 *
	N'whites'^-1 * -1),
top_element =  
	Ct(
	(NI'comment' + N'nl'^1)^0 *
	N'translation_unit'^0 *
	N'unknown'^0 *
	N'whites'^-1 * -1),
in_block = 
	Ct (
	(NI'comment' + N'nl'^1)^0 *
	N'block'^-1 *
	N'unknown'^0 *
	N'whites'^-1 ),

-- NONTERMINALS
translation_unit = N'preprocessors' + N'funct_definition' + N'declaration',

preprocessors = N'preprocessor'^1,

preprocessor = (NI'include' + ((TK"#define" + TK"#elif" + TK"#else" + TK"#endif" +
	TK"#error" + TK"#ifdef" + TK"#ifndef" + TK"#if" + TK"#import" + TK"#line" +
	TK"#pragma" + TK"#undef") * T((1 - S"\n\r" - P"//" - P"/*")^0))),
	
include = TK"#include" * N'header_file',

header_file = T"<" * T((1 - P">")^1) * T">" + T(P'"' * T((1 - P'"')^1) * P'"'),

funct_definition =
	NI'declaration_specifiers'^-1 * N'declarator' *N'declaration'^0 * T"{" * N'block'^-1 * T"}",

declaration =  NI'declaration_specifiers' * N'init_declarator' * (T"," * N'init_declarator')^0 * T";",

declaration_specifiers =
	NI'declaration_class_qualifier'^0 * N'type_specifier' * NI'declaration_class_qualifier'^0,

declaration_class_qualifier = N'storage_class_specifier' + N'type_qualifier',

storage_class_specifier =
	TK"auto" + TK"register" + TK"static" + TK"extern" + TK"typedef",

type_specifier = 
	(TK"void" + TK"char" + TK"short" + TK"int" + TK"long" + TK"float" +
	TK"double" + TK"signed" + TK"unsigned" + N'struct_or_union_specifier' +
	N'enum_specifier' + N'typedef_name'
	),

type_qualifier =  (TK"const" + TK"volatile"),

struct_or_union_specifier =
	(TK"struct" + TK"union") * 
	(N'identifier'^-1 * T"{" * N'struct_declaration'^1 * T"}" +
	N'identifier'
	),

init_declarator =
	N'declarator' * (T"=" * N'initializer')^-1,

struct_declaration =
-- 	(N'type_specifier' + N'type_qualifier')^1 * --TODO
	N'type_specifier' *
	N'struct_declarator' * (T"," * N'struct_declarator')^0 * T";",

struct_declarator =  N'declarator' + (N'declarator'^-1 * T":" * N'constant_expression'),

enum_specifier =  
	TK"enum" * (N'identifier' + N'identifier'^-1 *
	T"{" * N'enumerator' * (T"," * N'enumerator')^0 * T"}"),

enumerator =  N'identifier' * (T"=" * N'constant_expression')^-1,

declarator =
	N'pointer'^-1 * (N'identifier' + T"(" * N'declarator' * T")") * (
	T"[" * N'constant_expression'^-1 * T"]" +
	T"(" * N'parameter_type_list' * T")" +
	T"(" * (N'identifier' * (T"," * N'identifier')^0)^-1 * T")"
	)^0,

pointer = (T"*" * N'type_qualifier'^0)^1,

parameter_type_list =  N'parameter_declaration' * (T"," * N'parameter_declaration')^0 * (T"," * T"...")^-1,

parameter_declaration =
	N'declaration_specifiers' * (N'declarator' + N'abstract_declarator')^-1,

initializer =  N'assignment_expression' + 
	T"{" * N'initializer' * (T"," * N'initializer')^0 * T","^-1* T"}",

type_name =  (N'type_specifier' + N'type_qualifier')^1 * N'abstract_declarator'^-1,

abstract_declarator =
	N'pointer' * (T"(" * N'abstract_declarator' * T")")^-1 * (
	T"[" * N'constant_expression'^-1 * T"]" +
	T"(" * N'parameter_type_list'^-1 * T")"
	)^0 +
	(T"(" * N'abstract_declarator' * T")")^-1 * (
	T"[" * N'constant_expression'^-1 * T"]" +
	T"(" * N'parameter_type_list'^-1 * T")"
	)^1,
	
statement = 
	N'if_statement' +
	N'switch_statement' +
	N'while_statement' +
	N'for_statement' +
	N'simple_statement' +
	T"{" * N'block'^-1 * T"}",
	
if_statement = TK"if" * T"(" * N'expression' * T")" * NI'statement' * (TK"else" * NI'statement')^-1,

switch_statement = TK"switch" * T"(" * N'expression' * T")" * T"{" * N'case_statement'^0 * T"}",

while_statement = 
	TK"while" * T"(" * N'expression' * T")" * NI'statement' +
	TK"do" * NI'statement' * T"while" * T"(" * N'expression' * T")" * T";",
	
for_statement = 
	TK"for" * T"(" * N'expression'^-1 * T";" * N'expression'^-1 * T";" * N'expression'^-1 * T")" * NI'statement',
	
simple_statement =
	TK"goto" * N'identifier' * T";" +
	TK"continue" * T";" +
	TK"break" * T";" +
	TK"return" * N'expression'^-1 * T";" +
	N'funct_call' * T";" +
	N'expression_statement' +
	T";",
	
expression_statement = NI'expression' * T";",

block =  (NI'statement' + N'declaration' + N'preprocessor' + N'label')^1,

label = NI'identifier_name' * T":",

case_statement = 
	(N'identifier' + TK"case" * N'constant_expression' + TK"default") * T":" * N'block'^-1,

expression =
	NI'assignment_expression' * (T"," * NI'assignment_expression')^0,

assignment_expression =  
	(NI'unary_expression' * N'assignment_operator')^0 * NI'conditional_expression',

assignment_operator = T"*=" + T"/=" + T"%=" + T"+=" + T"-=" + T"<<=" + T">>=" + T"&=" +
	(T"=" *-P"=") + T"^=" + T"|=",
	
conditional_expression =
	NI'simple_expression' * (T"?" * NI'expression' * T":" * NI'conditional_expression')^-1,
	
constant_expression =  NI'conditional_expression',

simple_expression = 
	NI'unary_expression' * (N'binary_operator' * NI'unary_expression')^0,
	
binary_operator = T"*" + T"/" + T"%" + T"+" + T"-" + T"<<" + T">>" + T"<=" + T">=" +
	T"<" + T">" + T"==" + T"!=" + T"&&"+ T"&" + T"||" + T"|" + T"^",

cast_expression =
	N'cast'^1 * NI'unary_expression',
	
cast = T"(" * NI'type_name' * T")",

unary_expression =
	TK"sizeof" * T"(" * NI'type_name' * T")" +
	NI'prefix_operator' * NI'unary_expression' +
	NI'postfix_expression' +
	NI'cast_expression',

prefix_operator = T"++" + T"--" + T"&" + T"*" + T"+" + T"-" + T"~" + T"!" + TK'sizeof',
	
postfix_expression =
	(T"(" * N'expression' * T")" +
	N'funct_call' + 
	N'identifier' + N'constant') * 
	(
	T"[" * NI'expression' * T"]" +
	T"." * NI'postfix_expression' +
	T"->" * NI'postfix_expression' + 
	N'postfix_operator'
	)^0,
	
postfix_operator = T"++" + T"--",

funct_call = N'identifier' * T"(" * (N'funct_parameter' * (T"," * N'funct_parameter')^0)^-1 * T")",

funct_parameter = NI'assignment_expression',

constant =
	N'number_constant' +
	N'character_constant' +
	N'string_constant' --+
	-- N'enumeration_constant'
	,

comment =			-- spaces before comments are ignored 
	S(" \t")^0 * (N'doc_comment' + N'multiline_comment' + N'line_comment'),

-- TERMINALS
number_constant = NI'number_literal',
character_constant = NI'character_literal',
string_constant = NI'string_literal',
-- enumeration_constant = , -- TODO
identifier = NI'identifier_name',
typedef_name = NI'identifier_name',

doc_comment = TP(P"/**" * (1 - P"*/")^0 * P"*/"),
multiline_comment = TP(P"/*" * (1 - P"*/")^0 * P"*/"),
line_comment = TP(P"//" * (1 - S"\r\n")^0),

-- LITERALS
unknown =  N'unknown_word'^1 * N'nl'^0,	-- anything divided to words
unknown_word = N'whites'^-1 * TP((1 - S" \t\r\n")^1),
whites = TP(S(" \t")^1),						-- spaces and tabs
nl = S(" \t")^0 * TP(P"\r"^-1*P"\n"), 				-- single newline, preceding spaces are ignored	

digit = R"09",
hex = R("af", "AF", "09"),
e = S"eE" * S"+-"^-1 * NI'digit'^1,
fs = S"fFlL",
is = S"uUlL"^0,
hexnum = P"0" * S"xX" * NI'hex'^1 * NI'is'^-1,
octnum = P"0" * NI'digit'^1 * NI'is'^-1,
decnum = NI'digit'^1 * NI'is'^-1,
floatnum = NI'digit'^1 * NI'e' * NI'fs'^-1 +
	NI'digit'^0 * P"." * NI'digit'^1 * NI'e'^-1 * NI'fs'^-1 +
	NI'digit'^1 * P"." * NI'digit'^0 * NI'e'^-1 * NI'fs'^-1,
		 
number_literal = T(NI'hexnum' + NI'octnum' + NI'floatnum' + NI'decnum'),
character_literal = T(P"L"^-1 * P"'" * (P"\\" * P(1) + (1 - S"\\'"))^1 * P"'"),
string_literal = T(P"L"^-1 * P'"' * (P"\\" * P(1) + (1 - S'\\"'))^0 * P'"'),

letter = R("az", "AZ") + P"_",
alnum = NI'letter' + NI'digit',
keyword =
	P"auto" + P"break" + P"case" + P"char" + P"const" +
	P"continue" + P"default" + P"do" + P"double" + P"else" + P"enum" + P"extern" +
	P"float" + P"for" + P"goto" + P"if" + P"inline" + P"int" + P"long" +
	P"register" + P"return" + P"short" + P"signed" + P"sizeof" + P"static" +
	P"struct" + P"switch" + P"typedef" + P"union" + P"unsigned" + P"void" + P"volatile" +
	P"while",
identifier_name = T(NI'letter' * NI'alnum'^0 - NI'keyword' * (-NI'alnum')),
} 
-- ***  END OF GRAMMAR  ****

-- *** POSSIBLE GRAMMARS (ENTRY POINTS) ****
grammar[1] = "program"
program = P(grammar)
grammar[1] = "top_element"
top_element = P(grammar)
grammar[1] = "in_block"
in_block = P(grammar)

--*******************************************************************
-- TESTING - this script cannot be used by Analyzer.cpp when these lines are uncommented !!!

-- dofile('default_grammar.lua')
-- test("../../input/in.c", program)
