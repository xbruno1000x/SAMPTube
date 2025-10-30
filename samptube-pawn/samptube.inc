/**
 * SAMPTube - YouTube to MP3 API for SA-MP
 * 
 * FunÃ§Ãµes principais:
 * - SAMPTube_PlayURL(playerid, const url[]) - Toca uma Música por URL do YouTube
 * - SAMPTube_PlaySearch(playerid, const query[]) - Busca e toca uma Música
 * - SAMPTube_AddToPlaylist(playerid, const url[]) - Adiciona URL a playlist
 * - SAMPTube_AddSearchToPlaylist(playerid, const query[]) - Busca e adiciona a playlist
 * - SAMPTube_PlayPlaylist(playerid) - Inicia a playlist
 * - SAMPTube_Next(playerid) - Próxima Música da playlist
 * - SAMPTube_Previous(playerid) - Música anterior da playlist
 * - SAMPTube_Pause(playerid) - Pausa a reprodução
 * - SAMPTube_Resume(playerid) - Resume a reprodução
 * - SAMPTube_Stop(playerid) - Para a reprodução
 * - SAMPTube_ClearPlaylist(playerid) - Limpa a playlist
 * 
 * Callbacks:
 * - OnSAMPTubeTrackStart(playerid, const title[], duration) - Quando uma Música começa
 * - OnSAMPTubeTrackEnd(playerid) - Quando uma Música termina
 * - OnSAMPTubeError(playerid, const error[]) - Quando ocorre um erro
 */

#if defined _samptube_included
    #endinput
#endif
#define _samptube_included

#include <a_samp>
#include <requests>
#include <map>

// Configurações
#if !defined SAMPTUBE_API_ENDPOINT
    #define SAMPTUBE_API_ENDPOINT "http://127.0.0.1:8000"
#endif

// Estrutura de dados para cada track
enum E_SAMPTUBE_TRACK {
    E_TRACK_TITLE[128],
    E_TRACK_URL[256],
    E_TRACK_DURATION
}

// Estrutura de dados do player
enum E_SAMPTUBE_PLAYER {
    bool:E_PLAYER_PLAYING,
    bool:E_PLAYER_PAUSED,
    bool:E_PLAYER_IS_PLAYLIST,
    E_PLAYER_CURRENT_INDEX,
    E_PLAYER_TIMER,
    E_PLAYER_PAUSE_TIME,
    E_PLAYER_START_TIME,
    E_PLAYER_CURRENT_TRACK[E_SAMPTUBE_TRACK],
    Map:E_PLAYER_PLAYLIST
}

// Estrutura de dados para soundbox (caixa de som 3D)
#define MAX_SOUNDBOXES 100

enum E_SAMPTUBE_SOUNDBOX {
    bool:E_SOUNDBOX_ACTIVE,
    E_SOUNDBOX_OBJECTID,
    E_SOUNDBOX_AREAID,
    Float:E_SOUNDBOX_X,
    Float:E_SOUNDBOX_Y,
    Float:E_SOUNDBOX_Z,
    Float:E_SOUNDBOX_RANGE,
    E_SOUNDBOX_URL[256],
    E_SOUNDBOX_START_TIME,
    bool:E_SOUNDBOX_USE_POS,
    bool:E_SOUNDBOX_IS_PLAYLIST,
    Map:E_SOUNDBOX_PLAYLIST,
    E_SOUNDBOX_CURRENT_INDEX,
    E_SOUNDBOX_TIMER
}

// Estrutura de dados para som em veículos
#define MAX_VEHICLE_SOUNDS 100

enum E_SAMPTUBE_VEHICLE {
    bool:E_VEHICLE_ACTIVE,
    E_VEHICLE_ID,
    E_VEHICLE_URL[256],
    E_VEHICLE_START_TIME,
    bool:E_VEHICLE_IS_PLAYLIST,
    Map:E_VEHICLE_PLAYLIST,
    E_VEHICLE_CURRENT_INDEX,
    E_VEHICLE_TIMER
}

// Dados dos players
static g_PlayerData[MAX_PLAYERS][E_SAMPTUBE_PLAYER];
static RequestsClient:g_SAMPTubeClient = RequestsClient:-1;

// Dados das soundboxes
static g_SoundBoxes[MAX_SOUNDBOXES][E_SAMPTUBE_SOUNDBOX];

// Dados dos veículos com som
static g_VehicleSounds[MAX_VEHICLE_SOUNDS][E_SAMPTUBE_VEHICLE];

// Forwards
forward OnSAMPTubeTrackStart(playerid, const title[], duration);
forward OnSAMPTubeTrackEnd(playerid);
forward OnSAMPTubeError(playerid, const error[]);

// Internos
forward _SAMPTube_OnConvertResponse(Request:id, E_HTTP_STATUS:status, Node:node);
forward _SAMPTube_OnTrackEnd(playerid);
forward _SAMPTube_OnSoundBoxTrackEnd(soundboxid);
forward _SAMPTube_OnVehicleTrackEnd(vehicleid);

/**
 * Inicializa o SAMPTube (chamado automaticamente)
 */
stock SAMPTube_Init() {
    if (g_SAMPTubeClient == RequestsClient:-1) {
        g_SAMPTubeClient = RequestsClient(SAMPTUBE_API_ENDPOINT);
        
        for (new i = 0; i < MAX_PLAYERS; i++) {
            g_PlayerData[i][E_PLAYER_PLAYING] = false;
            g_PlayerData[i][E_PLAYER_PAUSED] = false;
            g_PlayerData[i][E_PLAYER_IS_PLAYLIST] = false;
            g_PlayerData[i][E_PLAYER_CURRENT_INDEX] = 0;
            g_PlayerData[i][E_PLAYER_TIMER] = -1;
            g_PlayerData[i][E_PLAYER_PAUSE_TIME] = 0;
            g_PlayerData[i][E_PLAYER_START_TIME] = 0;
            g_PlayerData[i][E_PLAYER_PLAYLIST] = MAP_NULL;
        }
        
        for (new i = 0; i < MAX_SOUNDBOXES; i++) {
            g_SoundBoxes[i][E_SOUNDBOX_ACTIVE] = false;
            g_SoundBoxes[i][E_SOUNDBOX_IS_PLAYLIST] = false;
            g_SoundBoxes[i][E_SOUNDBOX_PLAYLIST] = MAP_NULL;
            g_SoundBoxes[i][E_SOUNDBOX_CURRENT_INDEX] = 0;
            g_SoundBoxes[i][E_SOUNDBOX_TIMER] = -1;
        }
        
        for (new i = 0; i < MAX_VEHICLE_SOUNDS; i++) {
            g_VehicleSounds[i][E_VEHICLE_ACTIVE] = false;
            g_VehicleSounds[i][E_VEHICLE_IS_PLAYLIST] = false;
            g_VehicleSounds[i][E_VEHICLE_PLAYLIST] = MAP_NULL;
            g_VehicleSounds[i][E_VEHICLE_CURRENT_INDEX] = 0;
            g_VehicleSounds[i][E_VEHICLE_TIMER] = -1;
        }
    }
}

/**
 * Toca uma Música por URL do YouTube
 */
