EMAIL=pierre.sermanet@gmail.com
TGTDIR=`pwd`/$1
###########################################################################################
echo "Recursively analyzing *.log files in ${TGTDIR}..."
DATE=`/bin/date '+%Y.%m.%d.%H.%M.%S'`
DIRNAME="plots.$DATE"
rm -Rf $DIRNAME && mkdir $DIRNAME > /dev/null && cd $DIRNAME
meta_plot $TGTDIR
if [ $? -eq 0 ]
then
    plots=`ls *.p`
    all=""
    allpdf=""
    echo "________________________________________________________________________________"
    echo "Plotting *.p"
    for p in $plots
      do
      echo "plotting $p..."
      cat $p | gnuplot
      allpdf="$allpdf -a ${p}df"
      sleep .3
    done
    echo "________________________________________________________________________________"
    echo "Taring plot data:"
    cd .. && tar czvf $DIRNAME.tgz $DIRNAME && cd $DIRNAME
    allpdf="$allpdf -a ../$DIRNAME.tgz"
    echo "________________________________________________________________________________"
    echo "Mailing pdf plots and tgz to ${EMAIL}:"
    echo "mutt $EMAIL -s Plots $allpdf < /dev/null"
    mutt $EMAIL -s Plots $allpdf < /dev/null
fi
cd .. && rm -Rf $DIRNAME
