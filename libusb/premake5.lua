project 'libusb'
	kind 'staticlib'
	language 'c'
	warnings 'off'
	pic 'on'

	includedirs { '.' }
	files { '*.c', '*.h' }

	filter { 'system:windows' }
		files {
			'os/poll_windows.*',
			'os/threads_windows.*',
			'os/windows_common.*',
			'os/windows_nt_common.*',
			'os/windows_winusb.*'
		}
		os.copyfile('config_windows.h', 'config.h')
		
	filter { 'system:linux' }
		files {
			'os/poll_posix.*',
			'os/threads_posix.*',
			'os/linux_netlink.*',
			'os/linux_udev.*',
			'os/linuxj_usbfs.*',
		}
		os.copyfile('config_linux.h', 'config.h')

