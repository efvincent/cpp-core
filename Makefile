#=============================================
# Configuration and target swapping
#=============================================

.DEFAULT_GOAL := all

# Internal build mode selector (user-facing entry points are make targets).
MODE ?= debug

# Directory configuration
SRC_DIR   := src
TEST_DIR  := tests
BUILD_DIR := build/$(MODE)
MOD_DIR   := $(BUILD_DIR)/modules
OBJ_DIR   := $(BUILD_DIR)/obj
COMPDB_DIR := $(BUILD_DIR)/compdb
DOC_DIR   := docs

# Cross-platform utilities
RM := rm -rf
MKDIR := mkdir -p

# Binary output
TARGET := build/core_sandbox_$(MODE)
COMPDB_FILE := compile_commands.json
DOXYFILE := docs/Doxyfile

#=============================================
# Toolchain & compilation flags
#=============================================

CXX := clang++

BASE_FLAGS := -std=c++26 -fno-exceptions -fno-rtti -Wall -Wextra -Wpedantic -I./src

ifeq ($(MODE), release)
	# High optimization release
	CXXFLAGS := $(BASE_FLAGS) -O3 -march=native -DNDEBUG
else ifeq ($(MODE), lto)
	# High optimization release with full LTO
	CXXFLAGS := $(BASE_FLAGS) -O3 -march=native -DNDEBUG -flto=full
else ifeq ($(MODE), instrument)
	# Sanitizer and instrumentation profile
	CXXFLAGS := $(BASE_FLAGS) -O0 -g -fsanitize=address,undefined -fno-omit-frame-pointer -DBUILD_INSTRUMENT
else
	# Default debug mode
	MODE     := debug
	CXXFLAGS := $(BASE_FLAGS) -O0 -g -DBUILD_DEBUG
endif

# Clang module configuration flags
MOD_FLAGS := -fprebuilt-module-path=$(MOD_DIR)
MJFLAGS   = $(if $(GENERATE_COMPDB),-MJ $(COMPDB_DIR)/$(subst /,_,$(patsubst $(OBJ_DIR)/%,%,$@)).json)

# ===========================================
# Recursive source discovery
# ===========================================

MODULE_SRCS := $(shell find $(SRC_DIR) -type f -name '*.cppm' | sort)
TEST_CPP_SRCS := $(shell find $(TEST_DIR) -type f -name '*.cpp' | sort)
TEST_CPPM_SRCS := $(shell find $(TEST_DIR) -type f -name '*.cppm' | sort)

MODULE_OBJS := $(patsubst $(SRC_DIR)/%.cppm,$(OBJ_DIR)/src/%.o,$(MODULE_SRCS))
TEST_CPP_OBJS := $(patsubst $(TEST_DIR)/%.cpp,$(OBJ_DIR)/tests/%.o,$(TEST_CPP_SRCS))
TEST_CPPM_OBJS := $(patsubst $(TEST_DIR)/%.cppm,$(OBJ_DIR)/tests/%.o,$(TEST_CPPM_SRCS))

APP_OBJS := $(TEST_CPP_OBJS) $(TEST_CPPM_OBJS)
ALL_OBJS := $(MODULE_OBJS) $(APP_OBJS)

# Point linker/consumers at every discovered prebuilt module.
LINK_MOD_FLAGS := $(foreach src,$(MODULE_SRCS),-fmodule-file=$(basename $(notdir $(src)))=$(MOD_DIR)/$(patsubst $(SRC_DIR)/%.cppm,%.pcm,$(src)))

# Automatic module dependency extraction from `import <module>;` and
# `export import <module>;` statements.
module_obj_from_name = $(patsubst $(SRC_DIR)/%.cppm,$(OBJ_DIR)/src/%.o,$(firstword $(filter %/$(1).cppm,$(MODULE_SRCS))))
imports_of_src = $(shell sed -n 's/^[[:space:]]*\(export[[:space:]]\+\)\?import[[:space:]]\([A-Za-z0-9_.]*\)[[:space:]]*;.*/\2/p' $(1))
module_import_obj_deps = $(strip $(foreach m,$(call imports_of_src,$(1)),$(call module_obj_from_name,$(m))))

