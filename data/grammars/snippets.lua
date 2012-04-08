--[*[ this file lists default code that will be shown after creating new file
-- one snippet per grammar extension is allowed for now
-- snippets might take advantage of any accessible lua code]*]

-- C source code
c = os.date("//created %d.%m.%Y") .. [[

#include<stdio.h>

int main(int argc, char *argv[] )  {
	return 0;
}
]]

-- xml file
xml = [[
<?xml version="1.0" encoding="UTF-8"?>
<tag>
	
</tag>
]]

-- lua file
lua = [[
function f()
return
end
]]

-- todo file
txt = [[
<todolists>
	<done>
  	<todolist>
  		<title>TODO1</title>
  		<author>Author</author>
  		<creationDate>date</creationDate>
  		<description>description</description>
  	</todolist>
  	<todolist>
  		<title>TODO2</title>
  		<author>Author</author>
  		<creationDate>date</creationDate>
  		<description>description</description>
  	</todolist>
	</done>
	<undone>
  	<todolist>
  		<title>TODO3</title>
  		<author>Author</author>
  		<creationDate>date</creationDate>
  		<description>description</description>
  	</todolist>
	</undone>
</todolists>
]]

