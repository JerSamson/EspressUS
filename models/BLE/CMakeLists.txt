set(SOURCES GATT/gatts_table.h)

idf_component_register(SRCS ${SOURCES}
                      INCLUDE_DIRS .
                      REQUIRES driver) 