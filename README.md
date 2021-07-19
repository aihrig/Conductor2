# Conductor2 (WIP)

## This project is a (re)build of my LEGO train control system.

I wanted to dig into IoT tech stacks and use the LEGO train as a platform. The train's infra-red control system has been replaced with an Arduino based microcontroller, connects via WiFi to a pub-sub serivce to get its instructions.

An intermediary controller run on Node.JS to manage other various connections and activities for the track and supporting systems (switching tracks, automatic wireless charging, RFID tracking of cars, etc.) I'm still exploring controlling the train via pub/sub and/or REST API.

I am currently rebuilding everything from scratch, so this is a work in progress (isn't it always? :wink: ).
