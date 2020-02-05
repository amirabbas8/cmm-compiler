CFLAGS := -g -Wall -Wextra -Wpedantic -std=c++11


INC := -Imipser
BUILD_DIR := build
SRC_DIR := src
TEST_DIR := test


# Make the parser
it : $(BUILD_DIR)/parser_main.o $(BUILD_DIR)/parser.tab.o $(BUILD_DIR)/lexer.yy.o
	@echo "Linking..."
	@echo " g++ $^ -o $@"; g++ $^ -o $@

$(BUILD_DIR)/%.o : $(SRC_DIR)/%.cpp
	@echo "Building sources..."
	@echo " mkdir -p $(BUILD_DIR)"; mkdir -p $(BUILD_DIR)
	@echo " g++ $(CFLAGS) $(INC) -c -o $@ $<"; g++ $(CFLAGS) $(INC) -c -o $@ $<

$(BUILD_DIR)/%.yy.o : $(BUILD_DIR)/%.yy.cpp
	@echo "Building lex..."
	@echo " mkdir -p $(BUILD_DIR)"; mkdir -p $(BUILD_DIR)
	@echo " g++ $(CFLAGS) $(INC) -c -o $@ $<"; g++ $(CFLAGS) $(INC) -c -o $@ $<

$(BUILD_DIR)/parser.tab.o : $(BUILD_DIR)/parser.tab.cpp
	@echo "Building yacc..."
	@echo " mkdir -p $(BUILD_DIR)"; mkdir -p $(BUILD_DIR)
	@echo " g++ $(CFLAGS) $(INC) -c -o $@ $<"; g++ $(CFLAGS) $(INC) -c -o $@ $<

$(BUILD_DIR)/lexer.yy.cpp : $(SRC_DIR)/lexer.flex
	@echo "Creating lex..."
	@echo " flex -o $@  $<"; flex -o $@  $<

$(BUILD_DIR)/parser.tab.cpp $(BUILD_DIR)/parser.tab.hpp : $(SRC_DIR)/parser.y
	@echo "Creating yacc..."
	@echo " bison -v -d $< -o $(BUILD_DIR)/parser.tab.cpp"; bison -v -d $< -o $(BUILD_DIR)/parser.tab.cpp

clean :
	@echo "Cleaning..."
	rm -rf it
	rm -rf $(TEST_DIR)/*.s
	rm -rf $(TEST_DIR)/*.err
	rm -rf $(BUILD_DIR)
