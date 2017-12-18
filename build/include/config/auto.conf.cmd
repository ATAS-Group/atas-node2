deps_config := \
	/Users/martinschmidli/esp/esp-idf/components/app_trace/Kconfig \
	/Users/martinschmidli/esp/projects/atas-node2/components/arduino/Kconfig \
	/Users/martinschmidli/esp/esp-idf/components/aws_iot/Kconfig \
	/Users/martinschmidli/esp/esp-idf/components/bt/Kconfig \
	/Users/martinschmidli/esp/esp-idf/components/esp32/Kconfig \
	/Users/martinschmidli/esp/esp-idf/components/ethernet/Kconfig \
	/Users/martinschmidli/esp/esp-idf/components/fatfs/Kconfig \
	/Users/martinschmidli/esp/esp-idf/components/freertos/Kconfig \
	/Users/martinschmidli/esp/esp-idf/components/heap/Kconfig \
	/Users/martinschmidli/esp/esp-idf/components/libsodium/Kconfig \
	/Users/martinschmidli/esp/esp-idf/components/log/Kconfig \
	/Users/martinschmidli/esp/esp-idf/components/lwip/Kconfig \
	/Users/martinschmidli/esp/esp-idf/components/mbedtls/Kconfig \
	/Users/martinschmidli/esp/esp-idf/components/openssl/Kconfig \
	/Users/martinschmidli/esp/esp-idf/components/pthread/Kconfig \
	/Users/martinschmidli/esp/esp-idf/components/spi_flash/Kconfig \
	/Users/martinschmidli/esp/esp-idf/components/spiffs/Kconfig \
	/Users/martinschmidli/esp/esp-idf/components/tcpip_adapter/Kconfig \
	/Users/martinschmidli/esp/esp-idf/components/wear_levelling/Kconfig \
	/Users/martinschmidli/esp/esp-idf/components/bootloader/Kconfig.projbuild \
	/Users/martinschmidli/esp/esp-idf/components/esptool_py/Kconfig.projbuild \
	/Users/martinschmidli/esp/esp-idf/components/partition_table/Kconfig.projbuild \
	/Users/martinschmidli/esp/esp-idf/Kconfig

include/config/auto.conf: \
	$(deps_config)


$(deps_config): ;
