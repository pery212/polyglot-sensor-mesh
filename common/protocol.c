#pragma pack(push, 1) // We use this to avoid padding from the compiler
typedef struct {
  uint8_t node_id;
  uint16_t msg_id;
  float value;
  uint32_t timestamp;
} sensor_packet_t;
#pragma pack(pop)