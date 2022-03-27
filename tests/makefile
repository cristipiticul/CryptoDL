# All Target
CRYPTODL_DIR=..
$(shell $(CRYPTODL_DIR)/dependencies/versions.sh)
-include $(CRYPTODL_DIR)/dependencies/makefile.versions

SRC_FILES := $(wildcard *.cpp)
OBJ_FILES := $(patsubst %.cpp,%.o,$(SRC_FILES))
DEP_FILES := $(patsubst %.cpp,%.d,$(SRC_FILES))


all: $(OBJ_FILES)
	@echo Linking tests
	g++ -std=c++17 -o kalypso_tests $(OBJ_FILES) $(DEP_LIBS) ../Debug/libkalypso.a  -lntl -lboost_filesystem -lboost_system -ljpeg -lpthread -lgmpxx -lgmp $(DEP_RPATH)

-include $(DEP_FILES)

%.o: %.cpp
	@echo Building tests
	g++ -std=c++17 $(DEP_INCLUDES) -include../src/data/mnist/include/mnist/mnist_reader_less.hpp -O0 -g3 -Wall -c -fmessage-length=0 -fopenmp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o $@ $<

clean:
	rm kalypso_tests $(OBJ_FILES) $(DEP_FILES) 
	
