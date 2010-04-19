cfg_keys = {"text_style", "comment_style", "keyword", "line_comment", "multi_comment", "string_constant", "character_constant", "number_constant", "label", "header_file", "assignment_operator", "binary_operator", "prefix_operator", "postfix_operator", "markup_tag", "attribute_key", "attribute_value", "block_style", "if_statement", "unknown", "preprocessor", "while_statement", "for_statement", "switch_statement", "funct_param", "block", "funct_definition", "funct_call"}

text_style = {target="text", family="courier", size="12", color="black", bold="false", italic="false", underline="false"}
comment_style = {target="text", base_t="text_style", family="verdana", italic="true"}

keyword = {target="text", base_t="text_style", color="blue"}
line_comment = {target="text", base_t="comment_style", color="gray"}
multi_comment = {target="text", base_t="comment_style", color="gray"}
string_constant = {target="text", base_t="text_style", color="darkmagenta"}
character_constant = {target="text", base_t="text_style", color="darkmagenta"}
number_constant = {target="text", base_t="text_style", color="orangered"}
label = {target="text", base_t="text_style", color="maroon"}
header_file = {target="text", base_t="text_style", color="lightpink"}
assignment_operator = {target="text", base_t="text_style"}
binary_operator = {target="text", base_t="text_style"}
prefix_operator = {target="text", base_t="text_style"}
postfix_operator = {target="text", base_t="text_style"}
markup_tag = {target="text", base_t="text_style", color="brown"}
attribute_key = {target="text", base_t="text_style", color="red"}
attribute_value = {target="text", base_t="text_style", color="blue"}

block_style = {target="block", hovered="white", hovered_border="black", selected="green", showing="goldenrod"}

if_statement = {target="block", base_b="block_style", hovered="yellow", hovered_border="red"}
unknown = {target="block", base_b="block_style", selected="red", showing="red", hovered="red", hovered_border="red"}
preprocessor = {target="block", base_b="block_style"}
while_statement = {target="block", base_b="block_style", hovered="blue", hovered_border="blue"}
for_statement = {target="block", base_b="block_style"}
switch_statement = {target="block", base_b="block_style"}
funct_param = {target="block", base_b="block_style"}
block = {target="block", base_b="block_style"}

funct_definition = {target="both", base_b="block_style", base_t="text_style", bold="true"}
funct_call = {target="both", base_t="text_style", base_b="block_style"}
	
	-- TODO: struct, union...?