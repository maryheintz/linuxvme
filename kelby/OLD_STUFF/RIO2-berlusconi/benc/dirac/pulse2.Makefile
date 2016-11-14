CXX = g++ -c -I/home/benc/include -fhandle-exceptions

OBJS = pulse2.o Signal.o A3in1Controller.o SwitchBox.o FermiADC.o TTCvi.o DigBoard.o

LIBS = -L/remote_root/lib/ces -L/home/benc/lib -lvme -luio -lgraph -lstdc++ -lSLINK -lpci -ls5933 -llynx

OUT = pulse2

pulse2: $(OBJS)
	gcc $(OBJS) $(LIBS) -o $(OUT)

pulse2.o: pulse2.cpp
	$(CXX) pulse2.cpp

Signal.o: Signal.cpp
	$(CXX) Signal.cpp

A3in1Controller.o: A3in1Controller.cpp
	$(CXX) A3in1Controller.cpp

SwitchBox.o: SwitchBox.cpp
	$(CXX) SwitchBox.cpp

FermiADC.o: FermiADC.h FermiADC.cpp
	$(CXX) FermiADC.cpp

TTCvi.o: TTCvi.cpp
	$(CXX) TTCvi.cpp

DigBoard.o: DigBoard.h DigBoard.cpp
	$(CXX) DigBoard.cpp
