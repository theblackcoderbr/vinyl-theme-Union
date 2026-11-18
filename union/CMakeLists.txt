cmake_minimum_required(VERSION 3.16)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/../VERSION" VINYL_VERSION LIMIT_COUNT 1)
    project(vinyl-union VERSION ${VINYL_VERSION} LANGUAGES NONE)
endif()

# No architecture detection is needed: this project installs only data files.
set(CMAKE_INSTALL_DATADIR "share" CACHE PATH "Read-only architecture-independent data")

install(DIRECTORY org.ekaaty.vinyl
    DESTINATION "${CMAKE_INSTALL_DATADIR}/union/styles"
    COMPONENT Union
)

if(WITH_UNION_ONLY OR CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../colors/src/"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/color-schemes"
        COMPONENT Union
        FILES_MATCHING PATTERN "*.colors"
    )
endif()
