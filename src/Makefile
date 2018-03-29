OPENCV = -lopencv_core -lopencv_highgui -lopencv_imgproc

CFLAGS = -m64 -lrt
# -m64: generate code for 64 bit environment
# -lrt link with libraries

LIBSPATH = -L../lib/x64


videoFireball: main.cpp 
	g++ main.cpp -o videoFireball $(CFLAGS) $(OPENCV) -lASICamera2


clean: 
	rm videoFireball

