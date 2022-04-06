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

// Printing all datagrams that are complete and have been reassembled
void print_reassembled (struct Datagram *head_dg)
{
    // If an empty file is passed, implies no datagrams
    if ( head_dg == NULL )
    {
        printf("No datagrams received!\n");
        return;
    }
    
    struct in_addr addr;
    struct Datagram *ptr = head_dg;
    while ( ptr != NULL )
    {
        if ( ptr->error_bit == 0 || ptr->total_datalen == ptr->curr_len ) 
        {
            addr.s_addr = ptr->ip_addr;
            printf("IP: %s, ID: %d, Total Data Length: %d\n", 
                   inet_ntoa(addr), ptr->id, ptr->total_datalen);
            struct Fragment *temp_frag = ptr->frag;
            while ( temp_frag != NULL )
            {
                printf("\tMF: %u, Offset: %d, Fragment Data Length: %d\n",
                temp_frag->MF, temp_frag->offset, temp_frag->datalen);

                temp_frag = temp_frag->next;
            }
        }
        ptr = ptr->next;
    }
}