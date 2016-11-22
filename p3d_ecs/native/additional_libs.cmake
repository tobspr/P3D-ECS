
if (WIN32)

    set(PROTOBUF_LOC "${CMAKE_CURRENT_LIST_DIR}/../../thirdparty/protobuf/protobuf-built")

    if (NOT (EXISTS "${PROTOBUF_LOC}"))
        message(FATAL_ERROR "Could not find protobuf library! Expected it at: '${PROTOBUF_LOC}'")
    endif()

    include_directories(${PROTOBUF_LOC})
    link_directories(${PROTOBUF_LOC})

    set(LIBRARIES "${LIBRARIES};libprotobuf")

else()
    
    find_package(Protobuf REQUIRED)
    include_directories(${PROTOBUF_INCLUDE_DIRS})
    set(LIBRARIES "${LIBRARIES};${PROTOBUF_LIBRARIES}")

endif()

add_definitions("/DGOOGLE_PROTOBUF_NO_RTTI=1")
