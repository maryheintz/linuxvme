CXX = g++ -c

OBJS = findcard.o A3in1Controller.o SwitchBox.o

LIBS = -L/remote_root/lib/ces -lvme -lstdc++

OUT = findcard

findcard: $(OBJS)
	gcc $(OBJS) $(LIBS) -o $(OUT)

findcard.o: findcard.cpp
	$(CXX) findcard.cpp

A3in1Controller.o: A3in1Controller.cpp
	$(CXX) A3in1Controller.cpp

SwitchBox.o: SwitchBox.cpp
	$(CXX) SwitchBox.cpp