stock SAMPTube_PlayURL(playerid, const url[]) {
    if (!IsPlayerConnected(playerid)) return 0;
    
    SAMPTube_Init();
    SAMPTube_Stop(playerid);
    
    new Node:body = JsonObject(
        "url", JsonString(url)
    );
    
    new Request:req = RequestJSON(
        g_SAMPTubeClient,
        "/api/convert",
        HTTP_METHOD_POST,
        "_SAMPTube_OnConvertResponse",
        body
    );
    
    SetPVarInt(playerid, "SAMPTube_RequestID", _:req);
    SetPVarInt(playerid, "SAMPTube_IsPlaylist", 0);
    
    return 1;
}

/**
 * Busca e toca uma Música
 */
stock SAMPTube_PlaySearch(playerid, const query[]) {
    if (!IsPlayerConnected(playerid)) return 0;
    
    SAMPTube_Init();
    SAMPTube_Stop(playerid);
    
    new Node:body = JsonObject(
        "query", JsonString(query)
    );
    
    new Request:req = RequestJSON(
        g_SAMPTubeClient,
        "/api/convert",
        HTTP_METHOD_POST,
        "_SAMPTube_OnConvertResponse",
        body
    );
    
    SetPVarInt(playerid, "SAMPTube_RequestID", _:req);
    SetPVarInt(playerid, "SAMPTube_IsPlaylist", 0);
    
    return 1;
}

/**
 * Adiciona uma URL a playlist do jogador
 */
stock SAMPTube_AddToPlaylist(playerid, const url[]) {
    if (!IsPlayerConnected(playerid)) return 0;
    
    SAMPTube_Init();
    
    new Node:body = JsonObject(
        "url", JsonString(url)
    );
    
    new Request:req = RequestJSON(
        g_SAMPTubeClient,
        "/api/convert",
        HTTP_METHOD_POST,
        "_SAMPTube_OnConvertResponse",
        body
    );
    
    SetPVarInt(playerid, "SAMPTube_RequestID", _:req);
    SetPVarInt(playerid, "SAMPTube_AddToPlaylist", 1);
    
    return 1;
}

/**
 * Busca e adiciona à playlist
 */
stock SAMPTube_AddSearchToPlaylist(playerid, const query[]) {
    if (!IsPlayerConnected(playerid)) return 0;
    
    SAMPTube_Init();
    
    new Node:body = JsonObject(
        "query", JsonString(query)
    );
    
    new Request:req = RequestJSON(
        g_SAMPTubeClient,
        "/api/convert",
        HTTP_METHOD_POST,
        "_SAMPTube_OnConvertResponse",
        body
    );
    
    SetPVarInt(playerid, "SAMPTube_RequestID", _:req);
    SetPVarInt(playerid, "SAMPTube_AddToPlaylist", 1);
    
    return 1;
}

/**
 * Inicia a reprodução da playlist
 */
stock SAMPTube_PlayPlaylist(playerid) {
    if (!IsPlayerConnected(playerid)) return 0;
    
    new count = MAP_count(g_PlayerData[playerid][E_PLAYER_PLAYLIST]);
    if (count == 0) {
        CallRemoteFunction("OnSAMPTubeError", "is", playerid, "Playlist vazia");
        return 0;
    }
    
    g_PlayerData[playerid][E_PLAYER_IS_PLAYLIST] = true;
    g_PlayerData[playerid][E_PLAYER_CURRENT_INDEX] = 0;
    
    _SAMPTube_PlayTrackAtIndex(playerid, 0);
    
    return 1;
}

/**
 * Próxima Música da playlist
 */
stock SAMPTube_Next(playerid) {
    if (!IsPlayerConnected(playerid)) return 0;
    if (!g_PlayerData[playerid][E_PLAYER_IS_PLAYLIST]) return 0;
    
    new count = MAP_count(g_PlayerData[playerid][E_PLAYER_PLAYLIST]);
    new nextIndex = g_PlayerData[playerid][E_PLAYER_CURRENT_INDEX] + 1;
    
    if (nextIndex >= count) {
        SAMPTube_Stop(playerid);
        CallRemoteFunction("OnSAMPTubeTrackEnd", "i", playerid);
        return 0;
    }
    
    g_PlayerData[playerid][E_PLAYER_CURRENT_INDEX] = nextIndex;
    _SAMPTube_PlayTrackAtIndex(playerid, nextIndex);
    
    return 1;
}

/**
 * Música anterior da playlist
 */
stock SAMPTube_Previous(playerid) {
    if (!IsPlayerConnected(playerid)) return 0;
    if (!g_PlayerData[playerid][E_PLAYER_IS_PLAYLIST]) return 0;
    
    new prevIndex = g_PlayerData[playerid][E_PLAYER_CURRENT_INDEX] - 1;
    
    if (prevIndex < 0) {
        prevIndex = 0;
    }
    
    g_PlayerData[playerid][E_PLAYER_CURRENT_INDEX] = prevIndex;
    _SAMPTube_PlayTrackAtIndex(playerid, prevIndex);
    
    return 1;
}

/**
 * Para a reprodução
 */
stock SAMPTube_Stop(playerid) {
    if (!IsPlayerConnected(playerid)) return 0;
    
    StopAudioStreamForPlayer(playerid);
    
    if (g_PlayerData[playerid][E_PLAYER_TIMER] != -1) {
        KillTimer(g_PlayerData[playerid][E_PLAYER_TIMER]);
        g_PlayerData[playerid][E_PLAYER_TIMER] = -1;
    }
    
    g_PlayerData[playerid][E_PLAYER_PLAYING] = false;
    g_PlayerData[playerid][E_PLAYER_PAUSED] = false;
    g_PlayerData[playerid][E_PLAYER_IS_PLAYLIST] = false;
    g_PlayerData[playerid][E_PLAYER_PAUSE_TIME] = 0;
    
    return 1;
}

/**
 * Pausa a reprodução
 */
stock SAMPTube_Pause(playerid) {
    if (!IsPlayerConnected(playerid)) return 0;
    if (!g_PlayerData[playerid][E_PLAYER_PLAYING]) return 0;
    if (g_PlayerData[playerid][E_PLAYER_PAUSED]) return 0;
    
    // Calcular tempo decorrido
    new elapsed = GetTickCount() - g_PlayerData[playerid][E_PLAYER_START_TIME];
    g_PlayerData[playerid][E_PLAYER_PAUSE_TIME] = elapsed;
    
    // Parar o stream
    StopAudioStreamForPlayer(playerid);
    
    // Matar o timer
    if (g_PlayerData[playerid][E_PLAYER_TIMER] != -1) {
        KillTimer(g_PlayerData[playerid][E_PLAYER_TIMER]);
        g_PlayerData[playerid][E_PLAYER_TIMER] = -1;
    }
    
    g_PlayerData[playerid][E_PLAYER_PAUSED] = true;
    g_PlayerData[playerid][E_PLAYER_PLAYING] = false;
    
    return 1;
}

/**
 * Resume a reprodução
 */
