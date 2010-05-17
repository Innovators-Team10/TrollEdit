cfg_keys = {"text_style", "comment_style", "keyword", "line_comment", "multiline_comment", "doc_comment", "string_constant", "character_constant", "number_constant", "label", "header_file", "funct_call", "assignment_operator", "binary_operator", "prefix_operator", "postfix_operator", "markup_tag", "attribute_key", "attribute_value", "block_style", "control_st_style", "if_statement", "unknown", "preprocessor", "while_statement", "for_statement", "switch_statement", "simple_statement", "funct_parameter", "expression", "block", "funct_definition"}

text_style = {target="text", family="courier", size="12", color="black", bold="false", italic="false", underline="false"}
comment_style = {target="text", base_t="text_style", family="verdana", italic="true", color="black"}

keyword = {target="text", base_t="text_style", color="blue"}
line_comment = {target="text", base_t="comment_style"}
multiline_comment = {target="text", base_t="comment_style"}
doc_comment = {target="text", base_t="comment_style"}
string_constant = {target="text", base_t="text_style", color="darkmagenta"}
character_constant = {target="text", base_t="text_style", color="darkmagenta"}
number_constant = {target="text", base_t="text_style", color="orangered"}
label = {target="text", base_t="text_style", color="maroon"}
header_file = {target="both", base_t="text_style", color="brown", base_b="block_style", hovered="brown", hovered_border="brown"}
funct_call = {target="text", base_t="text_style"}
assignment_operator = {target="text", base_t="text_style"}
binary_operator = {target="text", base_t="text_style"}
prefix_operator = {target="text", base_t="text_style"}
postfix_operator = {target="text", base_t="text_style"}
markup_tag = {target="text", base_t="text_style", color="brown"}
attribute_key = {target="text", base_t="text_style", color="red"}
attribute_value = {target="text", base_t="text_style", color="blue"}

block_style = {target="block", hovered="white", hovered_border="gray", selected="gray", showing="gray"}
control_st_style ={target="block", hovered="blue", hovered_border="blue"}

unknown = {target="block", base_b="block_style", selected="red", showing="red", hovered="red", hovered_border="red"}
preprocessor = {target="block", base_b="block_style"}

while_statement = {target="block", base_b="control_st_style"}
if_statement = {target="block", base_b="control_st_style"}
for_statement = {target="block", base_b="control_st_style"}
switch_statement = {target="block", base_b="control_st_style"}
simple_statement = {target="block", base_b="block_style", hovered="green", hovered_border="green"}

funct_parameter = {target="block", base_b="block_style", hovered="brown", hovered_border="brown"}
expression = {target="block", base_b="block_style", hovered="brown", hovered_border="brown"}
block = {target="block", base_b="block_style", hovered="orange", hovered_border="orange"}

funct_definition = {target="both", base_b="block_style", base_t="text_style", bold="true"}
	
-- TODO: struct, union...?
-- styly by mali mat rieto bloky:
-- 	"preprocessor", "funct_definition", "declaration", 
-- 	"initializer", "block", "funct_parameter", "expression",  
-- 	"program", "header_file",
-- 	"unknown", "if_statement", "while_statement", 
-- 	"for_statement", "switch_statement", "simple_statement"