/**
 * SAMPTube - Exemplo de Uso
 * 
 * Demonstra todas as funcionalidades do sistema SAMPTube:
 * - Reprodução de músicas por URL ou busca
 * - Sistema de playlists
 * - Controles de reprodução (pause, resume, next, previous)
 */

#include <a_samp>
#include <zcmd>
#include <streamer>
#include <mapandreas>
#include <samptube>

// === CONSTANTES ===

// Cores
#define COLOR_SUCCESS   0x00FF00FF
#define COLOR_ERROR     0xFF0000FF
#define COLOR_INFO      0xFFFFFFFF
#define COLOR_WARNING   0xFFFF00FF

// Mensagens
#define MSG_PREFIX      "{00FF00}[SAMPTube]{FFFFFF} "
#define MSG_ERROR       "{FF0000}[SAMPTube]{FFFFFF} "

// === MAIN ===

main() {}

// === CALLBACKS ===

/**
 * Chamado quando uma música começa a tocar
 */
public OnSAMPTubeTrackStart(playerid, const title[], duration) {
    new msg[256];
    format(msg, sizeof(msg), MSG_PREFIX "Tocando: {FFFF00}%s {FFFFFF}(%d segundos)", title, duration / 1000);
    SendClientMessage(playerid, COLOR_INFO, msg);
    
    // Verificar se deve criar uma soundbox
    if (GetPVarInt(playerid, "SoundBox_Create") == 1) {
        DeletePVar(playerid, "SoundBox_Create");
        
        SendClientMessage(playerid, COLOR_SUCCESS, MSG_PREFIX "Música carregada! Use /criarcaixa com a URL da música");
        SendClientMessage(playerid, COLOR_INFO, "{FFFF00}Exemplo: /criarcaixa http://localhost:8000/api/audio/nome-da-musica.mp3");
    }
    
    return 1;
}

/**
 * Chamado quando a playlist termina
 */
public OnSAMPTubeTrackEnd(playerid) {
    SendClientMessage(playerid, COLOR_INFO, MSG_PREFIX "Playlist finalizada!");
    return 1;
}

/**
 * Chamado quando ocorre um erro
 */
public OnSAMPTubeError(playerid, const error[]) {
    new msg[256];
    format(msg, sizeof(msg), MSG_ERROR "%s", error);
    SendClientMessage(playerid, COLOR_ERROR, msg);
    return 1;
}

// === FUNÇÕES AUXILIARES ===

/**
 * Verifica se o player pode executar comandos de controle
 */
stock bool:CanControlPlayback(playerid) {
    if (!SAMPTube_IsPlaying(playerid)) {
        SendClientMessage(playerid, COLOR_ERROR, MSG_ERROR "Você não está ouvindo música!");
        return false;
    }
    return true;
}

/**
 * Verifica se o player pode pausar
 */
stock bool:CanPause(playerid) {
    if (!CanControlPlayback(playerid)) {
        return false;
    }
    
    if (SAMPTube_IsPaused(playerid)) {
        SendClientMessage(playerid, COLOR_ERROR, MSG_ERROR "A música já está pausada!");
        return false;
    }
    
    return true;
}

/**
 * Verifica se o player pode resumir
 */
stock bool:CanResume(playerid) {
    if (!SAMPTube_IsPaused(playerid)) {
        SendClientMessage(playerid, COLOR_ERROR, MSG_ERROR "A música não está pausada!");
        return false;
    }
    return true;
}

/**
 * Valida se o comando tem parâmetros
 */
stock bool:ValidateParams(playerid, const params[], const usage[]) {
    if (isnull(params)) {
        new msg[128];
        format(msg, sizeof(msg), "{FFFF00}Uso: %s", usage);
        SendClientMessage(playerid, COLOR_WARNING, msg);
        return false;
    }
    return true;
}

// === COMANDOS DE REPRODUÇÃO ===

/**
 * Toca uma música por URL do YouTube
 */
CMD:tocar(playerid, params[]) {
    if (!ValidateParams(playerid, params, "/tocar [URL do YouTube]")) {
        return 1;
    }
    
    SAMPTube_PlayURL(playerid, params);
    SendClientMessage(playerid, COLOR_INFO, MSG_PREFIX "Carregando música...");
    return 1;
}