stock SAMPTube_Resume(playerid) {
    if (!IsPlayerConnected(playerid)) return 0;
    if (!g_PlayerData[playerid][E_PLAYER_PAUSED]) return 0;
    
    // Criar URL com parâmetro de início (em segundos)
    new url[512];
    format(url, sizeof(url), "%s?start=%d", 
        g_PlayerData[playerid][E_PLAYER_CURRENT_TRACK][E_TRACK_URL], 
        g_PlayerData[playerid][E_PLAYER_PAUSE_TIME] / 1000
    );
    
    // Tocar do ponto pausado
    PlayAudioStreamForPlayer(playerid, url);
    g_PlayerData[playerid][E_PLAYER_START_TIME] = GetTickCount() - g_PlayerData[playerid][E_PLAYER_PAUSE_TIME];
    g_PlayerData[playerid][E_PLAYER_PLAYING] = true;
    g_PlayerData[playerid][E_PLAYER_PAUSED] = false;
    
    // Recalcular tempo restante para o timer (apenas para playlists)
    if (g_PlayerData[playerid][E_PLAYER_IS_PLAYLIST]) {
        new timeRemaining = g_PlayerData[playerid][E_PLAYER_CURRENT_TRACK][E_TRACK_DURATION] - g_PlayerData[playerid][E_PLAYER_PAUSE_TIME];
        if (timeRemaining > 0) {
            g_PlayerData[playerid][E_PLAYER_TIMER] = SetTimerEx("_SAMPTube_OnTrackEnd", timeRemaining, false, "i", playerid);
        }
    }
    
    return 1;
}

/**
 * Limpa a playlist do jogador
 */
stock SAMPTube_ClearPlaylist(playerid) {
    if (!IsPlayerConnected(playerid)) return 0;
    
    MAP_clear(g_PlayerData[playerid][E_PLAYER_PLAYLIST]);
    g_PlayerData[playerid][E_PLAYER_CURRENT_INDEX] = 0;
    
    return 1;
}

/**
 * Retorna o número de Músicas na playlist
 */
stock SAMPTube_GetPlaylistCount(playerid) {
    if (!IsPlayerConnected(playerid)) return 0;
    return MAP_count(g_PlayerData[playerid][E_PLAYER_PLAYLIST]);
}

/**
 * Lista todas as músicas da playlist
 * Retorna o número de músicas listadas
 */
stock SAMPTube_ListPlaylist(playerid) {
    if (!IsPlayerConnected(playerid)) return 0;
    
    new count = MAP_count(g_PlayerData[playerid][E_PLAYER_PLAYLIST]);
    
    if (count == 0) {
        SendClientMessage(playerid, 0xFFFF00FF, "{FFFF00}[SAMPTube] {FFFFFF}Playlist vazia!");
        return 0;
    }
    
    SendClientMessage(playerid, 0x00FF00FF, "{00FF00}[SAMPTube] {FFFFFF}=== Sua Playlist ===");
    
    new track_data[385], title[128];
    for (new i = 0; i < count; i++) {
        new Pointer:ptr = MAP_get_ptr_val(g_PlayerData[playerid][E_PLAYER_PLAYLIST], i);
        
        if (ptr != MEM_NULLPTR) {
            MEM_get_arr(ptr, _, track_data, 385);
            
            // Extrair título (está em track_data[0] até track_data[127])
            for (new j = 0; j < 128; j++) {
                title[j] = track_data[j];
            }
            
            new msg[160];
            format(msg, sizeof(msg), "{FFFF00}%d. {FFFFFF}%s", i + 1, title);
            SendClientMessage(playerid, 0xFFFFFFFF, msg);
        }
    }
    
    new footer[128];
    format(footer, sizeof(footer), "{00FF00}[SAMPTube] {FFFFFF}Total: {FFFF00}%d {FFFFFF}música(s)", count);
    SendClientMessage(playerid, 0x00FF00FF, footer);
    
    return count;
}

/**
 * Remove uma música da playlist por índice (ID)
 * índice começa em 0
 */
stock SAMPTube_RemoveByIndex(playerid, index) {
    if (!IsPlayerConnected(playerid)) return 0;
    
    new count = MAP_count(g_PlayerData[playerid][E_PLAYER_PLAYLIST]);
    
    if (index < 0 || index >= count) {
        SendClientMessage(playerid, 0xFF0000FF, "{FF0000}[SAMPTube] {FFFFFF}Índice inválido!");
        return 0;
    }
    
    // Obter título antes de remover (para feedback)
    new track_data[385], title[128];
    new Pointer:ptr = MAP_get_ptr_val(g_PlayerData[playerid][E_PLAYER_PLAYLIST], index);
    
    if (ptr != MEM_NULLPTR) {
        MEM_get_arr(ptr, _, track_data, 385);
        for (new j = 0; j < 128; j++) {
            title[j] = track_data[j];
        }
    }
    
    // Remover da playlist
    MAP_remove_val(g_PlayerData[playerid][E_PLAYER_PLAYLIST], index);
    
    // Reorganizar índices (mover todas as músicas posteriores uma posição para trás)
    for (new i = index; i < count - 1; i++) {
        new Pointer:nextPtr = MAP_get_ptr_val(g_PlayerData[playerid][E_PLAYER_PLAYLIST], i + 1);
        if (nextPtr != MEM_NULLPTR) {
            new next_track[385];
            MEM_get_arr(nextPtr, _, next_track, 385);
            MAP_remove_val(g_PlayerData[playerid][E_PLAYER_PLAYLIST], i + 1);
            MAP_insert_val_arr(g_PlayerData[playerid][E_PLAYER_PLAYLIST], i, next_track, 385);
        }
    }
    
    // Ajustar índice atual se necessário
    if (g_PlayerData[playerid][E_PLAYER_IS_PLAYLIST]) {
        if (g_PlayerData[playerid][E_PLAYER_CURRENT_INDEX] > index) {
            g_PlayerData[playerid][E_PLAYER_CURRENT_INDEX]--;
        }
    }
    
    new msg[192];
    format(msg, sizeof(msg), "{00FF00}[SAMPTube] {FFFFFF}Removido: {FFFF00}%s", title);
    SendClientMessage(playerid, 0x00FF00FF, msg);
    
    return 1;
}

/**
 * Remove uma música da playlist por nome (busca parcial)
 * Remove a primeira ocorrência encontrada
 */
stock SAMPTube_RemoveByName(playerid, const searchName[]) {
    if (!IsPlayerConnected(playerid)) return 0;
    
    new count = MAP_count(g_PlayerData[playerid][E_PLAYER_PLAYLIST]);
    
    if (count == 0) {
        SendClientMessage(playerid, 0xFF0000FF, "{FF0000}[SAMPTube] {FFFFFF}Playlist vazia!");
        return 0;
    }
    
    new track_data[385], title[128];
    new bool:found = false;
    new foundIndex = -1;
    
    // Buscar música pelo nome (case-insensitive, busca parcial)
    for (new i = 0; i < count; i++) {
        new Pointer:ptr = MAP_get_ptr_val(g_PlayerData[playerid][E_PLAYER_PLAYLIST], i);
        
        if (ptr != MEM_NULLPTR) {
            MEM_get_arr(ptr, _, track_data, 385);
            
            // Extrair título
            for (new j = 0; j < 128; j++) {
                title[j] = track_data[j];
            }
            
            // Verificar se o nome buscado está contido no título (case-insensitive)
            if (strfind(title, searchName, true) != -1) {
                found = true;
                foundIndex = i;
                break;
            }
        }
    }
    
    if (!found) {
        new msg[192];
        format(msg, sizeof(msg), "{FF0000}[SAMPTube] {FFFFFF}Música não encontrada: {FFFF00}%s", searchName);
        SendClientMessage(playerid, 0xFF0000FF, msg);
        return 0;
    }
    
    // Remover usando a função por índice
    return SAMPTube_RemoveByIndex(playerid, foundIndex);
}

