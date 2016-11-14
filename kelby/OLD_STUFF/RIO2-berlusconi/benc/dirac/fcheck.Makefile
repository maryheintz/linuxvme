CXX = g++ -c -I/home/benc/include

OBJS = fcheck.o A3in1Controller.o SwitchBox.o FermiADC.o

LIBS = -L/remote_root/lib/ces -L/home/benc/lib -lvme -lkja -lstdc++

OUT = fcheck

fcheck: $(OBJS)
	gcc $(OBJS) $(LIBS) -o $(OUT)

fcheck.o: fcheck.cpp
	$(CXX) fcheck.cpp

A3in1Controller.o: A3in1Controller.cpp
	$(CXX) A3in1Controller.cpp

SwitchBox.o: SwitchBox.cpp
	$(CXX) SwitchBox.cpp

FermiADC.o: FermiADC.cpp
	$(CXX) FermiADC.cpp
