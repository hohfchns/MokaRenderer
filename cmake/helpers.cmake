function(DEPENDENCIES LIB_NAME)
  foreach (DEP IN LISTS ${LIB_NAME}_DEPS)
    get_property(glob_dep GLOBAL PROPERTY ${DEP})
    if (${glob_dep})
      get_property(glob_dep_incs GLOBAL PROPERTY ${DEP}_INCS)
      get_property(glob_dep_libs GLOBAL PROPERTY ${DEP}_LIBS)

      list(APPEND ${LIB_NAME}_DEP_INCS ${glob_dep_incs})
      list(APPEND ${LIB_NAME}_DEP_LIBS ${glob_dep_libs})

      message("Found dependency ${DEP}")
    else()
      list(APPEND ${LIB_NAME}_MISSING_DEPS ${DEP} )
    endif()
  endforeach()

  if(NOT ${${LIB_NAME}_MISSING_DEPS})
    message(FATAL_ERROR "Cannot compile ${LIB_NAME} due to missing dependencies ${${LIB_NAME}_MISSING_DEPS}")
  endif()

  set(${LIB_NAME}_DEP_INCS "${${LIB_NAME}_DEP_INCS}" PARENT_SCOPE)
  set(${LIB_NAME}_DEP_LIBS "${${LIB_NAME}_DEP_LIBS}" PARENT_SCOPE)
endfunction()
