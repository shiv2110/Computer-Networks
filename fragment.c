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

// To check if its the last fragment
bool is_last_frag (int MF, int offset)
{
    if ( MF == 0 && offset > 0 )
        return true;
    return false;
}

// Counting the number of fragments
int num_of_fragments (struct Fragment *frag)
{
    int n = 0;
    struct Fragment *ptr = frag;
    while ( ptr != NULL )
    {
        n++;
        ptr = ptr->next;
    }
    return n;
}

// Inserting fragments
void insert_fragment (struct Datagram **datagram, unsigned MF, int offset, int datalen,
                      struct Unfragmented **unfrag_head)
{
    struct Fragment *fragment = (struct Fragment *)malloc(sizeof(struct Fragment));
    fragment->MF = MF;
    fragment->offset = offset;
    fragment->datalen = datalen;
    fragment->next = NULL;
    struct Fragment *ptr;
    ptr = (*datagram)->frag;

    // If offset of the fragment 
    if ( fragment->offset % payload_len != 0 )
    {
        // offsetError;
        (*datagram)->error_bit = 7;
        return;
    }
    // If fragment length exceeds maximum data length
    else if ( fragment->datalen > payload_len )
    {
        // reassembly unsuccessful (fraglen > payload_len)
        (*datagram)->error_bit = 4;
        return;
    }
    // If its not the last fragment but fraglen is less than max payload length
    else if ( MF == 1 && fragment->datalen < payload_len )
    {
        // reassembly unsuccessful (not a last frag, fraglen < payload_len)
        (*datagram)->error_bit = 5;
        return;
    }

    // Check for inconsistency
    check_for_inconsistency(datagram, unfrag_head);


    // If the incoming fragment is the last one, update total length
    if ( is_last_frag(MF, offset) )
    {
        // total_len = offset + datalen
        (*datagram)->total_datalen = offset + datalen;
    }
    // If its the first fragment
    if ( (*datagram)->frag == NULL )
    {
        (*datagram)->curr_len += datalen;
        (*datagram)->frag = fragment;

    }
    // If the offset is less than the head of the fragments
    // This becomes the new head
    else if ( fragment->offset < ptr->offset )
    {
        fragment->next = ptr;
        (*datagram)->frag = fragment;
        (*datagram)->curr_len += datalen;

    }
    // To insert fragments when its not the first one
    // To get ordered list, we insert such that the resultant list is ordered
    else {
        while ( ptr != NULL ) 
        {
            // A duplicate fragment
            if ( ptr->offset == fragment->offset && ptr->datalen == fragment->datalen && 
                 ptr->MF == fragment->MF )
            {
                // duplicate fragment
                (*datagram)->error_bit = 3;
                return;
            }
            ptr = ptr->next;
        }
        // Inserting fragment at the end when all the others have fraglen less than
        // this incoming fragment
        ptr = (*datagram)->frag;
        while ( ptr->next != NULL &&  fragment->offset > ptr->next->offset )
            ptr = ptr->next;
        
        fragment->next = ptr->next;
        ptr->next = fragment;
        (*datagram)->curr_len += datalen;
    }

}