import Aedes from 'aedes'
import {createServer} from 'net'
import http from 'http'
import ws from 'websocket-stream'

// --- Configuration des Ports ---
const PORT_TCP = 1883  // Pour les objets connectés / backend
const PORT_WS = 8888   // Pour votre Front-end (WebSockets)

const aedesInstance = new Aedes()

// 1. Création du serveur TCP standard
const server = createServer(aedesInstance.handle)

server.listen(PORT_TCP, function () {
  console.log('✅ Serveur MQTT (TCP) démarré sur le port', PORT_TCP)
})

// 2. Création du serveur HTTP pour les WebSockets
const httpServer = http.createServer()

// C'est ICI que la magie opère : on lie le flux WebSocket à Aedes
ws.createServer({ server: httpServer }, aedesInstance.handle)

httpServer.listen(PORT_WS, function () {
  console.log('✅ Serveur MQTT (Over WebSockets) démarré sur le port', PORT_WS)
})

// --- Logs pour voir ce qu'il se passe ---
aedesInstance.on('client', function (client) {
  console.log('Nouveau client connecté :', client ? client.id : client)
})