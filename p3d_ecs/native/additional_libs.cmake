
set(PROTOBUF_LOC "${CMAKE_CURRENT_LIST_DIR}/../../thirdparty/protobuf-built")

if (NOT (EXISTS "${PROTOBUF_LOC}"))
    message(FATAL_ERROR "Could not find protobuf library! Expected it at: '${PROTOBUF_LOC}'")
endif()

include_directories(${PROTOBUF_LOC})
link_directories(${PROTOBUF_LOC})

set(LIBRARIES "${LIBRARIES};libprotobuf")
add_definitions("/DGOOGLE_PROTOBUF_NO_RTTI=1")
