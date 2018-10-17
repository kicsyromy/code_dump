find_program (FXC fxc DOC "Microsoft Direct3D Shader Compiler")
#if (NOT FXC_FOUND)
#	message (FATAL_ERROR "Failed to find Direct3D Shader Compiler")
#endif ()

function (compile_shader SHADER_FILE OUTPUT_DIRECTORY SHADER_TYPE VARIABLE_NAME DEPENDANT_TARGET)
	message (STATUS "Preparing shader: ${SHADER_FILE}")

	get_filename_component (BASENAME ${SHADER_FILE} NAME_WE)

	file (MAKE_DIRECTORY ${OUTPUT_DIRECTORY})

	if (${SHADER_TYPE} STREQUAL "PIXEL_SHADER")
		add_custom_command (
			OUTPUT ${BASENAME}.h
			COMMAND ${FXC} /O3 /T ps_4_0_level_9_1 /Fh "${OUTPUT_DIRECTORY}/${BASENAME}.h" /Vn ${VARIABLE_NAME} "${SHADER_FILE}"
			COMMENT "Compiling pixel shader ${SHADER_FILE}"
			VERBATIM
		)
		add_custom_target(
		    compile_${BASENAME}_shader
			DEPENDS ${BASENAME}.h
		)
		add_dependencies (${DEPENDANT_TARGET} compile_${BASENAME}_shader)
	elseif (${SHADER_TYPE} STREQUAL "VERTEX_SHADER")
		add_custom_command (
			OUTPUT ${BASENAME}.h
			COMMAND ${FXC} /O3 /T vs_4_0_level_9_1 /Fh "${OUTPUT_DIRECTORY}/${BASENAME}.h" /Vn ${VARIABLE_NAME} "${SHADER_FILE}"
			COMMENT "Compiling vertex shader ${SHADER_FILE}"
			VERBATIM
		)
		add_custom_target(
		    compile_${BASENAME}_shader
			DEPENDS ${BASENAME}.h
		)
		add_dependencies (${DEPENDANT_TARGET} compile_${BASENAME}_shader)
	else ()
		message (WARNING "Unsupported shader type ${SHADER_TYPE}")
	endif ()
endfunction ()
