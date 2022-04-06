// All function prototypes

unsigned long convert_ip_to_long (char *);

int num_of_fragments (struct Fragment *); 

void insert_fragmented_datagram (unsigned long, int, struct Datagram **, 
                                 unsigned, int, int, struct Unfragmented **); 

void complete_fragmented_datagram (struct Datagram *);

void successful_but_duplicate (struct Datagram *);

void incomplete_fragmented_datagram (struct Datagram *);

void unsuccessful_fraglen_lt_payload_len_mf (struct Datagram *);

void unsuccessful_fraglen_gt_payload_len (struct Datagram *);

void insert_fragment (struct Datagram **, unsigned, int, int,
                      struct Unfragmented **); 

void check_for_inconsistency (struct Datagram **, struct Unfragmented **);

void inconsistency (struct Unfragmented *);

bool is_last_frag (int, int); 

bool isFragmented (int, int); 

void unfragmented_datagram (struct Unfragmented *);

void insert_unfragmented_datagram (struct Unfragmented **, unsigned long, 
                                   int, int, struct Datagram **); 

void unfragmented_duplicate_diff_len (struct Unfragmented *);  

void unfragmented_duplicate (struct Unfragmented *);  

void print_reassembled (struct Datagram *);

void offset_error (struct Datagram *);
// void duplicate_fragmented (struct Datagram *);


