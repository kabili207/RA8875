newoption {
	trigger = 'to',
	value   = 'path',
	default = 'build',
	description = 'Set the output location for the generated files'
}

solution 'displayTest'
	configurations { 'Release', 'Debug' }
	platforms { 'x86_64' }
	location ( _OPTIONS.to )

	flags { 
		'ExtraWarnings', 
		'StaticRuntime', 
		'MultiProcessorCompile' 
	}

	filter 'configurations:Debug'
		defines     '_DEBUG'
		symbols     'On'
	
	filter 'configurations:Release'
		defines     'NDEBUG'
		optimize    'Full'

	filter 'action:vs*'
		defines     { '_CRT_SECURE_NO_DEPRECATE', '_CRT_SECURE_NO_WARNINGS', '_CRT_NONSTDC_NO_WARNINGS' }


	filter {}


    include 'libusb'
    include 'libftdi'
    include 'displayTest'

