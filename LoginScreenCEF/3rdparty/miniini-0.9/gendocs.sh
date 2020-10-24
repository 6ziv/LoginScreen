#! /bin/bash
rm -r doc/html
doxygen
for file in ./doc/tutorials/*.txt
do
    txt2tags -t html -v --encoding utf-8 --css-sugar --style ../docstyle.css "$file"    
done
for file in ./doc/articles/*.txt
do
    txt2tags -t html -v --encoding utf-8 --css-sugar --style ../docstyle.css "$file"    
done
txt2tags -t html -v --encoding utf-8 --css-sugar --style doc/docstyle.css README.txt    
