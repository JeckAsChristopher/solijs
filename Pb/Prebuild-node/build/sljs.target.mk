# This file is generated by gyp; do not edit.

TOOLSET := target
TARGET := sljs
DEFS_Debug := \
	'-DNODE_GYP_MODULE_NAME=sljs' \
	'-DUSING_UV_SHARED=1' \
	'-DUSING_V8_SHARED=1' \
	'-DV8_DEPRECATION_WARNINGS=1' \
	'-D_GLIBCXX_USE_CXX11_ABI=1' \
	'-D_LARGEFILE_SOURCE' \
	'-D_FILE_OFFSET_BITS=64' \
	'-D__STDC_FORMAT_MACROS' \
	'-DNAPI_DISABLE_CPP_EXCEPTIONS' \
	'-DBUILDING_NODE_EXTENSION' \
	'-D_GLIBCXX_USE_C99_MATH' \
	'-DDEBUG' \
	'-D_DEBUG'

# Flags passed to all source files.
CFLAGS_Debug := \
	-fPIC \
	-Wall \
	-Wextra \
	-Wno-unused-parameter \
	-g \
	-O0 \
	-fPIC

# Flags passed to only C files.
CFLAGS_C_Debug :=

# Flags passed to only C++ files.
CFLAGS_CC_Debug := \
	-fno-rtti \
	-fno-strict-aliasing \
	-std=gnu++20 \
	-std=c++17

INCS_Debug := \
	-I/data/data/com.termux/files/home/.cache/node-gyp/23.11.0/include/node \
	-I/data/data/com.termux/files/home/.cache/node-gyp/23.11.0/src \
	-I/data/data/com.termux/files/home/.cache/node-gyp/23.11.0/deps/openssl/config \
	-I/data/data/com.termux/files/home/.cache/node-gyp/23.11.0/deps/openssl/openssl/include \
	-I/data/data/com.termux/files/home/.cache/node-gyp/23.11.0/deps/uv/include \
	-I/data/data/com.termux/files/home/.cache/node-gyp/23.11.0/deps/zlib \
	-I/data/data/com.termux/files/home/.cache/node-gyp/23.11.0/deps/v8/include \
	"-I$(srcdir)/\"/data/data/com.termux/files/home/sljs/node_modules/node-addon-api\"" \
	-I$(srcdir)/node_modules/node-addon-api

DEFS_Release := \
	'-DNODE_GYP_MODULE_NAME=sljs' \
	'-DUSING_UV_SHARED=1' \
	'-DUSING_V8_SHARED=1' \
	'-DV8_DEPRECATION_WARNINGS=1' \
	'-D_GLIBCXX_USE_CXX11_ABI=1' \
	'-D_LARGEFILE_SOURCE' \
	'-D_FILE_OFFSET_BITS=64' \
	'-D__STDC_FORMAT_MACROS' \
	'-DNAPI_DISABLE_CPP_EXCEPTIONS' \
	'-DBUILDING_NODE_EXTENSION' \
	'-D_GLIBCXX_USE_C99_MATH'

# Flags passed to all source files.
CFLAGS_Release := \
	-fPIC \
	-Wall \
	-Wextra \
	-Wno-unused-parameter \
	-O3 \
	-fno-omit-frame-pointer \
	-fPIC \
	-I/sources/android/cpufeatures

# Flags passed to only C files.
CFLAGS_C_Release :=

# Flags passed to only C++ files.
CFLAGS_CC_Release := \
	-fno-rtti \
	-fno-strict-aliasing \
	-std=gnu++20 \
	-std=c++17

INCS_Release := \
	-I/data/data/com.termux/files/home/.cache/node-gyp/23.11.0/include/node \
	-I/data/data/com.termux/files/home/.cache/node-gyp/23.11.0/src \
	-I/data/data/com.termux/files/home/.cache/node-gyp/23.11.0/deps/openssl/config \
	-I/data/data/com.termux/files/home/.cache/node-gyp/23.11.0/deps/openssl/openssl/include \
	-I/data/data/com.termux/files/home/.cache/node-gyp/23.11.0/deps/uv/include \
	-I/data/data/com.termux/files/home/.cache/node-gyp/23.11.0/deps/zlib \
	-I/data/data/com.termux/files/home/.cache/node-gyp/23.11.0/deps/v8/include \
	"-I$(srcdir)/\"/data/data/com.termux/files/home/sljs/node_modules/node-addon-api\"" \
	-I$(srcdir)/node_modules/node-addon-api

OBJS := \
	$(obj).target/$(TARGET)/libs/core.o

# Add to the list of files we specially track dependencies for.
all_deps += $(OBJS)

# CFLAGS et al overrides must be target-local.
# See "Target-specific Variable Values" in the GNU Make manual.
$(OBJS): TOOLSET := $(TOOLSET)
$(OBJS): GYP_CFLAGS := $(DEFS_$(BUILDTYPE)) $(INCS_$(BUILDTYPE))  $(CFLAGS_$(BUILDTYPE)) $(CFLAGS_C_$(BUILDTYPE))
$(OBJS): GYP_CXXFLAGS := $(DEFS_$(BUILDTYPE)) $(INCS_$(BUILDTYPE))  $(CFLAGS_$(BUILDTYPE)) $(CFLAGS_CC_$(BUILDTYPE))

# Suffix rules, putting all outputs into $(obj).

$(obj).$(TOOLSET)/$(TARGET)/%.o: $(srcdir)/%.cpp FORCE_DO_CMD
	@$(call do_cmd,cxx,1)

# Try building from generated source, too.

$(obj).$(TOOLSET)/$(TARGET)/%.o: $(obj).$(TOOLSET)/%.cpp FORCE_DO_CMD
	@$(call do_cmd,cxx,1)

$(obj).$(TOOLSET)/$(TARGET)/%.o: $(obj)/%.cpp FORCE_DO_CMD
	@$(call do_cmd,cxx,1)

# End of this set of suffix rules
### Rules for final target.
LDFLAGS_Debug := \
	-rdynamic \
	-fPIC

LDFLAGS_Release := \
	-rdynamic \
	-fPIC

LIBS := \
	-llog

$(obj).target/sljs.node: GYP_LDFLAGS := $(LDFLAGS_$(BUILDTYPE))
$(obj).target/sljs.node: LIBS := $(LIBS)
$(obj).target/sljs.node: TOOLSET := $(TOOLSET)
$(obj).target/sljs.node: $(OBJS) FORCE_DO_CMD
	$(call do_cmd,solink_module)

all_deps += $(obj).target/sljs.node
# Add target alias
.PHONY: sljs
sljs: $(builddir)/sljs.node

# Copy this to the executable output path.
$(builddir)/sljs.node: TOOLSET := $(TOOLSET)
$(builddir)/sljs.node: $(obj).target/sljs.node FORCE_DO_CMD
	$(call do_cmd,copy)

all_deps += $(builddir)/sljs.node
# Short alias for building this executable.
.PHONY: sljs.node
sljs.node: $(obj).target/sljs.node $(builddir)/sljs.node

# Add executable to "all" target.
.PHONY: all
all: $(builddir)/sljs.node

