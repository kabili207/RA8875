project 'displayTest'
	kind 'consoleapp'
	language 'c++'

	includedirs { '.', '../libusb', '../libftdi', '../libtft' }
	files { '*.cpp', '*.h' }

	links {
		'libusb',
		'libftdi',
		'libtft'
	}
