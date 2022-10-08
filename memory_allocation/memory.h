# ifndef MEMORY_H
# define MEMORY_H

# include <stdio.h>
# include <stdbool.h>

void *_malloc (size_t size);
void _free (size_t size);
void setup_heap(void *buf, size_t size);

# endif /*MEMORY_H*/
