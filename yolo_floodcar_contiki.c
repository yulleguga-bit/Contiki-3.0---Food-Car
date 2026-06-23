#include "contiki.h"
#include "net/rime/rime.h"
#include "random.h"
#include "dev/leds.h"
#include <stdio.h>
#include <string.h>

#define SEND_INTERVAL      (20 * CLOCK_SECOND)
#define FLOOD_THRESHOLD    70
#define MAX_HOPS           6
#define MSG_TYPE_ALERT     1
#define MSG_TYPE_STATUS    2

typedef struct {
  uint8_t msg_type;
  uint8_t source_id;
  uint8_t seq_id;
  uint8_t flood_level;
  uint8_t confidence;
  uint8_t hop_count;
  uint8_t previous_hop;
} flood_msg_t;

static struct broadcast_conn bc;
static uint8_t local_seq = 0;
static uint8_t sink_addr = 1;

PROCESS(yolo_floodcar_process, "YOLO-FloodCar Contiki process");
AUTOSTART_PROCESSES(&yolo_floodcar_process);

static int
is_sink(void)
{
  return linkaddr_node_addr.u8[0] == sink_addr;
}

static int
is_router(void)
{
  return linkaddr_node_addr.u8[0] >= 2 && linkaddr_node_addr.u8[0] <= 4;
}

static int
is_vehicle(void)
{
  return linkaddr_node_addr.u8[0] >= 5;
}

static uint8_t
simulate_flood_level(void)
{
  if(linkaddr_node_addr.u8[0] == 5) {
    return 82 + (random_rand() % 12);
  }
  if(linkaddr_node_addr.u8[0] == 6) {
    return 55 + (random_rand() % 25);
  }
  return random_rand() % 60;
}

static uint8_t
simulate_confidence(uint8_t flood_level)
{
  if(flood_level >= FLOOD_THRESHOLD) {
    return 80 + (random_rand() % 20);
  }
  return 40 + (random_rand() % 30);
}

static void
send_status_or_alert(void)
{
  flood_msg_t msg;
  uint8_t flood_level;

  flood_level = simulate_flood_level();

  msg.msg_type = (flood_level >= FLOOD_THRESHOLD) ? MSG_TYPE_ALERT : MSG_TYPE_STATUS;
  msg.source_id = linkaddr_node_addr.u8[0];
  msg.seq_id = ++local_seq;
  msg.flood_level = flood_level;
  msg.confidence = simulate_confidence(flood_level);
  msg.hop_count = 0;
  msg.previous_hop = linkaddr_node_addr.u8[0];

  packetbuf_clear();
  packetbuf_copyfrom(&msg, sizeof(msg));
  broadcast_send(&bc);

  printf("NODE %u SENT %s seq=%u level=%u conf=%u hop=%u\n",
         msg.source_id,
         msg.msg_type == MSG_TYPE_ALERT ? "ALERT" : "STATUS",
         msg.seq_id,
         msg.flood_level,
         msg.confidence,
         msg.hop_count);

  if(msg.msg_type == MSG_TYPE_ALERT) {
    leds_on(LEDS_RED);
  } else {
    leds_on(LEDS_GREEN);
  }
}

static void
recv_broadcast(struct broadcast_conn *c, const linkaddr_t *from)
{
  flood_msg_t msg;

  if(packetbuf_datalen() != sizeof(flood_msg_t)) {
    return;
  }

  memcpy(&msg, packetbuf_dataptr(), sizeof(msg));

  if(from->u8[0] == linkaddr_node_addr.u8[0]) {
    return;
  }

  if(is_sink()) {
    printf("SINK RECEIVED from=%u src=%u type=%s seq=%u level=%u conf=%u hops=%u prev=%u\n",
           from->u8[0],
           msg.source_id,
           msg.msg_type == MSG_TYPE_ALERT ? "ALERT" : "STATUS",
           msg.seq_id,
           msg.flood_level,
           msg.confidence,
           msg.hop_count,
           msg.previous_hop);

    if(msg.msg_type == MSG_TYPE_ALERT) {
      leds_on(LEDS_RED);
    } else {
      leds_on(LEDS_BLUE);
    }
    return;
  }

  if(is_router()) {
    if(msg.hop_count >= MAX_HOPS) {
      return;
    }

    if(msg.previous_hop == linkaddr_node_addr.u8[0]) {
      return;
    }

    msg.hop_count++;
    msg.previous_hop = linkaddr_node_addr.u8[0];

    clock_wait(CLOCK_SECOND / 8);
    packetbuf_clear();
    packetbuf_copyfrom(&msg, sizeof(msg));
    broadcast_send(&bc);

    printf("ROUTER %u FORWARDED src=%u seq=%u level=%u conf=%u hops=%u\n",
           linkaddr_node_addr.u8[0],
           msg.source_id,
           msg.seq_id,
           msg.flood_level,
           msg.confidence,
           msg.hop_count);

    leds_toggle(LEDS_BLUE);
  }
}

static const struct broadcast_callbacks broadcast_call = {recv_broadcast};

PROCESS_THREAD(yolo_floodcar_process, ev, data)
{
  static struct etimer periodic;

  PROCESS_EXITHANDLER(broadcast_close(&bc);)
  PROCESS_BEGIN();

  broadcast_open(&bc, 129, &broadcast_call);

  if(is_sink()) {
    printf("NODE %u STARTED AS SINK\n", linkaddr_node_addr.u8[0]);
    leds_on(LEDS_BLUE);
  } else if(is_router()) {
    printf("NODE %u STARTED AS ROUTER\n", linkaddr_node_addr.u8[0]);
  } else if(is_vehicle()) {
    printf("NODE %u STARTED AS VEHICLE SENSOR\n", linkaddr_node_addr.u8[0]);
  } else {
    printf("NODE %u STARTED AS GENERIC NODE\n", linkaddr_node_addr.u8[0]);
  }

  etimer_set(&periodic, CLOCK_SECOND * (10 + (linkaddr_node_addr.u8[0] % 5)));

  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic));

    if(is_vehicle()) {
      send_status_or_alert();
    }

    etimer_set(&periodic, SEND_INTERVAL + (random_rand() % (5 * CLOCK_SECOND)));
  }

  PROCESS_END();
}
