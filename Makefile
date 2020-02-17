# Makefile

# make sure these two macros are correct for your system
GLUTINCLUDEDIR  = /usr/include/GL
GLUTLIBDIR      = /usr/lib/

XLIBS           = -lX11 -lXmu
GLLIBS          = -lGL -lGLU
GLUTLIB         = -lglut
OSGLIBS         = -losg -losgDB
OSGVIEWERLIBS   = -losgViewer -losgGA
OSGEXTRALIBS    = -losgParticle

INCLUDEDIRS     = -I$(GLUTINCLUDEDIR) -I.
LIBDIRS         = -L$(GLUTLIBDIR) -L.

# make sure the next macro is correct for your system
CC      = /usr/bin/g++
CCFLAGS = -O3 -c
LIBS    = $(XLIBS) $(GLLIBS) $(GLUTLIB) \
    $(OSGLIBS) $(OSGVIEWERLIBS) $(OSGEXTRALIBS)

# all of our OpenGL programs will be named "main"
TARGET = FirstOSG
#TARGET = SecondOSG
#TARGET = Frisbee

# source files go here (not headers)
SRCS = KeyboardEventHandler.cpp

#################################################

# list of header files (one for each .cpp file)
HDRS = $(SRCS:.cpp=.hpp)

# list of object files (one for each .cpp file)
OBJS = $(SRCS:.cpp=.o) $(TARGET).o

# rule for making a .o file from a .cpp file
.cpp.o:
	$(CC) $(INCLUDEDIRS) $(CCFLAGS) $<

# rule for making the final executable
$(TARGET): $(OBJS) $(HDRS)
	$(CC) $(LIBDIRS) -o $(TARGET) $(OBJS) $(LIBS)

# rule for cleaning up the directory
clean:
	rm -f $(OBJS) $(TARGET)

# rule for building everything
over:
	make clean
	make
