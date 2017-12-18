#
# "main" pseudo-component makefile.
#
# (Uses default behaviour of compiling all source files in directory, adding 'include' to include path.)

# Order Error
# -Wall -Wformat=0, disable warning like -> warning: format '%x' expects type 'unsigned int', but argument 3 has type 'Int32'
CPPFLAGS += -Wno-reorder -Wall -Wformat=0