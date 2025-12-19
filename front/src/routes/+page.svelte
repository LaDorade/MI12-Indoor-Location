<script lang="ts">
    import mqtt from "mqtt";
    import SideBar from "$lib/components/SideBar.svelte";
    import { tril } from "$lib/stores/Trilaterate.svelte";
    import { onDestroy, onMount } from "svelte";
    import Canvas from "$lib/components/Canvas.svelte";
    import { dangerAreaStore } from "$lib/stores/DangerArea.svelte";

    let isConnected = $state(false);

    let client: ReturnType<typeof mqtt.connect>;

    onMount(() => {
        client = mqtt.connect("ws://192.168.4.99:8080");

        client.on("connect", () => {
            isConnected = true;
            client.subscribe("capteurs/rssi/#");
        });

        client.on("message", (topic, message) => {
            const beaconName = topic.split('/').pop();
            if (beaconName && beaconName in tril.sensorStore.sensors) {
                tril.readings[beaconName] = Number(message.toString());
            }
        });

        client.on("close", () => {
            isConnected = false;
        });
    });
    
    let lastVal = false;
    $effect(() => {
        if (tril.isInDangerZone(dangerAreaStore.rectangle) && 
        tril.isInDangerZone(dangerAreaStore.rectangle) !== lastVal) {
            console.log("danger");
            client.publish("capteurs/rssi/mobile", "danger")
            lastVal = true
        } 
        if ( !tril.isInDangerZone(dangerAreaStore.rectangle) &&
        tril.isInDangerZone(dangerAreaStore.rectangle) !== lastVal) {
            console.log("ok");
            client.publish("capteurs/rssi/mobile", "ok")
            lastVal = false;
        }
    });

    onDestroy(() => client?.end());
</script>

<div class="h-screen w-screen flex bg-gray-900 text-white font-sans overflow-hidden">
    <div class="flex-1 relative bg-gray-800 flex items-center justify-center border-r border-gray-700">
        
        <div class="absolute top-4 left-4 z-10">
            <h1 class="text-2xl font-bold bg-clip-text text-transparent bg-blue-500">
                MI12 Trilateration Radio
            </h1>
            <p class="text-xs text-gray-400 mt-1">
                MQTT: <span class={isConnected ? "text-green-400" : "text-red-400"}>{isConnected ? "Connecté" : "Déconnecté"}</span>
            </p>
            <p class="text-xs text-gray-400">Pos: {tril.position.x.toFixed(2)}m, {tril.position.y.toFixed(2)}m</p>
        </div>
        <Canvas />
    </div>

    <SideBar />
</div>