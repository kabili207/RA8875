project 'displayTest'
	kind 'consoleapp'
	language 'c++'
	flags { "C++11" }

	includedirs { '.', '../libusb', '../libftdi', '../libtft' }
	files { '*.cpp', '*.h' }
	
	links {
		'libtft',
		'libftdi',
		'libusb'
	}
