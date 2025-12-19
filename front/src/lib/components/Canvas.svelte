<script>
    import { dangerAreaStore } from "$lib/stores/DangerArea.svelte";
    import { tril } from "$lib/stores/Trilaterate.svelte";

</script>
<div
    class="relative bg-gray-900 border border-gray-600 rounded-lg shadow-2xl"
    style="width: 70vh; height: 70vh;"
>
    <div class="absolute" style="inset: -15%; width: 130%; height: 130%;">
        <!-- background grid -->
        <div
            class="absolute opacity-20 border border-gray-500"
            style="left: 11.5%; top: 11.5%; width: 77%; height: 77%;
                            background-image: linear-gradient(#444 1px, transparent 1px), linear-gradient(90deg, #444 1px, transparent 1px); 
                            background-size: {100 / tril.ROOM_SIZE}% {100 /
                tril.ROOM_SIZE}%;"
        ></div>

        <!-- Danger zone-->
        <div
            class="absolute border-2 border-red-500 bg-red-500/20"
            style="
                        left: {((dangerAreaStore.rectangle.x + 1) /
                (tril.ROOM_SIZE + 2)) *
                100}%; top: {((dangerAreaStore.rectangle.y + 1) /
                (tril.ROOM_SIZE + 2)) *
                100}%;
                        width: {(dangerAreaStore.rectangle.width /
                (tril.ROOM_SIZE + 2)) *
                100}%; height: {(dangerAreaStore.rectangle.height /
                (tril.ROOM_SIZE + 2)) *
                100}%;
                     "
        ></div>

        <!-- Balises et cercles de distance -->
        {#each Object.entries(tril.sensorStore.sensors) as [key, sensor]}
            {@const dist = tril.getDistance(tril.readings[key], sensor.rssi1m)}
            {@const margin = 1}
            {@const totalSize = tril.ROOM_SIZE + 2 * margin}
            {@const pxX = ((sensor.x + margin) / totalSize) * 100}
            {@const pxY = ((sensor.y + margin) / totalSize) * 100}

            <div
                class="absolute w-4 h-4 -ml-2 -mt-2 rounded-full {sensor.color} z-20 shadow-[0_0_10px_currentColor]"
                style="left: {pxX}%; top: {pxY}%;"
            >
                <span
                    class={[
                        "absolute -top-10 left-1/2 -translate-x-1/2 text-[10px] font-bold",
                        "flex flex-col items-center",
                        sensor.color.replace("bg-", "text-"),
                    ]}
                >
                    {key}
                    <span class=" text-nowrap">
                        x:{sensor.x}m y:{sensor.y}m
                    </span>
                </span>
            </div>

            <div
                class="absolute rounded-full border border-dashed opacity-30 pointer-events-none transition-all duration-300"
                style="
                            border-color: var(--tw-bg-opacity);
                            left: {pxX}%; top: {pxY}%;
                            width: {(dist / totalSize) * 200}%; height: {(dist /
                    totalSize) *
                    200}%;
                            transform: translate(-50%, -50%);
                            color: {sensor.color.replace('bg-', 'text-')}; 
                         "
            ></div>
        {/each}

        <!-- Position estimée -->
        <div
            class="absolute w-0 h-0 transition-all duration-300 ease-linear z-30"
            style="left: {((tril.position.x + 1) / (tril.ROOM_SIZE + 2)) *
                100}%; top: {((tril.position.y + 1) / (tril.ROOM_SIZE + 2)) *
                100}%;"
        >
            <div
                class={[
                    "absolute font-bold",
                    "text-[10px] flex flex-col items-center -top-10 left-1/2 -translate-x-1/2",
                ]}
            >
                You
                <span class=" text-nowrap">
                    x:{tril.position.x.toFixed(2)}m y:{tril.position.y.toFixed(
                        2,
                    )}m
                </span>
            </div>
            <div class="absolute -translate-x-1/2 -translate-y-1/2">
                <div
                    class="w-4 h-4 bg-white rounded-full animate-bounce shadow-[0_0_20px_white]"
                ></div>
                <div
                    class="w-12 h-12 border-2 border-white/50 rounded-full absolute top-1/2 left-1/2 -translate-x-1/2 -translate-y-1/2 animate-ping"
                ></div>
            </div>
        </div>

        <!-- Danger overlay-->
        {#if tril.isInDangerZone(dangerAreaStore.rectangle)}
            <div
                class="absolute inset-0 bg-red-600/30 backdrop-blur-sm pointer-events-none animate-pulse"
            ></div>
        {/if}

        <!-- Axes labels -->
        <div>
            <div
                class="absolute text-gray-500 text-[10px] select-none"
                style="left: 50%; top: 100%; transform: translateX(-50%) translateY(5px);"
            >
                X (m)
            </div>
            <div
                class="absolute text-gray-500 text-[10px] select-none"
                style="left: 0%; top: 50%; transform: translateX(-5px) translateY(-50%) rotate(-90deg);"
            >
                Y (m)
            </div>
        </div>
    </div>
</div>
