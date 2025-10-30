# 🎵 SAMPTube - PAWN Library

<p align="center">
  <b>Sistema completo de música do YouTube para SA-MP</b><br>
  <i>Playlists, Soundboxes 3D, Som em Veículos e muito mais!</i>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/SA--MP-0.3.7--R2-orange?style=for-the-badge" alt="SA-MP">
  <img src="https://img.shields.io/badge/PAWN-Compiler-blue?style=for-the-badge" alt="PAWN">
  <img src="https://img.shields.io/badge/sampctl-Package-green?style=for-the-badge" alt="sampctl">
</p>

---

## 📋 Sobre

**SAMPTube** é uma biblioteca PAWN completa que integra o YouTube ao seu servidor SA-MP. Permite que jogadores toquem músicas, criem playlists, posicionem caixas de som 3D no mapa e ativem som em veículos - tudo sincronizado entre os players!

### ✨ Características Principais

#### 🎧 Sistema de Música Individual
- Tocar músicas por URL ou busca
- Playlists com gerenciamento completo
- Pause, resume, próxima, anterior
- Sistema de callbacks para eventos

#### 📦 Soundboxes 3D (Caixas de Som)
- Posicione caixas de som no mapa com som 3D
- Alcance configurável
- Sincronização automática para quem entra na área
- Playlists com loop automático
- Controle de próxima/anterior música

#### 🚗 Som em Veículos
- Ative playlist no veículo atual
- Passageiros ouvem automaticamente
- Sincronização ao entrar no veículo
- Para automaticamente ao sair

#### 🔧 Recursos Técnicos
- HTTP Range Requests (pause/resume)
- Sincronização baseada em tempo
- MapAndreas para posicionamento 3D
- Sistema de slots reutilizáveis
- Zero memory leaks

---

## 📦 Instalação

### Método 1: sampctl (Recomendado)

```bash
sampctl package install xBruno1000x/samptube-pawn
```

### Método 2: Manual

