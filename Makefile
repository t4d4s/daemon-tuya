# =============================
# MAKEFILE FOR BUILDING EXECUTABLE
# =============================

# Directories
SRC_DIR := ./src
BUILD_DIR := ./build
SDK_DIR := ./tuya-sdk
SDK_BUILD_DIR := $(SDK_DIR)/build
EXEC_DIR := /usr/bin
CONF_DIR := /etc/ld.so.conf.d
LIB_DIR := /usr/lib/tuya_sdk
INCLUDE_DIR := /usr/include/tuya_sdk

# Output binary
TARGET_EXEC := tuyad

# Find source files (including utils/log.c)
SRC := $(wildcard $(SRC_DIR)/*.c) $(wildcard $(SDK_DIR)/utils/*.c)

# Object and dependency files
OBJS := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRC:.c=.o))
DEPS := $(OBJS:.o=.d)

# Shared libraries from Tuya SDK
LIBS := -L$(SDK_BUILD_DIR)/lib \
        -lutils_modules \
        -llink_core \
        -lmiddleware_implementation \
        -lplatform_port

# Headers required for compilation
CFLAGS := -Wall -fPIC \
          -I$(SDK_BUILD_DIR)/lib \
          -I$(SDK_DIR)/utils \
          -I$(SDK_DIR)/interface \
          -I$(SDK_DIR)/include

LDFLAGS := $(LIBS) -pthread -lm -lrt -Wl,-rpath=$(SDK_BUILD_DIR)/lib

# =============================
# PHONY TARGETS
# =============================

.PHONY: all clean install uninstall install-libs uninstall-libs \
        install-bin uninstall-bin cmake_check_build_dir help

# =============================
# BUILD TARGET
# =============================

# Default target
all: cmake_check_build_dir $(TARGET_EXEC)

# Ensure SDK is built before compiling
cmake_check_build_dir:
	@if [ ! -d "$(SDK_BUILD_DIR)" ]; then \
		echo "Creating Tuya SDK build directory and running CMake..."; \
		mkdir -p $(SDK_BUILD_DIR) && cd $(SDK_BUILD_DIR) && cmake -DBUILD_SHARED_LIBS=ON ..; \
	fi
	$(MAKE) -C $(SDK_BUILD_DIR)

# Build executable
$(TARGET_EXEC): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $(TARGET_EXEC)
	@if [ ! -d "$(LIB_DIR)" ]; then \
		echo "-----------------------------------------"; \
		echo "!!!!! Run 'sudo make install-libs' !!!!!"; \
		echo "-----------------------------------------"; \
	fi

# Compile source files into object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# =============================
# INSTALL & UNINSTALL TARGETS
# =============================

# Install binary & libraries
install: install-libs install-bin

uninstall: uninstall-libs uninstall-bin

# Install Tuya SDK shared libraries
install-libs:
	@echo '$(LIB_DIR)' > $(CONF_DIR)/tuya.conf
	@mkdir -p $(INCLUDE_DIR)
	@if [ -n "$(HEADERS)" ]; then install $(HEADERS) $(INCLUDE_DIR); fi
	@mkdir -p $(LIB_DIR)
	@install -m 755 $(SDK_BUILD_DIR)/lib/*.so $(LIB_DIR)
	@ldconfig

# Uninstall Tuya SDK libraries
uninstall-libs:
	rm -rf $(INCLUDE_DIR)
	rm -rf $(LIB_DIR)
	rm -f $(CONF_DIR)/tuya.conf
	ldconfig

# Install binary
install-bin: $(TARGET_EXEC)
	install -m 755 $(TARGET_EXEC) $(EXEC_DIR)/$(TARGET_EXEC)

# Uninstall binary
uninstall-bin:
	rm -f $(EXEC_DIR)/$(TARGET_EXEC)

# =============================
# CLEAN TARGET
# =============================

# Clean build files and Tuya SDK build directory
clean:
	rm -rf $(BUILD_DIR) $(TARGET_EXEC)
	rm -rf $(SDK_BUILD_DIR)
	rm -f $(SRC_DIR)/*.o

# =============================
# HELP COMMAND
# =============================

help:
	@echo "MAKEFILE COMMANDS:"
	@echo "  make              - Build the project"
	@echo "  make clean        - Remove compiled files and Tuya SDK build folder"
	@echo "  make install      - Install binary & libraries (requires sudo)"
	@echo "  make uninstall    - Uninstall everything"
	@echo "  make install-libs - Install Tuya SDK libraries"
	@echo "  make uninstall-libs - Remove Tuya SDK libraries"
	@echo "  make install-bin  - Install only the binary"
	@echo "  make uninstall-bin - Remove only the binary"
	@echo "  make cmake_check_build_dir - Ensure Tuya SDK is built"

# Include dependencies
-include $(DEPS)