/**
 * Verifica se o player está tocando Música
 */
stock bool:SAMPTube_IsPlaying(playerid) {
    if (!IsPlayerConnected(playerid)) return false;
    return g_PlayerData[playerid][E_PLAYER_PLAYING];
}

/**
 * Verifica se o player está pausado
 */
stock bool:SAMPTube_IsPaused(playerid) {
    if (!IsPlayerConnected(playerid)) return false;
    return g_PlayerData[playerid][E_PLAYER_PAUSED];
}

// === FUNÇÕES DE SOUNDBOX (CAIXA DE SOM 3D) ===

/**
 * Cria uma soundbox (caixa de som 3D) que toca música em uma área
 * Retorna o ID da soundbox ou -1 em caso de erro
 */
stock SAMPTube_CreateSoundBox(const url[], Float:x, Float:y, Float:z, Float:range = 8.0, usepos = 1) {
    // Buscar primeiro slot livre
    new id = -1;
    for (new i = 0; i < MAX_SOUNDBOXES; i++) {
        if (!g_SoundBoxes[i][E_SOUNDBOX_ACTIVE]) {
            id = i;
            break;
        }
    }
    
    if (id == -1) {
        return -1; // Sem slots disponíveis
    }
    
    // Criar objeto de caixa de som (boombox)
    g_SoundBoxes[id][E_SOUNDBOX_OBJECTID] = CreateDynamicObject(2226, x, y, z, 0.0, 0.0, 0.0);
    
    // Criar área dinâmica
    g_SoundBoxes[id][E_SOUNDBOX_AREAID] = CreateDynamicCircle(x, y, range);
    
    // Salvar dados
    g_SoundBoxes[id][E_SOUNDBOX_X] = x;
    g_SoundBoxes[id][E_SOUNDBOX_Y] = y;
    g_SoundBoxes[id][E_SOUNDBOX_Z] = z;
    g_SoundBoxes[id][E_SOUNDBOX_RANGE] = range;
    g_SoundBoxes[id][E_SOUNDBOX_URL][0] = EOS;
    strcat(g_SoundBoxes[id][E_SOUNDBOX_URL], url, 256);
    g_SoundBoxes[id][E_SOUNDBOX_START_TIME] = GetTickCount();
    g_SoundBoxes[id][E_SOUNDBOX_USE_POS] = bool:usepos;
    g_SoundBoxes[id][E_SOUNDBOX_ACTIVE] = true;
    
    return id;
}

/**
 * Destrói uma soundbox
 */
stock SAMPTube_DestroySoundBox(soundboxid) {
    if (soundboxid < 0 || soundboxid >= MAX_SOUNDBOXES) return 0;
    if (!g_SoundBoxes[soundboxid][E_SOUNDBOX_ACTIVE]) return 0;
    
    // Destruir objeto
    DestroyDynamicObject(g_SoundBoxes[soundboxid][E_SOUNDBOX_OBJECTID]);
    
    // Destruir área
    DestroyDynamicArea(g_SoundBoxes[soundboxid][E_SOUNDBOX_AREAID]);
    
    // Matar timer se existir
    if (g_SoundBoxes[soundboxid][E_SOUNDBOX_TIMER] != -1) {
        KillTimer(g_SoundBoxes[soundboxid][E_SOUNDBOX_TIMER]);
        g_SoundBoxes[soundboxid][E_SOUNDBOX_TIMER] = -1;
    }
    
    // Limpar playlist se existir
    if (g_SoundBoxes[soundboxid][E_SOUNDBOX_IS_PLAYLIST]) {
        MAP_clear(g_SoundBoxes[soundboxid][E_SOUNDBOX_PLAYLIST]);
    }
    
    // Parar música para todos que estão na área
    for (new i = 0; i < MAX_PLAYERS; i++) {
        if (IsPlayerConnected(i) && IsPlayerInDynamicArea(i, g_SoundBoxes[soundboxid][E_SOUNDBOX_AREAID])) {
            StopAudioStreamForPlayer(i);
        }
    }
    
    g_SoundBoxes[soundboxid][E_SOUNDBOX_ACTIVE] = false;
    
    return 1;
}

/**
 * Obtém ID da soundbox pela área
 */
stock SAMPTube_GetSoundBoxByArea(areaid) {
    for (new i = 0; i < MAX_SOUNDBOXES; i++) {
        if (g_SoundBoxes[i][E_SOUNDBOX_ACTIVE] && g_SoundBoxes[i][E_SOUNDBOX_AREAID] == areaid) {
            return i;
        }
    }
    return -1;
}

/**
 * Toca a música da soundbox para um player (com sincronização)
 */
stock SAMPTube_PlaySoundBoxForPlayer(playerid, soundboxid) {
    if (soundboxid < 0 || soundboxid >= MAX_SOUNDBOXES) return 0;
    if (!g_SoundBoxes[soundboxid][E_SOUNDBOX_ACTIVE]) return 0;
    if (!IsPlayerConnected(playerid)) return 0;
    
    new url[512];
    format(url, sizeof(url), "%s", g_SoundBoxes[soundboxid][E_SOUNDBOX_URL]);
    
    // Calcular tempo decorrido desde o início (sincronização)
    new elapsed = (GetTickCount() - g_SoundBoxes[soundboxid][E_SOUNDBOX_START_TIME]) / 1000;
    
    // Adicionar parâmetro de start para sincronizar
    if (elapsed > 0) {
        format(url, sizeof(url), "%s?start=%d", g_SoundBoxes[soundboxid][E_SOUNDBOX_URL], elapsed);
    }
    
    // Tocar com ou sem posição 3D
    if (g_SoundBoxes[soundboxid][E_SOUNDBOX_USE_POS]) {
        PlayAudioStreamForPlayer(
            playerid, 
            url, 
            g_SoundBoxes[soundboxid][E_SOUNDBOX_X],
            g_SoundBoxes[soundboxid][E_SOUNDBOX_Y],
            g_SoundBoxes[soundboxid][E_SOUNDBOX_Z],
            g_SoundBoxes[soundboxid][E_SOUNDBOX_RANGE],
            1
        );
    } else {
        PlayAudioStreamForPlayer(playerid, url);
    }
    
    return 1;
}

/**
 * Cria uma soundbox com playlist de um player
 */
