#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>

// codigo 1 (XX0H65)

// mac2: 88:57:21:B7:54:08 (mac do receptor)

#define WIFI_CHANNEL 1
uint8_t peerMac[] = { 0x88, 0x57, 0x21, 0xb7, 0x54, 0x08 }; // MAC do B

static void printMac(const uint8_t *m) {
  for (int i = 0; i < 6; i++) { if (i) Serial.print(":"); Serial.printf("%02X", m[i]); }
}

void onSent(const wifi_tx_info_t* info, esp_now_send_status_t status) {
  Serial.print("Enviado para ");
  if (info && info->des_addr) printMac(info->des_addr); else Serial.print("(desconhecido)");
  Serial.print(" -> ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "OK" : "FALHA");
}

void onRecv(const esp_now_recv_info_t* info, const uint8_t* data, int len) {
  Serial.print("Recebido de ");
  if (info && info->src_addr) printMac(info->src_addr); else Serial.print("(desconhecido)");
  Serial.print(" -> ");
  Serial.write(data, len);
  Serial.println();
}

bool addPeer(const uint8_t mac[6]) {
  esp_now_peer_info_t p{};
  memcpy(p.peer_addr, mac, 6);
  p.channel = WIFI_CHANNEL;
  p.encrypt = false;
  p.ifidx = WIFI_IF_STA;
  return esp_now_add_peer(&p) == ESP_OK;
}

void setup() {
  Serial.begin(115200);
  delay(300);

  WiFi.mode(WIFI_MODE_STA);
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_channel(WIFI_CHANNEL, WIFI_SECOND_CHAN_NONE);
  esp_wifi_set_promiscuous(false);

  if (esp_now_init() != ESP_OK) { Serial.println("ESP-NOW init falhou"); while (1) delay(1000); }
  esp_now_register_send_cb(onSent);
  esp_now_register_recv_cb(onRecv);

  if (!addPeer(peerMac)) Serial.println("Falha ao adicionar peer");

  Serial.print("Meu MAC: "); Serial.println(WiFi.macAddress());
  Serial.println("A pronto. Enviando 'oi de A' a cada 2s...");
}

void loop() {
  const char* msg = "oi de A";
  esp_err_t r = esp_now_send(peerMac, (const uint8_t*)msg, strlen(msg));
  if (r != ESP_OK) Serial.printf("Erro send: %d\n", r);
  delay(2000);
}
