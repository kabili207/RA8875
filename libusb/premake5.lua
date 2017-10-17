project 'libusb'
	kind 'staticlib'
	language 'c'
	warnings 'off'
	pic 'on'

	includedirs { '.' }
	files { '*.c', '*.h' }
	
	filter { 'system:windows' }
		defines { "_CONFIG_WINDOWS" }
		files {
			'os/poll_windows.*',
			'os/threads_windows.*',
			'os/windows_common.*',
			'os/windows_nt_common.*',
			'os/windows_winusb.*'
		}
		
	filter { 'system:linux' }
		defines { "_CONFIG_LINUX" }
		files {
			'os/poll_posix.*',
			'os/threads_posix.*',
			'os/linux_netlink.*',
			'os/linux_udev.*',
			'os/linuxj_usbfs.*',
		}