stock SAMPTube_CreateBoxPlaylist(playerid, Float:x, Float:y, Float:z, Float:range = 8.0, usepos = 1) {
    if (!IsPlayerConnected(playerid)) return -1;
    
    new count = MAP_count(g_PlayerData[playerid][E_PLAYER_PLAYLIST]);
    if (count == 0) {
        SendClientMessage(playerid, 0xFF0000FF, "{FF0000}[SAMPTube] {FFFFFF}Sua playlist está vazia!");
        return -1;
    }
    
    // Buscar primeiro slot livre
    new id = -1;
    for (new i = 0; i < MAX_SOUNDBOXES; i++) {
        if (!g_SoundBoxes[i][E_SOUNDBOX_ACTIVE]) {
            id = i;
            break;
        }
    }
    
    if (id == -1) {
        return -1; // Sem slots disponíveis
    }
    
    // Criar objeto de caixa de som (boombox)
    g_SoundBoxes[id][E_SOUNDBOX_OBJECTID] = CreateDynamicObject(2226, x, y, z, 0.0, 0.0, 0.0);
    
    // Criar área dinâmica
    g_SoundBoxes[id][E_SOUNDBOX_AREAID] = CreateDynamicCircle(x, y, range);
    
    // Copiar playlist do player para a soundbox
    g_SoundBoxes[id][E_SOUNDBOX_PLAYLIST] = MAP_NULL;
    
    new track_data[385];
    for (new i = 0; i < count; i++) {
        new Pointer:ptr = MAP_get_ptr_val(g_PlayerData[playerid][E_PLAYER_PLAYLIST], i);
        if (ptr != MEM_NULLPTR) {
            MEM_get_arr(ptr, _, track_data, 385);
            MAP_insert_val_arr(g_SoundBoxes[id][E_SOUNDBOX_PLAYLIST], i, track_data, 385);
        }
    }
    
    // Salvar dados
    g_SoundBoxes[id][E_SOUNDBOX_X] = x;
    g_SoundBoxes[id][E_SOUNDBOX_Y] = y;
    g_SoundBoxes[id][E_SOUNDBOX_Z] = z;
    g_SoundBoxes[id][E_SOUNDBOX_RANGE] = range;
    g_SoundBoxes[id][E_SOUNDBOX_USE_POS] = bool:usepos;
    g_SoundBoxes[id][E_SOUNDBOX_IS_PLAYLIST] = true;
    g_SoundBoxes[id][E_SOUNDBOX_CURRENT_INDEX] = 0;
    g_SoundBoxes[id][E_SOUNDBOX_ACTIVE] = true;
    g_SoundBoxes[id][E_SOUNDBOX_TIMER] = -1;
    
    // Iniciar primeira música
    _STUBE_PlayBoxTrackAtIndex(id, 0);
    
    return id;
}

/**
 * Próxima música da soundbox
 */
stock SAMPTube_SoundBoxNext(soundboxid) {
    if (soundboxid < 0 || soundboxid >= MAX_SOUNDBOXES) return 0;
    if (!g_SoundBoxes[soundboxid][E_SOUNDBOX_ACTIVE]) return 0;
    if (!g_SoundBoxes[soundboxid][E_SOUNDBOX_IS_PLAYLIST]) return 0;
    
    new count = MAP_count(g_SoundBoxes[soundboxid][E_SOUNDBOX_PLAYLIST]);
    new nextIndex = g_SoundBoxes[soundboxid][E_SOUNDBOX_CURRENT_INDEX] + 1;
    
    if (nextIndex >= count) {
        // Voltar para o início (loop)
        nextIndex = 0;
    }
    
    g_SoundBoxes[soundboxid][E_SOUNDBOX_CURRENT_INDEX] = nextIndex;
    _STUBE_PlayBoxTrackAtIndex(soundboxid, nextIndex);
    
    return 1;
}

/**
 * Música anterior da soundbox
 */
stock SAMPTube_SoundBoxPrevious(soundboxid) {
    if (soundboxid < 0 || soundboxid >= MAX_SOUNDBOXES) return 0;
    if (!g_SoundBoxes[soundboxid][E_SOUNDBOX_ACTIVE]) return 0;
    if (!g_SoundBoxes[soundboxid][E_SOUNDBOX_IS_PLAYLIST]) return 0;
    
    new count = MAP_count(g_SoundBoxes[soundboxid][E_SOUNDBOX_PLAYLIST]);
    new prevIndex = g_SoundBoxes[soundboxid][E_SOUNDBOX_CURRENT_INDEX] - 1;
    
    if (prevIndex < 0) {
        // Ir para o final (loop reverso)
        prevIndex = count - 1;
    }
    
    g_SoundBoxes[soundboxid][E_SOUNDBOX_CURRENT_INDEX] = prevIndex;
    _STUBE_PlayBoxTrackAtIndex(soundboxid, prevIndex);
    
    return 1;
}

/**
 * Obtém informações da música atual da soundbox
 */
stock SAMPTube_GetBoxCurrentTrack(soundboxid, title[], maxlen = sizeof(title)) {
    if (soundboxid < 0 || soundboxid >= MAX_SOUNDBOXES) return 0;
    if (!g_SoundBoxes[soundboxid][E_SOUNDBOX_ACTIVE]) return 0;
    
    if (g_SoundBoxes[soundboxid][E_SOUNDBOX_IS_PLAYLIST]) {
        new track_data[385];
        new index = g_SoundBoxes[soundboxid][E_SOUNDBOX_CURRENT_INDEX];
        new Pointer:ptr = MAP_get_ptr_val(g_SoundBoxes[soundboxid][E_SOUNDBOX_PLAYLIST], index);
        
        if (ptr != MEM_NULLPTR) {
            MEM_get_arr(ptr, _, track_data, 385);
            for (new i = 0; i < maxlen && i < 128; i++) {
                title[i] = track_data[i];
            }
            return 1;
        }
    }
    
    return 0;
}

// === FUNÇÕES DE VEÍCULO ===

/**
 * Cria um som em um veículo com URL única
 */
stock SAMPTube_CreateVehicleSound(vehicleid, const url[]) {
    // Buscar primeiro slot livre
    new id = -1;
    for (new i = 0; i < MAX_VEHICLE_SOUNDS; i++) {
        if (!g_VehicleSounds[i][E_VEHICLE_ACTIVE]) {
            id = i;
            break;
        }
    }
    
    if (id == -1) {
        return -1; // Sem slots disponíveis
    }
    
    // Salvar dados
    g_VehicleSounds[id][E_VEHICLE_ID] = vehicleid;
    g_VehicleSounds[id][E_VEHICLE_URL][0] = EOS;
    strcat(g_VehicleSounds[id][E_VEHICLE_URL], url, 256);
    g_VehicleSounds[id][E_VEHICLE_START_TIME] = GetTickCount();
    g_VehicleSounds[id][E_VEHICLE_ACTIVE] = true;
    g_VehicleSounds[id][E_VEHICLE_IS_PLAYLIST] = false;
    
    // Tocar para todos que já estão no veículo
    for (new i = 0; i < MAX_PLAYERS; i++) {
        if (IsPlayerConnected(i) && IsPlayerInVehicle(i, vehicleid)) {
            SAMPTube_PlayVehicleForPlayer(i, vehicleid);
        }
    }
    
    return id;
}

/**
 * Cria um som em veículo com playlist de um player
 */
stock SAMPTube_CreateVehiclePlaylist(playerid, vehicleid) {
    if (!IsPlayerConnected(playerid)) return -1;
    
    new count = MAP_count(g_PlayerData[playerid][E_PLAYER_PLAYLIST]);
    if (count == 0) {
        SendClientMessage(playerid, 0xFF0000FF, "{FF0000}[SAMPTube] {FFFFFF}Sua playlist está vazia!");
        return -1;
    }
    
    // Buscar primeiro slot livre
    new id = -1;
    for (new i = 0; i < MAX_VEHICLE_SOUNDS; i++) {
        if (!g_VehicleSounds[i][E_VEHICLE_ACTIVE]) {
            id = i;
            break;
        }
    }
    
    if (id == -1) {
        return -1; // Sem slots disponíveis
    }
    
    // Copiar playlist do player para o veículo
    g_VehicleSounds[id][E_VEHICLE_PLAYLIST] = MAP_NULL;
    
    new track_data[385];
    for (new i = 0; i < count; i++) {
        new Pointer:ptr = MAP_get_ptr_val(g_PlayerData[playerid][E_PLAYER_PLAYLIST], i);
        if (ptr != MEM_NULLPTR) {
            MEM_get_arr(ptr, _, track_data, 385);
            MAP_insert_val_arr(g_VehicleSounds[id][E_VEHICLE_PLAYLIST], i, track_data, 385);
        }
    }
    
    // Salvar dados
    g_VehicleSounds[id][E_VEHICLE_ID] = vehicleid;
    g_VehicleSounds[id][E_VEHICLE_IS_PLAYLIST] = true;
    g_VehicleSounds[id][E_VEHICLE_CURRENT_INDEX] = 0;
    g_VehicleSounds[id][E_VEHICLE_ACTIVE] = true;
    g_VehicleSounds[id][E_VEHICLE_TIMER] = -1;
    
    // Iniciar primeira música (já toca para todos que estão no veículo)
    _STUBE_PlayVehicleTrackAtIndex(id, 0);
    
    return id;
}

