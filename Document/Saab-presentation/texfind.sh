if grep $1 *.tex > grepout; then
    emacs grepout
else
    echo "The text '$1' could not be found"
fi
rm grepout -f
rm grepout~ -f
rm "#grepout#" -f
