/**
   dll.c

   A generic dynamically allocated doubly linked list in a circular
   representation.

   Given the circular representation of the list, the head pointer in the
   provided list operations is not limited to a fixed position in the list.
   The head pointer determines the "beginning" and "end" of a list each time
   with respect to a call to an operation and can be used as a positional
   pointer for searching and modifying the list from and at any position,
   including a fixed position if desired.
   
   A list node contains i) a pointer to a key that is an object within a 
   contiguous memory block (e.g. basic type, array, or struct), and ii) a 
   pointer to a contiguous element or to a pointer to a contiguous
   or noncontiguous element. The implementation provides a guarantee that
   a node keeps its address in memory throughout its lifetime in a list.

   The node implementation facilitates hashing applications, such as
   mapping a key to a node pointer for fast in-list access and using a list
   for chaining hash keys and their elements in a hash table. In combination
   with the circular representation, the node implementation also facilitates
   the parallelization of search in future versions.

   The implementation may not be slower (as tested) than a singly linked
   list due to instruction-level parallelism, does not use stdint.h, and
   is portable under C89/C90 and C99.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dll.h"
#include "utilities-mem.h"

/**
   Initializes an empty doubly linked list by setting a head pointer to NULL.
   head        : pointer to a preallocated block of size of a head pointer
*/
void dll_init(dll_node_t **head){
  *head = NULL;
}

/**
   Creates and prepends a node relative to a head pointer. A head pointer is
   NULL if the list is empty, or points to any node in the list to determine
   the position for the prepend operation.
   head        : pointer to a head pointer to an initialized list           
   key         : pointer to a key object of size key_size within a contiguous
                 memory block (e.g. basic type, array, struct)
   elt         : - pointer to an element, if the element is within a contiguous
                 memory block and a copy of the element is prepended,
                 - pointer to a pointer to an element, if the element is within
                 a noncontiguous memory block or a pointer to a contiguous
                 element is prepended
   key_size    : size of a key object
   elt_size    : - size of an element, if the element is within a contiguous
                 memory block and a copy of the element is prepended,
                 - size of a pointer to an element, if the element is within
                 a noncontiguous memory block or a pointer to a contiguous
                 element is prepended
*/
void dll_prepend_new(dll_node_t **head,
		     const void *key,
		     const void *elt,
		     size_t key_size,
		     size_t elt_size){
  /* allocate a single block to reduce admin and alignment bytes */
  dll_node_t *node =
    malloc_perror(1, add_sz_perror(sizeof(dll_node_t),
				   add_sz_perror(key_size, elt_size)));
  node->key = (char *)node + sizeof(dll_node_t);
  node->elt = (char *)node->key + key_size;
  memcpy(node->key, key, key_size);
  memcpy(node->elt, elt, elt_size);
  if (*head == NULL){
    node->next = node;
    node->prev = node;
  }else{
    node->next = *head;
    node->prev = (*head)->prev;
    (*head)->prev->next = node;
    (*head)->prev = node;
  }
  *head = node;
}

/**
   Creates and appends a node relative to a head pointer. Please see the
   parameter specification in dll_prepend_new.
*/
void dll_append_new(dll_node_t **head,
		    const void *key,
		    const void *elt,
		    size_t key_size,
		    size_t elt_size){
  dll_prepend_new(head, key, elt, key_size, elt_size);
  *head = (*head)->next;
}

/**
   Prepends a node relative to a head pointer. A head pointer is NULL if
   the list is empty, or points to any node in the list to determine the
   position for a prepend operation.
   head        : pointer to a head pointer to an initialized list           
   node        : pointer to a node to be prepended
*/
void dll_prepend(dll_node_t **head, dll_node_t *node){
  if (*head == NULL){
    node->next = node;
    node->prev = node;
  }else{
    node->next = *head;
    node->prev = (*head)->prev;
    (*head)->prev->next = node;
    (*head)->prev = node;
  }
  *head = node; 
}

/**
   Appends a node relative to a head pointer. Please see the parameter
   specification in dll_prepend.
*/
void dll_append(dll_node_t **head, dll_node_t *node){
  dll_prepend(head, node);
  *head = (*head)->next;
}

