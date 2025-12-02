<script lang="ts">
    import mqtt from "mqtt";
    const client = mqtt.connect("ws://localhost:8080");

    let snoupList: string[] = $state([]);
    client.on("connect", () => {
        console.log("Connecté !");

        client.subscribe("capteurs/#", (err) => {
          if (!err) {
          }
        });
        client.subscribe("snoup");
    });

    client.on("message", (topic, message) => {
        if (topic === "capteurs/alpha/rssi") {
            const valeurRSSI = message.toString();
        } else if (topic.startsWith("capteurs/rssi")) {
          console.log(topic.split('/').pop(), ':', Number(message.toString()));
        } else if (topic === "snoup") {
          console.log("Message sur snoup :", message.toString());
          snoupList.push(message.toString());
        }
    });


    import.meta.hot?.on("vite:beforeUpdate", () => {
        client.end();
    });
    import.meta.hot?.on("vite:afterUpdate", () => {
        client.reconnect();
    });
</script>

<main class="h-screen w-screen flex flex-col bg-gray-700 text-white items-center justify-center gap-4">
    <h1 class="text-3xl font-bold underline">Snoup Messages</h1>
    <ul>
      {#each snoupList as snoup}
        <li>{snoup}</li>
      {/each}
    </ul>
</main>