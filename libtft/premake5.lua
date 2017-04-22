project 'libtft'
	kind 'sharedlib'
	language 'c++'
	warnings 'off'

	includedirs { '.', '../libusb', '../libftdi' }
	files { '*.cpp', '*.h' }

	links {
		'libusb',
		'libftdi'
	}