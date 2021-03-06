
SRC := $(wildcard src/core/*.cpp,src/core/*.c)
OBJS := $(patsubst %.cpp,%.o,$(wildcard src/core/*.cpp)) $(patsubst %.c,%.o,$(wildcard src/core/*.c))
DS := $(patsubst %.cpp,%.d,$(wildcard src/core/*.cpp src/example/*.cpp))
EXOBJS := $(patsubst %.cpp,%.o,$(wildcard src/example/*.cpp))
#Must use at least gcc/g++ 4.8 for C++11 support. Some systems call it gcc48 or gcc-48
#Standard compiler version on MacOS is 4.2.1 (no good)
CXX = g++
CC = gcc
INC = -Iinclude/core/ -Iinclude/example/ -I/usr/local/include/ -I/usr/include/
CXXFLAGS:= -std=c++0x -DGL_GLEXT_PROTOTYPES=1 -O3 -Wall -c -fmessage-length=0 -MMD -MP -fopenmp $(INC)
CFLAGS:= -DGL_GLEXT_PROTOTYPES=1 -O3 -Wall -c -fmessage-length=0 -MMD -MP -fopenmp $(INC)
LDLIBS =-static-libstdc++ -static-libgcc -L./ -L/usr/lib/ -L/usr/local/lib/ -L/usr/lib/x86_64-linux-gnu/
#Pacakge dependencies are glfw 3.x, and glew, which are avaiable through apt-get and brew. 
#Make sure the libraries are compiled with the same version of g++/gcc as alloy.
#Ubuntu may also require GLU, GLX, Xrandr, Xxf86vm, Xinerama, and Xi to build glfw from source.
#Other dependencies are standard on Ubuntu and MacOS.
#Some systems will use -lglfw3 
LIBS =-lalloy -lglfw -lgomp -lGL -lXext -lGLU -lGLEW -lXi -lXrandr -lX11 -lXxf86vm -lXinerama -lXcursor -lXdamage -lpthread -lm -ldl
RM=rm -f

alloy : $(OBJS)
	ar ru liballoy.a $(OBJS)
	ranlib liballoy.a

examples : $(EXOBJS)
	$(CXX) $(LDLIBS) -o examples $(EXOBJS) $(LIBS)
		
clean :
	clear
	$(RM) $(OBJS) $(EXOBJS) $(DS)
	$(RM) liballoy.a
	$(RM) $(EXAMPLES) examples
	
all : 
	clear
	make -j8 alloy
	make -j8 examples
default : all
