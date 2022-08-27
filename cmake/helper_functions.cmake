function(get_project_name)
	# get relative path between PROJECT_SOURCE_DIR and this directory and store it in PROJECTPATH
	file(RELATIVE_PATH PROJECTPATH ${PROJECT_SOURCE_DIR} ${CMAKE_CURRENT_LIST_DIR})
	string(REPLACE "/" "." PROJECTNAME ${PROJECTPATH})
	# output by setting PROJECTNAME to the PROJECTNAME variable in the calling function
	set(PROJECTNAME ${PROJECTNAME} PARENT_SCOPE)
endfunction()

function(setup_additional_information PROJECTNAME)
	target_include_directories(${PROJECTNAME} PUBLIC
		${PROJECT_SOURCE_DIR}
	)
endfunction()

function(setup_library SOURCES)
	get_project_name()
	message("Creating ${PROJECTNAME} library")
	add_library(${PROJECTNAME} STATIC ${SOURCES})
	setup_additional_information(${PROJECTNAME})
endfunction()

function(setup_executable SOURCES)
	get_project_name()
	message("Creating ${PROJECTNAME} executable")
	add_executable(${PROJECTNAME} ${SOURCES})
	setup_additional_information(${PROJECTNAME})
endfunction()