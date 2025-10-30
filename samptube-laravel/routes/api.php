<?php

use Illuminate\Http\Request;
use Illuminate\Support\Facades\Route;
use App\Http\Controllers\ConvertController;
use App\Http\Controllers\AudioController;

Route::post('/convert', [ConvertController::class, 'store']);
Route::get('/audio/{filename}', [AudioController::class, 'stream'])->where('filename', '.*');

Route::get('/user', function (Request $request) {
    return $request->user();
})->middleware('auth:sanctum');