/**
 * Destrói o som de um veículo
 */
stock SAMPTube_DestroyVehicleSound(vehicleid) {
    new id = SAMPTube_GetVehicleSoundID(vehicleid);
    if (id == -1) return 0;
    
    // Parar música para todos os jogadores no veículo
    for (new i = 0; i < MAX_PLAYERS; i++) {
        if (IsPlayerConnected(i) && IsPlayerInVehicle(i, vehicleid)) {
            StopAudioStreamForPlayer(i);
        }
    }
    
    // Limpar playlist se existir
    if (g_VehicleSounds[id][E_VEHICLE_IS_PLAYLIST]) {
        if (g_VehicleSounds[id][E_VEHICLE_TIMER] != -1) {
            KillTimer(g_VehicleSounds[id][E_VEHICLE_TIMER]);
        }
        MAP_clear(g_VehicleSounds[id][E_VEHICLE_PLAYLIST]);
    }
    
    g_VehicleSounds[id][E_VEHICLE_ACTIVE] = false;
    
    return 1;
}

/**
 * Toca o som do veículo para um player
 */
stock SAMPTube_PlayVehicleForPlayer(playerid, vehicleid) {
    new id = SAMPTube_GetVehicleSoundID(vehicleid);
    if (id == -1) return 0;
    
    new url[512];
    format(url, sizeof(url), "%s", g_VehicleSounds[id][E_VEHICLE_URL]);
    
    // Calcular tempo decorrido para sincronização
    new elapsed = (GetTickCount() - g_VehicleSounds[id][E_VEHICLE_START_TIME]) / 1000;
    if (elapsed > 0) {
        format(url, sizeof(url), "%s?start=%d", url, elapsed);
    }
    
    // Tocar sem posição 3D (som normal do player)
    PlayAudioStreamForPlayer(playerid, url);
    
    return 1;
}

/**
 * Próxima música do veículo
 */
stock SAMPTube_VehicleNext(vehicleid) {
    new id = SAMPTube_GetVehicleSoundID(vehicleid);
    if (id == -1) return 0;
    if (!g_VehicleSounds[id][E_VEHICLE_IS_PLAYLIST]) return 0;
    
    new count = MAP_count(g_VehicleSounds[id][E_VEHICLE_PLAYLIST]);
    new nextIndex = g_VehicleSounds[id][E_VEHICLE_CURRENT_INDEX] + 1;
    
    if (nextIndex >= count) {
        // Voltar para o início (loop)
        nextIndex = 0;
    }
    
    g_VehicleSounds[id][E_VEHICLE_CURRENT_INDEX] = nextIndex;
    _STUBE_PlayVehicleTrackAtIndex(id, nextIndex);
    
    return 1;
}

/**
 * Música anterior do veículo
 */
stock SAMPTube_VehiclePrevious(vehicleid) {
    new id = SAMPTube_GetVehicleSoundID(vehicleid);
    if (id == -1) return 0;
    if (!g_VehicleSounds[id][E_VEHICLE_IS_PLAYLIST]) return 0;
    
    new count = MAP_count(g_VehicleSounds[id][E_VEHICLE_PLAYLIST]);
    new prevIndex = g_VehicleSounds[id][E_VEHICLE_CURRENT_INDEX] - 1;
    
    if (prevIndex < 0) {
        // Ir para o final (loop reverso)
        prevIndex = count - 1;
    }
    
    g_VehicleSounds[id][E_VEHICLE_CURRENT_INDEX] = prevIndex;
    _STUBE_PlayVehicleTrackAtIndex(id, prevIndex);
    
    return 1;
}

/**
 * Obtém informações da música atual do veículo
 */
stock SAMPTube_GetVehicleCurrentTrack(vehicleid, title[], maxlen = sizeof(title)) {
    new id = SAMPTube_GetVehicleSoundID(vehicleid);
    if (id == -1) return 0;
    
    if (g_VehicleSounds[id][E_VEHICLE_IS_PLAYLIST]) {
        new track_data[385];
        new index = g_VehicleSounds[id][E_VEHICLE_CURRENT_INDEX];
        new Pointer:ptr = MAP_get_ptr_val(g_VehicleSounds[id][E_VEHICLE_PLAYLIST], index);
        
        if (ptr != MEM_NULLPTR) {
            MEM_get_arr(ptr, _, track_data, 385);
            for (new i = 0; i < maxlen && i < 128; i++) {
                title[i] = track_data[i];
            }
            return 1;
        }
    }
    
    return 0;
}

/**
 * Obtém ID do som do veículo
 */
stock SAMPTube_GetVehicleSoundID(vehicleid) {
    for (new i = 0; i < MAX_VEHICLE_SOUNDS; i++) {
        if (g_VehicleSounds[i][E_VEHICLE_ACTIVE] && g_VehicleSounds[i][E_VEHICLE_ID] == vehicleid) {
            return i;
        }
    }
    return -1;
}

// === FUNÇÕES INTERNAS ===

stock _STUBE_PlayBoxTrackAtIndex(soundboxid, index) {
    new Pointer:ptr = MAP_get_ptr_val(g_SoundBoxes[soundboxid][E_SOUNDBOX_PLAYLIST], index);
    
    if (ptr == MEM_NULLPTR) return 0;
    
    // Buffer temporário sem tag
    new track_data[385];
    MEM_get_arr(ptr, _, track_data, 385);
    
    // Salvar URL atual
    g_SoundBoxes[soundboxid][E_SOUNDBOX_URL][0] = EOS;
    strcat(g_SoundBoxes[soundboxid][E_SOUNDBOX_URL], track_data[128], 256);
    
    // Resetar tempo de início
    g_SoundBoxes[soundboxid][E_SOUNDBOX_START_TIME] = GetTickCount();
    
    // Matar timer anterior se existir
    if (g_SoundBoxes[soundboxid][E_SOUNDBOX_TIMER] != -1) {
        KillTimer(g_SoundBoxes[soundboxid][E_SOUNDBOX_TIMER]);
    }
    
    // Criar timer para próxima música
    new duration = track_data[384];
    g_SoundBoxes[soundboxid][E_SOUNDBOX_TIMER] = SetTimerEx("_SAMPTube_OnSoundBoxTrackEnd", duration, false, "i", soundboxid);
    
    // Tocar para todos os players na área
    for (new i = 0; i < MAX_PLAYERS; i++) {
        if (IsPlayerConnected(i) && IsPlayerInDynamicArea(i, g_SoundBoxes[soundboxid][E_SOUNDBOX_AREAID])) {
            if (g_SoundBoxes[soundboxid][E_SOUNDBOX_USE_POS]) {
                PlayAudioStreamForPlayer(
                    i, 
                    track_data[128], 
                    g_SoundBoxes[soundboxid][E_SOUNDBOX_X],
                    g_SoundBoxes[soundboxid][E_SOUNDBOX_Y],
                    g_SoundBoxes[soundboxid][E_SOUNDBOX_Z],
                    g_SoundBoxes[soundboxid][E_SOUNDBOX_RANGE],
                    1
                );
            } else {
                PlayAudioStreamForPlayer(i, track_data[128]);
            }
        }
    }
    
    return 1;
}

