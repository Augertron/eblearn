#!/bin/sh

# directories
face_cmumit=/data/faces/cmumit/
eb=${HOME}/eblearn/

# run timing
$eb/bin/metarun $eb/tester/timing/go.sh
# stop here, rest is configuration for metarun
exit

# configuration
meta_name = forward_only_buffers2
meta_command = "${eb}/bin/mtdetect ${eb}/demos/face/trained/best.conf ${face_cmumit}"
meta_gnuplot_terminal=png
meta_output_dir = ${HOME}/timing/
meta_gnuplot_params = "set xlabel \"frame\"; set ylabel \"time in ms\";"
# send report by email when done
meta_send_email = 1
# use env variable "myemail"
meta_email = ${myemail}
