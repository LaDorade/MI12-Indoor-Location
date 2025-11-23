<script lang="ts">
    import mqtt from "mqtt";
    const client = mqtt.connect("ws://192.168.1.92:8888");

    client.on("connect", () => {
        console.log("Connecté !");

        client.subscribe("capteurs/#", (err) => {
          if (!err) {
            console.log("hyop");
            
          }
        });
    });

    // 2. C'est ici que TOUS les messages arrivent
    client.on("message", (topic, message) => {
        // 3. On filtre pour être sûr de traiter le bon canal
        if (topic === "capteurs/alpha/rssi") {
            // IMPORTANT : 'message' est un Buffer (des octets), il faut le convertir en texte
            const valeurRSSI = message.toString();

            // Ici, vous pouvez mettre à jour votre interface (State React, Vue, DOM, etc.)
            // ex: setRssi(valeurRSSI);
        } else if (topic.startsWith("capteurs/rssi")) {
          console.log(topic.split('/').pop(), ':', Number(message.toString()));
        }
        
    });
</script>

<h1>Welcome to SvelteKit</h1>
<p>
    Visit <a href="https://svelte.dev/docs/kit">svelte.dev/docs/kit</a> to read the
    documentation
</p>
