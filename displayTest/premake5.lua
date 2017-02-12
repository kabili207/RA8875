project 'displayTest'
	kind 'consoleapp'
	language 'c++'

	includedirs { '.', '../libusb', '../libftdi' }
	files { '*.cpp', '*.h' }

	links {
		'libusb',
		'libftdi'
	}
