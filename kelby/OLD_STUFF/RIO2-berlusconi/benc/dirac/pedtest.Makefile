CXX = g++ -c -I/home/benc/include -fhandle-exceptions

OBJS = pedtest.o Signal.o A3in1Controller.o TTCvi.o DigBoard.o

LIBS = -L/remote_root/lib/ces -L/home/benc/lib -lvme -luio -lgraph -lstdc++ -lSLINK -lpci -ls5933 -llynx

OUT = pedtest

pedtest: $(OBJS)
	gcc $(OBJS) $(LIBS) -o $(OUT)

pedtest.o: pedtest.cpp
	$(CXX) pedtest.cpp

Signal.o: Signal.h Signal.cpp
	$(CXX) Signal.cpp

A3in1Controller.o: A3in1Controller.cpp
	$(CXX) A3in1Controller.cpp

TTCvi.o: TTCvi.cpp
	$(CXX) TTCvi.cpp

DigBoard.o: DigBoard.h DigBoard.cpp
	$(CXX) DigBoard.cpp
