<?php

namespace App\Http\Controllers;

use App\Services\YouTubeConverter;
use App\Rules\YouTubeUrl;
use Illuminate\Http\Request;
use Illuminate\Support\Facades\Log;

class ConvertController extends Controller
{
    public function store(Request $request, YouTubeConverter $converter)
    {
        // Aumentar tempo de execução
        set_time_limit(600);
        
        $data = $request->validate([
            'url' => ['nullable', new YouTubeUrl()],
            'query' => ['nullable', 'string', 'min:3']
        ]);

        if (empty($data['url']) && empty($data['query'])) {
            return response()->json([
                'error' => 'Forneça uma URL ou query de busca'
            ], 400);
        }

        try {
            $url = $data['url'] ?? null;

            if ($url) {
                $url = YouTubeUrl::normalize($url);
                if (!$url) {
                    return response()->json([
                        'error' => 'URL do YouTube inválida'
                    ], 400);
                }
            }

            if (!$url && !empty($data['query'])) {
                Log::info('Buscando por: ' . $data['query']);
                $url = $converter->search($data['query']);
                Log::info('URL encontrada: ' . ($url ?? 'null'));
                
                if (!$url) {
                    return response()->json([
                        'error' => 'Nenhum resultado encontrado para a busca'
                    ], 404);
                }
            }

            Log::info('Convertendo URL: ' . $url);
            $result = $converter->convert($url);
            Log::info('Resultado: ' . json_encode($result));

            $audioUrl = url('api/audio/' . urlencode($result['filename']));

            return response()->json([
                'title' => $result['title'],
                'duration' => $result['duration'],
                'audioUrl' => $audioUrl,
            ]);
        } catch (\Exception $e) {
            Log::error('Erro na conversão: ' . $e->getMessage());
            Log::error($e->getTraceAsString());
            return response()->json([
                'error' => 'Erro ao converter vídeo',
                'details' => $e->getMessage()
            ], 500);
        }
    }
}
