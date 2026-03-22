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


void my_free(void *ptr) 
{
    if(ptr == NULL)
    {
        return;
    }
    struct block_meta *block = (struct block_meta *)ptr;
    struct block_meta *tgt = block - 1;//Tomamos el lugar inmediatamente anterior ya que ptr representa los datos del usuario y vamos a realizar cambios en nuestra metadata
    tgt->free = 1;//Liberamos el bloque
    
    if(base == NULL)
    {
        return;//Si base es igual a null no hay bloques que fusionar
    }

    struct block_meta *last = base;
    struct block_meta *current = last->next;
    while (current != NULL) //Recorremos la lista validando si un nodo y el siguiente estan libres
    {
        if(last->free == 1 && current->free == 1)//Si estan libres los funcionamos
        {
            last->size += current->size + sizeof(struct block_meta);//Asignamos el nuevo size del bloque
            last->next = current->next;//Hacemos la conexion del nuevo bloque a su siguiente bloque
            current->next = NULL;//Lo separamos de la lista
            current->size = 0;//Le quitamos su size
            current = last->next;
        }
        else//Si no estan libres, seguimos avanzando en la lista
        {
            last = current; 
            current = current->next;
        }   
    }
}


void *my_calloc(size_t nmemb, size_t size) 
{
    if(nmemb == 0 || size == 0)
    {
        return NULL;
    }

    size_t num = nmemb*size;//Calculamos la cantidad de memoria que vamos a solicitar
    void* block = my_malloc(num);
    if(block == NULL)
    {
        return NULL;
    }
    memset(block, 0, num );//Limpiamos la memoria que nos manda el SO
    return block;
}


void *my_realloc(void *ptr, size_t size) {
    // TODO: Redimensionar el bloque o moverlo a uno nuevo.
    return NULL;
}
