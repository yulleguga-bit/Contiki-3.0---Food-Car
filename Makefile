CONTIKI = ../..
TARGET = sky

CONTIKI_PROJECT = yolo_floodcar_contiki
all: $(CONTIKI_PROJECT)

CONTIKI_WITH_RIME = 1

include $(CONTIKI)/Makefile.include
