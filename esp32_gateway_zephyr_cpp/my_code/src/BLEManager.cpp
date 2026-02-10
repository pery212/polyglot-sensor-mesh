#include "BLEManager.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

// 3. Callback: Esta función se activa cuando el móvil escribe el byte
static ssize_t on_write(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                        const void *buf, uint16_t len, uint16_t offset,
                        uint8_t flags) {

  if (len != 1) {
    return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
  }

  uint8_t received_value = *((uint8_t *)buf);
  printk("¡Byte recibido por Bluetooth: %d!\n", received_value);

  return len;
}

// 4. Registro del servicio
BT_GATT_SERVICE_DEFINE(
    my_service, BT_GATT_PRIMARY_SERVICE(BT_UUID_MY_SERVICE),
    BT_GATT_CHARACTERISTIC(BT_UUID_MY_CHAR_BYTE,
                           BT_GATT_CHRC_WRITE, // Permite escribir
                           BT_GATT_PERM_WRITE, // Permiso de escritura
                           NULL, on_write, NULL), );

BLEManager::BLEManager() {
  printk("BLEManager::BLEManager()\n");

  int err = bt_enable(NULL);
  if (err) {
    printk("BLEManager::BLEManager() Error al encender la radio: %d\n", err);
    return;
  }

  // 2. Configuramos el anuncio(Advertising)
  // Definimos qué nombre verá el móvil
  uint8_t COMPANY_ID[] = {0x99, 0x99, 0x76, 0x61, 0x6e,
                          0x74, 0x72, 0x6f, 0x6e, 0x00};
  const struct bt_data ad[] = {
      BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
      BT_DATA(BT_DATA_NAME_SHORTENED, "ESP32", 5),
      // BT_DATA(BT_DATA_MANUFACTURER_DATA, COMPANY_ID, 10),
  };

  const struct bt_data sd[] = {
      BT_DATA(BT_DATA_MANUFACTURER_DATA, COMPANY_ID, sizeof(COMPANY_ID)),
      BT_DATA(BT_DATA_NAME_COMPLETE, "ESP32_LCD", 9),
  };

  // 3. ¡A gritar por la antena!
  // BT_LE_EXT_ADV_SCAN
  //
  // BT_LE_ADV_CONN_FAST_2
  err =
      bt_le_adv_start(BT_LE_ADV_NCONN, ad, ARRAY_SIZE(ad), sd, ARRAY_SIZE(sd));
  if (err) {
    printk("Advertising failed to start (err %d)\n", err);
    return;
  }

  printk("Bluetooth iniciado y anunciado correctamente\n");

  received_value = 0;

  bt_addr_le_t addrs[1];
  size_t count = 1;

  bt_id_get(addrs, &count);

  char addr_str[BT_ADDR_LE_STR_LEN];
  bt_addr_le_to_str(&addrs[0], addr_str, sizeof(addr_str));

  printk("Dirección MAC del dispositivo: %s\n", addr_str);
}

BLEManager::~BLEManager() {
  printk("BLEManager::~BLEManager()\n");
  // Apagamos para ahorrar energía o liberar el recurso
  bt_le_adv_stop();
  // Nota: Zephyr no tiene un "bt_disable" completo en todas las versiones,
  // pero detener el anuncio es lo mínimo indispensable.
  printk("Bluetooth detenido\n");
}

void *create_ble_manager() { return new BLEManager(); }