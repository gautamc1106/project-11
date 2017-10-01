################################################################################
# Makefile                                                                     #
#                                                                              #
# Description: This file contains the make rules for Recitation 1.             #
#                                                                              #
# Authors: Athula Balachandran <abalacha@cs.cmu.edu>,                          #
#          Wolf Richter <wolf@cs.cmu.edu>                                      #
#                                                                              #
################################################################################

default: server echo_client

server:
	@gcc server.c -o server -Wall -Werror

echo_client:
	@gcc echo_client.c -o echo_client -Wall -Werror

clean:
	@rm server echo_client
