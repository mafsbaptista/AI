# AI 2018/2019

Ambient Intelligence Project - HomeControl

## Requirements
You must have installed the following tools:
- Java Development Kit 8 (JDK 8)

To run the server locally:
- npm 6.4.x 
- node 10.x

## Getting started

### Arduino
Donwload the library located at https://github.com/washo4evr/Socket.io-v1.x-Library and save it in the Arduino libraries folder.
Upload the code located in the directory **/Arduino/** to the Arduino board.


## How to run
### Server
To run the server locally, run the following command in the directory **/Server/**
```
npm install
npm start
```
The server will be available at _http://localhost:3000_

If you have a heroku virtual machine configured, you can updated it by running the following command in the **root** directory:
```
git subtree push --prefix Server heroku master
```
The server will be available at _https://<name_of_the_machine>.herokuapp.com_.

We have a server deployed with the most recent code at https://homecontrolserver.herokuapp.com

### Arduino
Depending whether your server is running locally or on a virtual machine, you need to write the server url in the arduino code (line 14).
By default, the server is _homecontrolserver.herokuapp.com_.

Connect the arduino to a power source.
