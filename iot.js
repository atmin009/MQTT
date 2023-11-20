const express = require('express');
const mqtt = require('mqtt');

const app = express();
const port = 4000;

const mqttBroker = 'mqtt://172.20.10.6';
const mqttTopic = 'LED';

const client = mqtt.connect(mqttBroker);

app.use(express.static('public'));

app.get('/', (req, res) => {
  res.sendFile(__dirname + '/index.html');
});

app.listen(port, () => {
  console.log(`Server running at http://localhost:${port}`);
});

client.on('connect', function () {
  console.log('Connected to MQTT broker');
});

client.on('error', function (error) {
  console.error('MQTT error:', error);
});

app.get('/toggle', (req, res) => {
  const isOn = req.query.state === '1';
  const message = isOn ? 'On' : 'Off';
  client.publish(mqttTopic, message);

  res.send(`LED is ${isOn ? 'On' : 'Off'}`);
});