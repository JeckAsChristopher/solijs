CXX = clang++
CXXFLAGS = -O2 -std=c++17 -fPIC

# Node headers and node-addon-api include paths
NODE_INCLUDE = ./node_modules/node-addon-api
NODE_HEADERS = $(shell node -p "require('node:path').join(process.execPath, '..', '..', 'include', 'node')")

OUT_DIR = build
SRC_LINK = libs/core.cpp

OUT_LINK = $(OUT_DIR)/sljs.node

all: $(OUT_LINK)

$(OUT_LINK): $(SRC_LINK)
	@mkdir -p $(OUT_DIR)
	$(CXX) $(CXXFLAGS) -I$(NODE_INCLUDE) -I$(NODE_HEADERS) -shared -o $@ $^ -ldl

clean:
	rm -rf $(OUT_DIR)
