project 'libftdi'
	kind 'staticlib'
	language 'c'
	warnings 'off'

	includedirs { '.', '../libusb' }
	files { '*.c', '*.h' }
