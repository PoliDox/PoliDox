#!/bin/bash

# This small script opens three tabs and executes
# the server as well as two instances of the client
# N.B. Please take care that you build the applications in the
# right folders (../build-PoliDoxXXX/)
gnome-terminal --tab -e ../build-PoliDoxServer/polidox-server
gnome-terminal --tab -e ../build-PoliDoxClient/polidox-client
gnome-terminal --tab -e ../build-PoliDoxClient/polidox-client
