CXX = g++ -c -Wall -I/home/kelby/benc/include -fhandle-exceptions

OBJS = fcheck2.o A3in1Controller.o SwitchBox.o TTCvi.o DigBoard.o

LIBS = -L/remote_root/lib/ces -L/home/kelby/benc/lib -lvme -luio -lkja -lstdc++ -lSLINK -lpci -ls5933 -llynx

OUT = fcheck2

fcheck2: $(OBJS)
	gcc $(OBJS) $(LIBS) -o $(OUT)

fcheck2.o: fcheck2.cpp
	$(CXX) fcheck2.cpp

A3in1Controller.o: A3in1Controller.cpp
	$(CXX) A3in1Controller.cpp

SwitchBox.o: SwitchBox.cpp
	$(CXX) SwitchBox.cpp

TTCvi.o: TTCvi.cpp
	$(CXX) TTCvi.cpp

DigBoard.o: DigBoard.cpp
	$(CXX) DigBoard.cpp