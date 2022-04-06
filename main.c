/*
    Reassembly Algorithm Implementation
    Created by: Shivanvitha Ambati, 19MCME24
    Subjects: Computer Networks
    Done on: 10th March, 2022

*/

// include necessary libraries
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


int main ( int argc, char **argv ) 
{
    // Intermediate variables used for parsing
    char data[M], dot_ip[IPLEN], *token;
    int MF, offset, id, datalen;

    // Error in cmd line parameters
    if ( argc != 2 ) 
    {
        fprintf(stderr, "Error: Missing file as command line arg\n"
               "Usage: <executable> <file>\n");
        exit(1);
    }

    // Initialise file pointer
    FILE *fp;
    fp = fopen(argv[1], "r");

    // Error if file not found
    if (fp == NULL)
    {
		printf("Error in main.c: %d => %s\n", errno, strerror(errno));
		exit(2);
	}
    
    // Head for fragmented datagrams
    struct Datagram *start_node = (struct Datagram *)malloc(sizeof(struct Datagram));
    start_node = NULL;

    // Head for unfragmented datagrams
    struct Unfragmented *head = (struct Unfragmented *)malloc(sizeof(struct Unfragmented));
    head = NULL;

    // Read from file
    while ( fgets(data, M, fp) != NULL )
    {
        // Skip empty newlines
        if ( strcmp(data, "\n") != 0 )
        {
            // Get IP
            token = strtok(data, " ");
            strcpy(dot_ip, token);

            // Get ID
            token = strtok(NULL, " ");
            sscanf(token, "%d", &id);

            // Get offset
            token = strtok(NULL, " ");
            sscanf(token, "%d", &offset);

            // Get MF
            token = strtok(NULL, " ");
            sscanf(token, "%u", &MF);

            // Get data length
            token = strtok(NULL, " ");
            sscanf(token, "%d", &datalen);

            // Check if the datagram is fragmented
            if ( isFragmented(MF, offset) )
                // Insert fragmented datagram 
                insert_fragmented_datagram(convert_ip_to_long(dot_ip),
                                           id, &start_node, MF, offset, datalen, &head); 
            else 
                // Insert unfragmented datagram
                insert_unfragmented_datagram(&head, convert_ip_to_long(dot_ip),
                                             id, datalen, &start_node);
            

        }

    }

    // function calling for errors and success messages 
    inconsistency(head);
    unfragmented_datagram(head);
    unfragmented_duplicate(head);
    unfragmented_duplicate_diff_len(head);
    incomplete_fragmented_datagram(start_node);
    complete_fragmented_datagram(start_node);
    unsuccessful_fraglen_gt_payload_len(start_node);
    unsuccessful_fraglen_lt_payload_len_mf(start_node);
    successful_but_duplicate(start_node);
    offset_error(start_node);

    printf("\n");
    // Printing the datagrams with successful reassembly
    print_reassembled(start_node);


    exit(0);
}