/**
 * Busca e toca uma música
 */
CMD:buscar(playerid, params[]) {
    if (!ValidateParams(playerid, params, "/buscar [nome da música]")) {
        return 1;
    }
    
    SAMPTube_PlaySearch(playerid, params);
    SendClientMessage(playerid, COLOR_INFO, MSG_PREFIX "Buscando música...");
    return 1;
}

// === COMANDOS DE PLAYLIST ===

/**
 * Adiciona uma música à playlist por URL
 */
CMD:adicionar(playerid, params[]) {
    if (!ValidateParams(playerid, params, "/adicionar [URL do YouTube]")) {
        return 1;
    }
    
    SAMPTube_AddToPlaylist(playerid, params);
    SendClientMessage(playerid, COLOR_INFO, MSG_PREFIX "Adicionando à playlist...");
    return 1;
}

/**
 * Busca e adiciona uma música à playlist
 */
CMD:addbusca(playerid, params[]) {
    if (!ValidateParams(playerid, params, "/addbusca [nome da música]")) {
        return 1;
    }
    
    SAMPTube_AddSearchToPlaylist(playerid, params);
    SendClientMessage(playerid, COLOR_INFO, MSG_PREFIX "Buscando e adicionando...");
    return 1;
}

/**
 * Inicia a reprodução da playlist
 */
CMD:playlist(playerid) {
    new count = SAMPTube_GetPlaylistCount(playerid);
    
    if (count == 0) {
        SendClientMessage(playerid, COLOR_ERROR, MSG_ERROR "Sua playlist está vazia!");
        SendClientMessage(playerid, COLOR_WARNING, "{FFFF00}Use /adicionar ou /addbusca para adicionar músicas");
        return 1;
    }
    
    SAMPTube_PlayPlaylist(playerid);
    
    new msg[128];
    format(msg, sizeof(msg), MSG_PREFIX "Iniciando playlist com {FFFF00}%d {FFFFFF}música(s)", count);
    SendClientMessage(playerid, COLOR_SUCCESS, msg);
    return 1;
}

/**
 * Limpa a playlist do player
 */
CMD:limparplaylist(playerid) {
    SAMPTube_ClearPlaylist(playerid);
    SendClientMessage(playerid, COLOR_SUCCESS, MSG_PREFIX "Playlist limpa!");
    return 1;
}

/**
 * Lista todas as músicas da playlist
 */
CMD:listarplaylist(playerid) {
    SAMPTube_ListPlaylist(playerid);
    return 1;
}

/**
 * Remove uma música da playlist por ID (índice)
 * Uso: /remover 3
 */
CMD:remover(playerid, params[]) {
    if (!ValidateParams(playerid, params, "/remover [ID da música]")) {
        return 1;
    }
    
    new index = strval(params) - 1; // Converter para índice base-0
    
    if (index < 0) {
        SendClientMessage(playerid, COLOR_ERROR, MSG_ERROR "ID inválido! Use /listarplaylist para ver os IDs");
        return 1;
    }
    
    SAMPTube_RemoveByIndex(playerid, index);
    return 1;
}

/**
 * Remove uma música da playlist por nome (busca parcial)
 * Uso: /removernome don toliver
 */
CMD:removernome(playerid, params[]) {
    if (!ValidateParams(playerid, params, "/removernome [nome da música]")) {
        return 1;
    }
    
    SAMPTube_RemoveByName(playerid, params);
    return 1;
}

// === COMANDOS DE CONTROLE ===

/**
 * Toca a próxima música da playlist
 */
CMD:proxima(playerid) {
    if (!CanControlPlayback(playerid)) {
        return 1;
    }
    
    SAMPTube_Next(playerid);
    return 1;
}

/**
 * Toca a música anterior da playlist
 */
CMD:anterior(playerid) {
    if (!CanControlPlayback(playerid)) {
        return 1;
    }
    
    SAMPTube_Previous(playerid);
    return 1;
}

/**
 * Para a reprodução completamente
 */
