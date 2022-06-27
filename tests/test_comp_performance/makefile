MAKEFLAGS += -r # Remove implicit rules for building executables and .o files
# All Target
CRYPTODL_DIR=../..
$(shell $(CRYPTODL_DIR)/dependencies/versions.sh)
-include $(CRYPTODL_DIR)/dependencies/makefile.versions

SRC_FILES := $(wildcard *.cpp) $(wildcard */*.cpp)
OBJ_FILES := $(patsubst %.cpp,%.o,$(SRC_FILES))
DEP_FILES := $(patsubst %.cpp,%.d,$(SRC_FILES))
EXEC_FILES := $(patsubst %.cpp,%,$(SRC_FILES))

# Optimization (no debugging)
CXXFLAGS := -std=c++17 -O3 -g0 -Wall
# Debugging (no optimization)
# CXXFLAGS := -std=c++17 -O0 -g3 -Wall

all: $(EXEC_FILES)

-include $(DEP_FILES)

../TestCommons.o:
	cd ..; $(MAKE) TestCommons.o; 

%: %.o ../TestCommons.o
	@echo Linking tests
	g++ -std=c++17 $(CXXFLAGS) -o $@ $^ $(DEP_LIBS) $(CRYPTODL_DIR)/Debug/libkalypso.a  -lntl -lboost_filesystem -lboost_system -ljpeg -lpthread -lgmpxx -lgmp $(DEP_RPATH)

%.o: %.cpp
	@echo Building tests
	g++ -std=c++17 $(CXXFLAGS) $(DEP_INCLUDES) -I../../src -include../../src/data/mnist/include/mnist/mnist_reader_less.hpp -c -fmessage-length=0 -fopenmp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o $@ $<

clean:
	rm $(EXEC_FILES) $(DEP_FILES) $(OBJ_FILES)
	
