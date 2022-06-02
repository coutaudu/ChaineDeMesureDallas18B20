.PHONY: all, clean

# Disable implicit rules
.SUFFIXES:

# Keep intermediate files
#.PRECIOUS: %.o

CC = gcc
CFLAGS = -Wall -Werror
LDFLAGS =

# Pour multi thread/process
LIBS += -lpthread

INCLUDE =
OBJS = 
INCLDIR = -I.

PROGS = UdpDataLogger

all: $(PROGS)

%.o: %.c %.h $(INCLUDE)
	$(CC) $(CFLAGS) $(INCLDIR) -c -o $@ $<


%: %.o $(OBJS)
	$(CC) -o $@ $(LDFLAGS) $^ $(LIBS)
#       $(CC) -o $@ $(LDFLAGS) $(LIBS) $^

install_service: UdpDataLogger.service
	sudo cp UdpDataLogger.service /etc/systemd/system/UdpDataLogger.service

run_service: install_service
	sudo systemctl start UdpDataLogger
	sudo systemctl daemon-reload

auto_run_service:
	sudo systemctl enable UdpDataLogger

stop_service:
	sudo systemctl stop UdpDataLogger

disable_service:
	sudo systemctl disable UdpDataLogger


clean:
	rm -f $(PROGS) *.o output.txt *~
