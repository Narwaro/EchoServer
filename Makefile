CC=g++
Cflags=-c -MD
LDflags=-lmysqlclient -lpthread -lgmpxx -lgmp

Input=main.cpp db.cpp rsa.cpp padding.cpp aes256.c
ObjDir=obj
BinDir=bin
Output=EchoServer

Objects=$(addprefix $(ObjDir)/,$(addsuffix .o, $(Input)))

all: $(BinDir)/$(Output)
	$(BinDir)/$(Output)
    
$(BinDir)/$(Output): $(Objects)
	g++ -o $(BinDir)/$(Output) $(Objects) $(LDflags)

obj/%.c.o: %.c
	gcc $(Cflags) -o $@ $<

obj/%.cpp.o: %.cpp
	g++ $(Cflags) -o $@ $<

-include $(ObjDir)/*.d

.IGNORE: clean
clean:
	rm $(ObjDir)/*
	rm $(BinDir)/$(Output)

#More build targets soon for beta and pre-releases!
