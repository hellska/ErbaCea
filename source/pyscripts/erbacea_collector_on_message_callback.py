#!/usr/bin/env python

import time
import paho.mqtt.client as mqtt

def main():
    broker_url = "your.server.here"
    broker_port = 99999 # your port
    client_id="some_client_name"
    client_user="youUserName"
    client_pwd="YourPassword"

    client = mqtt.Client(client_id=client_id, clean_session=True)

    client.username_pw_set(client_user ,password=client_pwd)

    def on_connect(client, userdata, flags, rc):
        print("Connected with result code "+str(rc))

        # Subscribing in on_connect() means that if we lose the connection and
        # reconnect then subscriptions will be renewed.
        # client.subscribe("$SYS/#")
        client.publish(topic=client_id+"/TestinColi", payload="TestingConnez", qos=0, retain=False)

    def on_message(client, userdata, msg):
        print("Ricevuto:"+msg.topic+" "+str(msg.payload))
        messaggio = str(msg.payload.decode())
        client.publish(topic=client_id+"/ricevuto", payload=messaggio, qos=0, retain=False)

    client.on_connect = on_connect
    client.on_message = on_message

    client.connect(broker_url, broker_port, keepalive=60)

    client.subscribe([("pycea", 0), ("Testincoli", 0)])

    client.loop_start()

if __name__ == "__main__":
    main()

while True:
    time.sleep(20)
