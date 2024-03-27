# EspTelegraph

Hosts several tasks, including:

* Connecting to a wireless network
* Providing a temrinal console via serial to allow setting wifi parameters and testing morse output
* Hosting an HTTP server, allowing users to input morse commands
* Translating ASCII text into morse commands to send via discrete output
* Holding a queue of messages, allowing a certain number of messages to be held in reserve

The web server contains an API that allows external programs to request morse messages via the `/api` endpoint with an HTTP GET request. API endpoint options include:

* `msg`: The message to output via morse.
* `unit_millis`: The number of milliseconds per morse unit. By default, this is 50 ms if this option is either omitted or <= 0.

Morse units are translated such that a dot is one unit on, one unit off, a dash is three units on, one unit off, a space between words is seven units off, and the space between characters is three units off.
