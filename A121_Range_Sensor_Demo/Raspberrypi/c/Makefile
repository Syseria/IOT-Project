DIR_BIN = ./bin
DIR_Config = ./lib/Config
DIR_A121_Dev_Config = ./lib/A121_Dev_Config
DIR_A121_D_D_Sense = ./lib/A121_Distance_Detector
DIR_A121_P_D_Sense = ./lib/A121_Presence_Detector
DIR_A121_C_E_Sense = ./lib/A121_Cargo_Example
DIR_A121_B_R_Sense = ./lib/A121_Breathing_Reference

DIR_Examples = ./examples

# ===============================
# A121 APPLICATION SELECT
# ===============================
# 可选：
# DISTANCE | PRESENCE | CARGO | BREATHING
A121_APP ?= DISTANCE

# ===============================
# Compiler
# ===============================
CC = gcc
DEBUG = -g -O0 -Wall
CFLAGS += $(DEBUG)

# ===============================
# A121 Macro Define (ONLY ONE)
# ===============================
ifeq ($(A121_APP),DISTANCE)
    CFLAGS += -DA121_DISTANCE_DETECTOR
endif

ifeq ($(A121_APP),PRESENCE)
    CFLAGS += -DA121_PRESENCE_DETECTOR
endif

ifeq ($(A121_APP),CARGO)
    CFLAGS += -DA121_CARGO_EXAMPLE
endif

ifeq ($(A121_APP),BREATHING)
    CFLAGS += -DA121_BREATHING_REFERENCE
endif

# ===============================
# Source files
# ===============================
OBJ_C = $(wildcard \
	${DIR_Examples}/*.c \
	${DIR_Config}/*.c \
	${DIR_A121_Dev_Config}/*.c \
	${DIR_A121_D_D_Sense}/*.c \
	${DIR_A121_P_D_Sense}/*.c \
	${DIR_A121_C_E_Sense}/*.c \
	${DIR_A121_B_R_Sense}/*.c \)

OBJ_O = $(patsubst %.c,${DIR_BIN}/%.o,$(notdir ${OBJ_C}))

TARGET = main
BIN_TARGET = ${DIR_BIN}/${TARGET}

LIB = -lwiringPi -lm

# ===============================
# Build rules
# ===============================
${TARGET}: ${OBJ_O}
	$(CC) $(CFLAGS) $(OBJ_O) -o $@ $(LIB)

${DIR_BIN}/%.o : $(DIR_Examples)/%.c
	$(CC) $(CFLAGS) -c $< -o $@ \
		-I $(DIR_Config) \
		-I $(DIR_A121_Dev_Config) \
		-I $(DIR_A121_D_D_Sense) \
		-I $(DIR_A121_P_D_Sense) \
		-I $(DIR_A121_C_E_Sense) \
		-I $(DIR_A121_B_R_Sense)

${DIR_BIN}/%.o : $(DIR_Config)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

${DIR_BIN}/%.o : $(DIR_A121_Dev_Config)/%.c
	$(CC) $(CFLAGS) -c $< -o $@ -I $(DIR_Config)

${DIR_BIN}/%.o : $(DIR_A121_D_D_Sense)/%.c
	$(CC) $(CFLAGS) -c $< -o $@ \
		-I $(DIR_Config) \
		-I $(DIR_A121_Dev_Config)

${DIR_BIN}/%.o : $(DIR_A121_P_D_Sense)/%.c
	$(CC) $(CFLAGS) -c $< -o $@ \
		-I $(DIR_Config) \
		-I $(DIR_A121_Dev_Config)

${DIR_BIN}/%.o : $(DIR_A121_C_E_Sense)/%.c
	$(CC) $(CFLAGS) -c $< -o $@ \
		-I $(DIR_Config) \
		-I $(DIR_A121_Dev_Config)

${DIR_BIN}/%.o : $(DIR_A121_B_R_Sense)/%.c
	$(CC) $(CFLAGS) -c $< -o $@ \
		-I $(DIR_Config) \
		-I $(DIR_A121_Dev_Config)

# ===============================
# Clean
# ===============================
clean:
	rm -f $(DIR_BIN)/*.o
	rm -f $(TARGET)
