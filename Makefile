.SUFFIXES: .cpp .cxx .h

# compiler names:
CXX		= g++ 
CC		= gcc

# flags for C++ compiler:

GLAD_INC_DIR	=	-I/home/alopesajr/SourceCode/Libs/glad/build/include 
GLAD_LIB_DIR	=	-L/home/alopesajr/SourceCode/Libs/glad/build
GLAD_LIBS		= 	-lglad -ldl

SDL_INC_DIR		=
SDL_LIB_DIR		= 	-L/usr/lib/x86_64-linux-gnu
SDL_LIBS		= 	-lSDL2main -lSDL2

OPENGL_LIBS		= 	 -lGL -lGLU

CFLAGS		= -g -Wno-deprecated $(GLAD_INC_DIR) $(SDL_INC_DIR)
CXXFLAGS	= -g -Wno-deprecated $(GLAD_INC_DIR) $(SDL_INC_DIR)

# libraries to link with:

LIBPATH =   $(SDL_LIB_DIR) $(GLAD_LIB_DIR)

LDFLAGS =	$(GLAD_LIBS) $(SDL_LIBS) $(OPENGL_LIBS)

OBJFILES = Main.o Shader.o

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


