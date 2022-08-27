set(SOURCES
	Image.cpp
	Image.h
)

setup_library("${SOURCES}")

get_project_name()
target_link_libraries(${PROJECTNAME} PUBLIC
	Microsoft.GSL::GSL
)
