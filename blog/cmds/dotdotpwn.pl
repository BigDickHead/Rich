#################################################################################
#                                                                               #
#  CubilFelino                                                       Chatsubo   #
#  Security Research Lab              and            [(in)Security Dark] Labs   #
#  chr1x.sectester.net                             chatsubo-labs.blogspot.com   #
#                                                                               #
#                               pr0udly present:                                #
#                                                                               #
#  ________            __  ________            __  __________                   #
#  \______ \    ____ _/  |_\______ \    ____ _/  |_\______   \__  _  __ ____    #
#   |    |  \  /  _ \\   __\|    |  \  /  _ \\   __\|     ___/\ \/ \/ //    \   #
#   |    `   \(  <_> )|  |  |    `   \(  <_> )|  |  |    |     \     /|   |  \  #
#  /_______  / \____/ |__| /_______  / \____/ |__|  |____|      \/\_/ |___|  /  #
#          \/                      \/                                      \/   #
#                               - DotDotPwn v3.0 -                              #
#                         The Directory Traversal Fuzzer                        #
#                         http://dotdotpwn.sectester.net                        #
#                            dotdotpwn@sectester.net                            #
#                                                                               #
#                               by chr1x & nitr0us                              #
#################################################################################

Usage: ./dotdotpwn.pl -m <module> -h <host> [OPTIONS]
	Available options:
	-m	Module [http | http-url | ftp | tftp | payload | stdout]
	-h	Hostname
	-O	Operating System detection for intelligent fuzzing (nmap)
	-o	Operating System type if known ("windows", "unix" or "generic")
	-s	Service version detection (banner grabber)
	-d	Depth of traversals (e.g. deepness 3 equals to ../../../; default: 6)
	-f	Specific filename (e.g. /etc/motd; default: according to OS detected, defaults in TraversalEngine.pm)
	-E	Add @Extra_files in TraversalEngine.pm (e.g. web.config, httpd.conf, etc.)
	-S	Use SSL - for HTTP and Payload module (use https:// for in url for http-uri)
	-u	URL with the part to be fuzzed marked as TRAVERSAL (e.g. http://foo:8080/id.php?x=TRAVERSAL&y=31337)
	-k	Text pattern to match in the response (http-url & payload modules - e.g. "root:" if trying /etc/passwd)
	-p	Filename with the payload to be sent and the part to be fuzzed marked with the TRAVERSAL keyword
	-x	Port to connect (default: HTTP=80; FTP=21; TFTP=69)
	-t	Time in milliseconds between each test (default: 300 (.3 second))
	-X	Use the Bisection Algorithm to detect the exact deepness once a vulnerability has been found
	-e	File extension appended at the end of each fuzz string (e.g. ".php", ".jpg", ".inc")
	-U	Username (default: 'anonymous')
	-P	Password (default: 'dot@dot.pwn')
	-M	HTTP Method to use when using the 'http' module [GET | POST | HEAD | COPY | MOVE] (default: GET)
	-r	Report filename (default: 'HOST_MM-DD-YYYY_HOUR-MIN.txt')
	-b	Break after the first vulnerability is found
	-q	Quiet mode (doesn't print each attempt)
	-C	Continue if no data was received from host
