CXX = g++
CXXFLAGS = -std=c++17 -pthread

all: server client

server: server/server.cpp 
	$(CXX) $(CXXFLAGS) -o serv server/server.cpp

client: client/client.cpp
	$(CXX) $(CXXFLAGS) -o cli client/client.cpp

clean:
	rm -f serv cli