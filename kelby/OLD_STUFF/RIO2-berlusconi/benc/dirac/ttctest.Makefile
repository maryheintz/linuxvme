CXX = g++ -c -I/home/benc/include -fhandle-exceptions

OBJS = ttctest.o Signal.o TTCvi.o DigBoard.o

LIBS = -L/remote_root/lib/ces -L/home/benc/lib -lvme -luio -lgraph -lstdc++ -lSLINK -lpci -ls5933 -llynx

OUT = ttctest

ttctest: $(OBJS)
	gcc $(OBJS) $(LIBS) -o $(OUT)

ttctest.o: ttctest.cpp
	$(CXX) ttctest.cpp

Signal.o: Signal.h Signal.cpp
	$(CXX) Signal.cpp

TTCvi.o: TTCvi.h TTCvi.cpp
	$(CXX) TTCvi.cpp

DigBoard.o: DigBoard.h DigBoard.cpp
	$(CXX) DigBoard.cpp