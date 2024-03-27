idf_component_register(SRCS "cmd_system_sleep.cpp" "cmd_system.cpp" "cmd_system_common.cpp"
                    INCLUDE_DIRS .
                    REQUIRES console spi_flash driver)

target_sources(${COMPONENT_LIB} PRIVATE cmd_system_sleep.cpp)
