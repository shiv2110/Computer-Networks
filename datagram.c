#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <math.h>
#define M 50
#define IPLEN 17
#include "structs.h"
#include "reassembly.h"
#define payload_len 1480

// To convert dotted ip to unsigned long
unsigned long convert_ip_to_long (char *dot_ip)
{
    return inet_addr(dot_ip);
}
// To check if the datagram is fragmented
bool isFragmented (int MF, int offset)
{
    if ( MF == 0 && offset == 0 )
        return false;
    return true;
}

// To check for inconsistent data [Two way] - checking if unfragmented datagram is a duplicate
// by checking in fragmented datagram list
// When there is an unfragmented datagram and fragmented as well
void check_for_inconsistency_2 (struct Unfragmented **unfrag_dg, struct Datagram **datagram_head)
{
    if ( *datagram_head == NULL || *unfrag_dg == NULL )
        return;

    struct Datagram *ptr = *datagram_head;
    struct Unfragmented *ptr1 = *unfrag_dg;
    while ( ptr != NULL )
    {
        if ( ptr->ip_addr == ptr1->ip_iddr && ptr->id == ptr1->id )
        {
            (*unfrag_dg)->error_bit = 3;
            ptr->error_bit = 6;
            return;
            
        }
        ptr = ptr->next;
    }
}


// Insert unfragmented datagrams into the linked list
void insert_unfragmented_datagram (struct Unfragmented **head, unsigned long ip_addr, 
                                   int id, int datalen, struct Datagram **datagram_head)
{
    struct Unfragmented *ptr = *head;
    struct Unfragmented *new_node = (struct Unfragmented *)malloc(sizeof(struct Unfragmented));
    // Assign the parameters passed to the new_node
    new_node->ip_iddr = ip_addr;
    new_node->id = id;
    new_node->datalen = datalen;
    new_node->error_bit = 0;
    new_node->next = NULL;

    // First error check => inconsistency
    check_for_inconsistency_2(&new_node, datagram_head);

    // If its the first datagram in the list, assign the new_node to head
    if(*head == NULL)
    {
        *head = new_node;
        return;
    }
    while ( ptr != NULL )
    {
        // Duplicates in unfragmented data
        if ( ptr->ip_iddr == new_node->ip_iddr && ptr->id == new_node->id &&
             ptr->datalen == new_node->datalen )
        {
            // duplicate unfragmented datagrams
            if ( ptr->error_bit == 0 )
                ptr->error_bit = 1;

            return;
        }
        // Duplicates in unfragmented data but of different lengths
        else if ( ptr->ip_iddr == new_node->ip_iddr && ptr->id == new_node->id &&
             ptr->datalen != new_node->datalen )
        {
            // duplicate unfragmented datagrams with different length
            if ( ptr->error_bit == 0 )
                ptr->error_bit = 2;

            return;
        }
        ptr = ptr->next;
    }
    // Neither first node nor an error posing node, insert at the end

    ptr = *head;
    while(ptr->next != NULL)
        ptr = ptr->next;
    ptr->next = new_node;
}

// To check for inconsistent data [Two way] - checking if fragmented datagram is a duplicate
// by checking in unfragmented datagram list
void check_for_inconsistency (struct Datagram **datagram, struct Unfragmented **unfrag_head)
{
    if ( *datagram == NULL || *unfrag_head == NULL )
        return;

    struct Unfragmented *ptr = *unfrag_head;
    struct Datagram *ptr1 = *datagram;
    while ( ptr != NULL )
    {
        if ( ptr->ip_iddr == ptr1->ip_addr && ptr->id == ptr1->id )
        {
            ptr->error_bit = 3;
            (*datagram)->error_bit = 6;
            return;
        }
        ptr = ptr->next;
    }
    
}

// Inserting fragmented datagrams
void insert_fragmented_datagram (unsigned long ip_addr, int id, struct Datagram **start_node, 
                                 unsigned MF, int offset, int datalen, struct Unfragmented **unfrag_head)
{
    struct Datagram *ptr = *start_node;
    struct Datagram *new_node = (struct Datagram *)malloc(sizeof(struct Datagram));
    // Load the passed values in new_node members
    new_node->ip_addr = ip_addr;
    new_node->id = id;

    new_node->total_datalen = 0;
    new_node->curr_len = 0;
    new_node->error_bit = 0;

    new_node->frag = NULL;
    new_node->next = NULL;

    // If its the first node, insert it
    if( *start_node == NULL )
    {
        *start_node = new_node;  
        // Insert the fragment 
        insert_fragment(start_node, MF, offset, datalen, unfrag_head);

        return;
    }

    if ( ptr->next == NULL )
    {
        // if ( ptr->ip_addr == new_node->ip_addr && ptr->id == new_node->id )
        // {
        //     // printf("Same here!\n");
        //     return;
        // }
        ptr->next = new_node;
        insert_fragment(&ptr->next, MF, offset, datalen, unfrag_head);

        return;
        
    }

    while( ptr != NULL )
    {
        if ( ptr->ip_addr == new_node->ip_addr && ptr->id == new_node->id )
        {
            // Not a new datagram, but a fragment
            insert_fragment(&ptr, MF, offset, datalen, unfrag_head);
            return;

        }
        ptr = ptr->next;
    }
    // New datagram, insert at the end
    ptr = *start_node;
    while ( ptr->next != NULL )
        ptr = ptr->next;

    ptr->next = new_node;
    insert_fragment(&ptr->next, MF, offset, datalen, unfrag_head);


}