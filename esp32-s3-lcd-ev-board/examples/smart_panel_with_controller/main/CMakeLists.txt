set(ldfragments linker.lf)
idf_component_register(SRC_DIRS          "."
                                         "./panel_main"
                                         "./panel_main/app"
                                         "./panel_main/icon"
                                         "./panel_main/ui"
                                         "./panel_main/lv_fonts"
                       PRIV_INCLUDE_DIRS "."
                                         "./panel_main"
                                         "./panel_main/app"
                                         "./panel_main/ui"
                                         "./panel_main/lv_fonts"
                       LDFRAGMENTS       "${ldfragments}")

set_property(TARGET ${COMPONENT_LIB} PROPERTY CXX_STANDARD 17)
target_compile_options(${COMPONENT_LIB} PRIVATE "-DCHIP_HAVE_CONFIG_H")

spiffs_create_partition_image(storage ../spiffs FLASH_IN_PROJECT)
