set(COMPONENT_SRCS "main.c" "task1.cpp" "task2.c")
set(COMPONENT_ADD_INCLUDEDIRS ".")
set(COMPONENT_REQUIRES "spidriver" "tft" "spiffs")
set(COMPONENT_PRIV_REQUIRES "nvs_flash")

register_component()