CMD:parar(playerid) {
    SAMPTube_Stop(playerid);
    SendClientMessage(playerid, COLOR_SUCCESS, MSG_PREFIX "Reprodução parada");
    return 1;
}

/**
 * Pausa a reprodução atual
 */
CMD:pausar(playerid) {
    if (!CanPause(playerid)) {
        return 1;
    }
    
    SAMPTube_Pause(playerid);
    SendClientMessage(playerid, COLOR_SUCCESS, MSG_PREFIX "Reprodução pausada");
    return 1;
}

/**
 * Resume a reprodução pausada
 */
CMD:resumir(playerid) {
    if (!CanResume(playerid)) {
        return 1;
    }
    
    SAMPTube_Resume(playerid);
    SendClientMessage(playerid, COLOR_SUCCESS, MSG_PREFIX "Reprodução retomada");
    return 1;
}

// === COMANDOS DE SOUNDBOX (CAIXA DE SOM 3D) ===

/**
 * Cria uma caixa de som com URL direta
 * Uso: /criarcaixa [audioUrl]
 */
CMD:criarcaixa(playerid, params[]) {
    if (!ValidateParams(playerid, params, "/criarcaixa [audioUrl completa]")) {
        SendClientMessage(playerid, COLOR_WARNING, "{FFFF00}Exemplo: /criarcaixa http://localhost:8000/api/audio/musica.mp3");
        return 1;
    }
    
    new Float:range = 50.0; // Alcance padrão
    
    // Obter posição do player
    new Float:x, Float:y, Float:z, Float:groundZ;
    GetPlayerPos(playerid, x, y, z);
    
    // Encontrar Z do chão usando MapAndreas
    if (MapAndreas_FindZ_For2DCoord(x, y, groundZ)) {
        z = groundZ + 0.5;
    } else {
        MapAndreas_FindAverageZ(x, y, groundZ);
        z = groundZ + 0.5;
    }
    
    // Criar soundbox
    new soundboxid = SAMPTube_CreateSoundBox(params, x, y, z, range, 1);
    
    if (soundboxid != -1) {
        new msg[256];
        format(msg, sizeof(msg), MSG_PREFIX "Caixa de som criada! ID: {FFFF00}%d {FFFFFF}| Alcance: {FFFF00}%.1f metros", soundboxid, range);
        SendClientMessage(playerid, COLOR_SUCCESS, msg);
        
        // Tocar para o criador também
        SAMPTube_PlaySoundBoxForPlayer(playerid, soundboxid);
    } else {
        SendClientMessage(playerid, COLOR_ERROR, MSG_ERROR "Limite de caixas de som atingido!");
    }
    
    return 1;
}

/**
 * Cria uma caixa de som com a playlist do player
 * Uso: /caixaplaylist
 */
CMD:caixaplaylist(playerid) {
    new count = SAMPTube_GetPlaylistCount(playerid);
    
    if (count == 0) {
        SendClientMessage(playerid, COLOR_ERROR, MSG_ERROR "Sua playlist está vazia!");
        SendClientMessage(playerid, COLOR_WARNING, "{FFFF00}Use /adicionar ou /addbusca para adicionar músicas");
        return 1;
    }
    
    // Obter posição do player
    new Float:x, Float:y, Float:z, Float:groundZ;
    GetPlayerPos(playerid, x, y, z);
    
    // Encontrar Z do chão usando MapAndreas
    if (MapAndreas_FindZ_For2DCoord(x, y, groundZ)) {
        z = groundZ + 0.6;
    } else {
        MapAndreas_FindAverageZ(x, y, groundZ);
        z = groundZ + 0.6;
    }
    
    // Criar soundbox com playlist
    new soundboxid = SAMPTube_CreateBoxPlaylist(playerid, x, y, z);
    
    if (soundboxid != -1) {
        new msg[256];
        format(msg, sizeof(msg), MSG_PREFIX "Caixa de som com playlist criada! ID: {FFFF00}%d {FFFFFF}| Músicas: {FFFF00}%d {FFFFFF}| Alcance: {FFFF00}%.1f metros", soundboxid, count, 8.0);
        SendClientMessage(playerid, COLOR_SUCCESS, msg);
        SendClientMessage(playerid, COLOR_INFO, "{00FF00}[SAMPTube] {FFFFFF}A playlist tocará em loop automaticamente!");
    } else {
        SendClientMessage(playerid, COLOR_ERROR, MSG_ERROR "Erro ao criar caixa de som!");
    }
    
    return 1;
}

