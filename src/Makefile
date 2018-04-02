OPENCV = -lopencv_core -lopencv_highgui -lopencv_imgproc

CFLAGS = -g -m64 -lrt -fsanitize=address -fno-omit-frame-pointer
# -m64: generate code for 64 bit environment
# -lrt link with libraries

LIBSPATH = -L../lib/x64


videoFireball: main.cpp 
	g++ $(CFLAGS) main.cpp -o videoFireball $(OPENCV) -lASICamera2


clean: 
	rm videoFireball

