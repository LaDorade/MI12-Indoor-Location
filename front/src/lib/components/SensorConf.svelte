<script lang="ts">
    import { tril } from "$lib/stores/Trilaterate.svelte";
    import type { Sensor } from "../../types";

    interface Props {
        sensor: Sensor;
        key: string;
        readings: Record<string, number>;
    }

    let {
        sensor,
        key,
        readings
    }: Props = $props();
</script>
<div class="bg-gray-800 rounded-xl p-4 py-2 shadow-sm">
    <div class="flex justify-between items-center mb-3">
        <div class="flex items-center gap-2">
            <div class="w-3 h-3 rounded-full {sensor.color}"></div>
            <span class="font-bold text-lg">{key}</span>
        </div>
        <div class="text-xs text-gray-500">
            <input type="number" bind:value={sensor.x} class="bg-transparent w-12 border-b border-gray-600 focus:outline-none text-right mr-1" /> ,
            <input type="number" bind:value={sensor.y} class="bg-transparent w-12 border-b border-gray-600 focus:outline-none text-right ml-1" />
        </div>
    </div>

    <div class="grid grid-cols-2 gap-2 m-3">
        <div class="bg-gray-900 p-1 rounded text-center">
            <span class="block text-xs text-gray-500">RSSI (Live)</span>
            <span class="font-mono text-lg font-bold"
                >{readings[key].toFixed(1)}</span
            >
        </div>
        <div class="bg-gray-900 p-1 rounded text-center">
            <span class="block text-xs text-gray-500">Distance Est.</span>
            <span class="font-mono text-lg text-blue-400">
                {tril.getDistance(readings[key], sensor.rssi1m).toFixed(2)}m
            </span>
        </div>
    </div>

    <div class="flex items-center gap-2 pt-2">
        <div class="flex-1">
            <label for={"rssi" + key} class="text-xs text-gray-400 block"
                >Ref @ 1m (dBm)</label
            >
            <input
                name={"rssi" + key}
                type="number"
                bind:value={sensor.rssi1m}
                class="bg-transparent border-b border-gray-600 w-20 text-sm focus:outline-none"
            />
        </div>
        <button
            onclick={() => {
                sensor.rssi1m = readings[key];
            }}
            class="text-xs bg-indigo-600 hover:bg-indigo-500 px-3 py-1 rounded transition"
            title="Utiliser le RSSI actuel comme référence (placer le tag à 1m)"
        >
            Capturer
        </button>
    </div>
</div>
