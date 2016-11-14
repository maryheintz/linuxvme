CXX = g++ -c -I/home/benc/include -fhandle-exceptions

OBJS = pulse.o Signal.o A3in1Controller.o SwitchBox.o FermiADC.o

LIBS = -L/remote_root/lib/ces -L/home/benc/lib -lvme -lgraph -lstdc++

OUT = pulse

pulse: $(OBJS)
	gcc $(OBJS) $(LIBS) -o $(OUT)

pulse.o: pulse.cpp
	$(CXX) pulse.cpp

Signal.o: Signal.cpp
	$(CXX) Signal.cpp

A3in1Controller.o: A3in1Controller.cpp
	$(CXX) A3in1Controller.cpp

SwitchBox.o: SwitchBox.cpp
	$(CXX) SwitchBox.cpp

FermiADC.o: FermiADC.cpp
	$(CXX) FermiADC.cpp