/**
 * Pula para a próxima música de uma soundbox
 * Uso: /proximacaixa [soundboxid]
 */
CMD:proximacaixa(playerid, params[]) {
    if (!ValidateParams(playerid, params, "/proximacaixa [ID da caixa de som]")) {
        return 1;
    }
    
    new soundboxid = strval(params);
    
    if (SAMPTube_SoundBoxNext(soundboxid)) {
        new title[128];
        if (SAMPTube_GetBoxCurrentTrack(soundboxid, title)) {
            new msg[256];
            format(msg, sizeof(msg), MSG_PREFIX "Caixa #%d - Próxima: {FFFF00}%s", soundboxid, title);
            SendClientMessage(playerid, COLOR_SUCCESS, msg);
        } else {
            SendClientMessage(playerid, COLOR_SUCCESS, MSG_PREFIX "Próxima música iniciada!");
        }
    } else {
        SendClientMessage(playerid, COLOR_ERROR, MSG_ERROR "ID de caixa de som inválido ou sem playlist!");
    }
    
    return 1;
}

/**
 * Volta para a música anterior de uma soundbox
 * Uso: /anteriorcaixa [soundboxid]
 */
CMD:anteriorcaixa(playerid, params[]) {
    if (!ValidateParams(playerid, params, "/anteriorcaixa [ID da caixa de som]")) {
        return 1;
    }
    
    new soundboxid = strval(params);
    
    if (SAMPTube_SoundBoxPrevious(soundboxid)) {
        new title[128];
        if (SAMPTube_GetBoxCurrentTrack(soundboxid, title)) {
            new msg[256];
            format(msg, sizeof(msg), MSG_PREFIX "Caixa #%d - Anterior: {FFFF00}%s", soundboxid, title);
            SendClientMessage(playerid, COLOR_SUCCESS, msg);
        } else {
            SendClientMessage(playerid, COLOR_SUCCESS, MSG_PREFIX "Música anterior iniciada!");
        }
    } else {
        SendClientMessage(playerid, COLOR_ERROR, MSG_ERROR "ID de caixa de som inválido ou sem playlist!");
    }
    
    return 1;
}

/**
 * Destrói uma caixa de som
 * Uso: /destruircaixa [soundboxid]
 */
CMD:destruircaixa(playerid, params[]) {
    if (!ValidateParams(playerid, params, "/destruircaixa [ID da caixa de som]")) {
        return 1;
    }
    
    new soundboxid = strval(params);
    
    if (SAMPTube_DestroySoundBox(soundboxid)) {
        new msg[128];
        format(msg, sizeof(msg), MSG_PREFIX "Caixa de som #%d destruída!", soundboxid);
        SendClientMessage(playerid, COLOR_SUCCESS, msg);
    } else {
        SendClientMessage(playerid, COLOR_ERROR, MSG_ERROR "ID de caixa de som inválido!");
    }
    
    return 1;
}

// === COMANDOS DE VEÍCULO ===

CMD:veh(playerid) {
    new Float:Pos[3];
    GetPlayerPos(playerid, Pos[0], Pos[1], Pos[2]);
    CreateVehicle(413, Pos[0], Pos[1], Pos[2], 0.0, -1, -1, 60);
    return 1;
}

/**
 * Adiciona som com playlist no veículo atual
 * Uso: /somveiculo
 */
