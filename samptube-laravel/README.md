# 🎵 SAMPTube API

<p align="center">
  <b>YouTube to MP3 Converter & Streaming API for SA-MP</b><br>
  <i>Powered by Laravel 12, yt-dlp & FFmpeg</i>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/Laravel-12.35.1-FF2D20?style=for-the-badge&logo=laravel" alt="Laravel">
  <img src="https://img.shields.io/badge/PHP-8.x-777BB4?style=for-the-badge&logo=php" alt="PHP">
  <img src="https://img.shields.io/badge/yt--dlp-2025.10.22-red?style=for-the-badge" alt="yt-dlp">
  <img src="https://img.shields.io/badge/FFmpeg-N--121525-green?style=for-the-badge" alt="FFmpeg">
</p>

---

## 📋 Sobre

SAMPTube API é uma API REST construída em Laravel que converte vídeos do YouTube em arquivos MP3 e os transmite com suporte a streaming HTTP, pause/resume e sincronização. Desenvolvida especificamente para servidores SA-MP (San Andreas Multiplayer).

### ✨ Características

- 🎬 **Conversão YouTube → MP3**: Usando yt-dlp (mais confiável que bibliotecas Node.js)
- 🔍 **Busca Integrada**: Pesquise e converta vídeos diretamente pelo título
- 📡 **HTTP Range Requests**: Suporte a pause, resume e seek (RFC 7233)
- ⏱️ **Sincronização**: Query parameter `?start=` para sincronizar múltiplos players
- 📦 **Cache Inteligente**: Arquivos convertidos são armazenados para reuso
- 🎵 **Qualidade 128kbps**: Audio otimizado para streaming em jogos
- 🔄 **Timeout 600s**: Suporte a vídeos longos
- 🛡️ **Validação de URL**: Múltiplos formatos do YouTube suportados

### 🎯 Casos de Uso

- Sistema de música para servidores SA-MP
- Playlists compartilhadas entre jogadores
- Caixas de som 3D (soundboxes)
- Sistema de som para veículos
- Rádio personalizada in-game

---

## 🚀 Instalação

### Pré-requisitos

- PHP 8.x ou superior
- Composer
- Windows, Linux ou macOS

### 1️⃣ Clone o Repositório

```bash
git clone https://github.com/seu-usuario/samptube-laravel.git
cd samptube-laravel
```

### 2️⃣ Instale as Dependências

```bash
composer install
```

### 3️⃣ Configure o Ambiente

```bash
cp .env.example .env
php artisan key:generate
```

### 4️⃣ Baixe os Binários

#### **yt-dlp** (Requerido)

