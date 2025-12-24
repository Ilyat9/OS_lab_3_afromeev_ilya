TARGET = os_lab_3

CC = gcc
CFLAGS = -Wall -Wextra -Werror -g -pthread
# -pthread обязателен для работы с потоками POSIX

SRC_DIR = src
OBJ_DIR = obj

# Поиск всех .c файлов в папке src
SRCS = $(wildcard $(SRC_DIR)/*.c)
# Генерация имен объектных файлов в папке obj
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Цель по умолчанию
all: $(OBJ_DIR) $(TARGET)

# Линковка
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Компиляция объектных файлов
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Создание папки для объектных файлов, если её нет
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Очистка проекта
clean:
	rm -rf $(OBJ_DIR) $(TARGET)

# Запуск
run: all
	./$(TARGET)

.PHONY: all clean run