1. Baixe a última release
2. Extraia `samptube.inc` para `pawno/include/`
3. Instale as dependências:
   - [pawn-requests](https://github.com/Southclaws/pawn-requests)
   - [pawn-map](https://github.com/katursis/Pawn.Map)
   - [streamer](https://github.com/samp-incognito/samp-streamer-plugin)
   - [mapandreas](https://github.com/Southclaws/samp-plugin-mapandreas)
   - [zcmd](https://github.com/Southclaws/zcmd)

### Dependências no pawn.json

```json
{
  "dependencies": [
    "Southclaws/pawn-requests",
    "katursis/Pawn.Map",
    "samp-incognito/samp-streamer-plugin",
    "Southclaws/samp-plugin-mapandreas",
    "Southclaws/zcmd"
  ]
}
```

---

## 🚀 Uso Rápido

### Include no seu Gamemode

```pawn
#include <a_samp>
#include <samptube>

// Callbacks (opcional)
public OnSAMPTubeTrackStart(playerid, const title[], duration) {
    new msg[144];
    format(msg, sizeof(msg), "Tocando: %s (%d segundos)", title, duration/1000);
    SendClientMessage(playerid, -1, msg);
    return 1;
}

public OnSAMPTubeTrackEnd(playerid) {
    SendClientMessage(playerid, -1, "Música terminou!");
    return 1;
}

public OnSAMPTubeError(playerid, const error[]) {
    new msg[144];
    format(msg, sizeof(msg), "Erro: %s", error);
    SendClientMessage(playerid, 0xFF0000FF, msg);
    return 1;
}
```

---

## 📚 API Reference

### 🎵 Funções de Música Individual

#### `SAMPTube_PlayURL(playerid, const url[])`
Toca uma música do YouTube por URL.

```pawn
SAMPTube_PlayURL(playerid, "https://www.youtube.com/watch?v=dQw4w9WgXcQ");
```

#### `SAMPTube_PlaySearch(playerid, const query[])`
Busca e toca a primeira música encontrada.

```pawn
SAMPTube_PlaySearch(playerid, "Linkin Park Numb");
```

#### `SAMPTube_Pause(playerid)`
Pausa a reprodução atual.

```pawn
SAMPTube_Pause(playerid);
```

#### `SAMPTube_Resume(playerid)`
Resume a reprodução pausada.

```pawn
SAMPTube_Resume(playerid);
```

#### `SAMPTube_Stop(playerid)`
Para completamente a reprodução.

```pawn
SAMPTube_Stop(playerid);
```

---

### 📝 Funções de Playlist

#### `SAMPTube_AddToPlaylist(playerid, const url[])`
Adiciona uma música à playlist por URL.

```pawn
SAMPTube_AddToPlaylist(playerid, "https://www.youtube.com/watch?v=VIDEO_ID");
```

#### `SAMPTube_AddSearchToPlaylist(playerid, const query[])`
Busca e adiciona à playlist.

```pawn
SAMPTube_AddSearchToPlaylist(playerid, "Eminem Lose Yourself");
```

#### `SAMPTube_PlayPlaylist(playerid)`
Inicia a playlist do jogador.

```pawn
SAMPTube_PlayPlaylist(playerid);
```

#### `SAMPTube_Next(playerid)`
Próxima música da playlist.

```pawn
SAMPTube_Next(playerid);
```

#### `SAMPTube_Previous(playerid)`
Música anterior da playlist.

```pawn
SAMPTube_Previous(playerid);
```

#### `SAMPTube_ClearPlaylist(playerid)`
Limpa toda a playlist.

```pawn
SAMPTube_ClearPlaylist(playerid);
```

#### `SAMPTube_RemoveFromPlaylist(playerid, index)`
Remove música por índice (0-based).

```pawn
SAMPTube_RemoveFromPlaylist(playerid, 0); // Remove primeira música
```

#### `SAMPTube_RemoveFromPlaylistByName(playerid, const name[])`
Remove música por nome parcial.

```pawn
SAMPTube_RemoveFromPlaylistByName(playerid, "Numb");
```

#### `SAMPTube_GetPlaylistCount(playerid)`
Retorna quantidade de músicas na playlist.

```pawn
new count = SAMPTube_GetPlaylistCount(playerid);
printf("Player tem %d músicas na playlist", count);
```

#### `SAMPTube_GetPlaylistTrack(playerid, index, title[], maxlen)`
Obtém título de uma música da playlist.

```pawn
new title[128];
if (SAMPTube_GetPlaylistTrack(playerid, 0, title, sizeof(title))) {
    printf("Primeira música: %s", title);
}
```

---

### 📦 Funções de Soundbox (Caixas de Som 3D)

#### `SAMPTube_CreateSoundBox(const url[], Float:x, Float:y, Float:z, Float:range = 8.0, usepos = 1)`
Cria uma soundbox com URL única.

```pawn
new Float:x, Float:y, Float:z;
GetPlayerPos(playerid, x, y, z);
new boxid = SAMPTube_CreateSoundBox("https://youtu.be/VIDEO_ID", x, y, z, 50.0, 1);
```

**Parâmetros:**
- `url`: URL do YouTube
- `x, y, z`: Posição no mapa
- `range`: Alcance do som em metros (padrão: 8.0)
- `usepos`: 1 = som 3D, 0 = som normal (padrão: 1)

**Retorna:** ID da soundbox ou -1 em erro

#### `SAMPTube_CreateBoxPlaylist(playerid, Float:x, Float:y, Float:z, Float:range = 8.0, usepos = 1)`
Cria soundbox com a playlist do jogador.

```pawn
new Float:x, Float:y, Float:z;
GetPlayerPos(playerid, x, y, z);

// Player precisa ter músicas na playlist
SAMPTube_AddSearchToPlaylist(playerid, "Linkin Park");
SAMPTube_AddSearchToPlaylist(playerid, "Eminem");

new boxid = SAMPTube_CreateBoxPlaylist(playerid, x, y, z, 50.0, 1);
// Toca automaticamente em loop
```

#### `SAMPTube_DestroySoundBox(soundboxid)`
Remove uma soundbox do servidor.

```pawn
SAMPTube_DestroySoundBox(boxid);
```

#### `SAMPTube_PlaySoundBoxForPlayer(playerid, soundboxid)`
Toca a soundbox para um jogador específico.

```pawn
SAMPTube_PlaySoundBoxForPlayer(playerid, boxid);
```

#### `SAMPTube_SoundBoxNext(soundboxid)`
Avança para próxima música da soundbox (se for playlist).

```pawn
SAMPTube_SoundBoxNext(boxid);
```

#### `SAMPTube_SoundBoxPrevious(soundboxid)`
Volta para música anterior da soundbox.

```pawn
SAMPTube_SoundBoxPrevious(boxid);
```

#### `SAMPTube_GetBoxCurrentTrack(soundboxid, title[], maxlen = sizeof(title))`
Obtém título da música atual da soundbox.

```pawn
new title[128];
if (SAMPTube_GetBoxCurrentTrack(boxid, title)) {
    printf("Soundbox tocando: %s", title);
}
```

#### `SAMPTube_GetSoundBoxByArea(areaid)`
Obtém ID da soundbox por área dinâmica.

```pawn
// Interno, usado nos hooks
new boxid = SAMPTube_GetSoundBoxByArea(areaid);
```

---

### 🚗 Funções de Som em Veículos

#### `SAMPTube_CreateVehicleSound(vehicleid, const url[])`
Cria som com URL única em veículo.

```pawn
new vehicleid = GetPlayerVehicleID(playerid);
SAMPTube_CreateVehicleSound(vehicleid, "https://youtu.be/VIDEO_ID");
```

#### `SAMPTube_CreateVehiclePlaylist(playerid, vehicleid)`
Cria som com playlist do jogador em veículo.

```pawn
new vehicleid = GetPlayerVehicleID(playerid);

// Player precisa ter playlist
SAMPTube_AddSearchToPlaylist(playerid, "Rock");
SAMPTube_CreateVehiclePlaylist(playerid, vehicleid);
// Todos no veículo ouvem automaticamente
```

#### `SAMPTube_DestroyVehicleSound(vehicleid)`
Remove som do veículo.

```pawn
new vehicleid = GetPlayerVehicleID(playerid);
SAMPTube_DestroyVehicleSound(vehicleid);
```

#### `SAMPTube_PlayVehicleForPlayer(playerid, vehicleid)`
Toca som do veículo para jogador específico.

```pawn
// Usado internamente pelo OnPlayerStateChange
SAMPTube_PlayVehicleForPlayer(playerid, vehicleid);
```

#### `SAMPTube_VehicleNext(vehicleid)`
Próxima música do veículo.

```pawn
new vehicleid = GetPlayerVehicleID(playerid);
SAMPTube_VehicleNext(vehicleid);
```

#### `SAMPTube_VehiclePrevious(vehicleid)`
Música anterior do veículo.

```pawn
new vehicleid = GetPlayerVehicleID(playerid);
SAMPTube_VehiclePrevious(vehicleid);
```

#### `SAMPTube_GetVehicleCurrentTrack(vehicleid, title[], maxlen = sizeof(title))`
Obtém título da música atual do veículo.

```pawn
new title[128];
if (SAMPTube_GetVehicleCurrentTrack(vehicleid, title)) {
    printf("Veículo tocando: %s", title);
}
```

---

## 🎮 Exemplo Completo de Comandos

Veja o arquivo `exemplo.pwn` para implementação completa. Aqui estão alguns exemplos:

### Comandos de Música Individual

```pawn
CMD:tocar(playerid, params[]) {
    if (!strlen(params)) {
        SendClientMessage(playerid, -1, "Use: /tocar [URL do YouTube]");
        return 1;
    }
    
    SAMPTube_PlayURL(playerid, params);
    return 1;
}

CMD:buscar(playerid, params[]) {
    if (!strlen(params)) {
        SendClientMessage(playerid, -1, "Use: /buscar [nome da música]");
        return 1;
    }
    
    SAMPTube_PlaySearch(playerid, params);
    return 1;
}

CMD:pausar(playerid) {
    SAMPTube_Pause(playerid);
    SendClientMessage(playerid, -1, "Música pausada!");
    return 1;
}

CMD:resumir(playerid) {
    SAMPTube_Resume(playerid);
    SendClientMessage(playerid, -1, "Música resumida!");
    return 1;
}
```

### Comandos de Playlist

```pawn
CMD:adicionar(playerid, params[]) {
    if (!strlen(params)) {
        SendClientMessage(playerid, -1, "Use: /adicionar [URL]");
        return 1;
    }
    
    SAMPTube_AddToPlaylist(playerid, params);
    return 1;
}

CMD:addbusca(playerid, params[]) {
    if (!strlen(params)) {
        SendClientMessage(playerid, -1, "Use: /addbusca [nome]");
        return 1;
    }
    
    SAMPTube_AddSearchToPlaylist(playerid, params);
    return 1;
}

CMD:playlist(playerid) {
    SAMPTube_PlayPlaylist(playerid);
    return 1;
}

CMD:proxima(playerid) {
    SAMPTube_Next(playerid);
    return 1;
}

CMD:anterior(playerid) {
    SAMPTube_Previous(playerid);
    return 1;
}
```

### Comandos de Soundbox

```pawn
CMD:criarcaixa(playerid, params[]) {
    new Float:x, Float:y, Float:z, Float:groundZ;
    GetPlayerPos(playerid, x, y, z);
    
    // Usar MapAndreas para pegar Z do chão
    if (MapAndreas_FindZ_For2DCoord(x, y, groundZ)) {
        z = groundZ + 0.5;
    }
    
    new boxid = SAMPTube_CreateSoundBox(params, x, y, z, 50.0, 1);
    
    if (boxid != -1) {
        new msg[128];
        format(msg, sizeof(msg), "Caixa de som criada! ID: %d", boxid);
        SendClientMessage(playerid, -1, msg);
    }
    return 1;
}

CMD:caixaplaylist(playerid) {
    new count = SAMPTube_GetPlaylistCount(playerid);
    
    if (count == 0) {
        SendClientMessage(playerid, -1, "Sua playlist está vazia!");
        return 1;
    }
    
    new Float:x, Float:y, Float:z, Float:groundZ;
    GetPlayerPos(playerid, x, y, z);
    MapAndreas_FindZ_For2DCoord(x, y, groundZ);
    
    new boxid = SAMPTube_CreateBoxPlaylist(playerid, x, y, groundZ + 0.5, 50.0, 1);
    
    new msg[128];
    format(msg, sizeof(msg), "Caixa com playlist criada! ID: %d | Músicas: %d", boxid, count);
    SendClientMessage(playerid, -1, msg);
    return 1;
}

CMD:proximacaixa(playerid, params[]) {
    new boxid = strval(params);
    
    if (SAMPTube_SoundBoxNext(boxid)) {
        SendClientMessage(playerid, -1, "Próxima música iniciada!");
    } else {
        SendClientMessage(playerid, -1, "ID inválido ou sem playlist!");
    }
    return 1;
}
```

### Comandos de Veículo

```pawn
CMD:somveiculo(playerid) {
    if (!IsPlayerInAnyVehicle(playerid)) {
        SendClientMessage(playerid, -1, "Você precisa estar em um veículo!");
        return 1;
    }
    
    new count = SAMPTube_GetPlaylistCount(playerid);
    if (count == 0) {
        SendClientMessage(playerid, -1, "Sua playlist está vazia!");
        return 1;
    }
    
    new vehicleid = GetPlayerVehicleID(playerid);
    SAMPTube_CreateVehiclePlaylist(playerid, vehicleid);
    
    SendClientMessage(playerid, -1, "Som do veículo ativado! Passageiros também ouvirão!");
    return 1;
}

CMD:proximaveiculo(playerid) {
    if (!IsPlayerInAnyVehicle(playerid)) {
        SendClientMessage(playerid, -1, "Você precisa estar em um veículo!");
        return 1;
    }
    
    new vehicleid = GetPlayerVehicleID(playerid);
    
    if (SAMPTube_VehicleNext(vehicleid)) {
        SendClientMessage(playerid, -1, "Próxima música!");
    } else {
        SendClientMessage(playerid, -1, "Veículo sem som ativo!");
    }
    return 1;
}

CMD:removersomveiculo(playerid) {
    if (!IsPlayerInAnyVehicle(playerid)) {
        SendClientMessage(playerid, -1, "Você precisa estar em um veículo!");
        return 1;
    }
    
    new vehicleid = GetPlayerVehicleID(playerid);
    
    if (SAMPTube_DestroyVehicleSound(vehicleid)) {
        SendClientMessage(playerid, -1, "Som do veículo removido!");
    } else {
        SendClientMessage(playerid, -1, "Veículo não tem som ativo!");
    }
    return 1;
}
```

---

## ⚙️ Configuração

### Endpoint da API

Por padrão, usa `http://127.0.0.1:8000`. Para mudar:

```pawn
#define SAMPTUBE_API_ENDPOINT "http://seu-servidor.com:8000"
#include <samptube>
```

### Limites Configuráveis

```pawn
// No samptube.inc (linha ~53-54)
#define MAX_SOUNDBOXES 100        // Máximo de soundboxes
#define MAX_VEHICLE_SOUNDS 100    // Máximo de veículos com som
```

---

## 🔧 Como Funciona

### Sincronização Automática

Quando um jogador entra em uma área de soundbox ou veículo com som:

1. Calcula tempo decorrido: `elapsed = (GetTickCount() - start_time) / 1000`
2. Adiciona `?start=` na URL: `/api/audio/music.mp3?start=30`
3. A API faz seek para o byte correto (128kbps ≈ 16000 bytes/s)
4. Player ouve música sincronizada com outros

### Sistema de Callbacks

```pawn
// Música começou
public OnSAMPTubeTrackStart(playerid, const title[], duration) {
    // title = título da música
    // duration = duração em milissegundos
}

// Música terminou
public OnSAMPTubeTrackEnd(playerid) {
    // Chamado automaticamente
    // Playlist avança automaticamente se ativa
}

// Erro na conversão
public OnSAMPTubeError(playerid, const error[]) {
    // error = mensagem de erro
}
```

### Hooks Automáticos

A biblioteca implementa hooks para:

- `OnPlayerDisconnect`: Limpa dados do player
- `OnPlayerEnterDynamicArea`: Toca soundbox automaticamente
- `OnPlayerLeaveDynamicArea`: Para música ao sair da área
- `OnPlayerStateChange`: Controla som de veículos

**Importante**: Esses hooks usam ALS (Advanced Library System). Se você usar esses callbacks no seu gamemode, eles funcionarão normalmente.

---

## 📊 Estrutura Técnica

### Dados Armazenados por Player

```pawn
enum E_SAMPTUBE_PLAYER {
    bool:E_PLAYER_PLAYING,          // Está tocando?
    bool:E_PLAYER_PAUSED,           // Está pausado?
    bool:E_PLAYER_IS_PLAYLIST,      // Está tocando playlist?
    E_PLAYER_CURRENT_INDEX,         // Índice atual na playlist
    E_PLAYER_TIMER,                 // Timer da música
    E_PLAYER_PAUSE_TIME,            // Tempo quando pausou
    E_PLAYER_START_TIME,            // Timestamp de início
    E_PLAYER_CURRENT_TRACK[E_SAMPTUBE_TRACK],
    Map:E_PLAYER_PLAYLIST           // Playlist (pawn-map)
}
```

### Dados de Soundbox

```pawn
enum E_SAMPTUBE_SOUNDBOX {
    bool:E_SOUNDBOX_ACTIVE,         // Ativa?
    E_SOUNDBOX_OBJECTID,            // ID do objeto (boombox)
    E_SOUNDBOX_AREAID,              // ID da área dinâmica
    Float:E_SOUNDBOX_X/Y/Z/RANGE,   // Posição e alcance
    E_SOUNDBOX_URL[256],            // URL atual
    E_SOUNDBOX_START_TIME,          // Timestamp
    bool:E_SOUNDBOX_USE_POS,        // Som 3D?
    bool:E_SOUNDBOX_IS_PLAYLIST,    // É playlist?
    Map:E_SOUNDBOX_PLAYLIST,        // Dados da playlist
    E_SOUNDBOX_CURRENT_INDEX,       // Índice atual
    E_SOUNDBOX_TIMER                // Timer
}
```

### Dados de Som em Veículo

```pawn
enum E_SAMPTUBE_VEHICLE {
    bool:E_VEHICLE_ACTIVE,          // Ativo?
    E_VEHICLE_ID,                   // ID do veículo
    E_VEHICLE_URL[256],             // URL atual
    E_VEHICLE_START_TIME,           // Timestamp
    bool:E_VEHICLE_IS_PLAYLIST,     // É playlist?
    Map:E_VEHICLE_PLAYLIST,         // Dados da playlist
    E_VEHICLE_CURRENT_INDEX,        // Índice atual
    E_VEHICLE_TIMER                 // Timer
}
```

---

## 🐛 Troubleshooting

### Música não toca

1. Verifique se a API está rodando: `http://127.0.0.1:8000`
2. Teste a conversão: `curl -X POST http://127.0.0.1:8000/api/convert -d '{"query":"test"}'`
3. Verifique console do servidor para erros

### Soundbox não funciona

1. Certifique-se que o plugin streamer está carregado
2. Verifique se MapAndreas está inicializado
3. Confirme que a área dinâmica foi criada: `IsValidDynamicArea(areaid)`

### Som não sincroniza

1. Verifique se `GetTickCount()` está funcionando
2. Confirme que a API suporta `?start=` parameter
3. Teste manualmente: `http://127.0.0.1:8000/api/audio/file.mp3?start=30`

### Warnings de compilação

```
symbol "SAMPTube_X" is truncated to 31 characters
```

Isso é normal. O PAWN tem limite de 31 caracteres para nomes. A biblioteca usa nomes curtos internamente para evitar isso.

---

## 📝 Licença

Este projeto é open-source sob a licença MIT.

---

## 🤝 Contribuindo

Contribuições são bem-vindas!

1. Fork o projeto
2. Crie uma branch (`git checkout -b feature/NovaFuncionalidade`)
3. Commit suas mudanças (`git commit -m 'Add: Nova funcionalidade'`)
4. Push para a branch (`git push origin feature/NovaFuncionalidade`)
5. Abra um Pull Request

---

## 🙏 Créditos

- **API Laravel**: [samptube-laravel](../samptube-laravel)
- **pawn-requests**: HTTP client
- **pawn-map**: Key-value storage
- **streamer**: Dynamic areas/objects
- **MapAndreas**: Ground Z detection
- **SA-MP Team**: San Andreas Multiplayer

---

## 📞 Suporte

- **Issues**: [GitHub Issues](https://github.com/xBruno1000x/samptube-pawn/issues)
- **API Backend**: [samptube-laravel](../samptube-laravel)
- **Exemplo Completo**: Ver `exemplo.pwn`

---

Desenvolvido com ❤️ para a comunidade SA-MP
