import json
import requests
import paho.mqtt.client as mqtt
from datetime import datetime  


mqtt_broker = "172.20.10.6"
mqtt_topic = "workshop6"

json_server_url = "http://localhost:3000/workshop6"

def on_connect(client, userdata, flags, rc):
    print("Connected to MQTT Broker with result code "+str(rc))
    client.subscribe(mqtt_topic)

def on_message(client, userdata, msg):
    payload = msg.payload.decode("utf-8")
    timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S") 
    print("Received message at {} on topic {}: {}".format(timestamp, msg.topic, payload))

    data = {"topic": msg.topic, "payload": payload, "timestamp": timestamp}

    send_to_json_server(data)

def send_to_json_server(data):
    headers = {"Content-Type": "application/json"}
    response = requests.post(json_server_url, data=json.dumps(data), headers=headers)

    if response.status_code in [200, 201]: 
        print("Data sent to JSON server successfully.")
    else:
        print("Failed to send data to JSON server. Status code:", response.status_code)


def main():
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message

    client.connect(mqtt_broker, 1883, 60)
    client.loop_start()

    try:
        while True:
            pass 
    except KeyboardInterrupt:
        print("Exiting...")
        client.disconnect()
        client.loop_stop()

if __name__ == "__main__":
    main()
