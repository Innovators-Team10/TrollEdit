cfg_keys = {"text_style", "comment_style", "keyword", 
"line_comment", "multiline_comment", "doc_comment", "string_constant", "character_constant", 
"number_constant", "label", "header_file", "funct_call", 
"assignment_operator", "binary_operator", "prefix_operator", "postfix_operator", 
"markup_tag", "attribute_key", "attribute_value",  "funct_definition"}

text_style = {family="courier", size="12", color="black", bold="false", italic="false", underline="false"}
comment_style = {base="text_style", family="verdana", italic="true", color="teal"}

keyword = {base="text_style", color="blue"}
comment = {base="comment_style"}
line_comment = {base="comment_style"}
multiline_comment = {base="comment_style"}
doc_comment = {base="comment_style"}
string_constant = {base="text_style", color="darkmagenta"}
character_constant = {base="text_style", color="darkmagenta"}
number_constant = {base="text_style", color="orangered"}
label = {base="text_style", color="maroon"}
header_file = {base="text_style", color="brown"}
funct_call = {base="text_style"}
assignment_operator = {base="text_style"}
binary_operator = {base="text_style"}
prefix_operator = {base="text_style"}
postfix_operator = {base="text_style"}
markup_tag = {base="text_style", color="brown"}
attribute_key = {base="text_style", color="red"}
attribute_value = {base="text_style", color="blue"}
funct_definition = {base="text_style", bold="true"}