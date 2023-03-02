if(DEBUG)
    add_definitions(-DDEBUG=1)
endif(DEBUG)

if(UI)
    add_definitions(-DUI=1)
endif(UI)

# copying target dir to destination dir. If destination does not exist, create it.
macro(copy_dir dir1 dir2)
    set(srcDir ${CMAKE_SOURCE_DIR}/${dir1})
    set(destDir ${CMAKE_BINARY_DIR}/${dir2})

    message(STATUS "Creating directory ${destDir}")
    make_directory(${destDir})

    message(STATUS "Search files in directory ${srcDir}")
    file(GLOB_RECURSE templateFiles RELATIVE ${srcDir} ${srcDir}/*)

    message(STATUS "Copying files from directory ${srcDir}")
    foreach(templateFile ${templateFiles})
        set(srcTemplatePath ${srcDir}/${templateFile})
        if(NOT IS_DIRECTORY ${srcTemplatePath})
            message(STATUS "Target file path ${srcTemplatePath}")
            message(STATUS "Destination file path ${destDir}/${templateFile}")
            configure_file(
                    ${srcTemplatePath}
                    ${destDir}/${templateFile}
                    COPYONLY)
        endif(NOT IS_DIRECTORY ${srcTemplatePath})
    endforeach(templateFile)

endmacro(copy_dir)

# delete build directory
macro(delete_build_dir dir)
    set(buildDir ${CMAKE_BINARY_DIR}/${dir})
    message(STATUS "Deleting build directory ${buildDir}")
    file(REMOVE_RECURSE ${buildDir})
endmacro(delete_build_dir)

# delete destination dir and copy target dir into destination dir.
macro(replace_dirs targetDir destDir)
    delete_build_dir(${destDir})
    copy_dir(${targetDir} ${destDir})
endmacro(replace_dirs)

# replace file
macro(replace_file srcFile destFile)
    if (EXISTS ${srcFile})
        file(REMOVE_RECURSE ${destFile})
        configure_file(${srcFile} ${destFile} COPYONLY)
    endif()
endmacro()

# replace dll/lib files and link to lib file
macro(dynamic_link target src)
    replace_file(${PROJECT_SOURCE_DIR}/${src}.lib ${PROJECT_BINARY_DIR}/${src}.lib)
    replace_file(${PROJECT_SOURCE_DIR}/${src}.dll ${PROJECT_BINARY_DIR}/${src}.dll)
    target_link_libraries(${target} PUBLIC ${src})
endmacro()

macro(move_to_build file)
    replace_file(${PROJECT_SOURCE_DIR}/${file} ${PROJECT_BINARY_DIR}/${file})
endmacro()