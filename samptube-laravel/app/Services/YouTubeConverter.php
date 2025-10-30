<?php

namespace App\Services;

use Illuminate\Support\Facades\Storage;
use Illuminate\Support\Facades\Log;
use Symfony\Component\Process\Exception\ProcessFailedException;
use Symfony\Component\Process\Process;

class YouTubeConverter
{
    public function search(string $query): ?string
    {
        $baseDir = base_path();
        $tempDir = storage_path('app/temp');

        if (!file_exists($tempDir)) {
            mkdir($tempDir, 0755, true);
        }

        $env = [
            'TEMP' => $tempDir,
            'TMP' => $tempDir,
            'TMPDIR' => $tempDir,
            'SystemRoot' => getenv('SystemRoot') ?: 'C:\\WINDOWS',
            'PATH' => getenv('PATH'),
            'PATHEXT' => getenv('PATHEXT'),
            'COMSPEC' => getenv('COMSPEC'),
        ];

        $process = new Process([
            config('services.youtube.ytdlp'),
            "ytsearch1:{$query}",
            '--get-id',
            '--no-warnings',
            '--no-playlist',
            '--skip-download',
        ], $baseDir, $env);

        $process->setTimeout(60);
        $process->run();

        if (!$process->isSuccessful()) {
            throw new \Exception('Erro ao buscar: ' . $process->getErrorOutput());
        }

        $videoId = trim($process->getOutput());
        
        return $videoId ? "https://www.youtube.com/watch?v={$videoId}" : null;
    }

    public function convert(string $url): array
    {
        $this->ensureDirectory();

        $baseDir = base_path();
        $tempDir = storage_path('app/temp');
        
        if (!file_exists($tempDir)) {
            mkdir($tempDir, 0755, true);
        }

        $outputDir = storage_path('app/public/audio');

        $env = [
            'TEMP' => $tempDir,
            'TMP' => $tempDir,
            'TMPDIR' => $tempDir,
            'SystemRoot' => getenv('SystemRoot') ?: 'C:\\WINDOWS',
            'PATH' => getenv('PATH'),
            'PATHEXT' => getenv('PATHEXT'),
            'COMSPEC' => getenv('COMSPEC'),
        ];

        $infoProcess = new Process([
            config('services.youtube.ytdlp'),
            '--get-title',
            '--get-duration',
            '--no-warnings',
            '--no-playlist',
            $url
        ], $baseDir, $env);

        $infoProcess->setTimeout(120);
        $infoProcess->run();

        if (!$infoProcess->isSuccessful()) {
            throw new \Exception('Não foi possível obter informações do vídeo');
        }

        $output = explode("\n", trim($infoProcess->getOutput()));
        $title = trim($output[0]);
        $durationMs = $this->parseDuration(trim($output[1]));

        $sanitizedTitle = $this->sanitizeFilename($title);
        $safeFilename = $sanitizedTitle . '.mp3';
        $outputPath = $outputDir . '/' . $safeFilename;

        // Se já existe, retornar (cache)
        if (file_exists($outputPath)) {
            Log::info("Arquivo já existe (cache): {$outputPath}");
            return [
                'title' => $title,
                'duration' => $durationMs,
                'filename' => $safeFilename
            ];
        }

        $downloadProcess = new Process([
            config('services.youtube.ytdlp'),
            '--extract-audio',
            '--audio-format', 'mp3',
            '--audio-quality', '0',
            '--ffmpeg-location', config('services.youtube.ffmpeg'),
            '-o', $outputPath,  // Output direto para o arquivo final
            '--no-warnings',
            '--no-check-certificates',
            '--no-playlist',
            $url
        ], $baseDir, $env);

        $downloadProcess->setTimeout(600);
        $downloadProcess->run();

        if (!$downloadProcess->isSuccessful()) {
            throw new ProcessFailedException($downloadProcess);
        }

        // Verificar se o arquivo foi criado
        if (!file_exists($outputPath)) {
            throw new \Exception("Arquivo não foi criado: {$outputPath}");
        }

        Log::info("Arquivo criado com sucesso: {$outputPath}");

        return [
            'title' => $title,
            'duration' => $durationMs,
            'filename' => $safeFilename
        ];
    }

    protected function sanitizeFilename(string $filename): string
    {
        $filename = str_replace(['$', '\\', '/', ':', '*', '?', '"', '<', '>', '|'], '', $filename);
        
        $filename = preg_replace('/\s+/', ' ', $filename);
        
        if (strlen($filename) > 200) {
            $filename = substr($filename, 0, 200);
        }
        
        return trim($filename);
    }

    protected function parseDuration(string $duration): int
    {
        $parts = array_reverse(explode(':', $duration));
        $seconds = 0;
        $multiplier = 1;

        foreach ($parts as $part) {
            $seconds += (int)$part * $multiplier;
            $multiplier *= 60;
        }

        return $seconds * 1000;
    }

    protected function ensureDirectory(): void
    {
        $disk = config('services.audio.disk');
        $path = config('services.audio.path');

        if (!Storage::disk($disk)->exists($path)) {
            Storage::disk($disk)->makeDirectory($path);
        }
    }
}
