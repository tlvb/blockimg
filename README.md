# What
Prints images on the console.
# How
List of possible command line parameters and what they do,
this text is also printed if blockimg thinks you send it
the wrong parameters.

    ./blockimg [parameters] file
    	-b -g -a -m determines if the output is going to be in
    	            monochrome, gray, ansi escape or mirc escape colors
    	-i          invert gray level of pixels (though not hue), suitable for
    	            dark on bright text media, probably works best with -b or -g
    	-s W H      determines the max dimensions (cols and rows)
    	            of the output, if not specified, info from tput(1) is used
    	-p W H      determines the character aspect ratio, since pixels
    	            are usually squares, but terminal characters are not
    	            if not specified, default values 2 1 are used
    	-f FILE     explicitly specify file to read from
    
    parameters can be joined, eg ./blockimg -cs 80 25 SOMEFILE
    or even ./blockimg -fsap SOMEFILE 100 30 3 7
    
    bitmap mode is drawn with space, and unicode characters
    2580, 2588, 258C, 2590, and 2596-259F (hexadecimal)
    gray and color mode is drawn with space, and unicode characters
    2588, 2591-2593 (hexadecimal)

# Example use
    user@host~$ ./blockimg fiiiish.png -gis 87 50
                                                                                           
                                                                                           
                                                                                           
                                                                                           
                                                                                           
                                                                                           
                                    ░▓▓▓▓▓▓▒▒▒▒▒▒▒▒▓▓▒▒▒▒▒▒▒                               
                                    ▓████████████████████████▓                             
                                    ░███░▓▒░▓░▒▓░▓▓░▓▒▒▓░▒▓▓██▓                            
                                     ▓██▓▒▓▒▒▓░▒▓░▓▒░▓▒▒▓░▒▒▒███                           
                                     ░███▓▒▓█▓███████████████████▒░                        
                                    ░▒███████▓▓▓▓▓▓▒▒▒▒▒▒▒▒▒▒▒▓▓█████▓▒░                   
             ▒████████████▓▓▒▒▒▓▓███████▓░                        ░▒▓█████▒░               
            ░███▒▒▒▒▒▒▒▓▓▓███████▓▒▒░                        ░▓███▓░   ░▓█████▓▒           
            ░███            ░▓░░░                           ▒███████▓      ░▓▓███▓▒        
            ░███     ░  ▒▓▒▓▒▒▓▒▒▓░▒ ░                       ████████          ░▓███▒      
            ░██▓░▓▒▒▓▒▓▓░▓▒▒▓▒░▓▒▒▓▒▓▒▒▓░▒▒░▒ ░░ ░  ░    ░  ░ ░▓▒▓▓░▒░ ▒ ░░ ░░ ░░▒███      
            ░███▒▒▓▒▒▓░▓▒▒▓▒▒▓████▓▓░▓▒▒▓░▓▒▒▓▒▒▓▒▓▓▒▓▒▒▓▒▒▓▒▓▒▒▓▒▒▓▒▓▓▒▓▒▒▓▒▒▓████▓░      
            ░███▓▒▒▓░▓▒░▓█████▓▒▒████▓█▓▒▓░▓▒▒▓▒▓▓░▓▒▒▓▒▒▓░▓▒▒▓▒▒▓▒▓▓░▓▒▒▓▒▒▓███▓▒         
            ▒██▓▒▓░▓▓▓████▓▒░      ░▓██████▓█▓▒█▒▓▒░▓▒▒▓░▓▓▒▓▒▒▓▒▒▓░▓▓▒▓█████▓▒            
            ▓██▓▒▓█████▓░             ▓███▓████████████▓█▓▓▓▒▓▓▒█▓▓██████▓▒░               
            ███████▓▒                ▒██▓░▓▓░▓▒▒▓▒▓▓███████████████▓▓▒░                    
            ▒█▓▒░                   ▓███▓█▓▓█▓▓█▓█▓▓█▒▓████░                               
                                    ▓████████████████████▓                                 
                                                                                           
                                                                                           
                                                                                           
                                                                                           
                                                                                           
                                                                                           
                                                                                           
                                                                                           
    user@host~$
