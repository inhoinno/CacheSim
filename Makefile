CC = g++
OBJS = main.o cache.o interpreter.o 
DEPS= $(OBJS:.o=.d)
TARGET = hw4
INCLUDE = -Iinclude/
CXXFLAGS = -Wall -Wextra -Werror

all:$(TARGET)

%.o : %.cc %.h
		g++ $(INCLUDE) -c -o $@ ${CXXFLAGS} $< 

$(TARGET) : $(OBJS) 
		$(CC) $(CXXFLAGS) $(OBJS) -o $(TARGET)

clean : 
	rm -f *.o
	rm -f $(TARGET)
-include $(DEPS)