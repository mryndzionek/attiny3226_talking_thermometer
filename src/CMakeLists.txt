include_directories("${CMAKE_CURRENT_SOURCE_DIR}")

avr_add_executable(
    main
    main.c
    speech_synth.c
)

