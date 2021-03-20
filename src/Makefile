CC=cc

NAME=picolisp
CPU=seudo_cpu
HTTP=http_server
MONITOR=seudo_monitor
EVENT_MANAGER=event_manager
SPEAKER=seudo_speaker
BUILD_PATH=../build
SRCS=$(NAME).c
DEV_SRCS_DIR=device
COMM_SRCS_DIR=comm
UTIL_SRCS_DIR=util
OBJ_DIR=../obj
DEV_OBJ_DIR=../dev_obj
COMM_OBJ_DIR=../comm_obj
UTIL_OBJ_DIR=../util_obj
FLAGS=-Wall -I./
DEBUGFLAGS=-g -DDEBUG $(FLAGS)

OBJ=$(NAME).o
CPU_OBJ=$(CPU).o
$(OBJ_DIR)/%.o: %.c
	if test ! -d $(OBJ_DIR); then mkdir $(OBJ_DIR); fi;\
	$(CC) $(DEBUGFLAGS) -c $< -o $@ -lpthread

$(DEV_OBJ_DIR)/%.o: $(DEV_SRCS_DIR)/%.c
	if test ! -d $(DEV_OBJ_DIR); then mkdir $(DEV_OBJ_DIR); fi;\
	$(CC) $(DEBUGFLAGS) -c $< -o $@ -lpthread

$(COMM_OBJ_DIR)/%.o: $(COMM_SRCS_DIR)/%.c
	if test ! -d $(COMM_OBJ_DIR); then mkdir $(COMM_OBJ_DIR); fi;\
	$(CC) $(DEBUGFLAGS) -c $< -o $@ -lpthread

$(UTIL_OBJ_DIR)/%.o: $(UTIL_SRCS_DIR)/%.c
	if test ! -d $(UTIL_OBJ_DIR); then mkdir $(UTIL_OBJ_DIR); fi;\
	$(CC) $(DEBUGFLAGS) -c $< -o $@ -lpthread

default:debug clean

debug: $(OBJ)
	$(CC) $(DEBUGFLAGS) $< -o $(BUILD_PATH)/$(NAME)

cpu: $(DEV_OBJ_DIR)/$(CPU).o $(OBJ_DIR)/$(OBJ)
	if test ! -d ../build; then mkdir ../build; fi;\
	$(CC) $(DEBUGFLAGS) $^ -o $(BUILD_PATH)/$(CPU);\
	make clean;

monitor: $(DEV_OBJ_DIR)/$(MONITOR).o $(OBJ_DIR)/$(OBJ) ../comm_obj/file_comm.o ../util_obj/str.o
	if test ! -d ../build; then mkdir ../build; fi;\
	$(CC) $(DEBUGFLAGS) $^ -o $(BUILD_PATH)/$(MONITOR) -lpthread;\
	make clean;

speaker: $(DEV_OBJ_DIR)/$(SPEAKER).o $(OBJ_DIR)/$(OBJ) ../comm_obj/file_comm.o ../util_obj/str.o
	if test ! -d ../build; then mkdir ../build; fi;\
	$(CC) $(DEBUGFLAGS) $^ -o $(BUILD_PATH)/$(SPEAKER) -w -lpthread;\
	make clean;

http: $(COMM_OBJ_DIR)/$(HTTP).o $(OBJ_DIR)/$(OBJ)
	if test ! -d ../build; then mkdir ../build; fi;\
	$(CC) $(DEBUGFLAGS) $^ -o $(BUILD_PATH)/$(HTTP);\
	make clean;

event: $(DEV_OBJ_DIR)/$(EVENT_MANAGER).o $(OBJ_DIR)/$(OBJ) ../comm_obj/file_comm.o ../util_obj/str.o
	if test ! -d ../build; then mkdir ../build; fi;\
	$(CC) $(DEBUGFLAGS) $^ -o $(BUILD_PATH)/$(EVENT_MANAGER);\
	make clean;

.PHONY:clean
clean: 
	rm -rf $(COMM_OBJ_DIR);\
	rm -rf $(DEV_OBJ_DIR);\
	rm -rf $(UTIL_OBJ_DIR);\
	rm -rf $(OBJ_DIR);