/**
   Relative to a head pointer, returns a pointer to the clockwise (next)
   first node with a key that has the same bit pattern as the block pointed
   to by key, or NULL if such a node in not found.
   head        : pointer to a head pointer to an initialized list
   key         : pointer to a key object of size key_size within a contiguous
                 memory block
   key_size    : size of a key object in bytes
*/
dll_node_t *dll_search_key(dll_node_t **head,
			   const void *key,
			   size_t key_size){
  dll_node_t *node = *head;
  if (node == NULL || key == NULL){
    return NULL;
  }else if (memcmp(node->key, key, key_size) == 0){
    return node;
  }else{
    node = node->next;
    while(node != *head){
      if (memcmp(node->key, key, key_size) == 0){
	return node;
      }
      node = node->next;
    }
  }
  return NULL;
}

/**
   Relative to a head pointer, returns a pointer to the clockwise (next)
   first node with an element that satisfies cmp_elt, or NULL if such a node
   in not found.
   head        : pointer to a head pointer to an initialized list
   elt         : - pointer to an element, if the element is within a contiguous
                 memory block and a copy of the element was prepended or
                 appended,
                 - pointer to a pointer to an element, if the element is within
                 a noncontiguous memory block or a pointer to a contiguous
                 element was prepended or appended
   cmp_elt     : comparison function which returns a zero integer value iff
                 the two elements accessed through the first and the second 
                 arguments are equal; each argument is a pointer to an
                 elt_size block that stores an element or a pointer to an
                 element (see specification of dll_prepend_new)
                 
*/
dll_node_t *dll_search_elt(dll_node_t **head,
			   const void *elt,
			   int (*cmp_elt)(const void *, const void *)){
  dll_node_t *node = *head;
  if (node == NULL){
    return NULL;
  }else if (cmp_elt(node->elt, elt) == 0){
    return node;
  }else{
    node = node->next;
    while(node != *head){
      if (cmp_elt(node->elt, elt) == 0){
	return node;
      }
      node = node->next;
    }
  }
  return NULL;
}

/**
   Removes a node in a doubly linked list.
   head        : pointer to a head pointer to an initialized list
   node        : pointer to a node in an initialized list; if the pointer
                 points to the node pointed to by the head pointer, then the
                 head pointer is set to point to the next node from the
                 removed node, or to NULL if the last node is removed
*/
void dll_remove(dll_node_t **head, const dll_node_t *node){
  if (*head == NULL || node == NULL){
    return;
  }else if (node->prev == node && node->next == node){
    *head = NULL;
  }else{
    /* at least two nodes */
    node->next->prev = node->prev;
    node->prev->next = node->next;
    if (*head == node){
      *head = node->next;
    }
  }
}

/**
   Deletes a node in a doubly linked list.
   head        : pointer to a head pointer to an initialized list
   node        : pointer to a node in an initialized list; if the pointer
                 points to the node pointed to by the head pointer, then the
                 head pointer is set to point to the next node from the
                 deleted node, or to NULL if the last node is deleted
   free_elt    : - if an element is within a contiguous memory block and
                 a copy of the element was prepended or appended, then NULL
                 as free_elt is sufficient to delete the node,
                 - if an element is within a noncontiguous memory block or
                 a pointer to a contiguous element was prepended or appended,
                 then an element-specific free_elt, taking a pointer to a
                 pointer to an element as its argument and leaving a block of
                 size elt_size pointed to by the argument, is necessary to
                 delete the node
*/
void dll_delete(dll_node_t **head,
		dll_node_t *node,
		void (*free_elt)(void *)){
  if (*head == NULL || node == NULL){
    return;
  }else if (node->prev == node && node->next == node){
    if (free_elt != NULL) free_elt(node->elt);
    *head = NULL;
    free(node);
    node = NULL;
  }else{
    /* at least two nodes */
    if (free_elt != NULL) free_elt(node->elt);
    node->next->prev = node->prev;
    node->prev->next = node->next;
    if (*head == node){
      *head = node->next;
    }
    free(node);
    node = NULL;
  }
}

/**
   Frees a doubly linked list. Please see the parameter specification in
   dll_delete.
*/
void dll_free(dll_node_t **head, void (*free_elt)(void *)){
  dll_node_t *node = *head, *next_node = NULL;
  if (node != NULL){
    next_node = node->next;
    if (free_elt != NULL) free_elt(node->elt);
    free(node);
    node = next_node;
    while(node != *head){
      next_node = node->next;
      if (free_elt != NULL) free_elt(node->elt);
      free(node);
      node = next_node;
    }
    *head = NULL;
  }
}
