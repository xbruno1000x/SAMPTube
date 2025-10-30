<?php

namespace App\Http\Controllers;
use Symfony\Component\HttpFoundation\StreamedResponse;
use Illuminate\Http\Request;

class AudioController extends Controller
{
    public function stream($filename, Request $request)
    {
        $filename = urldecode($filename);
        $path = storage_path('app/public/audio/' . $filename);

        if (!file_exists($path)) {
            abort(404, "File not found: {$path}");
        }

        $fileSize = filesize($path);
        $start = 0;
        $end = $fileSize - 1;
        $length = $fileSize;

        // Suporte a parâmetro ?start=segundos (para SA-MP)
        if ($request->has('start')) {
            $startSeconds = intval($request->input('start'));
            // MP3 bitrate médio: 128kbps = 16000 bytes/segundo
            $start = $startSeconds * 16000;
            
            // Garantir que não ultrapasse o tamanho do arquivo
            if ($start >= $fileSize) {
                $start = 0;
            }
            
            $length = $fileSize - $start;
        }
        // Suporte a Range Requests (para navegadores)
        elseif ($request->header('Range')) {
            $range = $request->header('Range');
            
            if (preg_match('/bytes=(\d+)-(\d*)/', $range, $matches)) {
                $start = intval($matches[1]);
                if (!empty($matches[2])) {
                    $end = intval($matches[2]);
                }
                $length = $end - $start + 1;
            }
        }

        $stream = function () use ($path, $start, $length) {
            $stream = fopen($path, 'rb');
            if ($start > 0) {
                fseek($stream, $start);
            }
            
            $buffer = 8192;
            $bytesRemaining = $length;
            
            while (!feof($stream) && $bytesRemaining > 0) {
                $bytesToRead = min($buffer, $bytesRemaining);
                echo fread($stream, $bytesToRead);
                $bytesRemaining -= $bytesToRead;
                flush();
            }
            
            fclose($stream);
        };

        $statusCode = ($start > 0) ? 206 : 200;
        $response = new StreamedResponse($stream, $statusCode);
        
        $response->headers->set('Content-Type', 'audio/mpeg');
        $response->headers->set('Content-Length', $length);
        $response->headers->set('Accept-Ranges', 'bytes');
        $response->headers->set('Cache-Control', 'public, max-age=86400');
        
        if ($start > 0) {
            $response->headers->set('Content-Range', "bytes {$start}-{$end}/{$fileSize}");
        }

        return $response;
    }
}
