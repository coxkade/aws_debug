current_dir = $(shell pwd)
build_dir := $(current_dir)/build
graph_dir := $(build_dir)/graph
config_file := build/sdkconfig
UNAME_S := $(shell uname -s)
X_GCC := $(shell which xtensa-esp32-elf-gcc)


ifeq ($(USE_NINJA),1)
	generator = "-GNinja"
	builder = ninja
	build_message = "Using Ninja"
else
	generator = ""
	builder = make
	build_message = "Using Make"
endif

 ifeq (, $(X_GCC))
 $(error "Failed to find xtensa-esp32-elf-gcc in path")
 endif

 afs_path := $(shell dirname $(X_GCC))
 ifeq ($(UNAME_S),Darwin)
	mon_port=/dev/cu.SLAB_USBtoUART
else
	mon_port=/dev/ttyUSB0
endif



.PHONY: all clean rm_dir flash config menuconfig iconconfig print_test conf_gen disassembly monitor

all: $(config_file)
	@echo $(build_message)
	AFR_TOOLCHAIN_PATH=$(afs_path) $(builder) -C $(build_dir) kade_app

clean: $(config_file)
	@echo $(build_message)
	AFR_TOOLCHAIN_PATH=$(afs_path) $(builder) -C $(build_dir) clean

flash: $(config_file)
	@echo $(build_message)
	ESPPORT=${mon_port} AFR_TOOLCHAIN_PATH=$(afs_path) $(builder) -C $(build_dir) flash

menuconfig: $(config_file)
	@echo $(build_message)
	AFR_TOOLCHAIN_PATH=$(afs_path) $(builder) -C $(build_dir) menuconfig

disassembly: $(config_file)
	@echo $(build_message)
	AFR_TOOLCHAIN_PATH=$(afs_path) $(builder) -C $(build_dir) disassembly

monitor: $(config_file)
	ESPPORT=${mon_port} AFR_TOOLCHAIN_PATH=$(afs_path) $(builder) -C $(build_dir) monitor

rm_dir: $(build_dir)
	@echo $(build_message)
	$(RM) -rf $(build_dir)

config: $(build_dir) 
	cmake -S . -B build ${generator} -DCMAKE_TOOLCHAIN_FILE=amazon-freertos/tools/cmake/toolchains/xtensa-esp32.cmake

conf_gen: $(build_dir) $(graph_dir)

$(config_file): $(build_dir)
	@echo "$(config_file) Changed"
	@if [ -f $(config_file) ]; \
	then echo "Check File Exists"; \
	else make -C ${current_dir} config; \
	fi

$(build_dir):
	mkdir $(build_dir)	

$(graph_dir): $(build_dir)
	@if [ -d $(graph_dir) ]; \
	then echo "$(graph_dir) Exists"; \
	else mkdir $(graph_dir); \
	fi
