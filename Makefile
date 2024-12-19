# Variables
CC = 
    gcc
CFLAGS =  
        -Wall -Wextra -std=c99
LDFLAGS =
TARGET = 
        enseash
# Liste des fichiers source
SRCS = 
      main.c Q1.c Q2.c Q3.c Q4.c Q5.c

# Liste des fichiers objets
OBJS = 
      $(SRCS:.c=.o)

# Règle par défaut
all: 
    $(TARGET)

# Règle pour construire l'exécutable
$(TARGET): 
          $(OBJS)
          $(CC) $(CFLAGS) $(OBJS) -o $@ $(LDFLAGS)

# Règle pour construire les fichiers objets
%.o: 
    %.c
    $(CC) 
	$(CFLAGS) -c $< -o $@

# Règle pour nettoyer les fichiers objets et l'exécutable
clean:
      rm -f $(OBJS) $(TARGET)
# Règle pour exécuter le programme
run:
    $(TARGET)
    ./$(TARGET)
