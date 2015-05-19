#ifndef FREERTPS_UDP_H
#define FREERTPS_UDP_H

#include "freertps/freertps.h"
#include <stdint.h>
#include <stdbool.h>
#include <netinet/in.h>

// NOTE: the prefix freertps_udp_ is too long to type, so it will heretofore 
// be shortened to fu_

// NOTE: everything is assumed to be little-endian. if we ever need to run on
// big-endian, we'll have to rethink some of this.

/////////////////////////////////////////////////////////////////////
// TYPES 
/////////////////////////////////////////////////////////////////////


typedef struct 
{ 
  uint8_t major; 
  uint8_t minor; 
} frudp_pver_t; // protocol version

typedef uint16_t frudp_vid_t; // vendor ID
const char *frudp_vendor(const frudp_vid_t vid);

// for now let's pretend that our vendor ID is 11311 in hex
#define FREERTPS_VENDOR_ID 0x2C2F

#define FRUDP_GUIDPREFIX_LEN 12
typedef uint8_t frudp_guid_prefix_t[FRUDP_GUIDPREFIX_LEN];

bool frudp_guid_prefix_identical(frudp_guid_prefix_t * const a,
                                 frudp_guid_prefix_t * const b);

typedef struct
{
  uint32_t magic_word; // RTPS in ASCII
  frudp_pver_t pver; // protocol version
  frudp_vid_t  vid;  // vendor ID
  frudp_guid_prefix_t guid_prefix;
} fu_header_t;

typedef struct
{
  fu_header_t header;
  uint8_t submsgs[];
} fu_msg_t;

typedef struct
{
  uint8_t id;
  uint8_t flags;
  uint16_t len;
} fu_submsg_header_t;

typedef struct
{
  fu_submsg_header_t header;
  uint8_t contents[];
} fu_submsg_t;

typedef union
{
  struct
  {
    uint8_t key[3];
    uint8_t kind;
  } s;
  uint32_t u;
} __attribute__((packed)) frudp_entityid_t;

#define ENTITYID_UNKNOWN 0

typedef struct
{
  frudp_guid_prefix_t guid_prefix;
  frudp_entityid_t entity_id;
} __attribute__((packed)) frudp_guid_t;

typedef struct
{
  frudp_pver_t       src_pver;
  frudp_vid_t        src_vid;
  frudp_guid_prefix_t src_guid_prefix;
  frudp_guid_prefix_t dst_guid_prefix;
  bool            have_timestamp;
  fr_time_t       timestamp;
} fu_receiver_state_t;


typedef struct
{
  int32_t high;
  uint32_t low;
} fu_sequencenumber_t;

typedef struct
{
  uint16_t extraflags;
  uint16_t octets_to_inline_qos;
  frudp_entityid_t reader_id;
  frudp_entityid_t writer_id;
  fu_sequencenumber_t writer_sn;
} fu_submsg_contents_data_t;

typedef uint16_t fu_parameterid_t;
typedef struct
{
  fu_parameterid_t pid;
  uint16_t len;
  uint8_t value[];
} fu_parameter_list_item_t;

#define FU_PID_SENTINEL 1

#define FRUDP_DATA_ENCAP_SCHEME_PL_CDR_LE 0x0003

typedef void (*frudp_rx_cb_t)(fu_receiver_state_t *rcvr,
                              const fu_submsg_t *submsg,
                              const uint16_t scheme,
                              const uint8_t *data);

typedef struct
{
  frudp_entityid_t reader_id;
  frudp_entityid_t writer_id;
  frudp_rx_cb_t cb;
} frudp_subscription_t;

#ifndef FRUDP_MAX_SUBSCRIPTIONS
#  define FRUDP_MAX_SUBSCRIPTIONS 10
#endif

typedef struct
{
  int32_t kind;
  uint32_t port;
  uint8_t address[16];
} __attribute__((packed)) frudp_locator_t;

#define FRUDP_LOCATOR_KIND_INVALID -1
#define FRUDP_LOCATOR_KIND_RESERVED 0
#define FRUDP_LOCATOR_KIND_UDPV4    1
#define FRUDP_LOCATOR_KIND_UDPV6    2

typedef struct
{
  int32_t sec;
  uint32_t nanosec;
} frudp_duration_t;

typedef uint32_t frudp_builtin_endpoint_set_t;

/////////////////////////////////////////////////////////////////////
// FUNCTIONS 
/////////////////////////////////////////////////////////////////////

bool fu_init();
void fu_fini();

bool fu_add_mcast_rx(const in_addr_t group, 
                     const uint16_t port); //,
                               //const freertps_udp_rx_callback_t rx_cb);

bool fu_listen(const uint32_t max_usec);

bool fu_rx(const in_addr_t src_addr,
           const in_port_t src_port,
           const uint8_t *rx_data,
           const uint16_t rx_len);

bool frudp_subscribe(const frudp_entityid_t reader_id,
                     const frudp_entityid_t writer_id,
                     const frudp_rx_cb_t cb);

#endif

