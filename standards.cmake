
function(target_set_standards target)

  set_target_properties("${target}" PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
  )

endfunction()
