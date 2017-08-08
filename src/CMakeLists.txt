set(pwd ${CMAKE_CURRENT_SOURCE_DIR})
set(SOURCE 
	${SOURCE} 
	${pwd}/main.c 
	${pwd}/nhtml_string.c
	${pwd}/attribute.c 
	${pwd}/html.c
	${pwd}/includes.c
	PARENT_SCOPE)
