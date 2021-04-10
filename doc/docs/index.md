# Primeiros Passos

Este documento tem como objetivo ajudá-lo a configurar o ESP-IDF (Espressif IoT Development Framework) para trabalhar com a Franzininho WiFi que usa o ESP32-S2 da Espressif.

Ao final, faremos um exemplo simples para compilar, gravar e monitorar usando o ESP-IDF assim garantiremos que tudo estará funcionando.

## Introdução

Para essa configuração, vamos instalar o ESP-IDF e usá-lo através de linha de comando. Caso você queira usar o IDF integrado a ambientes de desenvolvimento integrado (IDE) como VScode e Eclipse, confira os seguintes links:
- [Eclipse Plugin](https://github.com/espressif/idf-eclipse-plugin)
- [VS Code Extension](https://github.com/espressif/vscode-esp-idf-extension)

Você poderá instar o ESP-IDF no seu sistemas operacional preferido (Linux, Windows, macOS).

Para a experiência completa, você precisará de uma placa Franzininho WiFi, Computador: com Windows, Linux ou macOS

## Passo 1 - Instalação dos pré-requisitos

Algumas ferramentas precisam ser instaladas no computador antes de prosseguir para as próximas etapas. Siga os links abaixo para obter as instruções para o seu sistema operacional:
- [Windows](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s2/get-started/windows-setup.html)
- [Linux](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s2/get-started/linux-setup.html)
- [Mac OS](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s2/get-started/macos-setup.html)

É muito importante a instalação de todos os pré-requisitos.

## Passo 2 - Instalação do ESP-IDF

Nessa etapa vamos instalar o ESP-IDF e conjunto de bibliotecas mantidas pela Espressif no repositório do [ESP-IDF](https://github.com/espressif/esp-idf)

O ESP-IDF é o framework oficial da Espressif para o desenvolvimento com toda a família ESP32.O procedimento apresentado aqui servirá para trabalhar com toda a família ESP32. Porém vamos dar foco ao ESP32-S2 que é usado na Franzininho WiFi.

### Linux e macOS

Abra o terminal e execute:
```console
mkdir -p ~/esp
cd ~/esp
git clone --recursive https://github.com/espressif/esp-idf.git
```
O ESP-IDF será baixado no seguinte repositório `~/esp/esp-idf`.

### Windows

Além de instalar as ferramentas, o [ESP-IDF Tools Installer para Windows](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s2/get-started/windows-setup.html#get-started-windows-tools-installer) apresentado na Etapa 1 também pode baixar uma cópia do ESP-IDF. Dessa forma você não precisará baixar o ESP-IDF agora, se já tiver baixado anteriormente junto aos pré-requisitos

Se desejar fazer o download do ESP-IDF sem a ajuda do ESP-IDF Tools Installer, consulte [estas instruções](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s2/get-started/windows-setup-scratch.html#get-esp-idf-windows-command-line).

### Instalando O ESP-IDF e ferramentas

Além do ESP-IDF, você também precisa instalar as ferramentas usadas pelo ESP-IDF, como compilador, depurador, pacotes Python, etc.

#### Windows

O ESP-IDF Tools Installer para Windows apresentado no passo 1 instala todas as ferramentas necessárias.

Se você deseja instalar as ferramentas sem a ajuda do ESP-IDF Tools Installer, abra o Prompt de Comando e siga estas etapas:
```console
cd %userprofile%\esp\esp-idf
install.bat
```

Ou no Windows PowerShell
```console
cd ~/esp/esp-idf
./install.ps1
```

#### Linux e macOS

No Linux ou macOS há um script para instalação.Abra o terminal e execute:
```console
cd ~/esp/esp-idf
./install.sh
```

### Configurando as variáveis de ambientes

Windows

Linux e macOS


## Passo 3 - Criando um novo projeto
