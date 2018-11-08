prebuilt_cxx_library(
  name = 'argh', 
  header_namespace = '', 
  header_only = True, 
  exported_headers = [
    'argh.h', 
  ], 
  visibility = [
    'PUBLIC', 
  ], 
)

cxx_binary(
  name = 'tests', 
  header_namespace = '', 
  headers = [
    'doctest.h', 
  ], 
  srcs = [
    'argh_tests.cpp', 
  ], 
  deps = [
    ':argh', 
  ], 
)

cxx_binary(
  name = 'example', 
  srcs = [
    'example.cpp', 
  ], 
  deps = [
    ':argh', 
  ], 
)
