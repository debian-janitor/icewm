cmake_minimum_required(VERSION 3.2)
PROJECT(ICEWM CXX)

#
# Which themes to install in alphabetical order
#
set(themesList CrystalBlue Helix Infadel2 NanoBlue
    default icedesert metal2 motif win95)

foreach(_src keys menu programs toolbar winoptions IceWM.jpg)
    set(_pre "${CMAKE_CURRENT_SOURCE_DIR}/${_src}.in")
    if(EXISTS "${_pre}")
        CONFIGURE_FILE("${_pre}" "${CMAKE_CURRENT_BINARY_DIR}/${_src}")
        install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${_src}"
                DESTINATION "${LIBDIR}")
    else()
        install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/${_src}"
                DESTINATION "${LIBDIR}")
    endif()
endforeach()

install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/icewm.desktop" DESTINATION "${DATADIR}/xsessions/")
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/icewm-session.desktop" DESTINATION "${DATADIR}/xsessions/")

install(DIRECTORY mailbox taskbar icons ledclock
        DESTINATION "${LIBDIR}"
        FILES_MATCHING PATTERN "*"
        PATTERN "*.in" EXCLUDE)

foreach(theme ${themesList})
    install(DIRECTORY "themes/${theme}"
            DESTINATION "${LIBDIR}/themes")
endforeach()