stock _STUBE_PlayVehicleTrackAtIndex(id, index) {
    new Pointer:ptr = MAP_get_ptr_val(g_VehicleSounds[id][E_VEHICLE_PLAYLIST], index);
    
    if (ptr == MEM_NULLPTR) return 0;
    
    // Buffer temporário sem tag
    new track_data[385];
    MEM_get_arr(ptr, _, track_data, 385);
    
    // Salvar URL atual
    g_VehicleSounds[id][E_VEHICLE_URL][0] = EOS;
    strcat(g_VehicleSounds[id][E_VEHICLE_URL], track_data[128], 256);
    
    // Resetar tempo de início
    g_VehicleSounds[id][E_VEHICLE_START_TIME] = GetTickCount();
    
    // Criar timer para próxima música (duração em ms)
    new duration_ms = track_data[384];
    if (g_VehicleSounds[id][E_VEHICLE_TIMER] != -1) {
        KillTimer(g_VehicleSounds[id][E_VEHICLE_TIMER]);
    }
    g_VehicleSounds[id][E_VEHICLE_TIMER] = SetTimerEx("_SAMPTube_OnVehicleTrackEnd", duration_ms, false, "i", g_VehicleSounds[id][E_VEHICLE_ID]);
    
    // Tocar para todos os players no veículo
    new vehicleid = g_VehicleSounds[id][E_VEHICLE_ID];
    for (new i = 0; i < MAX_PLAYERS; i++) {
        if (IsPlayerConnected(i) && IsPlayerInVehicle(i, vehicleid)) {
            PlayAudioStreamForPlayer(i, track_data[128]);
        }
    }
    
    return 1;
}

stock _SAMPTube_PlayTrackAtIndex(playerid, index) {
    new Pointer:ptr = MAP_get_ptr_val(g_PlayerData[playerid][E_PLAYER_PLAYLIST], index);
    
    if (ptr == MEM_NULLPTR) {
        CallRemoteFunction("OnSAMPTubeError", "is", playerid, "Track nÃ£o encontrado na playlist");
        return 0;
    }
    
    // Buffer temporário sem tag (128 + 256 + 1 = 385 células)
    new track_data[385];
    MEM_get_arr(ptr, _, track_data, 385);
    
    // Salvar track atual (copiar campo por campo)
    g_PlayerData[playerid][E_PLAYER_CURRENT_TRACK][E_TRACK_TITLE][0] = EOS;
    strcat(g_PlayerData[playerid][E_PLAYER_CURRENT_TRACK][E_TRACK_TITLE], track_data[0], 128);
    g_PlayerData[playerid][E_PLAYER_CURRENT_TRACK][E_TRACK_URL][0] = EOS;
    strcat(g_PlayerData[playerid][E_PLAYER_CURRENT_TRACK][E_TRACK_URL], track_data[128], 256);
    g_PlayerData[playerid][E_PLAYER_CURRENT_TRACK][E_TRACK_DURATION] = track_data[384];
    
    // Para qualquer reprodução anterior
    if (g_PlayerData[playerid][E_PLAYER_TIMER] != -1) {
        KillTimer(g_PlayerData[playerid][E_PLAYER_TIMER]);
    }
    
    // Resetar tempo de pause
    g_PlayerData[playerid][E_PLAYER_PAUSE_TIME] = 0;
    g_PlayerData[playerid][E_PLAYER_PAUSED] = false;
    
    // Toca a Música
    PlayAudioStreamForPlayer(playerid, track_data[128]);
    g_PlayerData[playerid][E_PLAYER_PLAYING] = true;
    g_PlayerData[playerid][E_PLAYER_START_TIME] = GetTickCount();

    // Cria timer para próxima Música (apenas se for playlist)
    if (g_PlayerData[playerid][E_PLAYER_IS_PLAYLIST]) {
        g_PlayerData[playerid][E_PLAYER_TIMER] = SetTimerEx("_SAMPTube_OnTrackEnd", track_data[384], false, "i", playerid);
    }
    
    // Callback
    CallRemoteFunction("OnSAMPTubeTrackStart", "isi", playerid, track_data[0], track_data[384]);
    
    return 1;
}

public _SAMPTube_OnConvertResponse(Request:id, E_HTTP_STATUS:status, Node:node) {
    // Encontrar o playerid pela request
    new playerid = -1;
    for (new i = 0; i < MAX_PLAYERS; i++) {
        if (GetPVarInt(i, "SAMPTube_RequestID") == _:id) {
            playerid = i;
            break;
        }
    }
    
    if (playerid == -1) return 0;
    if (!IsPlayerConnected(playerid)) return 0;
    
    new bool:addToPlaylist = bool:GetPVarInt(playerid, "SAMPTube_AddToPlaylist");
    DeletePVar(playerid, "SAMPTube_RequestID");
    DeletePVar(playerid, "SAMPTube_AddToPlaylist");
    
    if (status != HTTP_STATUS_OK) {
        new error[128];
        JsonGetString(node, "error", error);
        CallRemoteFunction("OnSAMPTubeError", "is", playerid, error);
        return 0;
    }
    
    new title[128], audioUrl[256], duration;
    JsonGetString(node, "title", title);
    JsonGetString(node, "audioUrl", audioUrl);
    JsonGetInt(node, "duration", duration);
    
    if (addToPlaylist) {
        // Adicionar à playlist (buffer sem tag: 128 + 256 + 1 = 385)
        new track_data[385];
        track_data[0] = EOS;
        strcat(track_data[0], title, 128);
        track_data[128] = EOS;
        strcat(track_data[128], audioUrl, 256);
        track_data[384] = duration;
        
        new count = MAP_count(g_PlayerData[playerid][E_PLAYER_PLAYLIST]);
        MAP_insert_val_arr(g_PlayerData[playerid][E_PLAYER_PLAYLIST], count, track_data, 385);
        
        new msg[144];
        format(msg, sizeof(msg), "Adicionado Ã  playlist: %s", title);
        SendClientMessage(playerid, 0x00FF00FF, msg);
    } else {
        // Tocar imediatamente (Música única)
        if (g_PlayerData[playerid][E_PLAYER_TIMER] != -1) {
            KillTimer(g_PlayerData[playerid][E_PLAYER_TIMER]);
            g_PlayerData[playerid][E_PLAYER_TIMER] = -1;
        }
        
        // Salvar track atual (copiar campo por campo)
        g_PlayerData[playerid][E_PLAYER_CURRENT_TRACK][E_TRACK_TITLE][0] = EOS;
        strcat(g_PlayerData[playerid][E_PLAYER_CURRENT_TRACK][E_TRACK_TITLE], title, 128);
        g_PlayerData[playerid][E_PLAYER_CURRENT_TRACK][E_TRACK_URL][0] = EOS;
        strcat(g_PlayerData[playerid][E_PLAYER_CURRENT_TRACK][E_TRACK_URL], audioUrl, 256);
        g_PlayerData[playerid][E_PLAYER_CURRENT_TRACK][E_TRACK_DURATION] = duration;
        
        // Resetar estado de pause
        g_PlayerData[playerid][E_PLAYER_PAUSE_TIME] = 0;
        g_PlayerData[playerid][E_PLAYER_PAUSED] = false;
        
        PlayAudioStreamForPlayer(playerid, audioUrl);
        g_PlayerData[playerid][E_PLAYER_PLAYING] = true;
        g_PlayerData[playerid][E_PLAYER_START_TIME] = GetTickCount();
        
        CallRemoteFunction("OnSAMPTubeTrackStart", "isi", playerid, title, duration);
    }
    
    return 1;
}

