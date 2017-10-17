project 'libtft'
	kind 'sharedlib'
	language 'c++'
	flags { "C++11" }

	includedirs { '.', '../libusb', '../libftdi' }
	files { '*.cpp', '*.h' }
	links {
		'libftdi',
		'libusb',
	}