Baixe a versão x86 (32-bit) do [yt-dlp releases](https://github.com/yt-dlp/yt-dlp/releases) e coloque em `bin/yt-dlp.exe`

**Windows:**
```bash
mkdir bin
# Baixar yt-dlp_x86.exe e renomear para yt-dlp.exe
move yt-dlp_x86.exe bin/yt-dlp.exe
```

#### **FFmpeg** (Requerido)

Baixe do [ffmpeg.org](https://ffmpeg.org/download.html) e coloque em `bin/ffmpeg.exe`

**Windows:**
```bash
# Extrair ffmpeg e copiar binários
copy ffmpeg-xxx-win64-static\bin\ffmpeg.exe bin\
```

### 5️⃣ Configure Permissões (Linux/Mac)

```bash
chmod +x bin/yt-dlp
chmod +x bin/ffmpeg
```

### 6️⃣ Crie o Diretório de Storage

```bash
php artisan storage:link
mkdir -p storage/app/public/audio
```

### 7️⃣ Inicie o Servidor

```bash
php artisan serve
```

A API estará disponível em `http://127.0.0.1:8000`

---

## 📚 Endpoints da API

### 🎵 `POST /api/convert`

Converte um vídeo do YouTube em MP3 ou busca por título.

**Parâmetros:**

| Campo | Tipo | Obrigatório | Descrição |
|-------|------|-------------|-----------|
| `url` | string | Não* | URL completa do YouTube |
| `query` | string | Não* | Termo de busca (retorna primeiro resultado) |

*Um dos dois é obrigatório

**Formatos de URL Suportados:**
- `https://www.youtube.com/watch?v=VIDEO_ID`
- `https://youtu.be/VIDEO_ID`
- `https://www.youtube.com/shorts/VIDEO_ID`
- `https://www.youtube.com/clip/CLIP_ID`

**Exemplo - Conversão por URL:**

```bash
curl -X POST http://127.0.0.1:8000/api/convert \
  -H "Content-Type: application/json" \
  -d '{"url": "https://www.youtube.com/watch?v=dQw4w9WgXcQ"}'
```

**Exemplo - Busca por Título:**

```bash
curl -X POST http://127.0.0.1:8000/api/convert \
  -H "Content-Type: application/json" \
  -d '{"query": "Linkin Park Numb"}'
```

**Resposta de Sucesso (200):**

```json
{
  "success": true,
  "message": "Converted successfully",
  "data": {
    "title": "Rick Astley - Never Gonna Give You Up",
    "filename": "Rick_Astley_-_Never_Gonna_Give_You_Up.mp3",
    "url": "http://127.0.0.1:8000/api/audio/Rick_Astley_-_Never_Gonna_Give_You_Up.mp3",
    "duration": 213000
  }
}
```

**Campos da Resposta:**
- `title`: Título do vídeo
- `filename`: Nome do arquivo MP3 sanitizado
- `url`: URL completa para streaming
- `duration`: Duração em milissegundos

---

### 📡 `GET /api/audio/{filename}`

Transmite o arquivo MP3 com suporte a HTTP Range Requests.

**Parâmetros de Query:**

| Campo | Tipo | Descrição |
|-------|------|-----------|
| `start` | int | Seek em segundos (para sincronização) |

**Headers Suportados:**
- `Range`: Para pause/resume (ex: `bytes=0-1023`)

**Exemplo - Streaming Normal:**

```bash
curl http://127.0.0.1:8000/api/audio/Rick_Astley_-_Never_Gonna_Give_You_Up.mp3
```

**Exemplo - Seek para 30 segundos:**

```bash
curl "http://127.0.0.1:8000/api/audio/Rick_Astley_-_Never_Gonna_Give_You_Up.mp3?start=30"
```

**Exemplo - Range Request (Pause/Resume):**

```bash
curl -H "Range: bytes=500000-" \
  http://127.0.0.1:8000/api/audio/Rick_Astley_-_Never_Gonna_Give_You_Up.mp3
```

**Resposta:**
- `200 OK`: Streaming completo
- `206 Partial Content`: Range request
- `404 Not Found`: Arquivo não existe

---

## ⚙️ Configuração Avançada

### Variáveis de Ambiente

```env
# .env

# Timeout para conversões longas (segundos)
YOUTUBE_TIMEOUT=600

# Diretório de storage
AUDIO_STORAGE_PATH=storage/app/public/audio
```

### Estrutura de Diretórios

```
samptube-laravel/
├── app/
│   ├── Http/
│   │   └── Controllers/
│   │       ├── AudioController.php      # Streaming com Range Requests
│   │       └── ConvertController.php    # Conversão e busca
│   ├── Services/
│   │   ├── YouTubeConverter.php         # Lógica de conversão yt-dlp
│   │   └── YouTubeUrl.php              # Validação de URLs
├── bin/
│   ├── yt-dlp.exe                       # Binário yt-dlp (x86)
│   └── ffmpeg.exe                       # Binário FFmpeg
├── storage/
│   └── app/
│       └── public/
│           └── audio/                   # Arquivos MP3 convertidos
└── routes/
    └── api.php                          # Rotas da API
```

---

## 🔧 Como Funciona

### Fluxo de Conversão

1. **Request**: Cliente envia URL ou query
2. **Validação**: Verifica formato da URL (se fornecida)
3. **Busca**: Se query, usa yt-dlp para buscar primeiro resultado
4. **Cache Check**: Verifica se já existe MP3 convertido
5. **Download**: yt-dlp baixa o vídeo
6. **Conversão**: FFmpeg extrai audio em MP3 128kbps
7. **Sanitização**: Remove caracteres especiais do nome
8. **Response**: Retorna URL de streaming

### Streaming com Sincronização

```php
// Para sincronizar players que entraram depois:
$elapsed = (time() - $start_time); // segundos desde início
$url = "/api/audio/music.mp3?start={$elapsed}";

// A API calcula: 
$byte_offset = $elapsed * 16000; // 128kbps ≈ 16KB/s
fseek($stream, $byte_offset);
```

### Cache System

- Arquivos são salvos em `storage/app/public/audio/`
- Nome do arquivo é sanitizado (remove `/\:*?"<>|`)
- Se arquivo existe, retorna imediatamente (sem reconverter)
- Para limpar cache: `rm storage/app/public/audio/*`

---

## 🐛 Troubleshooting

### Erro: "yt-dlp not found"

```bash
# Verifique se o binário existe
ls -la bin/yt-dlp.exe

# Windows: Verifique permissões
icacls bin\yt-dlp.exe

# Linux/Mac: Adicione permissão de execução
chmod +x bin/yt-dlp
```

### Erro: "Conversion failed"

```bash
# Teste yt-dlp manualmente
./bin/yt-dlp --version
./bin/yt-dlp "https://www.youtube.com/watch?v=dQw4w9WgXcQ"

# Teste FFmpeg
./bin/ffmpeg -version
```

### Erro: "File not found" no streaming

```bash
# Verifique storage link
php artisan storage:link

# Verifique permissões
chmod -R 775 storage/app/public/audio
```

### Timeout em vídeos longos

```env
# Aumente timeout no .env
YOUTUBE_TIMEOUT=1200
```

---

## 🎮 Integração com SA-MP

Veja a biblioteca PAWN em [samptube-pawn](../samptube-pawn) para integração completa com SA-MP.

**Exemplo básico em PAWN:**

```pawn
#include <samptube>

// Tocar música
SAMPTube_PlaySearch(playerid, "Linkin Park Numb");

// Adicionar à playlist
SAMPTube_AddSearchToPlaylist(playerid, "Eminem Lose Yourself");

// Criar soundbox 3D
SAMPTube_CreateBoxPlaylist(playerid, x, y, z, 50.0, 1);

// Som em veículo
SAMPTube_CreateVehiclePlaylist(playerid, vehicleid);
```

---

## 📝 Licença

Este projeto é open-source sob a licença [MIT](https://opensource.org/licenses/MIT).

---

## 🤝 Contribuindo

Contribuições são bem-vindas! Por favor:

1. Fork o projeto
2. Crie uma branch para sua feature (`git checkout -b feature/NovaFuncionalidade`)
3. Commit suas mudanças (`git commit -m 'Add: Nova funcionalidade'`)
4. Push para a branch (`git push origin feature/NovaFuncionalidade`)
5. Abra um Pull Request

---

## 📞 Suporte

- **Issues**: [GitHub Issues](https://github.com/seu-usuario/samptube-laravel/issues)
- **Documentação**: Este README
- **SA-MP Library**: [samptube-pawn](../samptube-pawn)

---

## 🙏 Créditos

- **Laravel**: Framework PHP
- **yt-dlp**: YouTube downloader
- **FFmpeg**: Audio conversion
- **SA-MP**: San Andreas Multiplayer

Desenvolvido com ❤️ para a comunidade SA-MP