$(foreach src,$(MODULE_SRCS),$(eval $(patsubst $(SRC_DIR)/%.cppm,$(OBJ_DIR)/src/%.o,$(src)): $(call module_import_obj_deps,$(src))))

# Ensure tests only compile after modules are built.
$(APP_OBJS): $(MODULE_OBJS)

#====================================
# Phase execution and meta rules
#====================================

.PHONY: all debug release lto instrument clean clean_all bear docs docs_pdf

all: $(TARGET)

debug:
	@$(MAKE) MODE=debug all

release:
	@$(MAKE) MODE=release all

lto:
	@$(MAKE) MODE=lto all

instrument:
	@$(MAKE) MODE=instrument all

# Rebuilds with Clang -MJ fragments and merges them into compile_commands.json.
bear:
	@$(RM) $(COMPDB_FILE)
	@$(RM) $(BUILD_DIR)
	@$(MKDIR) $(COMPDB_DIR)
	@$(MAKE) MODE=$(MODE) GENERATE_COMPDB=1 all
	@{ printf '[\n'; find $(COMPDB_DIR) -type f -name '*.json' | sort | xargs -r cat; } | sed '$$s/,$$//' > $(COMPDB_FILE)
	@printf '\n]\n' >> $(COMPDB_FILE)

docs:
	@$(MKDIR) $(DOC_DIR)
	doxygen $(DOXYFILE)

docs_pdf: docs
	@missing=0; \
	for pkg in listofitems.sty ulem.sty; do \
		if ! kpsewhich $$pkg >/dev/null 2>&1; then \
			echo "Missing TeX package file: $$pkg"; \
			missing=1; \
		fi; \
	done; \
	if [ $$missing -ne 0 ]; then \
		echo "Install required LaTeX packages (example: tlmgr --usermode install listofitems ulem)."; \
		exit 1; \
	fi
	@$(MAKE) -C $(DOC_DIR)/latex

# Final link
$(TARGET): $(ALL_OBJS)
	@$(MKDIR) build
	$(CXX) $(CXXFLAGS) $(ALL_OBJS) -o $(TARGET)

# Compile module interface units discovered under src/
$(OBJ_DIR)/src/%.o: $(SRC_DIR)/%.cppm
	@$(MKDIR) $(dir $(MOD_DIR)/$*.pcm) $(@D) $(if $(GENERATE_COMPDB),$(COMPDB_DIR))
	$(CXX) $(CXXFLAGS) $(MOD_FLAGS) $(LINK_MOD_FLAGS) $(MJFLAGS) -fmodule-output=$(MOD_DIR)/$*.pcm -c $< -o $@

# Compile test .cpp translation units under tests/
$(OBJ_DIR)/tests/%.o: $(TEST_DIR)/%.cpp
	@$(MKDIR) $(@D) $(if $(GENERATE_COMPDB),$(COMPDB_DIR))
	$(CXX) $(CXXFLAGS) $(LINK_MOD_FLAGS) $(MJFLAGS) -c $< -o $@

# Compile test .cppm translation units as standard C++ sources
$(OBJ_DIR)/tests/%.o: $(TEST_DIR)/%.cppm
	@$(MKDIR) $(@D) $(if $(GENERATE_COMPDB),$(COMPDB_DIR))
	$(CXX) $(CXXFLAGS) $(LINK_MOD_FLAGS) $(MJFLAGS) -x c++ -c $< -o $@

clean:
	$(RM) build/$(MODE)
	$(RM) $(TARGET)

clean_all:
	$(RM) build
	$(RM) $(COMPDB_FILE)