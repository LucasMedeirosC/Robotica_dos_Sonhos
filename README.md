# Robótica dos Sonhos

## Descrição
Projeto de robótica utilizando Arduino com comunicação ESP-NOW para controle sem fio de dispositivos.

## Estrutura do Projeto

### Codes/
Contém os códigos-fonte do projeto organizados em diferentes módulos:

#### espnow/
Implementação de comunicação ESP-NOW entre dispositivos:
- **transmissor.ino**: Código para o dispositivo transmissor que envia comandos
- **receptor.ino**: Código para o dispositivo receptor que recebe e processa comandos

#### test/
Códigos de teste para validação do sistema:
- **transmissor.ino**: Versão de teste do transmissor
- **receptor.ino**: Versão de teste do receptor

## Tecnologias Utilizadas
- Arduino
- ESP-NOW (protocolo de comunicação sem fio)
- ESP32/ESP8266

## Como Usar

### Requisitos
- Arduino IDE
- Placas ESP32 ou ESP8266
- Biblioteca ESP-NOW instalada

### Instalação
1. Clone este repositório
2. Abra os arquivos `.ino` no Arduino IDE
3. Configure as placas ESP32/ESP8266 no Arduino IDE
4. Faça upload do código do transmissor para uma placa
5. Faça upload do código do receptor para outra placa

### Configuração
Certifique-se de configurar os endereços MAC dos dispositivos nos códigos para estabelecer a comunicação ESP-NOW corretamente.

## Funcionalidades
- Comunicação bidirecional sem fio entre dispositivos
- Baixa latência
- Baixo consumo de energia
- Ideal para projetos de robótica e automação

## Contribuição
Contribuições são bem-vindas! Sinta-se à vontade para abrir issues ou pull requests.

## Licença
Este projeto está em desenvolvimento para fins educacionais.
