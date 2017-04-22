project 'libftdi'
	kind 'staticlib'
	language 'c'
	warnings 'off'
	pic 'on'

	includedirs { '.', '../libusb' }
	files { '*.c', '*.h' }
