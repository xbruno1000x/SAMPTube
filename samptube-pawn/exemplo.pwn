/**
 * SAMPTube - Exemplo de Uso
 * 
 * Demonstra todas as funcionalidades do sistema SAMPTube:
 * - Reprodu��o de m�sicas por URL ou busca
 * - Sistema de playlists
 * - Controles de reprodu��o (pause, resume, next, previous)
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
 * Chamado quando uma m�sica come�a a tocar
 */
public OnSAMPTubeTrackStart(playerid, const title[], duration) {
    new msg[256];
    format(msg, sizeof(msg), MSG_PREFIX "Tocando: {FFFF00}%s {FFFFFF}(%d segundos)", title, duration / 1000);
    SendClientMessage(playerid, COLOR_INFO, msg);
    
    // Verificar se deve criar uma soundbox
    if (GetPVarInt(playerid, "SoundBox_Create") == 1) {
        DeletePVar(playerid, "SoundBox_Create");
        
        SendClientMessage(playerid, COLOR_SUCCESS, MSG_PREFIX "M�sica carregada! Use /criarcaixa com a URL da m�sica");
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

// === FUN��ES AUXILIARES ===

/**
 * Verifica se o player pode executar comandos de controle
 */
stock bool:CanControlPlayback(playerid) {
    if (!SAMPTube_IsPlaying(playerid)) {
        SendClientMessage(playerid, COLOR_ERROR, MSG_ERROR "Voc� n�o est� ouvindo m�sica!");
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
        SendClientMessage(playerid, COLOR_ERROR, MSG_ERROR "A m�sica j� est� pausada!");
        return false;
    }
    
    return true;
}

/**
 * Verifica se o player pode resumir
 */
stock bool:CanResume(playerid) {
    if (!SAMPTube_IsPaused(playerid)) {
        SendClientMessage(playerid, COLOR_ERROR, MSG_ERROR "A m�sica n�o est� pausada!");
        return false;
    }
    return true;
}

/**
 * Valida se o comando tem par�metros
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

// === COMANDOS DE REPRODU��O ===

/**
 * Toca uma m�sica por URL do YouTube
 */
CMD:tocar(playerid, params[]) {
    if (!ValidateParams(playerid, params, "/tocar [URL do YouTube]")) {
        return 1;
    }
    
    SAMPTube_PlayURL(playerid, params);
    SendClientMessage(playerid, COLOR_INFO, MSG_PREFIX "Carregando m�sica...");
    return 1;
}

/**
 * Busca e toca uma m�sica
 */
CMD:buscar(playerid, params[]) {
    if (!ValidateParams(playerid, params, "/buscar [nome da m�sica]")) {
        return 1;
    }
    
    SAMPTube_PlaySearch(playerid, params);
    SendClientMessage(playerid, COLOR_INFO, MSG_PREFIX "Buscando m�sica...");
    return 1;
}

// === COMANDOS DE PLAYLIST ===

/**
 * Adiciona uma m�sica � playlist por URL
 */
CMD:adicionar(playerid, params[]) {
    if (!ValidateParams(playerid, params, "/adicionar [URL do YouTube]")) {
        return 1;
    }
    
    SAMPTube_AddToPlaylist(playerid, params);
    SendClientMessage(playerid, COLOR_INFO, MSG_PREFIX "Adicionando � playlist...");
    return 1;
}

/**
 * Busca e adiciona uma m�sica � playlist
 */
CMD:addbusca(playerid, params[]) {
    if (!ValidateParams(playerid, params, "/addbusca [nome da m�sica]")) {
        return 1;
    }
    
    SAMPTube_AddSearchToPlaylist(playerid, params);
    SendClientMessage(playerid, COLOR_INFO, MSG_PREFIX "Buscando e adicionando...");
    return 1;
}

/**
 * Inicia a reprodu��o da playlist
 */
CMD:playlist(playerid) {
    new count = SAMPTube_GetPlaylistCount(playerid);
    
    if (count == 0) {
        SendClientMessage(playerid, COLOR_ERROR, MSG_ERROR "Sua playlist est� vazia!");
        SendClientMessage(playerid, COLOR_WARNING, "{FFFF00}Use /adicionar ou /addbusca para adicionar m�sicas");
        return 1;
    }
    
    SAMPTube_PlayPlaylist(playerid);
    
    new msg[128];
    format(msg, sizeof(msg), MSG_PREFIX "Iniciando playlist com {FFFF00}%d {FFFFFF}m�sica(s)", count);
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
 * Lista todas as m�sicas da playlist
 */
CMD:listarplaylist(playerid) {
    SAMPTube_ListPlaylist(playerid);
    return 1;
}

/**
 * Remove uma m�sica da playlist por ID (�ndice)
 * Uso: /remover 3
 */
CMD:remover(playerid, params[]) {
    if (!ValidateParams(playerid, params, "/remover [ID da m�sica]")) {
        return 1;
    }
    
    new index = strval(params) - 1; // Converter para �ndice base-0
    
    if (index < 0) {
        SendClientMessage(playerid, COLOR_ERROR, MSG_ERROR "ID inv�lido! Use /listarplaylist para ver os IDs");
        return 1;
    }
    
    SAMPTube_RemoveByIndex(playerid, index);
    return 1;
}

/**
 * Remove uma m�sica da playlist por nome (busca parcial)
 * Uso: /removernome don toliver
 */
CMD:removernome(playerid, params[]) {
    if (!ValidateParams(playerid, params, "/removernome [nome da m�sica]")) {
        return 1;
    }
    
    SAMPTube_RemoveByName(playerid, params);
    return 1;
}

// === COMANDOS DE CONTROLE ===

/**
 * Toca a pr�xima m�sica da playlist
 */
CMD:proxima(playerid) {
    if (!CanControlPlayback(playerid)) {
        return 1;
    }
    
    SAMPTube_Next(playerid);
    return 1;
}

/**
 * Toca a m�sica anterior da playlist
 */
CMD:anterior(playerid) {
    if (!CanControlPlayback(playerid)) {
        return 1;
    }
    
    SAMPTube_Previous(playerid);
    return 1;
}

/**
 * Para a reprodu��o completamente
 */
CMD:parar(playerid) {
    SAMPTube_Stop(playerid);
    SendClientMessage(playerid, COLOR_SUCCESS, MSG_PREFIX "Reprodu��o parada");
    return 1;
}

/**
 * Pausa a reprodu��o atual
 */
CMD:pausar(playerid) {
    if (!CanPause(playerid)) {
        return 1;
    }
    
    SAMPTube_Pause(playerid);
    SendClientMessage(playerid, COLOR_SUCCESS, MSG_PREFIX "Reprodu��o pausada");
    return 1;
}

/**
 * Resume a reprodu��o pausada
 */
CMD:resumir(playerid) {
    if (!CanResume(playerid)) {
        return 1;
    }
    
    SAMPTube_Resume(playerid);
    SendClientMessage(playerid, COLOR_SUCCESS, MSG_PREFIX "Reprodu��o retomada");
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
    
    new Float:range = 50.0; // Alcance padr�o
    
    // Obter posi��o do player
    new Float:x, Float:y, Float:z, Float:groundZ;
    GetPlayerPos(playerid, x, y, z);
    
    // Encontrar Z do ch�o usando MapAndreas
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
        
        // Tocar para o criador tamb�m
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
        SendClientMessage(playerid, COLOR_ERROR, MSG_ERROR "Sua playlist est� vazia!");
        SendClientMessage(playerid, COLOR_WARNING, "{FFFF00}Use /adicionar ou /addbusca para adicionar m�sicas");
        return 1;
    }
    
    // Obter posi��o do player
    new Float:x, Float:y, Float:z, Float:groundZ;
    GetPlayerPos(playerid, x, y, z);
    
    // Encontrar Z do ch�o usando MapAndreas
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
        format(msg, sizeof(msg), MSG_PREFIX "Caixa de som com playlist criada! ID: {FFFF00}%d {FFFFFF}| M�sicas: {FFFF00}%d {FFFFFF}| Alcance: {FFFF00}%.1f metros", soundboxid, count, 8.0);
        SendClientMessage(playerid, COLOR_SUCCESS, msg);
        SendClientMessage(playerid, COLOR_INFO, "{00FF00}[SAMPTube] {FFFFFF}A playlist tocar� em loop automaticamente!");
    } else {
        SendClientMessage(playerid, COLOR_ERROR, MSG_ERROR "Erro ao criar caixa de som!");
    }
    
    return 1;
}

/**
 * Pula para a pr�xima m�sica de uma soundbox
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
            format(msg, sizeof(msg), MSG_PREFIX "Caixa #%d - Pr�xima: {FFFF00}%s", soundboxid, title);
            SendClientMessage(playerid, COLOR_SUCCESS, msg);
        } else {
            SendClientMessage(playerid, COLOR_SUCCESS, MSG_PREFIX "Pr�xima m�sica iniciada!");
        }
    } else {
        SendClientMessage(playerid, COLOR_ERROR, MSG_ERROR "ID de caixa de som inv�lido ou sem playlist!");
    }
    
    return 1;
}

/**
 * Volta para a m�sica anterior de uma soundbox
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
            SendClientMessage(playerid, COLOR_SUCCESS, MSG_PREFIX "M�sica anterior iniciada!");
        }
    } else {
        SendClientMessage(playerid, COLOR_ERROR, MSG_ERROR "ID de caixa de som inv�lido ou sem playlist!");
    }
    
    return 1;
}

/**
 * Destr�i uma caixa de som
 * Uso: /destruircaixa [soundboxid]
 */
CMD:destruircaixa(playerid, params[]) {
    if (!ValidateParams(playerid, params, "/destruircaixa [ID da caixa de som]")) {
        return 1;
    }
    
    new soundboxid = strval(params);
    
    if (SAMPTube_DestroySoundBox(soundboxid)) {
        new msg[128];
        format(msg, sizeof(msg), MSG_PREFIX "Caixa de som #%d destru�da!", soundboxid);
        SendClientMessage(playerid, COLOR_SUCCESS, msg);
    } else {
        SendClientMessage(playerid, COLOR_ERROR, MSG_ERROR "ID de caixa de som inv�lido!");
    }
    
    return 1;
}

// === COMANDOS DE VE�CULO ===

CMD:veh(playerid) {
    new Float:Pos[3];
    GetPlayerPos(playerid, Pos[0], Pos[1], Pos[2]);
    CreateVehicle(413, Pos[0], Pos[1], Pos[2], 0.0, -1, -1, 60);
    return 1;
}

/**
 * Adiciona som com playlist no ve�culo atual
 * Uso: /somveiculo
 */
CMD:somveiculo(playerid) {
    if (!IsPlayerInAnyVehicle(playerid)) {
        SendClientMessage(playerid, COLOR_ERROR, MSG_ERROR "Voc� precisa estar em um ve�culo!");
        return 1;
    }
    
    new count = SAMPTube_GetPlaylistCount(playerid);
    
    if (count == 0) {
        SendClientMessage(playerid, COLOR_ERROR, MSG_ERROR "Sua playlist est� vazia!");
        SendClientMessage(playerid, COLOR_WARNING, "{FFFF00}Use /adicionar ou /addbusca para adicionar m�sicas");
        return 1;
    }
    
    new vehicleid = GetPlayerVehicleID(playerid);
    
    // Criar som no ve�culo com playlist
    new id = SAMPTube_CreateVehiclePlaylist(playerid, vehicleid);
    
    if (id != -1) {
        new msg[256];
        format(msg, sizeof(msg), MSG_PREFIX "Som do ve�culo ativado! M�sicas: {FFFF00}%d", count);
        SendClientMessage(playerid, COLOR_SUCCESS, msg);
        SendClientMessage(playerid, COLOR_INFO, "{00FF00}[SAMPTube] {FFFFFF}A playlist tocar� em loop automaticamente!");
        SendClientMessage(playerid, COLOR_INFO, "{00FF00}[SAMPTube] {FFFFFF}Passageiros tamb�m ouvir�o a m�sica!");
    } else {
        SendClientMessage(playerid, COLOR_ERROR, MSG_ERROR "Erro ao criar som no ve�culo!");
    }
    
    return 1;
}

/**
 * Pr�xima m�sica do ve�culo
 * Uso: /proximaveiculo
 */
CMD:proximaveiculo(playerid) {
    if (!IsPlayerInAnyVehicle(playerid)) {
        SendClientMessage(playerid, COLOR_ERROR, MSG_ERROR "Voc� precisa estar em um ve�culo!");
        return 1;
    }
    
    new vehicleid = GetPlayerVehicleID(playerid);
    
    if (SAMPTube_VehicleNext(vehicleid)) {
        new title[128];
        if (SAMPTube_GetVehicleCurrentTrack(vehicleid, title)) {
            new msg[256];
            format(msg, sizeof(msg), MSG_PREFIX "Ve�culo - Pr�xima: {FFFF00}%s", title);
            SendClientMessage(playerid, COLOR_SUCCESS, msg);
        } else {
            SendClientMessage(playerid, COLOR_SUCCESS, MSG_PREFIX "Pr�xima m�sica iniciada!");
        }
    } else {
        SendClientMessage(playerid, COLOR_ERROR, MSG_ERROR "Este ve�culo n�o tem som ativo ou playlist!");
    }
    
    return 1;
}

/**
 * M�sica anterior do ve�culo
 * Uso: /anteriorveiculo
 */
CMD:anteriorveiculo(playerid) {
    if (!IsPlayerInAnyVehicle(playerid)) {
        SendClientMessage(playerid, COLOR_ERROR, MSG_ERROR "Voc� precisa estar em um ve�culo!");
        return 1;
    }
    
    new vehicleid = GetPlayerVehicleID(playerid);
    
    if (SAMPTube_VehiclePrevious(vehicleid)) {
        new title[128];
        if (SAMPTube_GetVehicleCurrentTrack(vehicleid, title)) {
            new msg[256];
            format(msg, sizeof(msg), MSG_PREFIX "Ve�culo - Anterior: {FFFF00}%s", title);
            SendClientMessage(playerid, COLOR_SUCCESS, msg);
        } else {
            SendClientMessage(playerid, COLOR_SUCCESS, MSG_PREFIX "M�sica anterior iniciada!");
        }
    } else {
        SendClientMessage(playerid, COLOR_ERROR, MSG_ERROR "Este ve�culo n�o tem som ativo ou playlist!");
    }
    
    return 1;
}

/**
 * Remove som do ve�culo
 * Uso: /removersomveiculo
 */
CMD:removersomveiculo(playerid) {
    if (!IsPlayerInAnyVehicle(playerid)) {
        SendClientMessage(playerid, COLOR_ERROR, MSG_ERROR "Voc� precisa estar em um ve�culo!");
        return 1;
    }
    
    new vehicleid = GetPlayerVehicleID(playerid);
    
    if (SAMPTube_DestroyVehicleSound(vehicleid)) {
        SendClientMessage(playerid, COLOR_SUCCESS, MSG_PREFIX "Som do ve�culo removido!");
    } else {
        SendClientMessage(playerid, COLOR_ERROR, MSG_ERROR "Este ve�culo n�o tem som ativo!");
    }
    
    return 1;
}
