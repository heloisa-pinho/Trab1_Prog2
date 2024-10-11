# Nome do executável
TARGET = lbp

# Arquivos fonte
SRCS = main.c processamento.c imagem.c

# Arquivos objetos
OBJS = $(SRCS:.c=.o)

# Compilador e flags
CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -lm

# Regra padrão para compilar todos os arquivos
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

# Regra para compilar os arquivos .c em .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Limpeza dos arquivos objetos
clean:
	rm -f $(OBJS) $(TARGET)
