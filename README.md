# 🎵 SAMPTube

> Sistema completo de reprodução de áudio do YouTube para servidores SA-MP

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Laravel](https://img.shields.io/badge/Laravel-12.35.1-red)](https://laravel.com)
[![PAWN](https://img.shields.io/badge/PAWN-SA--MP-blue)](https://www.sa-mp.com)

## 📋 Sobre

SAMPTube é um sistema completo que permite aos servidores SA-MP reproduzir áudio do YouTube para os jogadores. O projeto consiste em duas partes principais:

1. **samptube-laravel**: API REST em Laravel que converte vídeos do YouTube para MP3
2. **samptube-pawn**: Biblioteca PAWN para integração com SA-MP

## ✨ Características

### API Laravel
- ✅ Conversão de vídeos do YouTube para MP3 (128kbps)
- ✅ Busca de vídeos por termo
- ✅ Suporte a múltiplos formatos de URL do YouTube
- ✅ Streaming com HTTP Range Requests
- ✅ Sistema de cache para evitar conversões duplicadas
- ✅ Funcionalidade de seek (avanço/retrocesso)

### Biblioteca PAWN
- ✅ Reprodução de música pessoal para jogadores
- ✅ Sistema de playlists completo (adicionar, remover, navegar)
- ✅ Pausar e resumir reprodução
- ✅ Soundboxes 3D com áudio posicional
- ✅ Sistema de áudio para veículos
- ✅ Auto-avanço de faixas em playlists
- ✅ Suporte a 100 soundboxes e 100 veículos simultaneamente

## 🚀 Início Rápido

### 1. Configure a API

```bash
cd samptube-laravel
composer install
php artisan key:generate
php artisan storage:link
php artisan serve
```

**⚠️ Importante**: Baixe `yt-dlp.exe` e `ffmpeg.exe` e coloque em `samptube-laravel/bin/`

📖 [Documentação completa da API](./samptube-laravel/README.md)

### 2. Instale a biblioteca PAWN

**Via sampctl:**
```bash
sampctl package install xbruno1000x/SAMPTube
```

**Manual:**
1. Baixe as dependências:
   - [pawn-requests](https://github.com/Southclaws/pawn-requests)
   - [pawn-map](https://github.com/BigETI/pawn-map)
   - [streamer plugin](https://github.com/samp-incognito/samp-streamer-plugin)
   - [MapAndreas](https://github.com/Southclaws/samp-plugin-mapandreas)

2. Adicione no seu gamemode:
```pawn
#define SAMPTUBE_API_URL "http://localhost:8000"
#include <samptube>
```

📖 [Documentação completa da biblioteca PAWN](./samptube-pawn/README.md)

## 📂 Estrutura do Projeto

```
SAMPTube/
├── samptube-laravel/       # API REST em Laravel
│   ├── app/
│   │   ├── Http/Controllers/
│   │   │   ├── ConvertController.php
│   │   │   └── AudioController.php
│   │   └── Services/
│   │       ├── YouTubeConverter.php
│   │       └── YouTubeUrl.php
│   ├── bin/                # Binários (yt-dlp, ffmpeg)
│   ├── storage/app/public/audio/  # Arquivos MP3 convertidos
│   └── README.md           # Documentação da API
│
└── samptube-pawn/          # Biblioteca PAWN
    ├── samptube.inc        # Arquivo principal da biblioteca
    ├── exemplo.pwn         # Gamemode de exemplo
    └── README.md           # Documentação da biblioteca
```

## 💡 Exemplo de Uso

### Comandos In-Game

```pawn
// Reprodução pessoal
/tocar <URL>              // Reproduz música por URL
/buscar <termo>           // Busca e reproduz
/pausar                   // Pausa a música
/resumir                  // Resume a reprodução

// Playlists
/playlist <nome>          // Cria playlist
/adicionar <URL>          // Adiciona à playlist
/remover <índice>         // Remove da playlist
/proxima                  // Próxima música
/anterior                 // Música anterior

// Soundboxes 3D
/criarcaixa               // Cria soundbox na posição
/caixaplaylist <nome>     // Playlist para soundbox

// Áudio em veículos
/somveiculo <nome>        // Cria playlist no veículo
/proximaveiculo           // Próxima música no veículo
/removersomveiculo        // Remove áudio do veículo
```

## 🔧 Requisitos

### API
- PHP 8.0+
- Composer
- Laravel 12.35.1
- yt-dlp (binary)
- FFmpeg (binary)

### PAWN
- SA-MP Server 0.3.7+
- pawn-requests
- pawn-map
- streamer plugin
- MapAndreas plugin

## 📝 Documentação Detalhada

- **[API Laravel](./samptube-laravel/README.md)**: Endpoints, instalação, configuração
- **[Biblioteca PAWN](./samptube-pawn/README.md)**: 43 funções documentadas, exemplos, troubleshooting

## 🤝 Contribuindo

Contribuições são bem-vindas! Por favor:

1. Fork o projeto
2. Crie uma branch para sua feature (`git checkout -b feature/AmazingFeature`)
3. Commit suas mudanças (`git commit -m 'Add some AmazingFeature'`)
4. Push para a branch (`git push origin feature/AmazingFeature`)
5. Abra um Pull Request

## 📄 Licença

Este projeto é licenciado sob a [Creative Commons Attribution-NonCommercial 4.0 International License (CC BY-NC 4.0)](https://creativecommons.org/licenses/by-nc/4.0/legalcode.pt).


## 👤 Autor

**xbruno1000x**

- GitHub: [@xbruno1000x](https://github.com/xbruno1000x)

## 🙏 Créditos

- [yt-dlp](https://github.com/yt-dlp/yt-dlp) - Downloader do YouTube
- [FFmpeg](https://ffmpeg.org) - Conversão de áudio
- [pawn-requests](https://github.com/Southclaws/pawn-requests) - Cliente HTTP para PAWN
- [pawn-map](https://github.com/BigETI/pawn-map) - Estrutura de dados Map
- [streamer plugin](https://github.com/samp-incognito/samp-streamer-plugin) - Objetos dinâmicos
- [MapAndreas](https://github.com/Southclaws/samp-plugin-mapandreas) - Detecção de altitude

---

⭐ Se este projeto foi útil, considere dar uma estrela!
