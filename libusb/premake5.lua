project 'libusb'
	kind 'staticlib'
	language 'c'
	warnings 'off'

	includedirs { '.' }
	files { '*.c', '*.h' }

	filter { 'system:windows' }
		files {
			'os/poll_windows.*',
			'os/threads_windows.*',
			'os/windows_common.*',
			'os/windows_nt_common.*',
			'os/windows_winusb.*',
		}

