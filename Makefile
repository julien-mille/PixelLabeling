CC = g++
CPPFLAGS = -c -g -fPIC -O3
LD = g++
LDFLAGS = -O3
FINAL_TARGET = pixelLabeling
OPENCV_DIR = /usr/local/opencv-4.1.0-bin
QT_INCLUDE_DIR = /usr/include/x86_64-linux-gnu/qt5
INCLUDE_DIR = -I$(OPENCV_DIR)/include/opencv4 -I$(QT_INCLUDE_DIR)
LIB_DIR = -L$(OPENCV_DIR)/lib
LIBS = -lopencv_imgproc -lopencv_imgcodecs -lopencv_core -lQt5Core -lQt5Gui -lQt5Widgets

default: $(FINAL_TARGET)

$(FINAL_TARGET): dialogclassselection.o dialogparams.o labeling.o labelingwindow.o main.o
	$(LD) $+ -o $@ $(LDFLAGS) $(LIB_DIR) $(LIBS)

%.o: %.cpp
	$(CC) $(CPPFLAGS) $(INCLUDE_DIR) $< -o $@

clean:
	rm -f *.o $(FINAL_TARGET)

