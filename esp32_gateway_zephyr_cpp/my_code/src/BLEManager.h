#ifndef BLE_MANAGER_H
#define BLE_MANAGER_H

#ifdef __cplusplus

#include <zephyr/bluetooth/addr.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/uuid.h>

// 1. Definimos el UUID del Servicio y de la Característica
// Este UUID es para el "Cajón" (Servicio)
#define BT_UUID_MY_SERVICE                                                     \
  BT_UUID_DECLARE_128(                                                         \
      BT_UUID_128_ENCODE(0x12345678, 0x1234, 0x5678, 0x1234, 0x56789abcdef0))

// Este UUID es para la "Carpeta" de 1 byte (Característica)
#define BT_UUID_MY_CHAR_BYTE                                                   \
  BT_UUID_DECLARE_128(                                                         \
      BT_UUID_128_ENCODE(0x12345678, 0x1234, 0x5678, 0x1234, 0x56789abcdef1))

class BLEManager {
private:
  uint8_t received_value;

public:
  BLEManager();
  ~BLEManager();
};

extern "C" {
#endif

void *create_ble_manager();

#ifdef __cplusplus
}
#endif

#endif