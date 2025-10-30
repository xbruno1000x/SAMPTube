<?php

namespace App\Rules;

use Closure;
use Illuminate\Contracts\Validation\ValidationRule;

class YouTubeUrl implements ValidationRule
{
    /**
     * Run the validation rule.
     */
    public function validate(string $attribute, mixed $value, Closure $fail): void
    {
        if (!$this->isValidYouTubeUrl($value)) {
            $fail('A URL fornecida não é uma URL válida do YouTube.');
        }
    }

    /**
     * Verifica se a URL é uma URL válida do YouTube
     */
    private function isValidYouTubeUrl(string $url): bool
    {
        // Padrões aceitos do YouTube
        $patterns = [
            '/youtube\.com\/watch\?v=([a-zA-Z0-9_-]+)/',           // youtube.com/watch?v={vidid}
            '/youtube\.com\/watch\?vi=([a-zA-Z0-9_-]+)/',          // youtube.com/watch?vi={vidid}
            '/youtube\.com\/\?v=([a-zA-Z0-9_-]+)/',                // youtube.com/?v={vidid}
            '/youtube\.com\/\?vi=([a-zA-Z0-9_-]+)/',               // youtube.com/?vi={vidid}
            '/youtube\.com\/v\/([a-zA-Z0-9_-]+)/',                 // youtube.com/v/{vidid}
            '/youtube\.com\/vi\/([a-zA-Z0-9_-]+)/',                // youtube.com/vi/{vidid}
            '/youtu\.be\/([a-zA-Z0-9_-]+)/',                       // youtu.be/{vidid}
            '/youtube\.com\/embed\/([a-zA-Z0-9_-]+)/',             // youtube.com/embed/{vidid}
            '/youtube\.com\/shorts\/([a-zA-Z0-9_-]+)/',            // youtube.com/shorts/{vidid}
            '/youtube\.com\/clip\/([a-zA-Z0-9_-]+)/',              // youtube.com/clip/{clipid}
        ];

        foreach ($patterns as $pattern) {
            if (preg_match($pattern, $url)) {
                return true;
            }
        }

        return false;
    }

    /**
     * Extrai o ID do vídeo da URL
     */
    public static function extractVideoId(string $url): ?string
    {
        $patterns = [
            '/youtube\.com\/watch\?v=([a-zA-Z0-9_-]+)/',
            '/youtube\.com\/watch\?vi=([a-zA-Z0-9_-]+)/',
            '/youtube\.com\/\?v=([a-zA-Z0-9_-]+)/',
            '/youtube\.com\/\?vi=([a-zA-Z0-9_-]+)/',
            '/youtube\.com\/v\/([a-zA-Z0-9_-]+)/',
            '/youtube\.com\/vi\/([a-zA-Z0-9_-]+)/',
            '/youtu\.be\/([a-zA-Z0-9_-]+)/',
            '/youtube\.com\/embed\/([a-zA-Z0-9_-]+)/',
            '/youtube\.com\/shorts\/([a-zA-Z0-9_-]+)/',
            '/youtube\.com\/clip\/([a-zA-Z0-9_-]+)/',
        ];

        foreach ($patterns as $pattern) {
            if (preg_match($pattern, $url, $matches)) {
                return $matches[1];
            }
        }

        return null;
    }

    /**
     * Normaliza a URL para o formato padrão do YouTube
     */
    public static function normalize(string $url): ?string
    {
        // Para clips, o yt-dlp consegue processar a URL diretamente
        if (preg_match('/youtube\.com\/clip\//', $url)) {
            return $url;
        }

        $videoId = self::extractVideoId($url);
        
        if (!$videoId) {
            return null;
        }

        return "https://www.youtube.com/watch?v={$videoId}";
    }
}