CMD:somveiculo(playerid) {
    if (!IsPlayerInAnyVehicle(playerid)) {
        SendClientMessage(playerid, COLOR_ERROR, MSG_ERROR "Você precisa estar em um veículo!");
        return 1;
    }
    
    new count = SAMPTube_GetPlaylistCount(playerid);
    
    if (count == 0) {
        SendClientMessage(playerid, COLOR_ERROR, MSG_ERROR "Sua playlist está vazia!");
        SendClientMessage(playerid, COLOR_WARNING, "{FFFF00}Use /adicionar ou /addbusca para adicionar músicas");
        return 1;
    }
    
    new vehicleid = GetPlayerVehicleID(playerid);
    
    // Criar som no veículo com playlist
    new id = SAMPTube_CreateVehiclePlaylist(playerid, vehicleid);
    
    if (id != -1) {
        new msg[256];
        format(msg, sizeof(msg), MSG_PREFIX "Som do veículo ativado! Músicas: {FFFF00}%d", count);
        SendClientMessage(playerid, COLOR_SUCCESS, msg);
        SendClientMessage(playerid, COLOR_INFO, "{00FF00}[SAMPTube] {FFFFFF}A playlist tocará em loop automaticamente!");
        SendClientMessage(playerid, COLOR_INFO, "{00FF00}[SAMPTube] {FFFFFF}Passageiros também ouvirão a música!");
    } else {
        SendClientMessage(playerid, COLOR_ERROR, MSG_ERROR "Erro ao criar som no veículo!");
    }
    
    return 1;
}

/**
 * Próxima música do veículo
 * Uso: /proximaveiculo
 */
CMD:proximaveiculo(playerid) {
    if (!IsPlayerInAnyVehicle(playerid)) {
        SendClientMessage(playerid, COLOR_ERROR, MSG_ERROR "Você precisa estar em um veículo!");
        return 1;
    }
    
    new vehicleid = GetPlayerVehicleID(playerid);
    
    if (SAMPTube_VehicleNext(vehicleid)) {
        new title[128];
        if (SAMPTube_GetVehicleCurrentTrack(vehicleid, title)) {
            new msg[256];
            format(msg, sizeof(msg), MSG_PREFIX "Veículo - Próxima: {FFFF00}%s", title);
            SendClientMessage(playerid, COLOR_SUCCESS, msg);
        } else {
            SendClientMessage(playerid, COLOR_SUCCESS, MSG_PREFIX "Próxima música iniciada!");
        }
    } else {
        SendClientMessage(playerid, COLOR_ERROR, MSG_ERROR "Este veículo não tem som ativo ou playlist!");
    }
    
    return 1;
}

/**
 * Música anterior do veículo
 * Uso: /anteriorveiculo
 */
CMD:anteriorveiculo(playerid) {
    if (!IsPlayerInAnyVehicle(playerid)) {
        SendClientMessage(playerid, COLOR_ERROR, MSG_ERROR "Você precisa estar em um veículo!");
        return 1;
    }
    
    new vehicleid = GetPlayerVehicleID(playerid);
    
    if (SAMPTube_VehiclePrevious(vehicleid)) {
        new title[128];
        if (SAMPTube_GetVehicleCurrentTrack(vehicleid, title)) {
            new msg[256];
            format(msg, sizeof(msg), MSG_PREFIX "Veículo - Anterior: {FFFF00}%s", title);
            SendClientMessage(playerid, COLOR_SUCCESS, msg);
        } else {
            SendClientMessage(playerid, COLOR_SUCCESS, MSG_PREFIX "Música anterior iniciada!");
        }
    } else {
        SendClientMessage(playerid, COLOR_ERROR, MSG_ERROR "Este veículo não tem som ativo ou playlist!");
    }
    
    return 1;
}

/**
 * Remove som do veículo
 * Uso: /removersomveiculo
 */
CMD:removersomveiculo(playerid) {
    if (!IsPlayerInAnyVehicle(playerid)) {
        SendClientMessage(playerid, COLOR_ERROR, MSG_ERROR "Você precisa estar em um veículo!");
        return 1;
    }
    
    new vehicleid = GetPlayerVehicleID(playerid);
    
    if (SAMPTube_DestroyVehicleSound(vehicleid)) {
        SendClientMessage(playerid, COLOR_SUCCESS, MSG_PREFIX "Som do veículo removido!");
    } else {
        SendClientMessage(playerid, COLOR_ERROR, MSG_ERROR "Este veículo não tem som ativo!");
    }
    
    return 1;
}
