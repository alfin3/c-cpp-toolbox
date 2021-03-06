/**
   queue.h

   Struct declarations, declarations of accessible functions, and macro
   definitions for a generic dynamically allocated fifo queue.

   Through a user-defined deallocation function, the implementation provides
   a dynamic set of any objects in the fifo queue form.
*/

#ifndef QUEUE_H  
#define QUEUE_H

#include <stddef.h>

typedef struct{
  size_t count;
  size_t count_max;
  size_t num_elts;
  size_t num_popped_elts;
  size_t elt_size;
  void *elts;
  void (*free_elt)(void *);
} queue_t;

/**
   Initializes a queue.
   q                : pointer to a preallocated block of size sizeof(queue_t)
   init_count       : > 0
   elt_size         : - the size of an element, if the element is within a
                      contiguous memory block
                      - the size of a pointer to an element, if the element
                      is within a noncontiguous memory block
   free_elt         : - if an element is within a contiguous memory block,
                      as reflected by elt_size, and a pointer to the element
                      is passed as elt in queue_push, then the element is
                      fully copied onto the queue, and NULL as free_elt is
                      sufficient to free the queue;
                      - if an element is an object within a noncontiguous
                      memory block, and a pointer to a pointer to the element
                      is passed as elt in queue_push, then the pointer to the
                      element is copied onto the queue, and an element-
                      specific free_elt, taking a pointer to a pointer to an
                      element as its parameter, is necessary to free the
                      queue.
*/
void queue_init(queue_t *q,
		size_t init_count,
		size_t elt_size,
		void (*free_elt)(void *));

/**
   Pushes an element onto a queue. The elt parameter is not NULL.
*/
void queue_push(queue_t *q, const void *elt);

/**
   Pops an element off a queue. Elt points to a preallocated memory block of
   size elt_size. If the queue is empty, the memory block pointed to by elt
   remains unchanged.
*/
void queue_pop(queue_t *q, void *elt);

/**
   If a queue is not empty, returns a pointer to the first element,
   otherwise returns NULL. The returned pointer is guaranteed to point to
   the first element until a queue modifying operation is performed.
*/
void *queue_first(const queue_t *q);

/**
   Frees a queue, and leaves a block of size sizeof(queue_t) pointed
   to by the q parameter.
*/
void queue_free(queue_t *q);

/**
   Sets the queue count maximum that may be reached, if possible, as a queue
   grows by repetitive doubling from its initial count and by adding, if
   necessary, the difference between QUEUE_COUNT_MAX and the last count in
   the last step.

   The program exits with an error message, if a) the value of the init_count
   parameter in queue_init is greater than QUEUE_COUNT_MAX, or b) if a queue
   growth step is attempted after QUEUE_COUNT_MAX was reached. The macro is
   set to the maximal value of size_t by default and is used as size_t.
*/
#define QUEUE_COUNT_MAX ((size_t)-1)

#endif
