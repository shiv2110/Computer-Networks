// Structs for datagram, fragment and unfragmented datagrams

struct Unfragmented 
{
    unsigned long ip_iddr;
    int id;
    int datalen;
    int error_bit;
    struct Unfragmented *next;
};

struct Fragment
{
    unsigned MF;
    int offset;
    int datalen;
    struct Fragment *next;
};

struct Datagram 
{
    unsigned long ip_addr;
    int id;
    int total_datalen;
    int curr_len;
    int error_bit;
    struct Datagram *next;
    struct Fragment *frag;
};
