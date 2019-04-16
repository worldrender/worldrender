.SUFFIXES: .cpp .cxx .h

# compiler names:
CXX		= g++ 
CC		= gcc

# flags for C++ compiler:

OPENGL_LIBS	=	-lGL -lGLU -lGLEW
GLFW_LIBS 	=	-lglfw
THREAD_LIBS	= 	-lpthread

CFLAGS		= 	-g -Wno-deprecated
CXXFLAGS	= 	-g -Wno-deprecated

# libraries to link with:

LIBPATH =   

LDFLAGS =	$(THREAD_LIBS) $(GLFW_LIBS) $(OPENGL_LIBS)

OBJFILES = controls.o Main.o Planet.o LoadShaders.o NoiseFeedback.o QuadTree.o


# ***********************************************************************************
all:	planet
	
planet:	$(OBJFILES)
	@echo "Linking ...."
	$(CXX) $^ -o $@ $(CXXFLAG) $(INCFLAGS) $(LIBPATH) $(LDFLAGS)
		
.c.o:	$*.h
	@echo "Compiling C code ...."
	$(CXX) -o $*.o -c $(CXXFLAGS) $(DEFFLAGS) $(INCFLAGS) $*.c

.cpp.o:	$*.h
	@echo "Compiling C++ code ...."
	$(CXX) -o $*.o -c $(CXXFLAGS) $(DEFFLAGS) $(INCFLAGS) $*.cpp

clean:	
	@echo "Clearing ..."
	rm -f *.o core *.*~ *~ planet


