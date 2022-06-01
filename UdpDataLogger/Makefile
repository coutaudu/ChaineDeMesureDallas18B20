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
	sudo cp ./UdpDataLogger /bin/UdpDataLogger

run_service: install_service
	systemctl start UdpDataLogger
	systemctl daemon-reload

auto_run_service:
	systemctl enable UdpDataLogger

stop_service:
	systemctl stop UdpDataLogger

disable_service:
	systemctl disable UdpDataLogger


clean:
	rm -f $(PROGS) *.o output.txt *~
