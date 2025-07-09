file(GLOB_RECURSE OBJC_SOURCES ${PLATFORM_SRC}/Source/*.mm)
target_sources(SDK PRIVATE ${OBJC_SOURCES})

target_link_libraries(SDK PUBLIC "-framework Foundation")

target_compile_options(SDK PRIVATE "-fobjc-arc")