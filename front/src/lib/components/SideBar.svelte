<script>
    import { dangerAreaStore } from "$lib/stores/DangerArea.svelte";
    import { tril } from "$lib/stores/Trilaterate.svelte";
    import SensorConf from "./SensorConf.svelte";
</script>

<div
    class="w-96 bg-gray-900 flex flex-col overflow-y-auto "
>
    <div class="p-4 ">
        <h2 class="font-bold text-lg text-gray-200">Configuration</h2>
    </div>

    <!-- Paramètres globaux -->
    <div class="p-4 bg-gray-800/50">
        <h3 class="font-semibold text-sm text-gray-300 mb-3">
            Paramètres de propagation
        </h3>

        <div class="space-y-1">
            <div>
                <label
                    for="pathLoss"
                    class="text-xs text-gray-400 flex justify-between"
                >
                    <span>Path Loss Exponent (N)</span>
                    <span class="text-cyan-400 font-mono"
                        >{tril.N_ENV.toFixed(1)}</span
                    >
                </label>
                <input
                    name="pathLoss"
                    type="range"
                    min="1.5"
                    max="5"
                    step="0.1"
                    bind:value={tril.N_ENV}
                    class="w-full h-2 bg-gray-700 rounded-lg appearance-none cursor-pointer accent-cyan-500"
                />
                <div class="flex justify-between text-[10px] text-gray-500">
                    <span>Espace libre (2.0)</span>
                    <span>Indoor dense (4.0+)</span>
                </div>
            </div>
            <div>
                Triangulation method:
                <select
                    bind:value={tril.trilaterateStrategy}
                    class="bg-gray-800 border-b border-gray-600 w-full text-sm focus:outline-none"
                >
                    <option value="weighted">Weighted</option>
                    <option value="simple">Simple</option>
                </select>
            </div>
        </div>
    </div>

    <div class="flex flex-col gap-2 p-2 justify-between h-full">
        <div class="flex flex-col gap-2">
            {#each Object.entries(tril.sensorStore.sensors) as [key, sensor]}
                <SensorConf {sensor} {key} readings={tril.readings} />
            {/each}
        </div>

        <div>
            <span>Zone de danger</span>
            <div class="grid grid-cols-2 gap-1 p-2">
                <div>
                    <label for="dangerX" class="text-xs text-gray-400 block"
                        >X (m)</label
                    >
                    <input
                        id="dangerX"
                        type="number"
                        bind:value={dangerAreaStore.rectangle.x}
                        class="bg-gray-800 border-b border-gray-600 w-20 text-sm focus:outline-none"
                    />
                </div>
                <div>
                    <label for="dangerY" class="text-xs text-gray-400 block"
                        >Y (m)</label
                    >
                    <input
                        id="dangerY"
                        type="number"
                        bind:value={dangerAreaStore.rectangle.y}
                        class="bg-gray-800 border-b border-gray-600 w-20 text-sm focus:outline-none"
                    />
                </div>
                <div>
                    <label for="dangerWidth" class="text-xs text-gray-400 block"
                        >Largeur (m)</label
                    >
                    <input
                        id="dangerWidth"
                        type="number"
                        bind:value={dangerAreaStore.rectangle.width}
                        class="bg-gray-800 border-b border-gray-600 w-20 text-sm focus:outline-none"
                    />
                </div>
                <div>
                    <label for="dangerHeight" class="text-xs text-gray-400 block"
                        >Hauteur (m)</label
                    >
                    <input
                        id="dangerHeight"
                        type="number"
                        bind:value={dangerAreaStore.rectangle.height}
                        class="bg-gray-800 border-b border-gray-600 w-20 text-sm focus:outline-none"
                    />
                </div>
            </div>
        </div>

        <button
            onclick={() => {
                tril.sensorStore.resetCalibration();
            }}
            class={["w-full text-xs bg-gray-700 hover:bg-gray-600 py-2 rounded transition",
                "border border-red-500 text-red-400 hover:text-white",
                'cursor-pointer'
            ]}
        >
            Réinitialiser calibration
        </button>
    </div>
</div>
