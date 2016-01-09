# FishFeeder
An IOT fish feeder powered by NETPIE platform

###Instruction

- Sign up at https://netpie.io
- Generate the App ID and get the AppKey and AppSecret

####ESP8266
- Edit the file ESP8266/FishFeeder/AppKey.h with the above AppID, AppKey and AppSecret
- Build with Arduino IDE with ESP8266 SDK

####HTML5
- Open HTML5/index.html directly with any web browsers (except IE) passing a URL argument like this
```
index.html?arg=<NETPIE_APPID>:<NETPIE_APPKEY>:<NETPIE_APPSECRET>
```
- Replace NETPIE_APPID, NETPIE_APPKEY and NETPIE_APPSECRET with the same keys in ESP8266.
- You don't need a web server to host HTML files as they can be accessed from the following URL
```
http://rawgit.com/chavee/FishFeeder/master/HTML5/index.html?arg=<NETPIE_APPID>:<NETPIE_APPKEY>:<NETPIE_APPSECRET>
```
- For your convenience, you may attch a QR code of the URL above on the feeder.
- For more information about NETPIE please visit https://netpie.io
- NETPIE microgear library for ESP8266 is available here
https://github.com/netpieio/microgear-esp8266-arduino