public _SAMPTube_OnTrackEnd(playerid) {
    if (!IsPlayerConnected(playerid)) return 0;
    
    g_PlayerData[playerid][E_PLAYER_TIMER] = -1;
    
    // Tenta tocar a próxima Música
    if (!SAMPTube_Next(playerid)) {
        // Se não houver próxima, para tudo
        SAMPTube_Stop(playerid);
        CallRemoteFunction("OnSAMPTubeTrackEnd", "i", playerid);
    }
    
    return 1;
}

public _SAMPTube_OnSoundBoxTrackEnd(soundboxid) {
    if (soundboxid < 0 || soundboxid >= MAX_SOUNDBOXES) return 0;
    if (!g_SoundBoxes[soundboxid][E_SOUNDBOX_ACTIVE]) return 0;
    
    g_SoundBoxes[soundboxid][E_SOUNDBOX_TIMER] = -1;
    
    // Tocar próxima música automaticamente
    if (g_SoundBoxes[soundboxid][E_SOUNDBOX_IS_PLAYLIST]) {
        SAMPTube_SoundBoxNext(soundboxid);
    }
    
    return 1;
}

public _SAMPTube_OnVehicleTrackEnd(vehicleid) {
    new id = SAMPTube_GetVehicleSoundID(vehicleid);
    if (id == -1) return 0;
    
    g_VehicleSounds[id][E_VEHICLE_TIMER] = -1;
    
    // Tocar próxima música automaticamente
    if (g_VehicleSounds[id][E_VEHICLE_IS_PLAYLIST]) {
        SAMPTube_VehicleNext(vehicleid);
    }
    
    return 1;
}

// Hooks
public OnPlayerDisconnect(playerid, reason) {
    SAMPTube_Stop(playerid);
    SAMPTube_ClearPlaylist(playerid);
    
    #if defined SAMPTube_OnPlayerDisconnect
        return SAMPTube_OnPlayerDisconnect(playerid, reason);
    #else
        return 1;
    #endif
}

#if defined _ALS_OnPlayerDisconnect
    #undef OnPlayerDisconnect
#else
    #define _ALS_OnPlayerDisconnect
#endif
#define OnPlayerDisconnect SAMPTube_OnPlayerDisconnect
#if defined SAMPTube_OnPlayerDisconnect
    forward SAMPTube_OnPlayerDisconnect(playerid, reason);
#endif

public OnPlayerEnterDynamicArea(playerid, areaid) {
    // Verificar se é uma área de soundbox
    new soundboxid = SAMPTube_GetSoundBoxByArea(areaid);
    
    if (soundboxid != -1) {
        // Tocar música sincronizada para o player
        SAMPTube_PlaySoundBoxForPlayer(playerid, soundboxid);
    }
    
    #if defined STUBE_OnPlayerEnterDynArea
        return STUBE_OnPlayerEnterDynArea(playerid, areaid);
    #else
        return 1;
    #endif
}

#if defined _ALS_OnPlayerEnterDynamicArea
    #undef OnPlayerEnterDynamicArea
#else
    #define _ALS_OnPlayerEnterDynamicArea
#endif
#define OnPlayerEnterDynamicArea STUBE_OnPlayerEnterDynArea
#if defined STUBE_OnPlayerEnterDynArea
    forward STUBE_OnPlayerEnterDynArea(playerid, areaid);
#endif

public OnPlayerLeaveDynamicArea(playerid, areaid) {
    // Verificar se é uma área de soundbox
    new soundboxid = SAMPTube_GetSoundBoxByArea(areaid);
    
    if (soundboxid != -1) {
        // Parar música quando sair da área
        StopAudioStreamForPlayer(playerid);
    }
    
    #if defined STUBE_OnPlayerLeaveDynArea
        return STUBE_OnPlayerLeaveDynArea(playerid, areaid);
    #else
        return 1;
    #endif
}

#if defined _ALS_OnPlayerLeaveDynamicArea
    #undef OnPlayerLeaveDynamicArea
#else
    #define _ALS_OnPlayerLeaveDynamicArea
#endif
#define OnPlayerLeaveDynamicArea STUBE_OnPlayerLeaveDynArea
#if defined STUBE_OnPlayerLeaveDynArea
    forward STUBE_OnPlayerLeaveDynArea(playerid, areaid);
#endif

public OnPlayerStateChange(playerid, newstate, oldstate) {
    // Player entrou em um veículo (motorista ou passageiro)
    if ((newstate == PLAYER_STATE_DRIVER || newstate == PLAYER_STATE_PASSENGER) && oldstate != PLAYER_STATE_DRIVER && oldstate != PLAYER_STATE_PASSENGER) {
        new vehicleid = GetPlayerVehicleID(playerid);
        new id = SAMPTube_GetVehicleSoundID(vehicleid);
        
        if (id != -1) {
            // Tocar música sincronizada para o player
            SAMPTube_PlayVehicleForPlayer(playerid, vehicleid);
        }
    }
    // Player saiu do veículo
    else if ((oldstate == PLAYER_STATE_DRIVER || oldstate == PLAYER_STATE_PASSENGER) && newstate != PLAYER_STATE_DRIVER && newstate != PLAYER_STATE_PASSENGER) {
        new vehicleid = GetPlayerVehicleID(playerid);
        if (vehicleid == 0) {
            // GetPlayerVehicleID retorna 0 quando não está em veículo, pegar do estado anterior
            // Verificar todos os veículos com som ativo
            for (new i = 0; i < MAX_VEHICLE_SOUNDS; i++) {
                if (g_VehicleSounds[i][E_VEHICLE_ACTIVE]) {
                    // Parar música
                    StopAudioStreamForPlayer(playerid);
                    break;
                }
            }
        } else {
            new id = SAMPTube_GetVehicleSoundID(vehicleid);
            if (id != -1) {
                // Parar música quando sair do veículo
                StopAudioStreamForPlayer(playerid);
            }
        }
    }
    
    #if defined STUBE_OnPlayerStateChange
        return STUBE_OnPlayerStateChange(playerid, newstate, oldstate);
    #else
        return 1;
    #endif
}

#if defined _ALS_OnPlayerStateChange
    #undef OnPlayerStateChange
#else
    #define _ALS_OnPlayerStateChange
#endif
#define OnPlayerStateChange STUBE_OnPlayerStateChange
#if defined STUBE_OnPlayerStateChange
    forward STUBE_OnPlayerStateChange(playerid, newstate, oldstate);
#endif
