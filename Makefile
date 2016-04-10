CXX = g++
# Define warnings pedantically in order to get comprehensive error messages
CXXFLAGS = -g -O -std=c++11 -Wall -Weffc++ -pedantic  \
-pedantic-errors -Wextra \
-Wcast-qual -Wconversion \
-Werror -Wfloat-equal -Wformat=2 \
-Wformat-nonliteral -Wformat-security  \
-Wformat-y2k \
-Wimport  -Winit-self  \
-Winvalid-pch   \
-Wlong-long \
-Wmissing-field-initializers -Wmissing-format-attribute   \
-Wmissing-include-dirs -Wmissing-noreturn \
-Wpacked -Wpointer-arith \
-Wredundant-decls \
-Wshadow -Wstack-protector \
-Wunreachable-code -Wunused \
-Wunused-parameter \
-Wvariadic-macros \

TARGET = matlang2c

SRCDIR = src
BUILDDIR = build

SOURCES = $(SRCDIR)/main.cpp $(SRCDIR)/preprocessor.cpp \
		  $(SRCDIR)/token.cpp $(SRCDIR)/regex.cpp \
		  $(SRCDIR)/lexer.cpp $(SRCDIR)/parser.cpp \
		  $(SRCDIR)/symbol_table.hpp $(SRCDIR)/semantic_analyzer.cpp \
		  $(SRCDIR)/code_generator.cpp

OBJECTS = $(BUILDDIR)/main.o $(BUILDDIR)/preprocessor.o \
		  $(BUILDDIR)/token.o $(BUILDDIR)/regex.o \
		  $(BUILDDIR)/lexer.o $(BUILDDIR)/parser.o \
		  $(BUILDDIR)/symbol_table.o $(BUILDDIR)/semantic_analyzer.o \
		  $(BUILDDIR)/code_generator.o

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET)

$(BUILDDIR)/main.o: $(SRCDIR)/main.cpp \
					$(SRCDIR)/regex.hpp \
					$(SRCDIR)/preprocessor.hpp \
					$(SRCDIR)/lexer.hpp \
					$(SRCDIR)/token.hpp \
					$(SRCDIR)/definitions.hpp
	$(CXX) $(CXXFLAGS) $(SRCDIR)/main.cpp -c -o $(BUILDDIR)/main.o

$(BUILDDIR)/preprocessor.o: $(SRCDIR)/preprocessor.hpp \
							$(SRCDIR)/preprocessor.cpp
	$(CXX) $(CXXFLAGS) $(SRCDIR)/preprocessor.cpp -c -o $(BUILDDIR)/preprocessor.o

$(BUILDDIR)/token.o: $(SRCDIR)/token.hpp \
					$(SRCDIR)/regex.hpp \
					$(SRCDIR)/token.cpp
	$(CXX) $(CXXFLAGS) $(SRCDIR)/token.cpp -c -o $(BUILDDIR)/token.o

$(BUILDDIR)/regex.o: $(SRCDIR)/regex.hpp \
					$(SRCDIR)/regex.cpp
	$(CXX) $(CXXFLAGS) $(SRCDIR)/regex.cpp -c -o $(BUILDDIR)/regex.o

$(BUILDDIR)/lexer.o: $(SRCDIR)/lexer.hpp \
					$(SRCDIR)/token.hpp \
					$(SRCDIR)/regex.hpp \
					$(SRCDIR)/lexer.cpp
	$(CXX) $(CXXFLAGS) $(SRCDIR)/lexer.cpp -c -o $(BUILDDIR)/lexer.o

$(BUILDDIR)/parser.o: $(SRCDIR)/parser.hpp \
					$(SRCDIR)/token.hpp \
					$(SRCDIR)/parser.cpp
	$(CXX) $(CXXFLAGS) $(SRCDIR)/parser.cpp -c -o $(BUILDDIR)/parser.o

$(BUILDDIR)/symbol_table.o: $(SRCDIR)/symbol_table.hpp \
							$(SRCDIR)/token.hpp \
							$(SRCDIR)/symbol_table.cpp
	$(CXX) $(CXXFLAGS) $(SRCDIR)/symbol_table.cpp -c -o $(BUILDDIR)/symbol_table.o

$(BUILDDIR)/semantic_analyzer.o: $(SRCDIR)/semantic_analyzer.hpp \
								$(SRCDIR)/token.hpp \
								$(SRCDIR)/symbol_table.hpp \
								$(SRCDIR)/semantic_analyzer.cpp
	$(CXX) $(CXXFLAGS) $(SRCDIR)/semantic_analyzer.cpp -c -o $(BUILDDIR)/semantic_analyzer.o

$(BUILDDIR)/code_generator.o: $(SRCDIR)/code_generator.hpp \
							$(SRCDIR)/symbol_table.hpp \
							$(SRCDIR)/code_generator.cpp
	$(CXX) $(CXXFLAGS) $(SRCDIR)/code_generator.cpp -c -o $(BUILDDIR)/code_generator.o

clean:
	rm $(OBJECTS)
	rm $(TARGET)
