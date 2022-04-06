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

// Unfragmented duplicate datagrams display
void unfragmented_duplicate (struct Unfragmented *head)
{
    struct in_addr addr;
    struct Unfragmented *ptr = head;
    while ( ptr != NULL )
    {
        if ( ptr->error_bit == 1 )
        {
            addr.s_addr = ptr->ip_iddr;
            printf("%s\t%d\tduplicate unfragmented datagram\n", inet_ntoa(addr), ptr->id);
        }
        ptr = ptr->next;
    }

}

// Unfragmented duplicate datagrams with different lengths display
void unfragmented_duplicate_diff_len (struct Unfragmented *head)
{
    struct in_addr addr;
    struct Unfragmented *ptr = head;
    while ( ptr != NULL )
    {
        if ( ptr->error_bit == 2 )
        {
            addr.s_addr = ptr->ip_iddr;
            printf("%s\t%d\tduplicate unfragmented datagram with different lengths\n", inet_ntoa(addr), 
            ptr->id);
        }
        ptr = ptr->next;
    }

}

// Unfragmented datagrams display
void unfragmented_datagram (struct Unfragmented *head)
{
    struct in_addr addr;
    struct Unfragmented *ptr = head;
    while ( ptr != NULL )
    {
        if ( ptr->error_bit == 0 )
        {

            addr.s_addr = ptr->ip_iddr;
            printf("%s\t%d\tunfragmented datagram\t%d\n", inet_ntoa(addr), 
                    ptr->id, ptr->datalen);
        }
        ptr = ptr->next;
    }
}

// Inconsistency in datagrams display
void inconsistency (struct Unfragmented *head)
{

    struct in_addr addr;
    struct Unfragmented *ptr = head;
    while ( ptr != NULL )
    {
        if ( ptr->error_bit == 3 )
        {
            addr.s_addr = ptr->ip_iddr;
            printf("%s\t%d\tduplicate datagram with inconsistent data "
            "\n\t\t\t(unfragmented datagram and the other is fragmented, either complete or incomplete)\n",
            inet_ntoa(addr), ptr->id);
        }
        ptr = ptr->next;
    }
}

// Reassembly unsuccessful because fraglen > max data len
void unsuccessful_fraglen_gt_payload_len (struct Datagram *head)
{
    struct in_addr addr;
    struct Datagram *ptr = head;
    while ( ptr != NULL )
    {
        if ( ptr->error_bit == 4 )
        {
            addr.s_addr = ptr->ip_addr;
            printf("%s\t%d\treassembly unsuccessful (fraglen > max payload_len)\n", inet_ntoa(addr), ptr->id);

        }
        ptr = ptr->next;
    }
}

// Offset not a multiple of payload len
void offset_error (struct Datagram *head)
{
    struct in_addr addr;
    struct Datagram *ptr = head;
    while ( ptr != NULL )
    {
        if ( ptr->error_bit == 7 )
        {
            addr.s_addr = ptr->ip_addr;
            printf("%s\t%d\toffset is not a multiple of max payload len (%d)\n", inet_ntoa(addr), ptr->id, payload_len);

        }
        ptr = ptr->next;
    }
}

// Reassembly unsuccessful because fraglen < max data len, not last fragment
void unsuccessful_fraglen_lt_payload_len_mf (struct Datagram *head)
{
    struct in_addr addr;
    struct Datagram *ptr = head;
    while ( ptr != NULL )
    {
        if ( ptr->error_bit == 5 )
        {
            addr.s_addr = ptr->ip_addr;
            printf("%s\t%d\treassembly unsuccessful (fraglen < max payload_len, not last fragment)\n",
            inet_ntoa(addr), ptr->id);

        }
        ptr = ptr->next;
    }
}

// Incomplete fragmented datagram (missing fragments)
void incomplete_fragmented_datagram (struct Datagram *head)
{
    struct in_addr addr;
    struct Datagram *ptr = head;
    while ( ptr != NULL )
    {
        if ( ptr->total_datalen == 0 ||  ceil(ptr->total_datalen/(float)payload_len) > (float)num_of_fragments(ptr->frag) )
        {
            if ( ptr->error_bit == 0 )
            {
                ptr->error_bit = 1;
                addr.s_addr = ptr->ip_addr;
                printf("%s\t%d\tincomplete fragmented datagram (missing fragments)\n", inet_ntoa(addr), ptr->id);
            }
        }
        ptr = ptr->next;
    }
}

// Reassembly either success or not with duplicates
void successful_but_duplicate (struct Datagram *head)
{
    struct in_addr addr;
    struct Datagram *ptr = head;
    while ( ptr != NULL )
    {
        if ( ptr->total_datalen == ptr->curr_len && ptr->error_bit == 3 )
        {
            addr.s_addr = ptr->ip_addr;
            printf("%s\t%d\treassembly successful and duplicate fragment\n", inet_ntoa(addr), ptr->id);
            
        }
        else if ( ptr->total_datalen != ptr->curr_len && ptr->error_bit == 3 )
        {
            addr.s_addr = ptr->ip_addr;
            printf("%s\t%d\treassembly unsuccessful and duplicate fragment\n", inet_ntoa(addr), ptr->id);
            
        }
        ptr = ptr->next;
    }
}

// Complete datagrams (Reassembly done)
void complete_fragmented_datagram (struct Datagram *head)
{
    struct in_addr addr;
    struct Datagram *ptr = head;
    while ( ptr != NULL )
    {
        if ( ptr->error_bit == 0 )
        {
            addr.s_addr = ptr->ip_addr;
            printf("%s\t%d\tcomplete fragmented datagram\t%d\n", inet_ntoa(addr), ptr->id, ptr->total_datalen);
        }
        ptr = ptr->next;
    }
}