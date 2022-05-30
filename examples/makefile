# All Target
CRYPTODL_DIR=..
$(shell $(CRYPTODL_DIR)/dependencies/versions.sh)
-include $(CRYPTODL_DIR)/dependencies/makefile.versions

# CXXFLAGS := -std=c++17 -O3 -g0 -Wall
CXXFLAGS := -std=c++17 -O0 -g3 -Wall

all: cnn cnn_seal rnn

cnn:
	@echo Building cnn examples
	g++ $(CXXFLAGS) $(INCLUDE_DIRS) $(DEP_INCLUDES) -c mnist_cnn/mnist_cnn.cpp
	@echo Linking rnn examples
	g++ $(CXXFLAGS) -o mnist_cnn_example mnist_cnn.o $(CRYPTODL_DIR)/Debug/libkalypso.a $(DEP_LIBS) $(DEP_RPATH)

cnn_seal:
	@echo Building cnn seal examples
	g++ $(CXXFLAGS) $(INCLUDE_DIRS) $(DEP_INCLUDES) -c mnist_cnn/mnist_cnn_seal.cpp
	@echo Linking cnn seal examples
	g++ $(CXXFLAGS) -o mnist_cnn_seal_example mnist_cnn_seal.o $(CRYPTODL_DIR)/Debug/libkalypso.a $(DEP_LIBS) $(DEP_RPATH)
	
rnn:
	@echo Building rnn examples
	g++ $(CXXFLAGS) -include../src/data/mnist/include/mnist/mnist_reader_less.hpp $(INCLUDE_DIRS) $(DEP_INCLUDES) -c mnist_rnn/mnist_rnn.cpp
	@echo Linking rnn examples
	g++ $(CXXFLAGS) -o mnist_rnn_example mnist_rnn.o ../Debug/libkalypso.a -lpthread  ../dependencies/install/helib_pack/lib/fhe.a -lboost_filesystem -lboost_system -ljpeg -lgmpxx -lntl -lgmp

clean:
	-rm mnist_cnn_example mnist_cnn.o mnist_rnn_example mnist_rnn.o
	
