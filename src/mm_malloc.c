#include <unistd.h> // Para sbrk
#include <string.h> // Para memcpy y memset
#include "mm_malloc.h"

// Inicio de la lista enlazada del heap
struct block_meta *base = NULL;//Base block_meta

struct block_meta *request_mem(struct block_meta *last, size_t size)
{
    struct block_meta* block;

    void *request = sbrk(size + sizeof(struct block_meta));//Le pedimos memoria al SO

    if(request == (void*) -1)
    {
        return NULL;//Si me rechaza la peticion retornamos null
    }

    block = (struct block_meta *)request;//Le asignamos la memoria
    
    block->size = size;//Asignamos el tamanho
    block->next = NULL;
    block->free = 0;

    if(last)
    {
        last->next = block;
    }

    return block;
}

struct block_meta *find_free_block(struct block_meta **last, size_t size) 
{
    struct block_meta *current = *last; 
    while (current != NULL) //Recorremos toda la lista hasta encontrar un bloque libre
    {
    
        if (current->free == 1 && current->size >= size) 
        {
            return current;
        }
        *last = current; 
        current = current->next;
    }
    return NULL; //Si no lo encontramos retornamos NULL
}

void *my_malloc(size_t size) 
{
    struct block_meta *block;

    if(size <= 0)
    {
        return NULL;
    }

    if(base == NULL)//Caso base: cuando base = Null
    { 
        block = request_mem(NULL, size);//Pedimos memoria
        if(block == NULL)
        {
            return NULL;//Si niegan la peticion retornamos null
        }
        base = block;//Le asignamos block a base para tomarla como inicio de la lista
    }

    else {
        struct block_meta *last = base;

        block = find_free_block(&last, size);//Buscams un bloque libre
        
        if(block == NULL) //Si no lo encontramos pedimos mas memoria
        {
            block = request_mem(last, size);
            if(block == NULL) 
            {
                return NULL;//Si nos niegan la peticion, retornamos null.
            }
        } 
        else 
        {
            block->free = 0; //Si lo encontramos, lo ocupamos
        }
    }

    void *user_data = (void *)(block + 1);//Le retornamos al usuario el lugar
    return user_data;//inmediatamente siguiente para no darle acceso y pueda cambiar nuestra meta_data
}


void my_free(void *ptr) {
    // TODO: Marcar el bloque como libre.
    // TODO: Fusionar bloques adyacentes (Coalescing).
}

void *my_calloc(size_t nmemb, size_t size) {
    // TODO: Usar my_malloc y luego memset a 0.
    return NULL;
}

void *my_realloc(void *ptr, size_t size) {
    // TODO: Redimensionar el bloque o moverlo a uno nuevo.
    return NULL;
}
