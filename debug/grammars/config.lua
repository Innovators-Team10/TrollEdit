cfg_keys = {"text_style", "comment_style", "keyword", "line_comment", "multi_comment", "string_constant", "character_constant", "number_constant", "label", "funct_definition", "funct_call", "assignment_operator", "binary_operator", "prefix_operator", "postfix_operator"}

text_style = {family="courier", size="12", color="black", bold="false", italic="false", underline="false"}
comment_style = {base="text_style", family="verdana", italic="true"}

keyword = {base="text_style", color="blue"}
line_comment = {base="comment_style", color="gray"}
multi_comment = {base="comment_style", color="gray"}
string_constant = {base="text_style", color="darkmagenta"}
character_constant = {base="text_style", color="darkmagenta"}
number_constant = {base="text_style", color="orangered"}
label = {base="text_style", color="maroon"}
funct_definition = {base="text_style", bold="true"}
funct_call = {base="text_style"}
assignment_operator = {base="text_style"}
binary_operator = {base="text_style"}
prefix_operator = {base="text_style"}
postfix_operator = {base="text_style"}