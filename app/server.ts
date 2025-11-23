import { createServer } from 'node:net'
import Aedes from 'aedes'
import mqtt from 'mqtt'

const aedes = new Aedes();
const server = createServer(aedes.handle)

server.listen(1883, function () {
  console.log('server started and listening on port ', 1883)
})

aedes.on('client', function (client) {
  console.log('Client Connected: ' + (client ? client.id : client) )
});

aedes.on('publish', async function (packet, client) {
  if (client) {
    console.log('Message from client', client.id);
  }
  console.log('Published', packet.payload.toString());
});

aedes.on('subscribe', function (subscriptions, client) {
  console.log(
    'Client ' + (client ? client.id : client) +
    ' subscribed to topics: ' + subscriptions.map(s => s.topic).join(', ') + ''
  )
});

aedes.on('unsubscribe', function (subscriptions, client) {
  console.log(
    'Client ' + (client ? client.id : client) +
    ' unsubscribed from topics: ' + subscriptions.join(', ') + ''
  )
});
